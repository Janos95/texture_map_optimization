//
// Created by janos on 10.01.20.
//

#include "pcd_shader.hpp"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include "Magnum/Math/Matrix4.h"


PointCloudShader::PointCloudShader() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

    const Utility::Resource rs{"TextureMapOptimizationShaders"};
    const GL::Version version = GL::Version::GL450;

    GL::Shader vert{version, GL::Shader::Type::Vertex};
    GL::Shader frag{version, GL::Shader::Type::Fragment};
    vert.addSource(rs.get("pcd_shader.vert"));
    frag.addSource(rs.get("pcd_shader.frag"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    m_transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
    m_transformationProjectionMatrixUniform = uniformLocation("pointSize");
}



PointCloudShader& PointCloudShader::setTransformationProjectionMatrix(const MatrixTypeFor<3, Float>& matrix)
{
    setUniform(m_transformationProjectionMatrixUniform, matrix);
    return *this;
}


PointCloudShader& PointCloudShader::setPointSize(const Float pointSize)
{
    setUniform(m_pointSizeUniform, pointSize);
    return *this;
}
