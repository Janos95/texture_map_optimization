

#include "viewer.hpp"
#include "load_poses.hpp"
#include "io.hpp"

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Vector3.h>

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Containers/Optional.h>

#include <ceres/ceres.h>

#include <cassert>
#include <random>
#include <algorithm>


using namespace Corrade;
using namespace Magnum;

using namespace Magnum::Math::Literals;


//Camera primesenseCamera()
//{
//    auto fx = 525.0f;
//    auto fy = 525.0f;
//    auto cx = 319.5f;
//    auto cy = 239.5f;
//    Matrix3 cameraMatrix =
//            {{fx, 0, 0}, {0, fy, 0}, {cx, cy, 0}};
//    auto proj = projectionMatrixFromCameraMatrix(cameraMatrix, 640.f, 480.f);
//    Camera camera{Matrix4{}, proj};
//    return camera;
//}

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

struct PhotometricConsistency : public ceres::CostFunction {

    bool Evaluate(double const* const* parameters,
                          double* residuals,
                          double** jacobians) const override {

        const double x = parameters[0][0];
        residuals[0] = 10 - x;

        // Compute the Jacobian if asked for.
        if (jacobians != nullptr && jacobians[0] != nullptr) {
            jacobians[0][0] = -1;
        }
        return true;
    }
};


using namespace ceres;

int main(int argc, char** argv) {

    Viewer viewer(argc, argv);
    viewer.exec();
}