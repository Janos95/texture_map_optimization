//
// Created by janos on 08.11.19.
//

#pragma once

//#include "pcd_shader.hpp"
#include "smart_drawable.hpp"

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/VertexColor.h>

#include <map>
#include <memory>

using namespace Corrade;
using namespace Magnum;


using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;


class Viewer: public Platform::Application {
public:
    explicit Viewer(const Arguments& arguments);

    enum ShaderType{
        PCD,
        VERTEX_COLOR
    };

    SceneGraph::Drawable3D* addObject(GL::Mesh& object, ShaderType shader)
    {
        switch(shader){
            case PCD:
                return new SmartDrawable<Shaders::VertexColor3D>(m_manipulator, m_shader, object, m_drawables);
            case VERTEX_COLOR:
                return new SmartDrawable<Shaders::VertexColor3D>(m_manipulator, m_shader, object, m_drawables);
            default:
                return nullptr;
        }
    }


private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent& event) override;
    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    Vector3 positionOnSphere(const Vector2i& position) const;


    Scene3D m_scene;
    Object3D m_manipulator, m_cameraObject;
    SceneGraph::Camera3D* m_camera;
    SceneGraph::DrawableGroup3D m_drawables;
    Vector3 m_previousPosition;

    Shaders::VertexColor3D m_shader;
    std::tuple<Shaders::VertexColor3D, Shaders::VertexColor3D> m_shaders;
};