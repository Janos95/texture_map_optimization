//
// Created by janos on 05.02.20.
//

#pragma once

#include "Types.h"

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Math.h>
#include <Magnum/GL/GL.h>

namespace Mg = Magnum;

namespace TextureMapOptimization::Shaders {

class Diff : public GL::AbstractShaderProgram {
public:

    Diff();

    Diff& setRotation(const Mg::Vector3& rot);

    Diff& setCameraParameters(float fx, float fy, float cx, float cy);

    Diff& setTranslation(Mg::Vector3 const& translation);

    Diff& setProjectionTransformationMatrix(Mg::Matrix4 const& projTf);

    Diff& bindGroundTruthTexture(GL::Texture2D& texture);

    Diff& bindOptimizationTexture(GL::Texture2D& texture);

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
