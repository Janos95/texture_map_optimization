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
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/MeshVisualizer.h>

#include <vector>
#include <memory>
#include <map>
#include <variant>

using namespace Corrade;
using namespace Magnum;

using Object3D = SceneGraph::Object<SceneGraph::RigidMatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::RigidMatrixTransformation3D>;
using Camera3D = SceneGraph::Camera3D;

class Viewer;


template< class, class = std::void_t<> >
struct has_viewport_size : std::false_type { };

template< class T >
struct has_viewport_size<
        T,
        /* check if type T has a member setViewportSize that takes a Vector2 */
        std::void_t<decltype(std::declval<T>().setViewportSize(std::declval<Vector2>()))>> : std::true_type { };

template<class T>
constexpr auto has_viewport_size_v = has_viewport_size<T>::value;

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
           return std::addressof(it->second);
    }

    auto& root(){
        return m_scene;
    }

    auto& drawables(){
        return m_drawableGroup;
    }

    void setViewportSize(const Vector2i& size){
        for(auto& [_,shader] : m_shaders) {
            std::visit([size = Vector2(size)](auto &s) {
                if constexpr(has_viewport_size_v < std::remove_reference_t<decltype(s)>>)
                    s.setViewportSize(size);
            }, shader);
        }
    }


private:

    using shader_variant = std::variant<Shaders::Flat3D, Shaders::VertexColor3D, Shaders::MeshVisualizer, Shaders::Phong>;

    friend Viewer;

    Scene3D m_scene;

    SceneGraph::DrawableGroup3D m_drawableGroup;

    std::map<std::string, Object, std::less<>> m_objects;
    std::map<std::string, shader_variant, std::less<>> m_shaders;
};
