//
// Created by janos on 7/23/20.
//

#include "RenderPass.h"

#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/Math/Color.h>

using namespace Magnum;
using namespace Corrade;

namespace {

void setupTexture(GL::Texture2D& texture, Vector2i const& size, GL::TextureFormat format, bool generateMipMap = false){

    if(generateMipMap)
        texture.setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear);
    else
        texture.setMinificationFilter(GL::SamplerFilter::Linear);

    texture.setMagnificationFilter(GL::SamplerFilter::Linear)
           .setWrapping(GL::SamplerWrapping::ClampToEdge);

    if(generateMipMap){
        texture.setStorage(Mg::Math::log2(size.max()) + 1, Mg::GL::TextureFormat::RGBA32F, size)
               .generateMipmap();
    } else {
        texture.setStorage(1, format, size);
    }
}

}

RenderPass::RenderPass(GL::Mesh& mesh, GL::Texture2D& texture, Vector2i imageSize) :
    m_mesh(mesh), m_texture(texture), m_imageSize(imageSize), m_textureSize(texture.imageSize(0))
{
    m_fb = GL::Framebuffer{{{},m_imageSize}};

    /* generate mip maps for reduction step */
    setupTexture(m_mipMapRotations, m_imageSize, GL::TextureFormat::RGBA32F, true);
    setupTexture(m_mipMapTranslations, m_imageSize, GL::TextureFormat::RGBA32F, true);
    setupTexture(m_mipMapCost, m_imageSize, GL::TextureFormat::RG32F, true);

    /* textures for intermediate steps in gradient computation */
    setupTexture(m_depth, m_imageSize, GL::TextureFormat::R32F);
    setupTexture(m_cost, m_imageSize, GL::TextureFormat::RG32F);
    setupTexture(m_color, m_imageSize, GL::TextureFormat::RGBA32F);
    setupTexture(m_gradRot, m_imageSize, GL::TextureFormat::RGBA32F);
    setupTexture(m_gradTrans, m_imageSize, GL::TextureFormat::RGBA32F);

    /* texture for averaging image colors in texture */
    setupTexture(m_red, m_textureSize, GL::TextureFormat::R32F);
    setupTexture(m_green, m_textureSize, GL::TextureFormat::R32F);
    setupTexture(m_blue, m_textureSize, GL::TextureFormat::R32F);
    setupTexture(m_count, m_textureSize, GL::TextureFormat::R32F);

    m_jacs.setMagnificationFilter(GL::SamplerFilter::Linear)
          .setMinificationFilter(GL::SamplerFilter::Linear)
          .setWrapping(GL::SamplerWrapping::ClampToEdge)
          .setStorage(1, GL::TextureFormat::RG32F, {imageSize.x(), imageSize.y(), 6}); ;

}


RenderPass::GradientResult RenderPass::computeGradient(KeyFrame& kf, Vector3 const& angleAxis, Vector3 const& translation) {

    Mg::Vector2i size = m_imageSize;
    int level = 0;

    if(m_fbMode != FramebufferMode::GroundTruthImage){
        m_fb.attachTexture(GL::Framebuffer::BufferAttachment::Depth, m_depth, 0)
            .attachTexture(GL::Framebuffer::ColorAttachment{0}, m_color, 0)
            .attachTextureLayer(GL::Framebuffer::ColorAttachment{1}, m_jacs, 0, 0)
            .attachTextureLayer(GL::Framebuffer::ColorAttachment{2}, m_jacs, 0, 1)
            .attachTextureLayer(GL::Framebuffer::ColorAttachment{3}, m_jacs, 0, 2)
            .attachTextureLayer(GL::Framebuffer::ColorAttachment{4}, m_jacs, 0, 3)
            .attachTextureLayer(GL::Framebuffer::ColorAttachment{5}, m_jacs, 0, 4)
            .attachTextureLayer(GL::Framebuffer::ColorAttachment{6}, m_jacs, 0, 5)
            .mapForDraw()
            .bind();
    }

    m_fb.clear(GL::FramebufferClear::Depth|GL::FramebufferClear::Color);

    m_diff.setProjectionMatrix(kf.projection)
          .setRotation(angleAxis)
          .setTranslation(translation)
          .bindTexture(m_texture)
          .draw(m_mesh);

    m_chainRule.bindDepth(m_depth)
               .bindCost(m_cost)
               .bindGradRotation(m_gradRot)
               .bindGradTranslation(m_gradTrans)
               .bindGroundTruthImage(kf.image)
               .bindJacobian(m_jacs)
               .bindRenderedImage(m_color)
               .dispatchCompute({UnsignedInt(size.x()), UnsignedInt(size.y()), 1});

    Vector4 gradRotHomCheck{};
    Vector4 gradTransHomCheck{};
    Vector2 costHomCheck{};
    {
        ScopedTimer timer{"CPU Reduction", true};

        Cr::Containers::Array<char> dataRot(size.product() * sizeof(float) * 4);
        Cr::Containers::Array<char> dataTrans(size.product() * sizeof(float) * 4);
        Cr::Containers::Array<char> dataCost(size.product() * sizeof(float) * 2);

        m_mipMapRotations.image(level, Mg::MutableImageView2D{Mg::GL::PixelFormat::RGBA, Mg::GL::PixelType::Float, size, dataRot});
        m_mipMapTranslations.image(level, Mg::MutableImageView2D{Mg::GL::PixelFormat::RGBA, Mg::GL::PixelType::Float, size, dataTrans});
        m_mipMapCost.image(level, Mg::MutableImageView2D{Mg::GL::PixelFormat::RGBA, Mg::GL::PixelType::Float, size, dataCost});

        auto rotView = Containers::arrayCast<Vector4>(dataRot);
        auto transView = Containers::arrayCast<Vector4>(dataTrans);
        auto costView = Containers::arrayCast<Vector2>(dataCost);

        for(int i = 0; i < size.product(); ++i){
            gradRotHomCheck += rotView[i];
            gradTransHomCheck += transView[i];
            costHomCheck += costView[i];
        }
    }

    constexpr Mg::UnsignedInt stepSize = 4;
    constexpr Mg::UnsignedInt downloadThreshold = 10'000;
    static shaders::Reduction reductionShader{stepSize};

    Vector4 gradRotHom{};
    Vector4 gradTransHom{};
    Vector2 costHom{};
    {
        ScopedTimer timer{"Compute Shader Reduction", true};
        while(size.product() > downloadThreshold){
            Mg::Vector2ui wg{m_mipMapRotations.imageSize(level + stepSize)};

            reductionShader.bindCostInImage(level == 0 ? m_cost : m_mipMapCost, level)
                           .bindRotationInImage(level == 0 ? m_gradRot : m_mipMapRotations, level)
                           .bindTranslationInImage(level == 0 ? m_gradTrans : m_mipMapTranslations, level)
                           .bindCostOutImage(m_mipMapCost, level + stepSize)
                           .bindRotationOutImage(m_mipMapRotations, level + stepSize)
                           .bindTranslationOutImage(m_mipMapTranslations, level + stepSize)
                           .dispatchCompute({wg.x(), wg.y(), 1});
            size = m_mipMapRotations.imageSize(level + stepSize);
            level += stepSize;
            Mg::GL::Renderer::setMemoryBarrier(Mg::GL::Renderer::MemoryBarrier::TextureFetch);
        }

        Cr::Containers::Array<char> dataRot(size.product() * sizeof(float) * 4);
        Cr::Containers::Array<char> dataTrans(size.product() * sizeof(float) * 4);
        Cr::Containers::Array<char> dataCost(size.product() * sizeof(float) * 2);

        m_mipMapRotations.image(level, Mg::MutableImageView2D{Mg::GL::PixelFormat::RGBA, Mg::GL::PixelType::Float, size, dataRot});
        m_mipMapTranslations.image(level, Mg::MutableImageView2D{Mg::GL::PixelFormat::RGBA, Mg::GL::PixelType::Float, size, dataTrans});
        m_mipMapCost.image(level, Mg::MutableImageView2D{Mg::GL::PixelFormat::RGBA, Mg::GL::PixelType::Float, size, dataCost});

        auto rotView = Containers::arrayCast<Vector4>(dataRot);
        auto transView = Containers::arrayCast<Vector4>(dataTrans);
        auto costView = Containers::arrayCast<Vector2>(dataCost);

        for(int i = 0; i < size.product(); ++i){
            gradRotHom += rotView[i];
            gradTransHom += transView[i];
            costHom += costView[i];
        }
    }

    CORRADE_ASSERT(Mg::Math::abs((gradRotHom - gradRotHomCheck).dot()) < 1e-7, "Sums are not equal", {});
    CORRADE_ASSERT(Mg::Math::abs((gradTransHom - gradTransHomCheck).dot()) < 1e-7, "Sums are not equal", {});
    CORRADE_ASSERT(Mg::Math::abs((gradTransHom - gradTransHomCheck).dot()) < 1e-7, "Sums are not equal", {});

    return {gradRotHom.xyz()/gradRotHom.z(), gradTransHom.xyz()/gradTransHom.z(), costHom.x()/costHom.y()};
}

void RenderPass::averageImagesIntoTexture(Containers::Array<KeyFrame>& kfs) {
    ScopedTimer t{"Averaging all colors", true};


    if(m_fbMode != FramebufferMode::OptimizationTexture){
        m_fb.attachTexture(GL::Framebuffer::BufferAttachment::Depth, m_depth, 0)

            .bind();
    }

    m_fb.clear(GL::FramebufferClear::Depth|GL::FramebufferClear::Color);

    Range2Di viewport{{}, m_imageSize};
    GL::Framebuffer fb{viewport};

    GL::Texture2D texCoordsTexture;
    setupTexture(texCoordsTexture, m_imageSize, GL::TextureFormat::RGBA32F);

    fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, texCoordsTexture, 0)
      .mapForDraw(GL::Framebuffer::ColorAttachment{0})
      .bind();

    CORRADE_INTERNAL_ASSERT(
            fb.checkStatus(GL::FramebufferTarget::Draw) ==
            GL::Framebuffer::Status::Complete);


    Vector3ui wgCount1(m_imageSize.x(), m_imageSize.y(), 1);
    Vector3ui wgCount2(m_textureSize.x(), m_textureSize.y(), 1);

    const auto map = DebugTools::ColorMap::turbo();
    const Vector2i mapSize{Int(map.size()), 1};

    GL::Texture2D colorMapTexture;
    colorMapTexture
            .setMinificationFilter(SamplerFilter::Linear)
            .setMagnificationFilter(SamplerFilter::Linear)
            .setWrapping(SamplerWrapping::ClampToEdge) // or Repeat
            .setStorage(1, GL::TextureFormat::RGB8, mapSize) // or SRGB8
            .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, mapSize, map});

    fb.clearColor(0, Color4{});

    for(auto& kf : kfs){

        fb.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
        m_texCoordsShader.setTransformationProjectionMatrix(kf.projection * kf.pose.inverted())
                         .draw(m_mesh);

        m_remap.bindTextureR(m_red)
               .bindTextureG(m_green)
               .bindTextureB(m_blue)
               .bindTextureA(m_count)
               .bindImage(kf.image)
               .bindTextureCoordinates(texCoordsTexture)
               .dispatchCompute(wgCount1);
    }

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    m_combine.bindTextureR(m_red)
             .bindTextureG(m_green)
             .bindTextureB(m_blue)
             .bindTextureA(m_count)
             .bindRgbaImage(m_texture)
             .dispatchCompute(wgCount2);

    //GL::Renderer::setMemoryBarrier(Mg::GL::Renderer::MemoryBarrier::TextureFetch);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
