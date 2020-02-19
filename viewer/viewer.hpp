//
// Created by janos on 08.11.19.
//

#pragma once


#include "scene.hpp"
#include "arc_ball_camera.hpp"

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/ArrayViewStl.h> /** @todo remove once MeshData is sane */

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>

#include <map>
#include <memory>

using namespace Corrade;
using namespace Magnum;


class Viewer: public Platform::Application{
public:
    explicit Viewer(int argc, char** argv);

    std::vector<std::function<void(Scene&)>> callbacks;
    Scene scene;

private:

    void drawEvent() override;
    void keyPressEvent(KeyEvent& event) override;
    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    Corrade::Containers::Optional<ArcBallCamera> m_camera;
};