//
// Created by janos on 7/23/20.
//

#pragma once

#include "KeyFrame.h"
#include "Diff.h"
#include "DepthFilter.h"
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

    RenderPass(Mg::Trade::MeshData const&, Cr::Containers::Array<KeyFrame>&);

    void setTexture(GL::Texture2D& texture);

    void optimizationPass(double const* const* params, double* costs, double** jacobians);

    void averagingPass();

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

    Cr::Containers::Array<KeyFrame>& m_keyFrames;
    GL::Texture2D* m_texture = nullptr;

    Matrix4 m_projection;

    GL::Framebuffer m_fb{Mg::NoCreate};

    UnsignedInt m_depthFilterReductionFactor = 3;

    GL::Mesh m_mesh;
    GL::Buffer m_instancedTransformations;

    //Shaders::Diff m_diff;
    //Shaders::DepthFilter m_depthFilter{m_depthFilterReductionFactor};
    //Shaders::Reduction m_reduction{3};

    Shaders::TextureCoordinates m_texCoordsShader;
    Shaders::Remap m_remap;
    Shaders::Combine m_combine;

    GL::Texture2DArray m_gradientRotations, m_gradientTranslations, m_costs, m_depth;

    GL::Texture2D m_red, m_green, m_blue;
    GL::Texture2D m_count;
    GL::Texture2D m_texCoords;
    GL::Texture1D m_rotationParameters, m_translationParameters;

    GL::Texture2D m_depthSingle;
    GL::Texture2D m_texCoordsTexture;

    Mg::Vector2i m_imageSize;
    Mg::Vector2i m_textureSize;

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



