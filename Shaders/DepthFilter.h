//
// Created by janos on 7/6/20.
//

#pragma once

#include "Types.h"

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

namespace TextureMapOptimization::Shaders {

namespace Mg = Magnum;

class DepthFilter : public Magnum::GL::AbstractShaderProgram {
public:

    explicit DepthFilter(Mg::UnsignedInt);

    explicit DepthFilter(Mg::NoCreateT) : GL::AbstractShaderProgram{Magnum::NoCreate} {};

    DepthFilter& bindCosts(GL::Texture2D&);
    DepthFilter& bindRotationGradients(GL::Texture2D&);
    DepthFilter& bindTranslationGradients(GL::Texture2D&);

    DepthFilter& bindCostsAccumulated(GL::Texture2DArray&, UnsignedInt);
    DepthFilter& bindTranslationGradientsAccumulated(GL::Texture2DArray&, UnsignedInt);
    DepthFilter& bindRotationGradientsAccumulated(GL::Texture2DArray&, UnsignedInt);

    DepthFilter& bindDepth(GL::Texture2D&);

    DepthFilter& setDepthDiscontinuityThreshold(Mg::Float);

private:

    Mg::UnsignedInt m_costUnit = 0,
                    m_rotationUnit = 1,
                    m_translationUnit = 2,
                    m_costAccumulatedUnit = 3,
                    m_rotationAccumulatedUnit = 4,
                    m_translationAccumulatedUnit = 5,
                    m_depthUnit = 6;

    Mg::UnsignedInt m_thresholdUniform;
};

}

