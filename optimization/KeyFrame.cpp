//
// Created by janos on 7/22/20.
//

#include "KeyFrame.h"

#include <Magnum/Math/Quaternion.h>

using namespace Magnum;
using namespace Corrade;

namespace TextureMapOptimization {

void KeyFrame::compressPose() {
    Quaternion q = Quaternion::fromMatrix(pose.rotation());
    Deg angle = q.angle();
    auto axis = q.axis();
    Vector3 rotation = axis*float(angle);
    Vector3 translation = pose.translation();
    for(int i = 0; i < 3; ++i) {
        pose6D[i] = rotation[i];
        pose6D[i + 3] = translation[i];
    }
}

void KeyFrame::uncompressPose() {
    Vector3 rotation;
    Vector3 translation;
    for(int i = 0; i < 3; ++i) {
        rotation[i] = pose6D[i];
        translation[i] = pose6D[i + 3];
    }
    pose = Matrix4::translation(translation)*Matrix4::rotation(Deg{rotation.length()}, rotation.normalized());
}

}