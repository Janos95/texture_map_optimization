#include "background_shader.hpp"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

using namespace Magnum;


BackgroundShader::BackgroundShader(){

    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    const Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("background_shader.vert"));
    frag.addSource(rs.get("background_shader.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(Mg::GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    projectionMatrixUniform = uniformLocation("projectionMatrix");
    transformationMatrixUniform = uniformLocation("transformationMatrix");

}
