//
// Created by janos on 7/21/20.
//

#ifndef TEXTUREOPTIMIZATION_REDUCTION_H
#define TEXTUREOPTIMIZATION_REDUCTION_H

#include <ScopedTimer/ScopedTimer.h>

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/GL/PixelFormat.h>

#include <Corrade/Containers/Array.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Renderer.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

namespace shaders {

class Reduction : public Magnum::GL::AbstractShaderProgram {
public:

    explicit Reduction(Mg::UnsignedInt);

    explicit Reduction(Magnum::NoCreateT) : Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {};

    Reduction& bindRotationInImage(Magnum::GL::Texture2D&, int level);
    Reduction& bindTranslationInImage(Magnum::GL::Texture2D&, int level);
    Reduction& bindCostInImage(Magnum::GL::Texture2D&, int level);

    Reduction& bindRotationOutImage(Magnum::GL::Texture2D&, int level);
    Reduction& bindTranslationOutImage(Magnum::GL::Texture2D&, int level);
    Reduction& bindCostOutImage(Magnum::GL::Texture2D&, int level);

};

}

namespace {
void setupMipMapTexture(Mg::GL::Texture2D& mip, Mg::Vector2i const& size, Mg::GL::TextureFormat format){
    mip = Mg::GL::Texture2D{};
    mip.setMagnificationFilter(Mg::GL::SamplerFilter::Linear)
         .setMinificationFilter(Mg::GL::SamplerFilter::Linear, Mg::GL::SamplerMipmap::Linear)
         .setWrapping(Mg::GL::SamplerWrapping::ClampToEdge)
         .setStorage(Mg::Math::log2(size.max()) + 1, format, size)
         .generateMipmap();
}
}


float reduce(Mg::GL::Texture2D& input) {
    static Mg::GL::Texture2D mipMapTexture;

    Mg::Vector2i size = input.imageSize(0);
    int level = 0;

    if(mipMapTexture.imageSize(0) != size){
        mipMapTexture = Mg::GL::Texture2D{};
        mipMapTexture.setMagnificationFilter(Mg::GL::SamplerFilter::Linear)
                     .setMinificationFilter(Mg::GL::SamplerFilter::Linear, Mg::GL::SamplerMipmap::Linear)
                     .setWrapping(Mg::GL::SamplerWrapping::ClampToEdge)
                     .setStorage(Mg::Math::log2(size.max()) + 1, Mg::GL::TextureFormat::R32F, input.imageSize(0))
                     .generateMipmap();
    }

    float sumCheck = 0;
    {
         ScopedTimer timer{"CPU Reduction", true};
         Cr::Containers::Array<char> dataCheck(size.product() * sizeof(float));
         Mg::MutableImageView2D viewCheck{Mg::GL::PixelFormat::Red, Mg::GL::PixelType::Float, size, dataCheck};
         input.image(level, viewCheck);
         for(float x : Cr::Containers::arrayCast<float>(dataCheck))
             sumCheck += x;
    }

    constexpr Mg::UnsignedInt stepSize = 4;
    constexpr Mg::UnsignedInt downloadThreshold = 10'000;
    static shaders::Reduction reductionShader{stepSize};

    float sum = 0;
    {
        ScopedTimer timer{"Compute Shader Reduction", true};
        while(size.product() > downloadThreshold){
            Mg::Vector2ui wg{mipMapTexture.imageSize(level + stepSize)};
            reductionShader.bindInput(level == 0 ? input : mipMapTexture, level)
                           .bindMipMapTexture(mipMapTexture, level + stepSize)
                           .dispatchCompute({wg.x(), wg.y(), 1});
            size = mipMapTexture.imageSize(level + stepSize);
            level += stepSize;
            Mg::GL::Renderer::setMemoryBarrier(Mg::GL::Renderer::MemoryBarrier::TextureFetch);
        }

        Cr::Containers::Array<char> data(size.product() * sizeof(float));
        Mg::MutableImageView2D view{Mg::GL::PixelFormat::Red, Mg::GL::PixelType::Float, size, data};
        mipMapTexture.image(level, view);

        for(float x : Cr::Containers::arrayCast<float>(data))
            sum += x;
    }

    CORRADE_ASSERT(Mg::Math::abs(sum - sumCheck) < 1e-7, "Sums are not equal", 0);

    return sum;
}




#endif //TEXTUREOPTIMIZATION_REDUCTION_H
