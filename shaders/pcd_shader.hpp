//
// Created by janos on 10.01.20.
//


#pragma once


#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Color.h>
#include "Magnum/Shaders/Generic.h"

using namespace Magnum;

class PointCloudShader: public GL::AbstractShaderProgram {
public:

    using Position = Shaders::Generic3D::Position;
    using Color3 = Shaders::Generic3D::Color3;
    using Color4 = Shaders::Generic3D::Color4;

    enum: UnsignedInt {
        /**
         * Color shader output. @ref shaders-generic "Generic output",
         * present always. Expects three- or four-component floating-point
         * or normalized buffer attachment.
         */
                ColorOutput = Shaders::Generic<3>::ColorOutput
    };

    explicit PointCloudShader();

    PointCloudShader& setPointSize(const Float pointSize);
    PointCloudShader& setTransformationProjectionMatrix(const MatrixTypeFor<3, Float>& matrix);

private:
    Int m_transformationProjectionMatrixUniform{0};
    Int m_pointSizeUniform{0};
};
