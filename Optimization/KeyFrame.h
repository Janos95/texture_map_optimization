//
// Created by janos on 7/22/20.
//

#pragma once

#include "Types.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>
#include <Corrade/Containers/StaticArray.h>

namespace TextureMapOptimization {

struct KeyFrame {
    GL::Texture2D image;

    Matrix4 pose;
    StaticArray<6, double> pose6D;

    void compressPose();

    void uncompressPose();
};

}

