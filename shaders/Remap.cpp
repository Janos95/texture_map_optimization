//
// Created by janos on 7/5/20.
//

#include "Remap.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/ImageFormat.h>
#include <Magnum/Math/Matrix4.h>

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

using namespace Magnum;

namespace Shaders{

Remap::Remap() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL460);

    Utility::Resource rs{"tmo-data"};

    GL::Shader comp{GL::Version::GL460, GL::Shader::Type::Compute};


    comp.addSource("#extension GL_NV_shader_atomic_float : require\n")
        .addSource(rs.get("Remap.comp"));

    for(auto& line : comp.sources()){
        Debug{} << line.c_str();
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

    attachShaders({comp});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}


Remap& Remap::bindImage(GL::Texture2D& image) {
    image.bindImage(m_imageUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

Remap& Remap::bindTextureCoordinates(GL::Texture2D& texCoords){
    texCoords.bindImage(m_texCoordsUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

Remap& Remap::bindTextureR(GL::Texture2D& texture){
    texture.bindImage(m_textureRUnit, 0, GL::ImageAccess::ReadWrite, GL::ImageFormat::R32F);
    return *this;
}

Remap& Remap::bindTextureG(GL::Texture2D& texture){
    texture.bindImage(m_textureGUnit, 0, GL::ImageAccess::ReadWrite, GL::ImageFormat::R32F);
    return *this;
}

Remap& Remap::bindTextureB(GL::Texture2D& texture){
    texture.bindImage(m_textureBUnit, 0, GL::ImageAccess::ReadWrite, GL::ImageFormat::R32F);
    return *this;
}

Remap& Remap::bindTextureA(GL::Texture2D& texture){
    texture.bindImage(m_textureAUnit, 0, GL::ImageAccess::ReadWrite, GL::ImageFormat::R32F);
    return *this;
}

Remap& Remap::bindTextureRgba(Magnum::GL::Texture2D& rgba){
    rgba.bindImage(6, 0, GL::ImageAccess::ReadWrite, GL::ImageFormat::RGBA32F);
    return *this;
}

}
