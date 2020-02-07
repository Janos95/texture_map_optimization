//
// Created by janos on 03.02.20.
//

#include "render_pass.hpp"


void RenderPass::draw(Camera& camera, Scene& scene){
    scene.m_camera.setProjectionMatrix(camera.projection);
    scene.m_cameraObject.setTransformation(camera.transformation);
    scene.m_camera.draw(scene.m_drawableGroup);
}


void RenderPass::bindRenderTarget(RenderTarget& target){
    target.renderEnter();
}
