//
// Created by janos on 02.02.20.
//

#include "scene.hpp"

using namespace Magnum::Math::Literals;

Scene::Scene(): m_cameraObject(&m_scene), m_camera(m_cameraObject){
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


