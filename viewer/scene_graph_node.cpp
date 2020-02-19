
#include "scene_graph_node.hpp"
#include "object.hpp"

using namespace Magnum;

DefaultCallback::DefaultCallback(Object& object, Shaders::Flat3D& shader) :
    m_mesh(object.mesh),
    m_color(object.color),
    m_shader(shader)
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

    m_mesh.draw(m_shader);
}


SceneGraphNode::SceneGraphNode(Object3D* parent, const callback_type& callback, SceneGraph::DrawableGroup3D* group):
    m_callback(callback),
    Object3D(parent),
    SceneGraph::Drawable3D{*this, group}
{
}

void SceneGraphNode::draw(const Matrix4& tf, SceneGraph::Camera3D& camera){
    m_callback(tf, camera);
}
