//
// Created by janos on 7/6/20.
//

#ifndef TEXTUREOPTIMIZATION_SOBEL_H
#define TEXTUREOPTIMIZATION_SOBEL_H

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

namespace Shaders {

class Sobel : public Magnum::GL::AbstractShaderProgram {
public:

    explicit Sobel();

    explicit Sobel(Magnum::NoCreateT) : Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {};

    Sobel& bindImage(Magnum::GL::Texture2D&);
    Sobel& bindDerivativeX(Magnum::GL::Texture2D&);
    Sobel& bindDerivativeY(Magnum::GL::Texture2D&);

private:

    Magnum::UnsignedInt m_imageUnit = 0;
    Magnum::UnsignedInt m_derivativeXUnit = 1;
    Magnum::UnsignedInt m_derivativeYUnit = 2;

};

}

#endif //TEXTUREOPTIMIZATION_SOBEL_H
