//
// Created by janos on 27.02.20.
//

#pragma once

#include "types.hpp"
#include "pbr_shader.hpp"

#include <Magnum/GL/GL.h>
#include <Magnum/Math/Math.h>
#include <Magnum/Shaders/Shaders.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Math/Color.h>
#include <Corrade/Containers/Array.h>

namespace Cr = Corrade;
namespace Mg = Magnum;

using namespace Mg::Math::Literals;

struct MeshDrawable : Drawable {
    MeshDrawable(Object& obj, Mg::GL::Mesh& m, DrawableGroup* group, Mg::GL::Texture2D* t = nullptr);
    Mg::GL::Mesh& mesh;
    Mg::GL::Texture2D* texture;
    bool show = true;
};

struct FlatDrawable : MeshDrawable
{

    explicit FlatDrawable(Object&, Mg::GL::Mesh&, Mg::GL::AbstractShaderProgram&, DrawableGroup*);

    void draw(const Magnum::Matrix4&, Magnum::SceneGraph::Camera3D&) override;

    Mg::Shaders::Flat3D& shader;
    Mg::Color4 color;
};

struct VertexColorDrawable : MeshDrawable
{
    explicit VertexColorDrawable(Object& object, Mg::GL::Mesh& m, Mg::GL::AbstractShaderProgram& shader, DrawableGroup* group);

    void draw(const Magnum::Matrix4&, Mg::SceneGraph::Camera3D&) override;

    Magnum::Shaders::VertexColor3D& shader;
};


struct PhongDiffuseDrawable : MeshDrawable
{
public:

    explicit PhongDiffuseDrawable(Object&, Mg::GL::Mesh&, Mg::GL::AbstractShaderProgram&, DrawableGroup*);

    void draw(const Mg::Matrix4&, Mg::SceneGraph::Camera3D&) override;

    Mg::Shaders::Phong& shader;
    Mg::Color4 color{0x2f83cc_rgbf};
};


struct PbrDrawable : MeshDrawable
{
public:

    explicit PbrDrawable(Object&, Mg::GL::Mesh&, Mg::GL::AbstractShaderProgram&, DrawableGroup*);

    void draw(const Mg::Matrix4&, Mg::SceneGraph::Camera3D&) override;


    PbrShader& shader;
    Mg::Float metallness = 0.5, roughness = 0.5, ambientOcclusion = 1.;
    Mg::Vector3 albedo{.5f, 0.f, 0.f};
    Mg::Vector3 cameraPosition;

    Cr::Containers::Array<Mg::Vector3> lights{Cr::Containers::InPlaceInit, {
            Mg::Vector3{-10.0f,  10.0f, 10.0f},
            Mg::Vector3{10.0f,  10.0f, 10.0f},
            Mg::Vector3{-10.0f, -10.0f, 10.0f},
            Mg::Vector3{10.0f, -10.0f, 10.0f}
    }};

    Cr::Containers::Array<Mg::Vector3> lightColors{Cr::Containers::InPlaceInit, {
            Mg::Vector3{300.0f, 300.0f, 300.0f},
            Mg::Vector3{300.0f, 300.0f, 300.0f},
            Mg::Vector3{300.0f, 300.0f, 300.0f},
            Mg::Vector3{300.0f, 300.0f, 300.0f}
    }};

    Mg::GL::CubeMapTexture* irradianceMap = nullptr;
};

struct MeshVisualizerDrawable : MeshDrawable
{

    explicit MeshVisualizerDrawable(Object&, Mg::GL::Mesh&, Mg::GL::AbstractShaderProgram&, DrawableGroup*);

    void draw(const Magnum::Matrix4&, Magnum::SceneGraph::Camera3D&) override;

    Mg::Float wireframeWidth = 1.f;
    Mg::Float smoothness = 2.f;
    Mg::Color4 color{1};
    Mg::Color4 wireframeColor{0,0,0,1};
    Mg::Shaders::MeshVisualizer3D& shader;
};

struct FaceColorDrawable : MeshDrawable
{
    explicit FaceColorDrawable(Object&, Mg::GL::Mesh&, Mg::GL::AbstractShaderProgram&, DrawableGroup*);

    void draw(const Magnum::Matrix4&, Magnum::SceneGraph::Camera3D&) override;

    Mg::Shaders::MeshVisualizer3D& shader;
    Mg::Float offset = 0., scale = 1.f;
};


