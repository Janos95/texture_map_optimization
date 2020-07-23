//
// Created by janos on 7/4/20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/GL/GL.h>

namespace shaders {

class ScreenTriangle : public Magnum::GL::AbstractShaderProgram {
public:
    using Position = Magnum::Shaders::Generic3D::Position;

    explicit ScreenTriangle();

    ScreenTriangle& bindTexture(Magnum::GL::Texture2D& texture);
};

}
