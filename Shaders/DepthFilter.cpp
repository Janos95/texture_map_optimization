//
// Created by janos on 7/6/20.
//

#include "DepthFilter.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/TextureArray.h>
#include <Magnum/GL/ImageFormat.h>
#include <Magnum/Math/Matrix4.h>

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Corrade/Utility/FormatStl.h>

using namespace Magnum;

namespace TextureMapOptimization::Shaders {

DepthFilter::DepthFilter(UnsignedInt stepSize) {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL460);

    Utility::Resource rs{"tmo-data"};

    GL::Shader comp{GL::Version::GL460, GL::Shader::Type::Compute};

    comp.addSource(Utility::formatString("#define BLOCK_SIZE {}\n", 1u << stepSize))
        .addSource(rs.get("DepthFilter.comp"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

    attachShaders({comp});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

DepthFilter& DepthFilter::bindCosts(Magnum::GL::Texture2D& texture){
    texture.bindImage(m_costUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::R32F);
    return *this;
}

DepthFilter& DepthFilter::bindRotationGradients(Magnum::GL::Texture2D& texture){
    texture.bindImage(m_rotationUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

DepthFilter& DepthFilter::bindTranslationGradients(Magnum::GL::Texture2D& texture){
    texture.bindImage(m_translationUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

DepthFilter& DepthFilter::bindCostsAccumulated(Magnum::GL::Texture2DArray& texture, UnsignedInt layer){
    texture.bindImage(m_costAccumulatedUnit, 0, layer, GL::ImageAccess::WriteOnly, GL::ImageFormat::R32F);
    return *this;
}

DepthFilter& DepthFilter::bindTranslationGradientsAccumulated(Magnum::GL::Texture2DArray& texture, UnsignedInt layer){
    texture.bindImage(m_translationAccumulatedUnit, 0, layer, GL::ImageAccess::WriteOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

DepthFilter& DepthFilter::bindRotationGradientsAccumulated(Magnum::GL::Texture2DArray& texture, UnsignedInt layer){
    texture.bindImage(m_rotationAccumulatedUnit, 0, layer, GL::ImageAccess::WriteOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

DepthFilter& DepthFilter::bindDepth(Mg::GL::Texture2D& texture) {
    texture.bindImage(m_depthUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::R32F);
    return *this;
}

DepthFilter& DepthFilter::setDepthDiscontinuityThreshold(Mg::Float threshold) {
    setUniform(m_thresholdUniform, threshold);
    return *this;
}

}
