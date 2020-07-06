//
// Created by janos on 08.11.19.
//

#pragma once


#include "arc_ball_camera.hpp"
#include "TextureCoordinates.h"
#include "types.h"
#include "drawables.h"
#include "Remap.h"
#include "Combine.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StaticArray.h>

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/ImageData.h>

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/ImGuiIntegration/Context.h>
#include <Corrade/Containers/Array.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>


struct Viewer: public Platform::Application{
    explicit Viewer(int argc, char** argv);

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

    Cr::Containers::Optional<ArcBallCamera> camera;
    DrawableGroup drawables;
    Scene scene;

    Mg::GL::Texture2D texture{Mg::NoCreate}, texR{Mg::NoCreate}, texG{Mg::NoCreate}, texB{Mg::NoCreate}, texA{Mg::NoCreate};

    Mg::GL::Mesh mesh{Mg::NoCreate};
    Mg::Trade::MeshData meshData{Mg::MeshPrimitive::Points, 0};
    //Containers::Array<Mg::Trade::ImageData2D> images;
    Containers::Array<Mg::GL::Texture2D> imageTextures;
    Containers::Array<Matrix4> poses;
    Matrix4 projection;
    FlatDrawable* drawable = nullptr;

    Mg::Shaders::Flat3D shader{Mg::NoCreate};
    Mg::Shaders::Phong phong{Mg::NoCreate};
    Mg::Shaders::VertexColor3D vertexColored{Mg::NoCreate};
    ::Shaders::TextureCoordinates texCoordsShader{Mg::NoCreate};
    ::Shaders::Remap remap{Mg::NoCreate};
    ::Shaders::Combine combine{Mg::NoCreate};

    bool trackingMouse = false;

    Mg::ImGuiIntegration::Context imgui{Mg::NoCreate};

    bool showTexture = false;

    Vector2i textureSize{1024, 1024};
    Vector2i imageSize{640, 480};
    GL::Mesh cs{Mg::NoCreate};
};