//
// Created by jmeny on 08.11.19.
//

#include "vertex_colored_drawable.h"



void VertexColoredDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera)
{
    m_shader.setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix);
    m_mesh.draw(m_shader);
}
