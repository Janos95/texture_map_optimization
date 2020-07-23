//
// Created by janos on 7/5/20.
//

#include "Combine.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/ImageFormat.h>
#include <Magnum/Math/Matrix4.h>

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

using namespace Magnum;

namespace shaders{

Combine::Combine() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL460);

    Utility::Resource rs{"tmo-data"};

    GL::Shader comp{GL::Version::GL460, GL::Shader::Type::Compute};

    comp.addSource(rs.get("Combine.comp"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

    attachShaders({comp});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

Combine& Combine::bindTextureR(GL::Texture2D& texture){
    texture.bindImage(m_textureRUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::R32F);
    return *this;
}

Combine& Combine::bindTextureG(GL::Texture2D& texture){
    texture.bindImage(m_textureGUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::R32F);
    return *this;
}

Combine& Combine::bindTextureB(GL::Texture2D& texture){
    texture.bindImage(m_textureBUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::R32F);
    return *this;
}

Combine& Combine::bindTextureA(GL::Texture2D& texture){
    texture.bindImage(m_textureAUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::R32F);
    return *this;
}

Combine& Combine::bindRgbaImage(GL::Texture2D& rgba){
    rgba.bindImage(m_textureRgbaUnit, 0, GL::ImageAccess::WriteOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

}
