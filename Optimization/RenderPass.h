//
// Created by janos on 7/23/20.
//

#pragma once

#include "KeyFrame.h"
#include "Diff.h"
#include "Reduction.h"
#include "Vis.h"
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

    RenderPass(GL::Mesh&, Array<KeyFrame>&);

    void setTexture(GL::Texture2D& texture);

    void optimizationPass(size_t idx, double const* params, double& cost, double* gradient);

    /**
     * For each key frame computes dense texture coordinates.
     * Then uses the texture coordinates to map color values
     * from the ground truth image into the texture. Since this
     * has do be done atomically, the texture is split into its
     * three rgb channels and then combined later, when all
     * key frames have been processed.
     */
    void averagingPass();

    /**
     * prepares the framebuffer for either running
     * an averaging pass or an optimization pass.
     */
    void setFramebufferMode(FramebufferMode);

    enum class VisualizatonFlag {
        Cost = 1,
        RotationGradient = 2,
        TranslationGradient = 3,
        RenderedImage = 4,
    };

    /**
     * Utility for rendering some feature of
     * a specific key frame into a texture. This
     * is mainly useful for debuggin purposes.
     * @param image texture into which we want to render.
     * This must be allocated already.
     * @param idx keyframe index to render
     * @param flag to choose between different features
     * like gradients, cost or simply a synthetic view
     * of the key frame.
     */
    void renderIntoTexture(GL::Texture2D& image, size_t idx, VisualizatonFlag flag);

    /**
     * @param nx image width
     * @param ny image height
     * @param fx focal length wrt. pixel width
     * @param fy focal length wrt. pixel height
     * @param cx x coordinate of central point
     * @param cy y coordinate of central point
     */
    void setCameraParameters(float nx, float ny, float fx, float fy, float cx, float cy) {
        auto fov = Math::atan(nx/(2*fx));
        m_projection = Matrix4::perspectiveProjection(2*fov, nx/ny, 0.01, 10);

        m_fx = fx;
        m_fy = fy;
        m_cx = cx;
        m_cy = cy;
    }

    /**
     * This reloads and recompiles the shaders.
     * This is nice for interactivley debugging/editing
     * shaders;
     */
    void reloadShader();

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
    Shaders::Vis m_vis;
    Shaders::Reduction m_reduction;

    GL::Texture2D m_gradientRotations, m_gradientTranslations, m_costs;

    GL::Texture2D m_red, m_green, m_blue;
    GL::Texture2D m_count;

    GL::Texture2D m_depth;
    GL::Texture2D m_texCoordsTexture;

    Vector2i m_imageSize;
    Vector2i m_textureSize;

    FramebufferMode m_fbMode{0};

    /* Profiling */
    Mg::DebugTools::GLFrameProfiler m_profilerGenerateTexture{
            Mg::DebugTools::GLFrameProfiler::Value::FrameTime|
            Mg::DebugTools::GLFrameProfiler::Value::CpuDuration, 180};

    Mg::DebugTools::GLFrameProfiler m_profilerComputeGradient{
            Mg::DebugTools::GLFrameProfiler::Value::FrameTime|
            Mg::DebugTools::GLFrameProfiler::Value::CpuDuration, 180};

    GL::Texture2D m_turbo; //colormap

};

}



