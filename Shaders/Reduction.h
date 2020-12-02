//
// Created by janos on 7/21/20.
//

#ifndef TEXTUREOPTIMIZATION_REDUCTION_H
#define TEXTUREOPTIMIZATION_REDUCTION_H

#include "ScopedTimer.h"
#include "Types.h"

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

#include <Magnum/GL/GL.h>
#include <Magnum/GL/AbstractShaderProgram.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

namespace TextureMapOptimization::Shaders {

class Reduction : public GL::AbstractShaderProgram {
public:

    enum Flag {
        MinMaxCost = 1,
        MinMaxRotation = 2,
        MinMaxTranslation = 3,
    };

    explicit Reduction(UnsignedInt step = 2);

    explicit Reduction(Mg::NoCreateT) : GL::AbstractShaderProgram{Mg::NoCreate} {};

    Reduction& setTextures(GL::Texture2D&, GL::Texture2D&, GL::Texture2D&);

    Reduction& reduce(Vector2& cost, Vector4& rot, Vector4& trans);

private:

    enum {
        CostInputUnit = 0,
        CostOutputUnit = 1,
        RotationInputUnit = 2,
        RotationOutputUnit = 3,
        TranslationInputUnit = 4,
        TranslationOutputUnit = 5,
    };


    GL::Texture2D* m_costTex, *m_rotTex, *m_transTex;
    UnsignedInt m_stepSize = 2;
};

}

#endif //TEXTUREOPTIMIZATION_REDUCTION_H
