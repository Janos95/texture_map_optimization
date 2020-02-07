//author janos meny

#pragma once

#include "object.hpp"

#include <Magnum/Shaders/Shaders.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Mesh.h>

#include <functional>

using namespace Magnum;
using namespace Corrade;

using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;
using Drawable3D = SceneGraph::Drawable3D;


class DefaultCallback
{
public:
    explicit DefaultCallback(Object& object):
        m_mesh(object.mesh), m_texture(&object.texture), m_color(object.color)
    {
    }

    void operator()(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera){
        static Shaders::Flat3D shader;
        shader.setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix);

        if (m_texture) {
            shader.bindTexture(*m_texture);
        }
        else{
            shader.setColor(m_color);
        }

        m_mesh.draw(m_shader);
    }

private:
    GL::Mesh& m_mesh;
    GL::Texture2D* m_texture;
    Color4 m_color;
    Shaders::Flat3D m_shader;
};

class SceneGraphNode : public Drawable3D, Object3D {
public:

    using callback_type = std::function<void(const Matrix4&, SceneGraph::Camera3D&)>;

    explicit SceneGraphNode(Object3D* parent, callback_type callback, SceneGraph::DrawableGroup3D* group):
        Object3D(parent),
        Magnum::SceneGraph::Drawable3D{*this, group},
        m_callback(std::move(callback))
    {
    }

    void setDrawCallback(const callback_type& callback){ m_callback = callback; }

protected:

    void draw(const Magnum::Matrix4& tf, Magnum::SceneGraph::Camera3D& camera) override{
        m_callback(tf, camera);
    }

    callback_type m_callback;

};


