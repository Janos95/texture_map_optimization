
#include "scene_graph_node.hpp"
#include "object.hpp"

using namespace Magnum;

DefaultCallback::DefaultCallback(Object& object): m_mesh(&object.mesh), m_color(object.color)
{
    if(object.texture)
        m_texture = &(*object.texture);
}

void DefaultCallback::operator()(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera){
    static Shaders::Flat3D shader;
    shader.setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix);

    if (m_texture) {
        shader.bindTexture(*m_texture);
    }
    else{
        shader.setColor(m_color);
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
