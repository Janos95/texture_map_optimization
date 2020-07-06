//
// Created by janos on 7/4/20.
//

#include "ScreenTriangle.h"

#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/Reference.h>

#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>

using namespace Magnum;
using namespace Corrade;

namespace Shaders {

ScreenTriangle::ScreenTriangle() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    const Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("ScreenTriangle.vert"));

    frag.addSource(rs.get("ScreenTriangle.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

ScreenTriangle& ScreenTriangle::bindTexture(GL::Texture2D& texture) {
    texture.bind(0);
    return *this;
}

}