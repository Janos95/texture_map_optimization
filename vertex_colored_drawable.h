//
// Created by jmeny on 08.11.19.
//

#pragma once

#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/GL/Mesh.h>

using namespace Magnum;
using Object3d = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;

class VertexColoredDrawable: public SceneGraph::Drawable3D {
public:
    VertexColoredDrawable(Object3d& parent, Shaders::VertexColor3D& shader, GL::Mesh& mesh, SceneGraph::DrawableGroup3D& group):
        SceneGraph::Drawable3D(parent, &group),
        m_shader(shader),
        m_mesh(mesh)
        {
        }

private:

    void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

    Shaders::VertexColor3D& m_shader;
    GL::Mesh& m_mesh;
};
