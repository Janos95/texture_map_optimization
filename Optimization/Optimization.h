//
// Created by janos on 03.02.20.
//


#pragma once

#include "KeyFrame.h"
#include "RenderPass.h"
#include "Utilities.h"

namespace TextureMapOptimization {

/**
 *
 * @param keyFrames array of keyframes
 * @param texture, which should be optimized.
 * @param renderPass encapsulates some rendering state and implements
 * both the averaging pass and the calculation of the gradients.
 * @param cb, callback which is passed through to the optimization algorithm.
 * @return false if terminated by the user, false otherwise
 */
bool runOptimization(
        Array<KeyFrame>& keyFrames,
        GL::Texture2D& texture,
        RenderPass& renderPass,
        UniqueFunction<bool()>&& cb);

}

