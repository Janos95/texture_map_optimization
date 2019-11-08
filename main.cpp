

#include "viewer.h"
#include "vertex_colored_drawable.h"

#include <Magnum/Trade/Trade.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/MeshTools/Interleave.h>

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Containers/Optional.h>

#include <cassert>
#include <random>

using namespace Corrade;
using namespace Magnum;

using namespace Magnum::Math::Literals;


struct Vertex
{
    Vector3 position;
    Color3 color;
};


void addObject(Object3D& object, UnsignedInt i) {

}


int main(int argc, char** argv) {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> meshImporter = manager.loadAndInstantiate("AnySceneImporter");
    Containers::Pointer<Trade::AbstractImporter> imageImporter = manager.loadAndInstantiate("AnyImageImporter");

    if(!meshImporter)
        std::exit(1);

    if(!meshImporter->openFile("/home/jmeny/texture_map_optimization/tsdf_color_mapped.ply"))
        std::exit(4);

    assert(meshImporter->mesh3DCount() == 1);

    auto mesh = meshImporter->mesh3D(0);
    auto interleaved = MeshTools::interleave(mesh->positions(0), mesh->colors(0));

    //this creates the opengl context
    Viewer viewer({argc, argv});

    GL::Buffer vertices;
    vertices.setData(interleaved, GL::BufferUsage::StaticDraw);

    GL::Mesh glMesh;
    glMesh.addVertexBuffer(vertices, 0,
                         Shaders::VertexColor3D::Position{},
                         Shaders::VertexColor3D::Color3{})
           .setCount(vertices.size());

    viewer.addMesh(glMesh);
    viewer.exec();


    //    igl::lscm(
//    const Eigen::MatrixXd& V,
//    const Eigen::MatrixXi& F,
//    const Eigen::VectorXi& b,
//    const Eigen::MatrixXd& bc,
//    Eigen::MatrixXd & V_uv)

}