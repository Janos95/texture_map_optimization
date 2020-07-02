//
// Created by janos on 05.02.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum//DimensionTraits.h>

namespace Mg = Magnum;

namespace shaders {

class DiffShader : public Mg::GL::AbstractShaderProgram {
public:

    using Position = Mg::Shaders::Generic3D::Position;
    using TextureCoordinates = Mg::Shaders::Generic3D::TextureCoordinates;

    DiffShader();

    DiffShader &setRvec(const Mg::Vector3 &);

    DiffShader &setTvec(const Mg::Vector3 &);

    DiffShader & setProjectionMatrix(float fx, float fy, float cx, float cy, float width, float height) {
        return *this;
    }

    DiffShader &bindImageGradientX(Mg::GL::Texture2D &imageGradientX) { return *this; }

    DiffShader &bindImageGradientY(Mg::GL::Texture2D &imageGradientY) { return *this; }

    DiffShader &setTextureSize(Mg::VectorTypeFor<2, Mg::Int> const& size);


private:
    Mg::Int m_rvecUniform;
    Mg::Int m_tvecUniform;
    Mg::Int m_textureSizeUniform;
};

}
