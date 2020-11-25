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

    explicit Reduction(Mg::UnsignedInt);

    explicit Reduction(Magnum::NoCreateT) : Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {};

    Reduction& bindRotationInImage(Magnum::GL::Texture2DArray&, int level);
    Reduction& bindTranslationInImage(Magnum::GL::Texture2DArray&, int level);
    Reduction& bindCostInImage(Magnum::GL::Texture2DArray&, int level);

    Reduction& bindRotationOutImage(Magnum::GL::Texture2DArray&, int level);
    Reduction& bindTranslationOutImage(Magnum::GL::Texture2DArray&, int level);
    Reduction& bindCostOutImage(Magnum::GL::Texture2DArray&, int level);

};

}

#endif //TEXTUREOPTIMIZATION_REDUCTION_H
