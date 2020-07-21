//
// Created by janos on 7/22/20.
//

#ifndef TEXTUREOPTIMIZATION_KEYFRAME_H
#define TEXTUREOPTIMIZATION_KEYFRAME_H

#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>

namespace Mg = Magnum;

struct KeyFrame {
    Mg::GL::Texture2D image, derivativeX, derivativeY;
    Mg::Matrix4 pose;
};

#endif //TEXTUREOPTIMIZATION_KEYFRAME_H
