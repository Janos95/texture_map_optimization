//
// Created by janos on 05.02.20.
//



#include "optimization.hpp"
#include "visible_texture.hpp"
#include "interpolated_vertices.hpp"
#include "coords_filter.hpp"


#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/Platform/WindowlessEglApplication.h>
#include <Magnum/Platform/GLContext.h>


cv::Mat_<cv::Vec3f> computeInterpolatedMeshVertices(Trade::MeshData3D& meshData, const int H, const int W){

    //construct opengl mesh with vertices and texture coordinates swapped
    GL::Mesh glmesh;
    Containers::ArrayView<const Vector2> textureCoords = meshData.textureCoords2D(0);
    Containers::ArrayView<const Vector3> vertices = meshData.positions(0);
    auto interleaved = MeshTools::interleave(textureCoords, vertices);
    const auto& [indexData, indexType, indexStart, indexEnd] = MeshTools::compressIndices(meshData.indices());

    using Position = InterpolateVerticesShader::Position;
    using Vertex = InterpolateVerticesShader::Vertex;

    glmesh.addVertexBuffer(GL::Buffer(interleaved), 0, Position{}, Vertex{})
            .setPrimitive(MeshPrimitive::Triangles)
            .setIndexBuffer(GL::Buffer(indexData), 0, indexType, indexStart, indexEnd);


    //prepare offline rendering using interpolation shader
    GL::Texture2D interpolatedVertices; //TODO: this could be a renderbuffer
    interpolatedVertices.setStorage(0, GL::TextureFormat::RGB32F, {W,H});
    GL::Framebuffer framebuffer{{{}, {W,H}}};
    auto vertexAttachment = GL::Framebuffer::ColorAttachment{0};
    framebuffer.attachTexture(vertexAttachment, interpolatedVertices, 0)
                .clear(GL::FramebufferClear::Color)
                .bind();

    InterpolateVerticesShader shader;
    glmesh.draw(shader);

    //read everything into opencv matrix
    cv::Mat_<cv::Vec3f> img(W,H);
    Containers::ArrayView<float> data((float*)img.data, W*H*3);
    framebuffer.mapForRead(vertexAttachment).read(framebuffer.viewport(), MutableImageView2D{PixelFormat::RGB32F, {W, H}, data});
    return img;
}

void visibleTextureCoords(
        GL::Mesh& mesh,
        const Matrix4& tf,
        const Matrix4& proj,
        const float threshold,
        cv::Mat_<cv::Vec2i>& coords)
{
    auto W = coords.cols, H = coords.rows;

    //render texture visibility
    GL::Framebuffer coordsFramebuffer{{{}, {W,H}}};

    GL::Texture2D depthTexture;
    GL::Texture2D coordTexture;
    coordTexture.setStorage(0, GL::TextureFormat::RG32I, {W,H});
    depthTexture.setStorage(0, GL::TextureFormat::DepthComponent32F, {W,H});

    coordsFramebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, coordTexture, 0)
                     .attachTexture(GL::Framebuffer::BufferAttachment::Depth, depthTexture, 0);

    coordsFramebuffer.clearDepth(1.0);
    coordsFramebuffer.clearColor(0, Vector4i(-1));
    coordsFramebuffer.bind();

    VisibleTextureShader visibiltyShader;
    visibiltyShader.setTransformationProjectionMatrix(proj * tf)
                   .setTextureSize({W,H});
    mesh.draw(visibiltyShader);

    //setup framebuffer for depth unprojection
    GL::Framebuffer filteredCoordsBuffer{{{}, {W,H}}};
    GL::Renderbuffer coordBuffer;
    coordBuffer.setStorage(GL::RenderbufferFormat::RG32I, {W,H});
    filteredCoordsBuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, coordBuffer);
    filteredCoordsBuffer.bind();

    CoordsFilterShader filterShader;
    filterShader.setTextureSize({W,H})
                .setThreshold(proj, threshold)
                .bindCoordsTexture(coordTexture)
                .bindDepthTexture((depthTexture));

    GL::Mesh{}.setCount(3).draw(filterShader);

    //download filtered coords to host
    Containers::Array<Vector2i> data(W*H*2);
    auto coordView = MutableImageView2D{PixelFormat::RG32I, {W, H}, data};
    coordsFramebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0}).read(coordsFramebuffer.viewport(), coordView);

    //copy data into opencv matrix
    auto pixel = coordView.pixels();
    std::transform(pixel.begin(), pixel.end(), coords.begin(),[](const auto& v){ return cv::Vec2i(v[0],v[1]); });
}


TextureMapOptimization::TextureMapOptimization(
        Trade::MeshData3D& mesh,
        std::vector<Frame>& frames,
        Matrix3& cameraMatrix,
        Vector2i res,
        float depthThreshold):
    m_frames(frames),
    m_texture(res[1], res[0]),
    m_camera(compressCameraMatrix(cameraMatrix)),
    m_meshData(mesh)
{
    Platform::WindowlessGLContext glContext{{}};
    glContext.makeCurrent();
    Platform::GLContext context;

    auto glmesh = MeshTools::compile(mesh);

    auto frameH = frames.front().image.rows;
    auto frameW = frames.front().image.cols;

    auto texH = m_texture.rows;
    auto texW = m_texture.cols;

    m_visibility.resize(texH*texW);

    auto proj = projectionMatrixFromCameraParameters(cameraMatrix, frameW, frameH);

    for(std::size_t i = 0; i < frames.size(); ++i){
        const auto& frame = frames[i];

        cv::Mat_<cv::Vec2i> coords(frameH, frameW);
        visibleTextureCoords(glmesh, frame.tf, proj, depthThreshold, coords);

        auto begin = coords.begin(), end = coords.end();
        std::sort(begin, end);
        end = std::unique(begin, end);

        for(auto it = begin; it != end; ++it){
             auto x = (*it)[0];
             auto y = (*it)[1];
             if( x < 0 || y < 0)
                 continue;
             auto& vis = m_visibility[x + texW * y];
             vis.photometricCosts.emplace_back(i, frame.image, frame.xderiv, frame.yderiv, m_texture(y,x));
             vis.x = x;
             vis.y = y;
        }
    }

    //remove nowhere visible texture pixels from visibility information
    auto it = std::remove_if(m_visibility.begin(), m_visibility.end(),
            [](const auto& pix){ return pix.visibleImages.empty(); });
    m_visibility.erase(it, m_visibility.end());

    //build problem
    cv::Mat_<cv::Vec3f> ips = computeInterpolatedMeshVertices(mesh, texH, texW);
    for(const auto& [x,y,photoCosts] : m_visibility){
        for(const auto& photoCost: photoCosts){
            auto& frame = frames[photoCost.idx];
            auto* functor = new TexturePixelCost{ips(y,x), photoCost};
            auto* cost =  new ceres::AutoDiffCostFunction<TexturePixelCost, 3 /*rgb*/, 3 /*rvec*/, /*tvec*/3, /*cam*/ 4>(functor);
            m_problem.AddResidualBlock(cost, nullptr, m_texture(y,x).val, frame.rvec.val, frame.tvec.val, m_camera.val);
        }
    }
}




cv::Mat TextureMapOptimization::run(Vector2i res, bool vis){
    (*m_updateTexture)(ceres::IterationSummary{});

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::SPARSE_SCHUR;
    options.minimizer_progress_to_stdout = true;
    options.update_state_every_iteration = true;
    options.callbacks.push_back(m_updateTexture.get());
    ceres::Solver::Summary summary;

    std::thread t([&] {
        ceres::Solve(options, &m_problem, &summary);
    });

    if(vis){
        m_viewer = std::make_unique<Viewer>();
        m_viewer->scene.addObject("mesh", m_meshData);
        m_viewer->callbacks.emplace_back(UpdateScene{m_texture});
        m_viewer->exec();
    }
    t.join();

    return m_texture.clone();
}


