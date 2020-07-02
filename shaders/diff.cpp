//
// Created by janos on 05.02.20.
//

#include "diff.h"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

using namespace Magnum;

namespace shaders {

DiffShader::DiffShader() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("visible_texture.vert"));

    frag.addSource(rs.get("visible_texture.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    m_rvecUniform = uniformLocation("rvec");
    m_rvecUniform = uniformLocation("tvec");
    m_textureSizeUniform = uniformLocation("textureSize");
}

DiffShader &DiffShader::setRvec(const Vector3 &rvec) {
    setUniform(m_rvecUniform, rvec);
    return *this;
}

DiffShader &DiffShader::setTvec(const Vector3 &tvec) {
    setUniform(m_tvecUniform, tvec);
    return *this;
}

DiffShader &DiffShader::setTextureSize(const VectorTypeFor<2, Int> &size) {
    setUniform(m_textureSizeUniform, size);
    return *this;
}

}
