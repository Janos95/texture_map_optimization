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
Matrix4 projectionMatrixFromCameraParameters(float fx, float fy, float cx, float cy, float W, float H){

    // Source: https://blog.noctua-software.com/opencv-opengl-projection-matrix.html

    // far and near
    constexpr float f = 10.0f;
    constexpr float n = 0.01;

    const float L = -cx * n / fx;
    const float R = (W-cx) * n / fx;
    const float T = -cy * n / fy;
    const float B = (H-cy) * n / fy;

    // Caution, this is column-major
    // We perform an ugly hack here: We keep X and Y directions, but flip Z
    // with respect to the usual OpenGL conventions (in line with usual
    // computer vision practice). While we in fact keep a right-handed
    // coordinate system all the way, OpenGL expects a left-handed NDC
    // coordinate system. That affects triangle winding order
    // (see render_pass.cpp)
    Matrix4 P{
            {2.0f*n/(R-L),         0.0f,                   0.0f, 0.0f},
            {        0.0f, 2.0f*n/(B-T),                   0.0f, 0.0f},
            { (R+L)/(L-R),  (T+B)/(T-B),            (f+n)/(f-n), 1.0f},
            {        0.0f,         0.0f, (2.0f * f * n) / (n-f), 0.0f}
    };

    return P;
}
