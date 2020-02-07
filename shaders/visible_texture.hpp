//
// Created by janos on 05.02.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Matrix4.h>

using namespace Magnum;

class VisibleTextureShader : public GL::AbstractShaderProgram
{
public:

    using Position = Shaders::Generic3D::Position;
    using TextureCoordinates = Shaders::Generic3D::TextureCoordinates;

    VisibleTextureShader();
    VisibleTextureShader& setTransformationProjectionMatrix(const Matrix4& matrix) {
        setUniform(m_transformationProjectionMatrixUniform, matrix);
        return *this;
    }

private:
    Int m_transformationProjectionMatrixUniform;
};
