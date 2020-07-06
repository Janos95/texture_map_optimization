//
// Created by janos on 7/5/20.
//

#ifndef TEXTUREOPTIMIZATION_COMBINE_H
#define TEXTUREOPTIMIZATION_COMBINE_H

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

namespace Shaders {

class Combine : public Magnum::GL::AbstractShaderProgram {
public:

    enum class ImageUnit : Magnum::UnsignedInt {
        Image = 0,
        TextureCoordinate = 1,
        Texture = 2
    };

    explicit Combine();

    explicit Combine(Magnum::NoCreateT) : Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {};

    Combine& bindTextureR(Magnum::GL::Texture2D&);
    Combine& bindTextureG(Magnum::GL::Texture2D&);
    Combine& bindTextureB(Magnum::GL::Texture2D&);
    Combine& bindTextureA(Magnum::GL::Texture2D&);
    Combine& bindRgbaImage(Magnum::GL::Texture2D&);

private:

    Magnum::UnsignedInt m_textureRUnit = 0;
    Magnum::UnsignedInt m_textureGUnit = 1;
    Magnum::UnsignedInt m_textureBUnit = 2;
    Magnum::UnsignedInt m_textureAUnit = 3;
    Magnum::UnsignedInt m_textureRgbaUnit = 4;
};

}

#endif //TEXTUREOPTIMIZATION_COMBINE_H
