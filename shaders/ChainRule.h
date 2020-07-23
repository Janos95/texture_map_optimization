//
// Created by janos on 7/22/20.
//

#ifndef TEXTUREOPTIMIZATION_CHAINRULE_H
#define TEXTUREOPTIMIZATION_CHAINRULE_H

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

namespace shaders {

class ChainRule : public Magnum::GL::AbstractShaderProgram {
public:
    enum : Magnum::UnsignedInt {
        RenderedImageUnit = 0,
        GroundTruthUnit = 1,
        JacobiansUnit = 2,
        DepthUnit = 3,
        GradRotUnit = 4,
        GradTransUnit = 5,
        CostUnit = 6
    };

    explicit ChainRule();

    explicit ChainRule(Magnum::NoCreateT) : Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {};

    ChainRule& bindJacobian(Magnum::GL::Texture3D&);
    ChainRule& bindRenderedImage(Magnum::GL::Texture2D&);
    ChainRule& bindGroundTruthImage(Magnum::GL::Texture2D&);
    ChainRule& bindDepth(Magnum::GL::Texture2D&);
    ChainRule& bindGradRotation(Magnum::GL::Texture2D&);
    ChainRule& bindGradTranslation(Magnum::GL::Texture2D&);
    ChainRule& bindCost(Magnum::GL::Texture2D&);

};

}

#endif //TEXTUREOPTIMIZATION_CHAINRULE_H
