//
// Created by janos on 05.02.20.
//

#include "Diff.h"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

using namespace Magnum;

namespace TextureMapOptimization::Shaders {

Diff::Diff() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("Diff.vert"));

    frag.addSource(rs.get("Diff.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    m_rotationUniform = uniformLocation("rotation");
    m_translationUniform = uniformLocation("translation");
    m_projTfUniform = uniformLocation("projectionTransformationMatrix");
    m_fxUniform = uniformLocation("fx");
    m_fyUniform = uniformLocation("fy");
    m_cxUniform = uniformLocation("cx");
    m_cyUniform = uniformLocation("cy");
}

Diff& Diff::setRotation(Vector3 const& rotation) {
    setUniform(m_rotationUniform, rotation);
    return *this;
}

Diff& Diff::setTranslation(Vector3 const& translation) {
    setUniform(m_translationUniform, translation);
    return *this;
}

Diff& Diff::setCameraParameters(float fx, float fy, float cx, float cy) {
    setUniform(m_fxUniform, fx);
    setUniform(m_fyUniform, fy);
    setUniform(m_cxUniform, cx);
    setUniform(m_cyUniform, cy);
    return *this;
}

}
