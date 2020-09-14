//
// Created by janos on 7/22/20.
//

#pragma once

#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>
#include <Corrade/Containers/StaticArray.h>

namespace TextureMapOptimization {

namespace Mg = Magnum;
namespace Cr = Corrade;

struct KeyFrame {
    Mg::GL::Texture2D image;
    Mg::Matrix4 projection;

    Mg::Matrix4 pose;
    Cr::Containers::StaticArray<6, double> pose6D;

    void compressPose();

    void uncompressPose();
};

}

