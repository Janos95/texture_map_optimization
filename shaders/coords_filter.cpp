//
// Created by janos on 06.02.20.
//

#include "coords_filter.hpp"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

namespace {
    enum: Int { DepthTextureLayer = 0, CoordsTextureLayer = 1 };
}

CoordsFilterShader::CoordsFilterShader()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

    const Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

    vert//.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("coords_filter.vert"));

    frag.addSource(rs.get("coords_filter.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    m_thresholdUniform = uniformLocation("threshold");

    setUniform(uniformLocation("depthTexture"), DepthTextureLayer);
    setUniform(uniformLocation("coordsTexture"), CoordsTextureLayer);
}

CoordsFilterShader& CoordsFilterShader::bindDepthTexture(GL::Texture2D& depth){
    depth.bind(DepthTextureLayer);
    return *this;
}

CoordsFilterShader& CoordsFilterShader::bindCoordsTexture(GL::Texture2D& coords){
    coords.bind(CoordsTextureLayer);
    return *this;
}

CoordsFilterShader& CoordsFilterShader::setTextureSize(const VectorTypeFor<2, Int>& size){
    setUniform(m_texelHightOffsetUniform, 1.f / size[1]);
    setUniform(m_texelWidthOffsetUniform, 1.f / size[0]);
    return *this;
}

CoordsFilterShader &CoordsFilterShader::setThreshold(const Matrix4 &proj, const float threshold) {
    auto p = proj * Vector4(0,0,threshold, 1);
    auto ndc = p.xyz() / p.w();
    auto window = (ndc.z() + 1.f) / 2.;
    setUniform(m_thresholdUniform, window);
    return *this;
}
