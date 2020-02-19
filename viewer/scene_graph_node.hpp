//author janos meny

#pragma once


#include <Magnum/Shaders/Shaders.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/RigidMatrixTransformation3D.h>
#include <Magnum/GL/AbstractShaderProgram.h>

using namespace Magnum;
using namespace Corrade;

using Object3D = SceneGraph::Object<SceneGraph::RigidMatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::RigidMatrixTransformation3D>;
using Drawable3D = SceneGraph::Drawable3D;

struct Object;

class DefaultCallback
{
public:

    explicit DefaultCallback(Object&, Shaders::Flat3D&);

    void operator()(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera);

    DefaultCallback(const DefaultCallback&) = default;
    ~DefaultCallback(){
        puts("getting destructed");
    }
private:

    GL::Mesh& m_mesh;
    GL::Texture2D* m_texture = nullptr;
    Color4 m_color;
    Shaders::Flat3D& m_shader;
};

class SceneGraphNode : public Drawable3D, public Object3D {
public:

    using callback_type = std::function<void(const Matrix4&, SceneGraph::Camera3D&)>;

    explicit SceneGraphNode(Object3D* parent, const callback_type& callback, SceneGraph::DrawableGroup3D* group);

    void setDrawCallback(callback_type&& callback){ m_callback = std::move(callback); }

protected:

    void draw(const Magnum::Matrix4& tf, Magnum::SceneGraph::Camera3D& camera) override;

    callback_type m_callback;

};


