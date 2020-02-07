// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "depth_unproject.hpp"
#include "render_target.hpp"


#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>


namespace Cr = Corrade;
namespace Mn = Magnum;

const Mn::GL::Framebuffer::ColorAttachment RgbaBuffer =
    Mn::GL::Framebuffer::ColorAttachment{0};

const Mn::GL::Framebuffer::ColorAttachment UnprojectedDepthBuffer =
    Mn::GL::Framebuffer::ColorAttachment{0};

struct RenderTarget::Impl {
  Impl(const Mn::Vector2i& size,
       const Mn::Vector2& depthUnprojection,
       DepthShader* depthShader)
      : colorBuffer_{},
        depthRenderTexture_{},
        framebuffer_{Mn::NoCreate},
        depthUnprojection_{depthUnprojection},
        depthShader_{depthShader},
        unprojectedDepth_{Mn::NoCreate},
        depthUnprojectionMesh_{Mn::NoCreate},
        depthUnprojectionFrameBuffer_{Mn::NoCreate} {
    if (depthShader_) {
      CORRADE_INTERNAL_ASSERT(depthShader_->flags() &
                              DepthShader::Flag::UnprojectExistingDepth);
    }

    colorBuffer_.setStorage(Mn::GL::RenderbufferFormat::SRGB8Alpha8, size);
    depthRenderTexture_.setMinificationFilter(Mn::GL::SamplerFilter::Nearest)
        .setMagnificationFilter(Mn::GL::SamplerFilter::Nearest)
        .setWrapping(Mn::GL::SamplerWrapping::ClampToEdge)
        .setStorage(1, Mn::GL::TextureFormat::DepthComponent32F, size);

    framebuffer_ = Mn::GL::Framebuffer{{{}, size}};
    framebuffer_.attachRenderbuffer(RgbaBuffer, colorBuffer_)
        .attachTexture(Mn::GL::Framebuffer::BufferAttachment::Depth,
                       depthRenderTexture_, 0)
        .mapForDraw({{0, RgbaBuffer}});
    CORRADE_INTERNAL_ASSERT(
        framebuffer_.checkStatus(Mn::GL::FramebufferTarget::Draw) ==
        Mn::GL::Framebuffer::Status::Complete);
  }

  void initDepthUnprojector() {
    if (depthUnprojectionMesh_.id() == 0) {
      unprojectedDepth_ = Mn::GL::Renderbuffer{};
      unprojectedDepth_.setStorage(Mn::GL::RenderbufferFormat::R32F,
                                   framebufferSize());

      depthUnprojectionFrameBuffer_ =
          Mn::GL::Framebuffer{{{}, framebufferSize()}};
      depthUnprojectionFrameBuffer_
          .attachRenderbuffer(UnprojectedDepthBuffer, unprojectedDepth_)
          .mapForDraw({{0, UnprojectedDepthBuffer}});
      CORRADE_INTERNAL_ASSERT(
          framebuffer_.checkStatus(Mn::GL::FramebufferTarget::Draw) ==
          Mn::GL::Framebuffer::Status::Complete);

      depthUnprojectionMesh_ = Mn::GL::Mesh{};
      depthUnprojectionMesh_.setCount(3);
    }
  }

  void unprojectDepthGPU() {
    CORRADE_INTERNAL_ASSERT(depthShader_ != nullptr);
    initDepthUnprojector();

    depthUnprojectionFrameBuffer_.bind();
    depthShader_->bindDepthTexture(depthRenderTexture_)
        .setDepthUnprojection(depthUnprojection_);

    depthUnprojectionMesh_.draw(*depthShader_);
  }

  void renderEnter() {
    framebuffer_.clearDepth(1.0);
    framebuffer_.clearColor(0, Mn::Color4{0, 0, 0, 1});
    framebuffer_.bind();
  }

  void renderExit() {}

  void blitRgbaToDefault() {
    framebuffer_.mapForRead(RgbaBuffer);
    CORRADE_INTERNAL_ASSERT(framebuffer_.viewport() == Mn::GL::defaultFramebuffer.viewport());

    Mn::GL::AbstractFramebuffer::blit(
        framebuffer_, Mn::GL::defaultFramebuffer, framebuffer_.viewport(),
        Mn::GL::defaultFramebuffer.viewport(), Mn::GL::FramebufferBlit::Color,
        Mn::GL::FramebufferBlitFilter::Nearest);
  }

  void readFrameRgba(const Mn::MutableImageView2D& view) {
    framebuffer_.mapForRead(RgbaBuffer).read(framebuffer_.viewport(), view);
  }

  void readFrameDepth(const Mn::MutableImageView2D& view) {
    if (depthShader_) {
      unprojectDepthGPU();
      depthUnprojectionFrameBuffer_.mapForRead(UnprojectedDepthBuffer)
          .read(framebuffer_.viewport(), view);
    } else {
      Mn::MutableImageView2D depthBufferView{
          Mn::GL::PixelFormat::DepthComponent, Mn::GL::PixelType::Float,
          view.size(), view.data()};
      framebuffer_.read(framebuffer_.viewport(), depthBufferView);
      unprojectDepth(depthUnprojection_,
                     Cr::Containers::arrayCast<Mn::Float>(view.data()));
    }
  }


  Mn::Vector2i framebufferSize() const {
    return framebuffer_.viewport().size();
  }


 private:
  Mn::GL::Renderbuffer colorBuffer_;
  Mn::GL::Texture2D depthRenderTexture_;
  Mn::GL::Framebuffer framebuffer_;

  Mn::Vector2 depthUnprojection_;
  DepthShader* depthShader_;
  Mn::GL::Renderbuffer unprojectedDepth_;
  Mn::GL::Mesh depthUnprojectionMesh_;
  Mn::GL::Framebuffer depthUnprojectionFrameBuffer_;

};

RenderTarget::RenderTarget(const Mn::Vector2i& size,
                           const Mn::Vector2& depthUnprojection,
                           DepthShader* depthShader)
    : pimpl_(std::make_unique<Impl>(size,
                                    depthUnprojection,
                                    depthShader)) {}

void RenderTarget::renderEnter() {
  pimpl_->renderEnter();
}

void RenderTarget::renderExit() {
  pimpl_->renderExit();
}

void RenderTarget::readFrameRgba(const Mn::MutableImageView2D& view) {
  pimpl_->readFrameRgba(view);
}

void RenderTarget::readFrameDepth(const Mn::MutableImageView2D& view) {
  pimpl_->readFrameDepth(view);
}


void RenderTarget::blitRgbaToDefault() {
  pimpl_->blitRgbaToDefault();
}

Mn::Vector2i RenderTarget::framebufferSize() const {
  return pimpl_->framebufferSize();
}




