

#include "Viewer.h"

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Vector3.h>

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Containers/Optional.h>


using namespace Corrade;
using namespace Magnum;

using namespace Magnum::Math::Literals;

Matrix3 primeSenseCameraMatrix()
{
    Matrix3 cameraMatrix(Math::IdentityInit);
    cameraMatrix[0][0] = 525.0;
    cameraMatrix[1][1] = 525.0;
    cameraMatrix[2][0] = 319.5;
    cameraMatrix[2][1] = 239.5;
    return cameraMatrix;
}

void rodriguezRot(Vector3d const& rod, Vector3d const& v, Vector3d& result, Matrix3d* jac){
    const auto x = rod.x(), y = rod.y(), z = rod.z();
    const auto x2 = x * x,  y2 = y * y, z2 = z*z;
    const auto theta2 = x2 + y2 + z2;
    const auto theta = sqrt(theta2);
    const auto k = rod / theta;
    const auto cosTheta = cos(theta);
    const auto sinTheta = sin(theta);
    const auto subexpr1 = (1. - cosTheta) / theta2;
    const auto subexpr2 = sinTheta / theta;

    result = v * cosTheta + Math::cross(k, v) * sinTheta + k * Math::dot(k, v) * (1 - cosTheta);
    /* jac is column major */
    if(!jac) return;
    *jac = {{x2 * subexpr1 + cosTheta      ,    x * y * subexpr1 + z * subexpr2 ,   -y * subexpr2 + x * z * subexpr1},
           {x * y * subexpr1 - z * subexpr2,    y2 * subexpr1 + cosTheta        ,   x * subexpr2 + y * z * subexpr1},
           {x * z * subexpr1 + y * subexpr2,    -x * subexpr2 + y * z * subexpr1,   z2 * subexpr1 + cosTheta}};
}


int main(int argc, char** argv){
    TextureMapOptimization::Viewer viewer{{argc, argv}};

    while(viewer.mainLoopIteration()){
        if(viewer.isOptimizing){
            viewer.startOptimization();
            viewer.isOptimizing = false;
        }
    }
}

