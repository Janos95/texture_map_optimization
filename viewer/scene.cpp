//
// Created by janos on 02.02.20.
//

#include "scene.hpp"

using namespace Magnum::Math::Literals;

Scene::Scene(): m_cameraObject(&m_scene), m_camera(m_cameraObject){
}

bool Scene::addObject(
        std::string name,
        const Trade::MeshData3D& meshdata,
        const ImageView2D* image){
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
    obj.node = new SceneGraphNode(&m_scene, DefaultCallback(obj), &m_drawableGroup); //ownership is taking by parent node
    return true;
}

const Object3D& Scene::cameraObject() const {
    return m_cameraObject;
}

Object3D& Scene::cameraObject(){
    return m_cameraObject;
}

const SceneGraph::Camera3D& Scene::camera() const {
    return m_camera;
}

SceneGraph::Camera3D& Scene::camera(){
    return m_camera;
}


