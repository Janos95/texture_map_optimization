

#include <Magnum/Trade/Trade.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Shaders/VertexColor.h>


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


std::vector<Vertex> randomColor(const Trade::MeshData3D& mesh)
{
    std::random_device dev;
    std::default_random_engine gen(dev());
    std::uniform_real_distribution<float> distr(0,1);

    const auto& positions = mesh.positions(0);

    std::vector<Vertex> data(mesh.positions(0).size());
    for (int i = 0; i < data.size(); ++i) {
        data[i].position = positions[i];
        data[i].color.r() = distr(gen);
        data[i].color.g() = distr(gen);
        data[i].color.b() = distr(gen);
    }

    return data;
}

int main() {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> meshImporter = manager.loadAndInstantiate("AnySceneImporter");
    if(!meshImporter) std::exit(1);


    if(!meshImporter->openFile("/home/janos/CLionProjects/texture_map_optimization/bunny.ply"))
        std::exit(4);

    assert(meshImporter->mesh3DCount() == 1);

    auto mesh = meshImporter->mesh3D(0);
    auto data = randomColor(*mesh);

    GL::Buffer vertices;
    vertices.setData(data, GL::BufferUsage::StaticDraw);

    GL::Mesh glMesh;
    glMesh.addVertexBuffer(vertices, 0,
                         Shaders::VertexColor3D::Position{},
                         Shaders::VertexColor3D::Color3{});


    Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
    Matrix4 projectionMatrix =
            Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

    Shaders::VertexColor3D shader;
    shader.setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);


    glMesh.draw(shader);




    //    igl::lscm(
//    const Eigen::MatrixXd& V,
//    const Eigen::MatrixXi& F,
//    const Eigen::VectorXi& b,
//    const Eigen::MatrixXd& bc,
//    Eigen::MatrixXd & V_uv)

}