//
// Created by janos on 7/2/20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

namespace Mg = Magnum;

namespace shaders{

class AverageColor : public Mg::GL::AbstractShaderProgram {
public:

    enum: Mg::UnsignedInt {
        ColorOutput = Mg::Shaders::Generic<3>::ColorOutput,
    };

    explicit AverageColor();
    explicit AverageColor(Mg::NoCreateT) : Mg::GL::AbstractShaderProgram{Mg::NoCreate} {};

    AverageColor& setProjectionMatrix(Mg::Matrix4 const&);
    AverageColor& setTransformationMatrix(Mg::Matrix4 const&);

    AverageColor& bindColors(Mg::GL::Texture2D&);
    AverageColor& bindImage(Mg::GL::Texture2D&);

private:

    Mg::Int m_colorTextureUnit = 0;
    Mg::Int m_imageUnit = 1;
    Mg::Int m_projectionMatrixUniform;
    Mg::Int m_transformationMatrixUniform;
};

}


