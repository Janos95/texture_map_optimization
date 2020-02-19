//
// Created by janos on 02.02.20.
//

#include "scene.hpp"

#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/MeshVisualizer.h>

using namespace Magnum::Math::Literals;

Scene::Scene()
{
    m_shaders.emplace("flat_textured", new Shaders::Flat3D{Shaders::Flat3D::Flag::Textured});
    m_shaders.emplace("flat", new Shaders::Flat3D{});
    m_shaders.emplace("vertex_colored", new Shaders::VertexColor3D{});
    m_shaders.emplace("phong", new Shaders::Phong{});
    m_shaders.emplace("mesh_vis", new Shaders::MeshVisualizer{});
}

bool Scene::addObject(
        std::string name,
        const Trade::MeshData3D& meshdata,
        const Image2D* image){
    auto mesh = MeshTools::compile(meshdata);

    Containers::Optional<GL::Texture2D> texture = Containers::NullOpt;

    if(image)
    {
        texture = GL::Texture2D{};
        texture->setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setMinificationFilter(GL::SamplerFilter::Linear)
                .setStorage(1, GL::textureFormat(image->format()), image->size())
                .setSubImage(0, {}, *image);
    }

    Object object{
            std::move(mesh),
            std::move(texture),
            Color4::cyan()};

    auto [it, inserted] = m_objects.emplace(std::move(name), std::move(object));
    if(!inserted)
        return false;

    auto& obj = it->second;
    auto& flat = texture ? m_shaders["flat_textured"] : m_shaders["flat"];
    SceneGraphNode::callback_type cb = DefaultCallback(obj, dynamic_cast<Shaders::Flat3D&>(*flat));
    SceneGraphNode::callback_type cb2 = [](const Matrix4&, const Camera3D&){ };
    obj.node = new SceneGraphNode(&m_scene, cb2, &m_drawableGroup); //ownership is taking by parent node
    return true;
}




