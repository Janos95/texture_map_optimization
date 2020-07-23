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
    Mg::GL::Texture2D image;
    Mg::Matrix4 pose;
    Mg::Matrix4 projection;
    Mg::Vector3 rotation;
    Mg::Vector3 translation;

    void populateAngleAxis();
    void populatePose();
};

#endif //TEXTUREOPTIMIZATION_KEYFRAME_H
