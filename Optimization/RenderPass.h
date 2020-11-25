//
// Created by janos on 7/23/20.
//

#pragma once

#include "KeyFrame.h"
#include "Diff.h"
//#include "DepthFilter.h"
#include "Reduction.h"
#include "TextureCoordinates.h"
#include "Remap.h"
#include "Combine.h"
#include "DepthFilter.h"
#include "Types.h"

#include <Magnum/Magnum.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/TextureArray.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/DebugTools/FrameProfiler.h>
#include <Magnum/Trade/MeshData.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

namespace TextureMapOptimization {

class RenderPass {
public:

    enum class FramebufferMode : Mg::UnsignedInt {
        ColorAveragingPass = 1,
        OptimizationPass = 2
    };

    enum class Flag : Mg::UnsignedInt {
        DepthFiltering
    };

    using Flags = Cr::Containers::EnumSet<Flag>;

    CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

    RenderPass(GL::Mesh&, Cr::Containers::Array<KeyFrame>&);

    void setTexture(GL::Texture2D& texture);

    void optimizationPass(double const* const* params, double* costs, double** jacobians);

    void averagingPass();

    void setFramebufferMode(FramebufferMode);

    void renderKeyPose(GL::Texture2D& image, size_t idx);

    void renderGradient(GL::Texture2D& image, size_t idx);

    void setCameraParameters(float nx, float ny, float fx, float fy, float cx, float cy) {
        auto fov = Math::atan(nx/(2*fx));
        m_projection = Matrix4::perspectiveProjection(2*fov, nx/ny, 0.01, 10);

        m_fx = fx;
        m_fy = fy;
        m_cx = cx;
        m_cy = cy;
    }

    void clearImages() {

        //using L = std::initializer_list<std::pair<GL::Texture2D*, GL::TextureFormat>>;
        //for(auto[tex, format] : L{
        //        {&texture, GL::TextureFormat::RGBA32F},
        //        {&texR,    GL::TextureFormat::R32F},
        //        {&texG,    GL::TextureFormat::R32F},
        //        {&texB,    GL::TextureFormat::R32F},
        //        {&texA,    GL::TextureFormat::R32F}
        //}) {
        //    *tex = GL::Texture2D{};
        //    setupTexture(*tex, size, format);

        //    fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, *tex, 0)
        //      .clearColor(0, Color4{})
        //      .clear(GL::FramebufferClear::Color);
        //}
    }

private:

    GL::Mesh& m_mesh;
    Array<KeyFrame>& m_keyFrames;

    GL::Texture2D* m_texture = nullptr;

    Matrix4 m_projection;
    float m_fx, m_fy, m_cx, m_cy;

    GL::Framebuffer m_fb{Mg::NoCreate};

    UnsignedInt m_depthFilterReductionFactor = 3;


    Shaders::Diff m_diff;
    Shaders::TextureCoordinates m_texCoordsShader;
    Shaders::Remap m_remap;
    Shaders::Combine m_combine;

    GL::Texture2D m_gradientRotations, m_gradientTranslations, m_costs;

    GL::Texture2D m_red, m_green, m_blue;
    GL::Texture2D m_count;

    GL::Texture2D m_depth;
    GL::Texture2D m_texCoordsTexture;

    Vector2i m_imageSize;
    Vector2i m_textureSize;

    FramebufferMode m_fbMode = {};

    /* Profiling */
    Mg::DebugTools::GLFrameProfiler m_profilerGenerateTexture{
            Mg::DebugTools::GLFrameProfiler::Value::FrameTime|
            Mg::DebugTools::GLFrameProfiler::Value::CpuDuration, 180};

    Mg::DebugTools::GLFrameProfiler m_profilerComputeGradient{
            Mg::DebugTools::GLFrameProfiler::Value::FrameTime|
            Mg::DebugTools::GLFrameProfiler::Value::CpuDuration, 180};

};

}



