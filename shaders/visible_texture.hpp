//
// Created by janos on 05.02.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum//DimensionTraits.h>

using namespace Magnum;

class VisibleTextureShader : public GL::AbstractShaderProgram
{
public:

    using Position = Shaders::Generic3D::Position;
    using TextureCoordinates = Shaders::Generic3D::TextureCoordinates;

    VisibleTextureShader();

    VisibleTextureShader& setTransformationProjectionMatrix(const Matrix4& matrix);

    VisibleTextureShader& setTextureSize(const VectorTypeFor<2, Int>& size);


private:
    Int m_transformationProjectionMatrixUniform;
    Int m_textureSizeUniform;
};
