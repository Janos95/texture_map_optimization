//
// Created by janos on 7/2/20.
//

#include "TextureCoordinates.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/Math/Matrix4.h>

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

using namespace Magnum;

namespace shaders{

TextureCoordinates::TextureCoordinates() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL460);

    Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL460, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL460, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("TextureCoordinates.vert"));
    frag.addSource(rs.get("TextureCoordinates.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    m_transformationProjectionMatrix = uniformLocation("transformationProjectionMatrix");
    m_numPrimitives = uniformLocation("numPrimitives");
}

TextureCoordinates& TextureCoordinates::setTransformationProjectionMatrix(Mg::Matrix4 const& tf){
    setUniform(m_transformationProjectionMatrix, tf);
    return *this;
}

}