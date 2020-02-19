//
// Created by janos on 02.02.20.
//

#pragma once

#include "scene_graph_node.hpp"
#include "object.hpp"

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/ArrayViewStl.h> /** @todo remove once MeshData is sane */
#include <Corrade/Containers/Optional.h>

#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/MeshTools/Compile.h>

#include <vector>
#include <memory>
#include <map>

using namespace Corrade;
using namespace Magnum;

using Object3D = SceneGraph::Object<SceneGraph::RigidMatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::RigidMatrixTransformation3D>;
using Camera3D = SceneGraph::Camera3D;

class Viewer;


class Scene {
public:

    Scene();

    bool addObject(
            std::string name,
            const Trade::MeshData3D& meshdata,
            const Image2D* image = nullptr);

    Object* getObject(std::string_view name){
        auto it = m_objects.find(name);
        if(it == m_objects.end())
            return nullptr;
        else
           return &(it->second);
    }

    auto& root(){
        return m_scene;
    }

    auto& drawables(){
        return m_drawableGroup;
    }

private:

    friend Viewer;

    Scene3D m_scene;

    SceneGraph::DrawableGroup3D m_drawableGroup;

    std::map<std::string, Object, std::less<>> m_objects;
    std::map<std::string, std::unique_ptr<GL::AbstractShaderProgram>, std::less<>> m_shaders;
    std::vector<SceneGraphNode*> m_callbackHandles;
};
