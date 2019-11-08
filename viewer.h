//
// Created by janos on 08.11.19.
//

#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/VertexColor.h>


using namespace Corrade;
using namespace Magnum;


using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;

class Viewer: public Platform::Application {
public:
    explicit Viewer(const Arguments& arguments);

private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent& event) override;
    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    Vector3 positionOnSphere(const Vector2i& position) const;

    Shaders::VertexColor3D m_shader;

//    Shaders::Phong _coloredShader,
//            _texturedShader{Shaders::Phong::Flag::DiffuseTexture};
//    Containers::Array<Containers::Optional<GL::Mesh>> _meshes;
//    Containers::Array<Containers::Optional<GL::Texture2D>> _textures;

    Scene3D m_scene;
    Object3D m_manipulator, m_cameraObject;
    SceneGraph::Camera3D* m_camera;
    SceneGraph::DrawableGroup3D m_drawables;
    Vector3 m_previousPosition;
};