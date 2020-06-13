//
// Created by janos on 13.06.20.
//

#include "screen_triangle.hpp"

#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/Reference.h>

#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Mesh.h>

using namespace Magnum;
using namespace Corrade;

namespace shader {

ScreenTriangle::ScreenTriangle() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    const Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("screen_triangle.vert"));

    frag.addSource(rs.get("screen_triangle.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

ScreenTriangle &ScreenTriangle::bindTexture(GL::Texture2D &texture) {
    texture.bind(0);
    return *this;
}

void ScreenTriangle::draw(){
    GL::AbstractShaderProgram::draw(GL::Mesh{}.setCount(3));
}

}