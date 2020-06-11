//
// Created by janos on 05.02.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum//DimensionTraits.h>

using namespace Magnum;

class DiffShader : public GL::AbstractShaderProgram
{
public:

    using Position = Shaders::Generic3D::Position;
    using TextureCoordinates = Shaders::Generic3D::TextureCoordinates;

    DiffShader();

    DiffShader& setRvec(const Vector3&);
    DiffShader& setTvec(const Vector3&);
    DiffShader& setProjectionMatrix(float fx, float fy, float cx, float cy, float width, float height){ return *this;}
    DiffShader& bindImageGradientX(GL::Texture2D& imageGradientX){return *this;}
    DiffShader& bindImageGradientY(GL::Texture2D& imageGradientY){return *this;}
    DiffShader& setTextureSize(const VectorTypeFor<2, Int>& size);


private:
    Int m_rvecUniform;
    Int m_tvecUniform;
    Int m_textureSizeUniform;
};
