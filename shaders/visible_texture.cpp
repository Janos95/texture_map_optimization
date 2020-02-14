//
// Created by janos on 05.02.20.
//

#include "visible_texture.hpp"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

VisibleTextureShader::VisibleTextureShader() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

    Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

    vert//.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("visible_texture.vert"));

    frag.addSource(rs.get("visible_texture.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    m_transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
    m_textureSizeUniform = uniformLocation("textureSize");
}

VisibleTextureShader& VisibleTextureShader::setTransformationProjectionMatrix(const Matrix4& matrix) {
    setUniform(m_transformationProjectionMatrixUniform, matrix);
    return *this;
}

VisibleTextureShader& VisibleTextureShader::setTextureSize(const VectorTypeFor<2, Int>& size) {
    setUniform(m_textureSizeUniform, size);
    return *this;
}
