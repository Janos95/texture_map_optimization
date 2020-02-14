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

    CoordsFilterShader& setTextureSize(const VectorTypeFor<2, Int>& size);

    /**
     * @brief set thresholding parameter in world units.

        Given the texture size and the threshold, each pixel in the depth buffer
        is checked whether in a 3x3 window two pixel differ by more than threshold.

       @param[in] proj Projection matrix used for computing the depth texture.
       @param[in] threshold threshold value in world units.
     */
    CoordsFilterShader& setThreshold(const Matrix4& proj, const float threshold);



private:
    Int m_texelHightOffsetUniform;
    Int m_texelWidthOffsetUniform;
    Int m_thresholdUniform;
};

