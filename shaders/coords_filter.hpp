//
// Created by janos on 06.02.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/Texture.h>

using namespace Magnum;

class CoordsFilterShader: public GL::AbstractShaderProgram {
public:
    using Position = Shaders::Generic3D::Position;
    explicit CoordsFilterShader();
    CoordsFilterShader& setFilterSize(Vector2i size){

        return *this;
    }

    CoordsFilterShader& bindDepthTexture(GL::Texture2D& depth);

    CoordsFilterShader& bindCoordsTexture(GL::Texture2D& coords);

private:
    Int m_depthTextureUniform;
    Int m_coordsTextureUniform;
    Int m_texelHightOffsetUniform;
    Int m_texelWidthOffsetUniform;
};