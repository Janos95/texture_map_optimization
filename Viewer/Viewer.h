//
// Created by janos on 08.11.19.
//

#pragma once

#include "ArcBall.h"
#include "KeyFrame.h"
#include "FullScreenTriangle.h"
#include "Types.h"
#include "Utilities.h"
#include "RenderPass.h"
#include "UniqueFunction.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/Array.h>

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/ImageData.h>

#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/ImGuiIntegration/Context.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>

#include "imgui.h"

namespace TextureMapOptimization {

namespace Mg = Magnum;
namespace Cr = Corrade;

struct Viewer : public Mg::Platform::Application {

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

    void presetLayout();

    /*
     * this member function should not be called
     * from withing this class. Rather it is used
     * to start the optimization from the thread driving
     * the application. The optimization framework used (ceres)
     * does not allow to do a single interation, thus
     * we give it a callback which runs the main loop
     * iteration after each optimization pass to not block
     * the GUI.
     */
    void startOptimization();

    bool isOptimizing = false;

    Optional<ArcBall> arcball;

    GL::Texture2D texture{Mg::NoCreate};

    GL::Mesh mesh{Mg::NoCreate};
    Mg::Trade::MeshData meshData{Mg::MeshPrimitive::Points, 0};
    Array<KeyFrame> keyFrames;
    Matrix4 projection;

    Shaders::FullScreenTriangle triangleShader{Mg::NoCreate};
    Mg::Shaders::Flat3D flat{Mg::NoCreate};
    Mg::Shaders::Phong phong{Mg::NoCreate};
    Mg::Shaders::VertexColor3D vertexColored{Mg::NoCreate};

    bool trackingMouse = false;

    Mg::ImGuiIntegration::Context imgui{Mg::NoCreate};

    bool showTexture = true;
    bool drawPoses = false;

    Vector2i textureSize{1024, 1024};
    Vector2i imageSize;

    GL::Mesh axis{Mg::NoCreate};
    int currentKf = 0;
    int currentOption = 0;

    GL::Texture2D renderedImage{Mg::NoCreate};

    Optional<RenderPass> renderPass;

    GL::Texture2D* overlay = &texture;
    UniqueFunction<void()> onNewKeyFrame = []{};
};

}