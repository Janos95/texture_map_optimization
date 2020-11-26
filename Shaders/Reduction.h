//
// Created by janos on 7/21/20.
//

#ifndef TEXTUREOPTIMIZATION_REDUCTION_H
#define TEXTUREOPTIMIZATION_REDUCTION_H

#include "../ScopedTimer/ScopedTimer.h"

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/GL/PixelFormat.h>

#include <Corrade/Containers/Array.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Renderer.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

namespace TextureMapOptimization::Shaders {

class Reduction : public Magnum::GL::AbstractShaderProgram {
public:

    explicit Reduction(UnsignedInt step = 2);

    explicit Reduction(Magnum::NoCreateT) : Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {};

    Reduction& bindRotationImage(Magnum::GL::Texture2DArray&, int level);
    Reduction& bindTranslationImage(Magnum::GL::Texture2DArray&, int level);
    Reduction& bindCostImage(Magnum::GL::Texture2DArray&, int level);

private:
    UnsignedInt m_stepSize = 2;
};

}

#endif //TEXTUREOPTIMIZATION_REDUCTION_H
