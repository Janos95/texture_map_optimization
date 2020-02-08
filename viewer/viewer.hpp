//
// Created by janos on 08.11.19.
//

#pragma once


#include "compile_open_mesh.hpp"
#include "scene.hpp"
#include "../mesh.hpp"

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

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


using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;




class Viewer: public Platform::Application {
public:
    explicit Viewer();

    std::vector<std::function<void(Scene&)>> callbacks;
    Scene scene;

private:

    Float depthAt(const Vector2i& windowPosition);
    Vector3 unproject(const Vector2i& windowPosition, Float depth) const;

    void keyPressEvent(KeyEvent& event) override;
    void mousePressEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;
    void drawEvent() override;


    Float m_lastDepth;
    Vector2i m_lastPosition{-1};
    Vector3 m_rotationPoint, m_translationPoint;

    int m_dummy = 1;
};