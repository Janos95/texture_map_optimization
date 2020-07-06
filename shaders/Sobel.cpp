//
// Created by janos on 7/6/20.
//

#include "Sobel.h"

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

Sobel::Sobel() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL460);

    Utility::Resource rs{"tmo-data"};

    GL::Shader comp{GL::Version::GL460, GL::Shader::Type::Compute};

    comp.addSource(rs.get("Sobel.comp"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

    attachShaders({comp});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

Sobel& Sobel::bindImage(GL::Texture2D& texture){
    texture.bindImage(m_imageUnit, 0, GL::ImageAccess::ReadOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

Sobel& Sobel::bindDerivativeX(GL::Texture2D& texture){
    texture.bindImage(m_derivativeXUnit, 0, GL::ImageAccess::WriteOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

Sobel& Sobel::bindDerivativeY(GL::Texture2D& texture){
    texture.bindImage(m_derivativeYUnit, 0, GL::ImageAccess::WriteOnly, GL::ImageFormat::RGBA32F);
    return *this;
}

}
