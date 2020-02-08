//
// Created by janos on 02.02.20.
//

#pragma once

#include "scene_graph_node.hpp"
#include "compile_open_mesh.hpp"
#include "object.hpp"
#include "../mesh.hpp"

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/ArrayViewStl.h> /** @todo remove once MeshData is sane */
#include <Corrade/Containers/Optional.h>

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>

#include <vector>
#include <memory>

using namespace Corrade;
using namespace Magnum;

using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;
using Camera3D = SceneGraph::Camera3D;

class RenderPass;
class Viewer;

Matrix4 projectionMatrixFromCameraParameters(Matrix3 cameraMatrix, float W, float H);

class Scene {
public:

    Scene();

    bool addObject(
            std::string name,
            Trade::MeshData3D& meshdata,
            const Containers::Optional<ImageView2D> image = Containers::NullOpt){
        auto mesh = MeshTools::compile(meshdata);

        Containers::Optional<GL::Texture2D> texture = Containers::NullOpt;

        if(image)
        {
            texture = GL::Texture2D{};
            texture->setWrapping(GL::SamplerWrapping::ClampToEdge)
                    .setMagnificationFilter(GL::SamplerFilter::Linear)
                    .setMinificationFilter(GL::SamplerFilter::Linear)
                    .setStorage(1, GL::textureFormat(image->format()), image->size())
                    .setSubImage(0, {}, *image);
        }

        Object object{
            std::move(mesh),
            std::move(texture),
            Color4::cyan()};

        auto [it, inserted] = m_objects.emplace(std::move(name), std::move(object));
        if(!inserted)
            return false;

        auto& obj = it->second;
        //TODO obj.node = std::make_unique<SceneGraphNode>(&m_scene, DefaultCallback(obj), &m_drawableGroup);
        return true;
    }


    Object* getObject(std::string_view name){
        auto it = m_objects.find(name);
        if(it == m_objects.end())
            return nullptr;
        else
           return &(it->second);
    }

    const Object3D& cameraObject() const;

    Object3D& cameraObject();

    const SceneGraph::Camera3D& camera() const;

    SceneGraph::Camera3D& camera();

private:

    friend Viewer;

    Scene3D m_scene;
    Object3D m_cameraObject;
    Camera3D m_camera;

    SceneGraph::DrawableGroup3D m_drawableGroup;

    std::map<std::string, Object, std::less<>> m_objects;
    std::vector<SceneGraphNode*> m_callbackHandles;
};
