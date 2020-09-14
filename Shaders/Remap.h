//
// Created by janos on 7/5/20.
//

#ifndef TEXTUREOPTIMIZATION_REMAP_H
#define TEXTUREOPTIMIZATION_REMAP_H

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

namespace TextureMapOptimization::Shaders {

class Remap : public Magnum::GL::AbstractShaderProgram {
public:

    enum class ImageUnit : Magnum::UnsignedInt {
        Image = 0,
        TextureCoordinate = 1,
        Texture = 2
    };

    explicit Remap();

    explicit Remap(Magnum::NoCreateT) : Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {};

    Remap& bindImage(Magnum::GL::Texture2D&);

    Remap& bindTextureCoordinates(Magnum::GL::Texture2D&);

    Remap& bindTextureR(Magnum::GL::Texture2D&);
    Remap& bindTextureG(Magnum::GL::Texture2D&);
    Remap& bindTextureB(Magnum::GL::Texture2D&);
    Remap& bindTextureA(Magnum::GL::Texture2D&);

    Remap& bindTextureRgba(Magnum::GL::Texture2D& rgba);

private:

    Magnum::UnsignedInt m_imageUnit = 0;
    Magnum::UnsignedInt m_texCoordsUnit = 1;
    Magnum::UnsignedInt m_textureRUnit = 2;
    Magnum::UnsignedInt m_textureGUnit = 3;
    Magnum::UnsignedInt m_textureBUnit = 4;
    Magnum::UnsignedInt m_textureAUnit = 5;
};

}

#endif //TEXTUREOPTIMIZATION_REMAP_H
