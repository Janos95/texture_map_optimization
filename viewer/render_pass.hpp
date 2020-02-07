//
// Created by janos on 02.02.20.
//

#pragma once

#include "render_target.hpp"
#include "scene.hpp"
#include "camera.hpp"

class RenderPass{
public:

  void draw(Camera& camera, Scene& scene);
  void bindRenderTarget(RenderTarget& target);
private:

};

