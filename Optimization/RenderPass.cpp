//
// Created by janos on 7/23/20.
//

#include "RenderPass.h"

#include <Corrade/Utility/Resource.h>

#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/ImageView.h>

#include <assert.h>

namespace TextureMapOptimization {

namespace {

void setupTexture(GL::Texture2D& texture, Vector2i const& size, GL::TextureFormat format, bool generateMipMap = false) {

    if(generateMipMap)
        texture.setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear);
    else
        texture.setMinificationFilter(GL::SamplerFilter::Linear);

    texture.setMagnificationFilter(GL::SamplerFilter::Linear)
           .setWrapping(GL::SamplerWrapping::ClampToEdge);

    if(generateMipMap) {
        texture.setStorage(Mg::Math::log2(size.max()) + 1, format, size);
    } else {
        texture.setStorage(1, format, size);
    }
}

}

RenderPass::RenderPass(GL::Mesh& mesh, Array<KeyFrame>& keyFrames) : m_mesh(mesh), m_keyFrames(keyFrames) {

    m_imageSize = keyFrames.front().image.imageSize(0);
    m_fb = GL::Framebuffer{{{}, m_imageSize}};

    //m_mesh.addVertexBufferInstanced(m_instancedTransformations, 1, 0, Shaders::Diff::TransformationMatrix{});

    /* textures for intermediate steps in gradient computation */
    Int layerCount = m_keyFrames.size();
    setupTexture(m_depth, m_imageSize, GL::TextureFormat::DepthComponent32F);
    setupTexture(m_costs, m_imageSize, GL::TextureFormat::RG32F, true);
    setupTexture(m_gradientRotations, m_imageSize, GL::TextureFormat::RGBA32F, true);
    setupTexture(m_gradientTranslations, m_imageSize, GL::TextureFormat::RGBA32F, true);

    setupTexture(m_texCoordsTexture, m_imageSize, GL::TextureFormat::RGBA32F);

    const auto map = Mg::DebugTools::ColorMap::turbo();
    const Vector2i size{Int(map.size()), 1};

    m_turbo
            .setMinificationFilter(GL::SamplerFilter::Linear)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setWrapping(GL::SamplerWrapping::ClampToEdge) // or Repeat
            .setStorage(1, GL::TextureFormat::RGB8, size) // or SRGB8
            .setSubImage(0, {}, Mg::ImageView2D{Mg::PixelFormat::RGB8Srgb, size, map});
}

void RenderPass::setTexture(GL::Texture2D& texture) {
    m_texture = &texture;
    m_textureSize = m_texture->imageSize(0);

    /* texture for averaging image colors in texture */
    setupTexture(m_red, m_textureSize, GL::TextureFormat::R32F);
    setupTexture(m_green, m_textureSize, GL::TextureFormat::R32F);
    setupTexture(m_blue, m_textureSize, GL::TextureFormat::R32F);
    setupTexture(m_count, m_textureSize, GL::TextureFormat::R32F);
}

void RenderPass::setFramebufferMode(FramebufferMode mode) {

    if(m_fbMode != mode && mode == FramebufferMode::ColorAveragingPass) {
        m_fb.attachTexture(GL::Framebuffer::BufferAttachment::Depth, m_depth, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{0}, m_texCoordsTexture, 0)
            .mapForDraw(GL::Framebuffer::ColorAttachment{0});
    }

    if(m_fbMode != mode && mode == FramebufferMode::OptimizationPass) {
        m_fb.attachTexture(GL::Framebuffer::BufferAttachment::Depth, m_depth, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{0}, m_costs, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{1}, m_gradientRotations, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{2}, m_gradientTranslations, 0)
            .mapForDraw({{0, GL::Framebuffer::ColorAttachment{0}}, {1, GL::Framebuffer::ColorAttachment{1}}, {2, GL::Framebuffer::ColorAttachment{2}}});
    }

    m_fb.bind();
    CORRADE_INTERNAL_ASSERT(m_fb.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);
    m_fbMode = mode;
}

void RenderPass::optimizationPass(size_t idx, const double* params, double& residual, double* gradient) {
    m_profilerComputeGradient.beginFrame();

    setFramebufferMode(FramebufferMode::OptimizationPass);
    m_fb.clear(GL::FramebufferClear::Depth | GL::FramebufferClear::Color);

    Vector3 rotation(params[0], params[1], params[2]);
    Vector3 translation(params[3], params[4], params[5]);

    auto view = uncompress6DTransformation(StaticArrayView<6, const double>{params});

    m_diff.setTranslation(translation)
          .setRotation(rotation)
          .setProjectionTransformationMatrix(m_projection*view)
          .setCameraParameters(m_fx, m_fy, m_cx, m_cy)
          .bindGroundTruthTexture(m_keyFrames[idx].image)
          .bindOptimizationTexture(*m_texture)
          .draw(m_mesh);

    //TODO: replace this with a proper reduction shader
    //building a flatter mip chain.
    m_costs.generateMipmap();
    m_gradientRotations.generateMipmap();
    m_gradientTranslations.generateMipmap();

    /* download the last mip level */
    float costHom[2];
    float gradRotHom[4];
    float gradTransHom[4];

    Mg::MutableImageView2D costView{GL::PixelFormat::RG, GL::PixelType::Float, {1, 1}, costHom};
    Mg::MutableImageView2D rotView{GL::PixelFormat::RGBA, GL::PixelType::Float, {1, 1}, gradRotHom};
    Mg::MutableImageView2D transView{GL::PixelFormat::RGBA, GL::PixelType::Float, {1, 1}, gradTransHom};

    Int lastMipIdx = Math::log2(m_imageSize.max());
    m_costs.image(lastMipIdx, costView);
    m_gradientRotations.image(lastMipIdx, rotView);
    m_gradientTranslations.image(lastMipIdx, transView);

    residual = costHom[0]/costHom[1];
    if(gradient) {
        for(int j = 0; j < 3; ++j) {
            gradient[j] = gradRotHom[j]/gradRotHom[3];
            gradient[3 + j] = gradTransHom[j]/gradTransHom[3];
        }
    }

    m_profilerComputeGradient.endFrame();
    //m_profilerComputeGradient.printStatistics(10);
}

void RenderPass::averagingPass() {
    ScopedTimer t{"Averaging all colors", true};

    setFramebufferMode(FramebufferMode::ColorAveragingPass);

    m_fb.clear(GL::FramebufferClear::Depth | GL::FramebufferClear::Color);

    Vector3ui wgCount1(m_imageSize.x(), m_imageSize.y(), 1);
    Vector3ui wgCount2(m_textureSize.x(), m_textureSize.y(), 1);

    m_fb.clearColor(0, Color4{});

    for(auto& kf : m_keyFrames) {
        kf.uncompressPose();
        m_fb.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

        m_texCoordsShader.setTransformationProjectionMatrix(m_projection*kf.tf)
                         .draw(m_mesh);

        m_remap.bindTextureR(m_red)
               .bindTextureG(m_green)
               .bindTextureB(m_blue)
               .bindTextureA(m_count)
               .bindImage(kf.image)
               .bindTextureCoordinates(m_texCoordsTexture)
               .dispatchCompute(wgCount1);
    }

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    m_combine.bindTextureR(m_red)
             .bindTextureG(m_green)
             .bindTextureB(m_blue)
             .bindTextureA(m_count)
             .bindRgbaImage(*m_texture)
             .dispatchCompute(wgCount2);

    //GL::Renderer::setMemoryBarrier(Mg::GL::Renderer::MemoryBarrier::TextureFetch);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void RenderPass::renderIntoTexture(GL::Texture2D& image, size_t idx, VisualizatonFlag flag) {
    if(flag == VisualizatonFlag::RenderedImage) {
        m_fb.attachTexture(GL::Framebuffer::BufferAttachment::Depth, m_depth, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{0}, image, 0)
            .mapForDraw(GL::Framebuffer::ColorAttachment{0})
            .bind();

        CORRADE_INTERNAL_ASSERT(m_fb.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);
        auto& kf = m_keyFrames[idx];

        m_fb.clear(GL::FramebufferClear::Depth | GL::FramebufferClear::Color);
        Mg::Shaders::Flat3D flat{Mg::Shaders::Flat3D::Flag::Textured};
        assert(m_texture);
        flat.bindTexture(*m_texture)
            .setTransformationProjectionMatrix(m_projection*kf.tf)
            .draw(m_mesh);

        return;
    } else {
        setFramebufferMode(FramebufferMode::OptimizationPass);
    }
    m_fb.clear(GL::FramebufferClear::Depth | GL::FramebufferClear::Color);

    StaticArrayView<6, const double> params = m_keyFrames[idx].tf6D;
    Vector3 rotation(params[0], params[1], params[2]);
    Vector3 translation(params[3], params[4], params[5]);

    Shaders::Diff diff;

    diff.setTranslation(translation)
        .setRotation(rotation)
        .setProjectionTransformationMatrix(m_projection*m_keyFrames[idx].tf)
        .setCameraParameters(m_fx, m_fy, m_cx, m_cy)
        .bindGroundTruthTexture(m_keyFrames[idx].image)
        .bindOptimizationTexture(*m_texture)
        .draw(m_mesh);

    bool fetchCost = flag == VisualizatonFlag::Cost;
    bool fetchRot = flag == VisualizatonFlag::RotationGradient;
    bool fetchTrans = flag == VisualizatonFlag::TranslationGradient;

    Array<char> costData(NoInit, 2*sizeof(float)*m_imageSize.product());
    Array<char> rotData(4*sizeof(float)*m_imageSize.product());
    Array<char> transData(NoInit, 4*sizeof(float)*m_imageSize.product());

    if(fetchCost)
        m_costs.image(0, Mg::MutableImageView2D{GL::PixelFormat::RG, GL::PixelType::Float, m_imageSize, costData});
    if(fetchRot)
        m_gradientRotations.image(0,Mg::MutableImageView2D{GL::PixelFormat::RGBA, GL::PixelType::Float, m_imageSize, rotData});
    if(fetchTrans)
        m_gradientTranslations.image(0, Mg::MutableImageView2D{GL::PixelFormat::RGBA, GL::PixelType::Float, m_imageSize, transData});

    float min = std::numeric_limits<float>::max();
    float max = -std::numeric_limits<float>::max();
    for(size_t i = 0; i < m_imageSize.product(); ++i) {
        float value;

        if(fetchCost)
            value = arrayCast<Vector2>(costData)[i].x();
        if(fetchRot)
            value = arrayCast<Vector4>(rotData)[i].xyz().length();
        if(fetchTrans)
            value = arrayCast<Vector4>(transData)[i].xyz().length();

        if(value > std::numeric_limits<float>::epsilon()) {
            min = Math::min(value, min);
            max = Math::max(value, max);
        }
    }

    float scale = 1.f/(max - min);
    float offset = 1 - max*scale;

    Vector3ui wgCount(m_imageSize.x(), m_imageSize.y(), 1);
    m_vis.setColorMapTransform(scale, offset)
         .bindColorMap(m_turbo)
         .bindOutputImage(image);

    if(fetchCost)
        m_vis.bindCostImage(m_costs);
    if(fetchTrans)
        m_vis.bindGradientImage(m_gradientTranslations);
    if(fetchRot)
        m_vis.bindGradientImage(m_gradientRotations);

    m_vis.dispatchCompute(wgCount);
}

void RenderPass::reloadShader() {
    Cr::Utility::Resource::overrideGroup("tmo-data", "/home/janos/TextureMapOptimization/Shaders/resources.conf");
    m_diff = Shaders::Diff();
    m_texCoordsShader = Shaders::TextureCoordinates();
    m_remap = Shaders::Remap();
    m_combine = Shaders::Combine();
    m_vis = Shaders::Vis();
}

}