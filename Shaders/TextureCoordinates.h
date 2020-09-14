//
// Created by janos on 7/2/20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

namespace Mg = Magnum;

namespace TextureMapOptimization::Shaders{

class TextureCoordinates : public Mg::GL::AbstractShaderProgram {
public:

    enum: Mg::UnsignedInt {
        ColorOutput = Mg::Shaders::Generic<3>::ColorOutput,
    };

    explicit TextureCoordinates();
    explicit TextureCoordinates(Mg::NoCreateT) : Mg::GL::AbstractShaderProgram{Mg::NoCreate} {};

    TextureCoordinates& setTransformationProjectionMatrix(Mg::Matrix4 const&);

private:

    Mg::Int m_transformationProjectionMatrix;
    Mg::Int m_numPrimitives;
};

}


