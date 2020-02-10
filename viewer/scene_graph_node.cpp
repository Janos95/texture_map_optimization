
#include "scene_graph_node.hpp"
#include "object.hpp"

using namespace Magnum;

DefaultCallback::DefaultCallback(Object& object) :
    m_mesh(&object.mesh),
    m_color(object.color),
    m_shader(object.texture ? Shaders::Flat3D::Flag::Textured : Shaders::Flat3D::Flag{})
{
    if(object.texture){
        m_texture = std::addressof(*object.texture);
    }
}

void DefaultCallback::operator()(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera){
    m_shader.setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix);

    if (m_texture) {
        m_shader.bindTexture(*m_texture);
    }
    else{
        m_shader.setColor(m_color);
    }

    CORRADE_INTERNAL_ASSERT(m_mesh);
    m_mesh->draw(m_shader);
}


SceneGraphNode::SceneGraphNode(Object3D* parent, callback_type&& callback, SceneGraph::DrawableGroup3D* group):
    Object3D(parent),
    SceneGraph::Drawable3D{*this, group},
    m_callback(std::move(callback))
{
}

void SceneGraphNode::draw(const Matrix4& tf, SceneGraph::Camera3D& camera){
    m_callback(tf, camera);
}
