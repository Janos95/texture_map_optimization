// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "depth_unproject.hpp"

#include <Magnum/Magnum.h>

#include <memory>



class RenderTarget {
 public:
  /**
   * @brief Constructor
   * @param size               The size of the underlying framebuffers in WxH
   * @param depthUnprojection  Depth unprojection parameters.  See @ref
   *                           calculateDepthUnprojection()
   * @param depthShader        A DepthShader used to unproject depth on the GPU.
   *                           Unprojects the depth on the CPU if nullptr.
   *                           Must be not nullptr to use @ref
   *                           readFrameDepthGPU()
   */
  RenderTarget(const Magnum::Vector2i& size, DepthShader* depthShader);

  /**
   * @brief Constructor
   * @param size               The size of the underlying framebuffers in WxH
   * @param depthUnprojection  Depth unrpojection parameters.  See @ref
   *                           calculateDepthUnprojection()
   *
   * Equivalent to calling @ref RenderTarget(size, depthUnprojection, nullptr)
   */
  explicit RenderTarget(const Magnum::Vector2i& size)
      : RenderTarget{size, nullptr} {};

  ~RenderTarget() { Corrade::Utility::Debug{} << "Deconstructing RenderTarget"; }

  /**
   * @brief Called before any draw calls that target this RenderTarget
   * Clears the framebuffer and binds it
   */
  void renderEnter();

  /**
   * @brief Called after any draw calls that target this RenderTarget
   */
  void renderExit();

  /**
   * @brief The size of the framebuffer in WxH
   */
  Magnum::Vector2i framebufferSize() const;

  /**
   * @brief Retrieve the RGBA rendering results.
   *
   * @param[in, out] view Preallocated memory that will be populated with the
   * result.  The result will be read as the pixel format of this view.
   */
  void readFrameRgba(const Magnum::MutableImageView2D& view);

  /**
   * @brief Retrieve the depth rendering results.
   *
   * @param[in, out] view Preallocated memory that will be populated with the
   * result.  The PixelFormat of the image must only specify the R channel,
   * generally @ref Magnum::PixelFormat::R32F
   */
  void readFrameDepth(const Magnum::MutableImageView2D& view);


  /**
   * @brief Blits the rgba buffer from internal FBO to default frame buffer
   * which in case of EmscriptenApplication will be a canvas element.
   */
  void blitRgbaToDefault();

  // @brief Delete copy Constructor
  RenderTarget(const RenderTarget&) = delete;
  // @brief Delete copy operator
  RenderTarget& operator=(const RenderTarget&) = delete;

protected:
  struct Impl;
  std::unique_ptr<Impl> pimpl_;
};


