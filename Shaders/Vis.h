//
// Created by janos on 05.02.20.
//

#pragma once

#include "Types.h"

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/GL.h>

namespace Mg = Magnum;

namespace TextureMapOptimization::Shaders {

class Vis : public GL::AbstractShaderProgram {
public:

    Vis();

    Vis& bindOutputImage(GL::Texture2D& texture);

    Vis& bindCostImage(GL::Texture2D& texture);

    Vis& bindGradientImage(GL::Texture2D& texture);

    Vis& bindColorMap(GL::Texture2D& texture);

    Vis& setColorMapTransform(float scale, float offset);

private:

    enum {
        ColorMapUnit = 0,
        OutputImageUnit = 1,
        CostImageUnit = 2,
        GradientImageUnit = 3,
    };

    Int m_scaleUniform,
        m_offsetUniform,
        m_useCostUniform;
};

}
