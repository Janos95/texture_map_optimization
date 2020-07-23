//
// Created by janos on 7/22/20.
//

#include "KeyFrame.h"

#include <Magnum/Math/Quaternion.h>

using namespace Magnum;
using namespace Corrade;

void KeyFrame::populateAngleAxis(){
    Quaternion q = Quaternion::fromMatrix(pose.rotation());
    Deg angle = q.angle();
    auto axis = q.axis();
    rotation = axis * float(angle);
    translation = pose.translation();
}

void KeyFrame::populatePose(){
    pose = Matrix4::translation(translation) * Matrix4::rotation(Deg{rotation.length()}, rotation.normalized());
}
