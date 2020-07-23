//
// Created by janos on 05.02.20.
//

#include "Optimization.h"
#include "Cost.h"
#include "UniqueFunction.hpp"
#include "ScopedTimer/ScopedTimer.h"

#include <Magnum/Math/Color.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/PixelFormat.h>

using namespace Magnum;
using namespace Corrade;

namespace {

void setupTexture(GL::Texture2D& texture, Vector2i const& size, GL::TextureFormat format){
    texture.setMagnificationFilter(GL::SamplerFilter::Linear)
           .setMinificationFilter(GL::SamplerFilter::Linear)
           .setWrapping(GL::SamplerWrapping::ClampToEdge)
           .setStorage(1, format, size);
}


struct Callback : public ceres::IterationCallback {

    explicit Callback(UniqueFunction<void()> cb_) : cb(std::move(cb_)) {}

    ceres::CallbackReturnType operator()(ceres::IterationSummary const& summary) override {
       cb();
       return ceres::CallbackReturnType::SOLVER_CONTINUE;
    }

    UniqueFunction<void()> cb;
};

}

TextureMapOptimization::TextureMapOptimization(Cr::Containers::Array<KeyFrame>& kfs, Mg::GL::Mesh& mesh) :
    m_kfs(kfs), m_mesh(mesh)
{
}

void TextureMapOptimization::run(UniqueFunction<void()>&& vis){

    Callback callback{[this, &vis]{ averageColors(); vis(); }};

    ceres::Problem problem;
    for(auto& kf : m_kfs){
        problem.AddResidualBlock(new )
    }

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::SPARSE_SCHUR;
    options.minimizer_progress_to_stdout = true;
    options.update_state_every_iteration = true;
    options.callbacks.push_back(&callback);

    ceres::Solver::Summary summary;

    ceres::Solve(options, &problem, &summary);
}

void TextureMapOptimization::averageColors() {
    CORRADE_ASSERT(m_texture, "TextureMapOptimization: Call setTexture before running the optimization.",);
    ScopedTimer t{"Averaging all colors", true};
    auto imageSize = m_kfs.front().image.imageSize(0);
    auto textureSize = m_texture->imageSize(0);

    Range2Di viewport{{}, imageSize};
    GL::Framebuffer fb{viewport};
    GL::Renderbuffer depthBuffer;

    depthBuffer.setStorage(GL::RenderbufferFormat::DepthComponent24,
                           viewport.size());
    fb.attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth,
                          depthBuffer);

    GL::Texture2D texCoordsTexture;
    setupTexture(texCoordsTexture, imageSize, GL::TextureFormat::RGBA32F);

    fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, texCoordsTexture, 0)
      .mapForDraw(GL::Framebuffer::ColorAttachment{0})
      .bind();

    CORRADE_INTERNAL_ASSERT(
            fb.checkStatus(GL::FramebufferTarget::Draw) ==
            GL::Framebuffer::Status::Complete);


    Vector3ui wgCount1(imageSize.x(), imageSize.y(), 1);
    Vector3ui wgCount2(textureSize.x(), textureSize.y(), 1);

    const auto map = DebugTools::ColorMap::turbo();
    const Vector2i mapSize{Int(map.size()), 1};

    GL::Texture2D colorMapTexture;
    colorMapTexture
            .setMinificationFilter(SamplerFilter::Linear)
            .setMagnificationFilter(SamplerFilter::Linear)
            .setWrapping(SamplerWrapping::ClampToEdge) // or Repeat
            .setStorage(1, GL::TextureFormat::RGB8, mapSize) // or SRGB8
            .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, mapSize, map});

    fb.clearColor(0, Color4{});

    for(auto& kf : m_kfs){

        fb.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
        m_texCoordsShader.setTransformationProjectionMatrix(kf.projection * kf.pose.inverted())
                         .draw(m_mesh);

        m_remap.bindTextureR(m_texR)
             .bindTextureG(m_texG)
             .bindTextureB(m_texB)
             .bindTextureA(m_texA)
             .bindImage(kf.image)
             .bindTextureCoordinates(texCoordsTexture)
             .dispatchCompute(wgCount1);
    }

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    m_combine.bindTextureR(m_texR)
           .bindTextureG(m_texG)
           .bindTextureB(m_texB)
           .bindTextureA(m_texA)
           .bindRgbaImage(*m_texture)
           .dispatchCompute(wgCount2);

    //GL::Renderer::setMemoryBarrier(Mg::GL::Renderer::MemoryBarrier::TextureFetch);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


