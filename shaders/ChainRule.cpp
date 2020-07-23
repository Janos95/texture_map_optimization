//
// Created by janos on 7/22/20.
//

#include "ChainRule.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/ImageFormat.h>
#include <Magnum/Math/Matrix4.h>

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

using namespace Magnum;

namespace shaders {

ChainRule::ChainRule() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL460);

    Utility::Resource rs{"tmo-data"};

    GL::Shader comp{GL::Version::GL460, GL::Shader::Type::Compute};

    comp.addSource(rs.get("ChainRule.comp"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

    attachShaders({comp});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

ChainRule& ChainRule::bindDepth(GL::Texture2D& texture) {
    texture.bindImage(DepthUnit, 0, GL::ImageAccess::ReadOnly,
                      GL::ImageFormat::R32F);
    return *this;
}

ChainRule& ChainRule::bindCost(GL::Texture2D& texture) {
    texture.bindImage(CostUnit, 0, GL::ImageAccess::ReadOnly,
                      GL::ImageFormat::RG32F);
    return *this;
}

ChainRule& ChainRule::bindGradRotation(GL::Texture2D& texture) {
    texture.bindImage(GradRotUnit, 0, GL::ImageAccess::ReadOnly,
                      GL::ImageFormat::RGBA32F);
    return *this;
}

ChainRule& ChainRule::bindGradTranslation(GL::Texture2D& texture) {
    texture.bindImage(GradTransUnit, 0, GL::ImageAccess::ReadOnly,
                      GL::ImageFormat::RGBA32F);
    return *this;
}

ChainRule& ChainRule::bindRenderedImage(GL::Texture2D& texture) {
    texture.bindImage(RenderedImageUnit, 0, GL::ImageAccess::WriteOnly,
                   GL::ImageFormat::RGBA32F);
    return *this;
}

ChainRule& ChainRule::bindGroundTruthImage(GL::Texture2D& texture) {
    texture.bindImage(GroundTruthUnit, 0, GL::ImageAccess::WriteOnly,
                      GL::ImageFormat::RGBA32F);
    return *this;
}

ChainRule& ChainRule::bindJacobian(GL::Texture3D& jac) {
    jac.bindImageLayered(JacobiansUnit, 0, GL::ImageAccess::WriteOnly,
                   GL::ImageFormat::RG32F);
    return *this;
}

}