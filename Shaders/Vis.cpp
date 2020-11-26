//
// Created by janos on 05.02.20.
//

#include "Vis.h"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/ImageFormat.h>
#include <Magnum/GL/Texture.h>

namespace Cr = Corrade;

namespace TextureMapOptimization::Shaders {

Vis::Vis() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    Cr::Utility::Resource rs{"tmo-data"};

    GL::Shader comp{GL::Version::GL460, GL::Shader::Type::Compute};

    comp.addSource(rs.get("Vis.comp"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

    attachShaders({comp});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    m_scaleUniform = uniformLocation("scale");
    m_offsetUniform = uniformLocation("offset");
    m_useCostUniform = uniformLocation("useCost");
}

Vis& Vis::bindColorMap(GL::Texture2D& texture) {
    texture.bind(ColorMapUnit);
    return *this;
}

Vis& Vis::bindOutputImage(GL::Texture2D& texture) {
    texture.bindImage(OutputImageUnit, 0, GL::ImageAccess::WriteOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

Vis& Vis::bindCostImage(GL::Texture2D& texture) {
    texture.bindImage(CostImageUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::RG32F);
    setUniform(m_useCostUniform, true);
    return *this;
}

Vis& Vis::bindGradientImage(GL::Texture2D& texture) {
    texture.bindImage(GradientImageUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::RGBA32F);
    setUniform(m_useCostUniform, false);
    return *this;
}

Vis& Vis::setColorMapTransform(float scale, float offset) {
    setUniform(m_scaleUniform, scale);
    setUniform(m_offsetUniform, offset);
}

}
