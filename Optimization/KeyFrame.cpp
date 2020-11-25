//
// Created by janos on 7/22/20.
//

#include "KeyFrame.h"

#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Matrix3.h>

using namespace Magnum;
using namespace Corrade;

namespace TextureMapOptimization {

void KeyFrame::compressPose() {
    Matrix3 rot{pose[0].xyz(), pose[1].xyz(), pose[2].xyz()};
    Quaternion q = Quaternion::fromMatrix(rot);
    Rad angle = q.angle();
    auto axis = q.axis();
    CORRADE_ASSERT(angle <= Rad{2.f*M_PI} && angle >= Rad{0}, "angle negative",);
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
    Debug{} << rotation;
    pose = Matrix4::translation(translation)*Matrix4::rotation(Rad{rotation.length()}, rotation.normalized());
}

}