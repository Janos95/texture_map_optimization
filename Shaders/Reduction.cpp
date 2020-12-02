//
// Created by janos on 7/21/20.
//

#include "Reduction.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/ImageFormat.h>

#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

namespace TextureMapOptimization::Shaders {

using namespace Magnum;
using namespace Corrade;

Reduction::Reduction(UnsignedInt stepSize) {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL460);

    Utility::Resource rs{"tmo-data"};

    GL::Shader comp{GL::Version::GL460, GL::Shader::Type::Compute};

    //switch(flag) {
    //    case Flag::MinMaxCost :
    //        comp.addSource("#define COMPUTE_MIN_MAX_COST\n");
    //        break;
    //    case Flag::MinMaxRotation :
    //        comp.addSource("#define COMPUTE_MIN_MAX_ROTATION\n");
    //        break;
    //    case Flag::MinMaxTranslation :
    //        comp.addSource("#define COMPUTE_MIN_MAX_ROTATION\n");
    //        break;
    //    default : break;
    //}

    comp.addSource(Utility::formatString("#define BLOCK_SIZE {}\n", 1u << stepSize))
        .addSource(rs.get("Reduction.comp"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

    attachShaders({comp});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

Reduction& Reduction::reduce(Vector2& cost, Vector4& rot, Vector4& trans) {

    auto& costTex = *m_costTex;
    auto& rotTex = *m_rotTex;
    auto& transTex = *m_transTex;

    size_t currentMipLevel = 0;
    constexpr size_t maxPixelCount = 100;

    while(costTex.imageSize(currentMipLevel).product() > maxPixelCount) {
        costTex.bindImage(CostInputUnit, currentMipLevel, GL::ImageAccess::ReadOnly, GL::ImageFormat::RG32F);
        costTex.bindImage(CostOutputUnit, currentMipLevel+m_stepSize, GL::ImageAccess::WriteOnly, GL::ImageFormat::RG32F);

        rotTex.bindImage(RotationInputUnit, currentMipLevel, GL::ImageAccess::ReadOnly, GL::ImageFormat::RGBA32F);
        rotTex.bindImage(RotationOutputUnit, currentMipLevel+m_stepSize, GL::ImageAccess::WriteOnly, GL::ImageFormat::RGBA32F);

        transTex.bindImage(TranslationInputUnit, currentMipLevel, GL::ImageAccess::ReadOnly, GL::ImageFormat::RGBA32F);
        transTex.bindImage(TranslationOutputUnit, currentMipLevel+m_stepSize, GL::ImageAccess::WriteOnly, GL::ImageFormat::RGBA32F);

        Vector2ui size{costTex.imageSize(currentMipLevel)};
        Vector3ui wgCount{size.x(), size.y(), 1};
        dispatchCompute(wgCount);

        currentMipLevel += m_stepSize;
    }

    return *this;
}


Reduction& Reduction::setTextures(GL::Texture2D& cost, GL::Texture2D& rot, GL::Texture2D& trans) {
    m_costTex = &cost;
    m_rotTex = &rot;
    m_transTex = &trans;
    return *this;
}

}
