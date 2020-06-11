//
// Created by janos on 05.02.20.
//

#include "pbr_shader.hpp"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Corrade/Utility/FormatStl.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>

using namespace Magnum;

PbrShader::PbrShader(uint32_t lightCount) {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    const Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};

    std::string lightCountDefine = Utility::formatString("#define LIGHT_COUNT {}\n", lightCount);
    vert.addSource(lightCountDefine)
        .addSource(rs.get("pbr_shader.vert"));
    frag.addSource(lightCountDefine)
        .addSource(rs.get("pbr_shader.frag"));

//    for (auto& s : vert.sources()) {
//        Debug{} << s.c_str();
//    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(Mg::GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    albedoUniform = uniformLocation("albedo");
    metallicUniform = uniformLocation("metallic");
    roughnessUniform = uniformLocation("roughness");
    ambientOcclusionUniform = uniformLocation("ambientOcclusion");
    lightPositionUniform = uniformLocation("lightPositions");
    lightColorsUniform = uniformLocation("lightColors");
    projectionMatrixUniform = uniformLocation("projectionMatrix");
    normalMatrixUniform = uniformLocation("normalMatrix");
    transformationMatrixUniform = uniformLocation("transformationMatrix");

}