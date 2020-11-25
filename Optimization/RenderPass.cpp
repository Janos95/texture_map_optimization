//
// Created by janos on 7/23/20.
//

#include "RenderPass.h"

#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Shaders/Flat.h>

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

void setupTextureArray(GL::Texture2DArray& texture, Vector3i const& size, GL::TextureFormat format) {
    texture.setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
           .setMagnificationFilter(GL::SamplerFilter::Linear)
           .setWrapping(GL::SamplerWrapping::ClampToEdge)
           .setStorage(Mg::Math::log2(size.xy().max()) + 1, Mg::GL::TextureFormat::RGBA32F, size);
}

}

RenderPass::RenderPass(GL::Mesh& mesh, Array<KeyFrame>& keyFrames) : m_mesh(mesh), m_keyFrames(keyFrames) {

    m_imageSize = keyFrames.front().image.imageSize(0);
    m_fb = GL::Framebuffer{{{}, m_imageSize}};

    //m_mesh.addVertexBufferInstanced(m_instancedTransformations, 1, 0, Shaders::Diff::TransformationMatrix{});

    /* textures for intermediate steps in gradient computation */
    Int layerCount = m_keyFrames.size();
    setupTexture(m_depth, m_imageSize, GL::TextureFormat::DepthComponent32F);
    setupTexture(m_costs, m_imageSize, GL::TextureFormat::RG32F);
    setupTexture(m_gradientRotations, m_imageSize, GL::TextureFormat::RGBA32F);
    setupTexture(m_gradientTranslations, m_imageSize, GL::TextureFormat::RGBA32F);


    setupTexture(m_texCoordsTexture, m_imageSize, GL::TextureFormat::RGBA32F);
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

    if(m_fbMode != mode && mode ==  FramebufferMode::OptimizationPass) {
        m_fb.attachTexture(GL::Framebuffer::BufferAttachment::Depth, m_depth, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{0}, m_costs, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{1}, m_gradientRotations, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{2}, m_gradientTranslations, 0);
    }

    m_fb.bind();
    CORRADE_INTERNAL_ASSERT(m_fb.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);
    m_fbMode = mode;
}

void RenderPass::optimizationPass(const double* const* params, double* residuals, double** jacobians) {
    m_profilerComputeGradient.beginFrame();

    setFramebufferMode(FramebufferMode::OptimizationPass);

    Mg::Vector2i size = m_imageSize;
    int level = 0;

    m_fb.clear(GL::FramebufferClear::Depth|GL::FramebufferClear::Color);

    for(std::size_t i = 0; i < m_keyFrames.size(); ++i) {
        m_keyFrames[i].uncompressPose();

        m_fb.clear(GL::FramebufferClear::Depth | GL::FramebufferClear::Color);

        Vector3 rotation(params[i][0], params[i][1], params[i][2]);
        Vector3 translation(params[i][3], params[i][4], params[i][5]);

        auto view = m_keyFrames[i].pose.invertedRigid();

        m_diff.setTranslation(translation)
              .setRotation(rotation)
              .setProjectionTransformationMatrix(m_projection*view)
              .setCameraParameters(m_fx, m_fy, m_cx, m_cy)
              .bindGroundTruthTexture(m_keyFrames[i].image)
              .bindOptimizationTexture(*m_texture);

        //m_depthFilter.bindCosts(m_cost)
        //             .bindRotationGradients(m_gradientRotations)
        //             .bindTranslationGradients(m_gradientTranslations)
        //             .bindCostsAccumulated(m_costAccumulated, i)
        //             .bindRotationGradientsAccumulated(m_gradientRotationsAccumulated, i)
        //             .bindTranslationGradientsAccumulated(m_gradientTranslationsAccumulated, i)
        //             .bindDepth(m_depth);
    }

    m_costs.generateMipmap();
    m_gradientRotations.generateMipmap();
    m_gradientTranslations.generateMipmap();

    Int levelCount = Math::log2(m_imageSize.max()) + 1;

    /* download the last mip level */
    Cr::Containers::Array<char> dataCosts(sizeof(Float)*levelCount);
    Cr::Containers::Array<char> dataRotations(sizeof(Float)*3*levelCount);
    Cr::Containers::Array<char> dataTranslations(sizeof(Float)*3*levelCount);

    m_costs.image(levelCount - 1, Mg::MutableImageView2D{Mg::GL::PixelFormat::RGBA, Mg::GL::PixelType::Float, {1, 1}, dataCosts});
    m_gradientRotations.image(levelCount - 1, Mg::MutableImageView2D{Mg::GL::PixelFormat::RGBA, Mg::GL::PixelType::Float, {1, 1}, dataRotations});
    m_gradientTranslations.image(levelCount - 1, Mg::MutableImageView2D{Mg::GL::PixelFormat::RG, Mg::GL::PixelType::Float, {1, 1}, dataTranslations});

    auto rotView = arrayCast<Float>(dataCosts);
    auto transView = arrayCast<Vector3>(dataRotations);
    auto costView = arrayCast<Vector3>(dataTranslations);

    //for(std::size_t i = 0; i < m_results.size(); ++i) {
    //    residuals[i] = m_results[i].cost;
    //    if(jacobians && jacobians[i]) {
    //        for(int j = 0; j < 3; ++j) {
    //            jacobians[0][3*i + j] = m_results[i].rotationGrad[j];
    //            jacobians[1][3*i + j] = m_results[i].translationGrad[j];
    //        }
    //    }
    //}

    m_profilerComputeGradient.endFrame();
    m_profilerComputeGradient.printStatistics(10);
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

        m_texCoordsShader.setTransformationProjectionMatrix(m_projection*kf.pose.inverted())
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

void RenderPass::renderKeyPose(GL::Texture2D& image, size_t idx) {
    m_fb.attachTexture(GL::Framebuffer::BufferAttachment::Depth, m_depth, 0)
        .attachTexture(GL::Framebuffer::ColorAttachment{0}, image, 0)
        .mapForDraw(GL::Framebuffer::ColorAttachment{0})
        .bind();

    CORRADE_INTERNAL_ASSERT(m_fb.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);

    auto& kf = m_keyFrames[idx];

    m_fb.clear(GL::FramebufferClear::Depth|GL::FramebufferClear::Color);
    Mg::Shaders::Flat3D flat{Mg::Shaders::Flat3D::Flag::Textured};
    assert(m_texture);
    flat.bindTexture(*m_texture)
        .setTransformationProjectionMatrix(m_projection*kf.pose.invertedRigid())
        .draw(m_mesh);
}

void RenderPass::renderGradient(GL::Texture2D& image, size_t idx) {
   //TODO
}

}