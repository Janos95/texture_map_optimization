//
// Created by janos on 05.02.20.
//

#pragma once

#include "Types.h"

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/Texture.h>

namespace Mg = Magnum;

namespace TextureMapOptimization::Shaders {

class Diff : public GL::AbstractShaderProgram {
public:

    using Position = Mg::Shaders::Generic3D::Position;
    using TextureCoordinates = Mg::Shaders::Generic3D::TextureCoordinates;
    using TransformationMatrix = Mg::Shaders::Generic3D::TransformationMatrix;

    Diff();

    Diff& setRotation(const Mg::Vector3& rot);

    Diff& setCameraParameters(float, float, float, float);

    Diff& setTranslation(Mg::Vector3 const& translation);

    Diff& setProjectionTransformationMatrix(Mg::Matrix4 const& projTf) {
        setUniform(m_projTfUniform, projTf);
        return *this;
    }

    Diff& bindGroundTruthTexture(GL::Texture2D& texture) {
        texture.bind(0);
        return *this;
    }

    Diff& bindOptimizationTexture(GL::Texture2D& texture) {
        texture.bind(1);
        return *this;
    }

private:
    Int m_rotationUniform,
        m_translationUniform,
        m_projTfUniform,
        m_fxUniform,
        m_fyUniform,
        m_cxUniform,
        m_cyUniform;
};

}
