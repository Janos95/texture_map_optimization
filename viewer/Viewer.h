//
// Created by janos on 08.11.19.
//

#pragma once

#include "ArcBall.h"
#include "types.h"
#include "KeyFrame.h"
#include "Optimization.h"
#include "FullScreenTriangle.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StaticArray.h>

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/ImageData.h>

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/ImGuiIntegration/Context.h>
#include <Corrade/Containers/Array.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>

namespace TextureMapOptimization {

struct Viewer : public Platform::Application {

    explicit Viewer(Arguments const&);

    void drawEvent() override;

    void viewportEvent(ViewportEvent& event) override;

    void keyPressEvent(KeyEvent& event) override;

    void mousePressEvent(MouseEvent& event) override;

    void mouseReleaseEvent(MouseEvent& event) override;

    void mouseMoveEvent(MouseMoveEvent& event) override;

    void mouseScrollEvent(MouseScrollEvent& event) override;

    void keyReleaseEvent(KeyEvent& event) override;

    void textInputEvent(TextInputEvent& event) override;

    void drawOptions();

    ceres::TerminationType runOptimization(UniqueFunction<bool()> cb);

    bool isOptimizing = false;

    Cr::Containers::Optional<ArcBall> arcball;

    Mg::GL::Texture2D texture{Mg::NoCreate};

    Mg::GL::Mesh mesh{Mg::NoCreate};
    Mg::Trade::MeshData meshData{Mg::MeshPrimitive::Points, 0};
    Containers::Array<KeyFrame> keyFrames;
    Matrix4 projection;

    Shaders::FullScreenTriangle triangleShader{Mg::NoCreate};
    Mg::Shaders::Flat3D flat{Mg::NoCreate};
    Mg::Shaders::Phong phong{Mg::NoCreate};
    Mg::Shaders::VertexColor3D vertexColored{Mg::NoCreate};

    bool trackingMouse = false;

    Mg::ImGuiIntegration::Context imgui{Mg::NoCreate};

    bool showTexture = true;

    Vector2i textureSize{1024, 1024};
    Vector2i imageSize{640, 480};
    GL::Mesh cs{Mg::NoCreate};

    Containers::Optional<Optimization> m_tmo;
};

}