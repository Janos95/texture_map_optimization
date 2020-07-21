//
// Created by janos on 05.02.20.
//

#include "optimization.hpp"
#include "Diff.h"
#include "interpolated_vertices.hpp"
#include "coords_filter.hpp"


#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/Platform/WindowlessGlxApplication.h>
#include <Magnum/Platform/GLContext.h>



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

    auto proj = projectionMatrixFromCameraMatrix(cameraMatrix, frameW, frameH);
    cv::Mat_<cv::Vec3f> ips = computeInterpolatedMeshVertices(mesh, texH, texW);

    for(std::size_t i = 0; i < frames.size(); ++i){
        auto& frame = frames[i];

        cv::Mat_<cv::Vec2i> coords(frameH, frameW);
        visibleTextureCoords(glmesh, frame.tf, proj, depthThreshold, coords);

        auto begin = coords.begin(), end = coords.end();
        std::sort(begin, end,
                [](const auto& v1, const auto& v2){ return v1[0] < v2[0] || (v1[0] == v2[0] && v1[1] < v2[1]); });
        end = std::unique(begin, end,
                [](const auto& v1, const auto& v2){ return v1[0] == v2[0] && v1[1] == v2[1]; });

        for(auto it = begin; it != end; ++it){
             auto x = (*it)[0];
             auto y = (*it)[1];
             if( x < 0 || y < 0)
                 continue;
             auto& vis = m_visibility[x + texW * y];
             auto photoCost = new PhotometricCost(i, frame.image, frame.xderiv, frame.yderiv, m_texture(y,x));
             auto* functor = new TexturePixelFunctor(ips(y,x), photoCost);
             auto* cost =  new ceres::AutoDiffCostFunction<TexturePixelFunctor, 3 /*rgb*/, 3 /*rvec*/, /*tvec*/3, /*cam*/ 4>(functor);
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
        int dummy; //TODO: remove
        m_viewer = std::make_unique<Viewer>(dummy, nullptr);
        m_viewer->scene.addObject("mesh", m_meshData);
        m_viewer->callbacks.emplace_back(UpdateScene{m_texture});
        m_viewer->exec();
    }
    t.join();

    return m_texture.clone();
}


