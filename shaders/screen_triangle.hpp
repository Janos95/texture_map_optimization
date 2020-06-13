//
// Created by janos on 13.06.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/GL/GL.h>

namespace Mg = Magnum;

namespace shader{

class ScreenTriangle: public Mg::GL::AbstractShaderProgram {
public:
    using Position = Mg::Shaders::Generic3D::Position;

    explicit ScreenTriangle();

    ScreenTriangle& bindTexture(Mg::GL::Texture2D& texture);

    void draw();
};

}
