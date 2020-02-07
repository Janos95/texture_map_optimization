

#include "viewer.hpp"
#include "smart_drawable.hpp"
#include "glob.h"
#include "load_poses.hpp"
#include "render_pass.hpp"
#include "camera.hpp"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/SmartHandles.hh>

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/PixelFormat.h>

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Containers/Optional.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <cassert>
#include <random>
#include <algorithm>
#include <iostream>
#include <array>

using namespace Corrade;
using namespace Magnum;

using namespace Magnum::Math::Literals;




Camera primesenseCamera()
{
    auto fx = 525.0f;
    auto fy = 525.0f;
    auto cx = 319.5f;
    auto cy = 239.5f;
    auto proj = projectionMatrixFromCameraParameters(525.f,525.f,319.5f, 239.5f, 640, 480);
    Camera camera{Matrix4{}, proj};
}

Matrix3 primeSenseCameraMatrix()
{
    Matrix3 cameraMatrix(Math::IdentityInit);
    cameraMatrix[0][0] = 525.0;
    cameraMatrix[1][1] = 525.0;
    cameraMatrix[2][0] = 319.5;
    cameraMatrix[2][1] = 239.5;
    return cameraMatrix;
}



Object generatePointCloud(const cv::Mat depth, const Matrix3& cm){
    std::vector<Vector3> points;
    for (int x = 0; x < depth.rows; ++x) {
        for (int y = 0; y < depth.cols; ++y) {
            auto z = depth.at<float>(y,x);
            if(z > 0){
                points.emplace_back(
                        (x - cm[2][0]) * z/cm[0][0],
                        (y - cm[2][1]) * z/cm[1][1],
                        z
                );
            }
        }
    }

    GL::Buffer buffer;
    buffer.setData(Containers::ArrayView<Vector3>(points.data(), points.size()));

    GL::Mesh pcd;
    pcd.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(buffer, 0, Shaders::Generic3D::Position{})
        .setCount(points.size());

    return Object{std::move(pcd), Containers::NullOpt, Color4::red()};
}


int main(int argc, char** argv) {

    auto poses = loadPoses<float>("/home/janos/texture_map_optimization/assets/fountain_small/scene/key.log");
    auto camera = primesenseCamera();
    camera.transformation = poses.front();

    Utility::Debug{} << camera.projection;

    Mesh mesh;
    mesh.request_vertex_texcoords2D();
    OpenMesh::IO::Options opt;
    opt += OpenMesh::IO::Options::VertexTexCoord;
    if(!OpenMesh::IO::read_mesh(
            mesh,
            "/home/janos/texture_map_optimization/assets/fountain_small/scene/integrated.obj",
            opt)){
        std::cout << "Could not load mesh" << std::endl;
        std::exit(1);
    }

    assert(mesh.has_vertex_texcoords2D());

    std::cout << mesh.n_faces() << std::endl;
    //auto imagePaths = glob("/home/janos/shots/simon/crane_part1/rgb", ".*\\.png");
    //std::sort(imagePaths.begin(), imagePaths.end());

    //std::vector<cv::Mat> imgs(imagePaths.size());

    //for (int i = 0; i < imgs.size(); ++i) {
    //    std::cout << imagePaths[i] << std::endl;
        //imgs[i] = cv::imread(imagePaths[i].string());
    //}

    //assert(meshImporter->mesh3DCount() == 1);

    //auto mesh = meshImporter->mesh3D(0);
 //   mesh->indices().clear();


    Containers::Array<char> data(512 * 512 * 4);
    cv::Mat img(512, 512, CV_8UC4, data.data());
    img = cv::Scalar(100, 100, 100);

//    std::random_device dev;
//    std::default_random_engine gen(dev());
//    std::uniform_int_distribution<uchar> dist;
//    for(auto it = mesh.faces_begin(); it != mesh.faces_end(); ++it)
//    {
//        cv::Scalar randomColor(dist(gen), dist(gen), dist(gen));
//        auto vs = it->vertices().to_array<3>();
//        std::vector<cv::Point> contour = {cv::Point(0,0), cv::Point(511, 255), cv::Point(255, 511)};
//        std::vector contours{contour};
//      //  for(int i = 0; i < 3; ++i){
//      //      auto tc = mesh.texcoords2D()[vs[i].idx()];
//      //      contour[i] = cv::Point(512 * tc[0], 512 * tc[1]);
//      //  }
//        cv::drawContours(img, contours, -1, randomColor, 2, cv::LINE_8);
//        break;
//    }

//
//    cv::imshow("window", img);
//    cv::waitKey(0);


    //this creates the opengl context
   // Scene scene;
   // Image2D image{PixelFormat::RGBA8Unorm, {512, 512}, std::move(data)};
   // scene.addObject(mesh, image);

   // DepthShader depthShader;
   // RenderTarget target({640, 480}, &depthShader);
   // RenderPass renderer;
   // renderer.bindRenderTarget(target);
   // renderer.draw(camera, scene);


   // cv::Mat depth(640, 480, CV_32F);
   // target.readFrameDepth(MutableImageView2D{PixelFormat::R32F, {640, 480}, depth.data});
   // scene.addObject(generatePointCloud(depth, primeSenseCameraMatrix()));

   // Viewer viewer({argc, argv}, scene);
   // viewer.exec();



}