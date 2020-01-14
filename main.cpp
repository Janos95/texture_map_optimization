

#include "viewer.hpp"
#include "smart_drawable.hpp"
#include "glob.h"
#include "load_poses.hpp"

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Vector3.h>

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Containers/Optional.h>

//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>

#include <cassert>
#include <random>
#include <algorithm>
#include <iostream>

using namespace Corrade;
using namespace Magnum;

using namespace Magnum::Math::Literals;


Math::Matrix3<double> PrimeSenseDefault()
{
    Math::Matrix3<double> cameraMatrix(Math::IdentityInit);
    cameraMatrix[0][0] = 525.0;
    cameraMatrix[1][1] = 525.0;
    cameraMatrix[2][0] = 319.5;
    cameraMatrix[2][1] = 239.5;
    return cameraMatrix;
}



int main(int argc, char** argv) {

    auto poses = loadPoses<float>("/home/janos/texture_map_optimization/assets/fountain_small/scene/key.log");
    auto cameraMatrix = PrimeSenseDefault();

    Utility::Debug{} << cameraMatrix;


    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> meshImporter = manager.loadAndInstantiate("AnySceneImporter");

    if(!meshImporter)
        std::exit(1);

    if(!meshImporter->openFile("/home/janos/texture_map_optimization/assets/fountain_small/scene/integrated.ply"))
        std::exit(4);

    //auto imagePaths = glob("/home/janos/shots/simon/crane_part1/rgb", ".*\\.png");
    //std::sort(imagePaths.begin(), imagePaths.end());

    //std::vector<cv::Mat> imgs(imagePaths.size());

    //for (int i = 0; i < imgs.size(); ++i) {
    //    std::cout << imagePaths[i] << std::endl;
        //imgs[i] = cv::imread(imagePaths[i].string());
    //}

    assert(meshImporter->mesh3DCount() == 1);

    auto mesh = meshImporter->mesh3D(0);
 //   mesh->indices().clear();

    //this creates the opengl context
    Viewer viewer({argc, argv});

    auto glMesh = MeshTools::compile(*mesh);


    viewer.addObject(glMesh, Viewer::PCD);
    viewer.exec();

}