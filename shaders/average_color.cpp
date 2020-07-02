//
// Created by janos on 7/2/20.
//

#include "average_color.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/Math/Matrix4.h>

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

using namespace Magnum;

namespace shaders{

AverageColor::AverageColor() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    Utility::Resource rs{"tmo-data"};

    GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("average_color.vert"));
    frag.addSource(rs.get("average_color.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    m_projectionMatrixUniform = uniformLocation("projectionMatrix");
    m_transformationMatrixUniform = uniformLocation("transformationMatrix");
}


AverageColor& AverageColor::bindColors(Mg::GL::Texture2D& color){
    color.bind(m_colorTextureUnit);
    return *this;
}

AverageColor& AverageColor::bindImage(Mg::GL::Texture2D& image){
    image.bind(m_imageUnit);
    return *this;
}

AverageColor& AverageColor::setProjectionMatrix(Mg::Matrix4 const& proj){
    setUniform(m_projectionMatrixUniform, proj);
    return *this;
}
AverageColor& AverageColor::setTransformationMatrix(Mg::Matrix4 const& view){
    setUniform(m_transformationMatrixUniform, view);
    return *this;
}

}