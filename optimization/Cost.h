//
// Created by janos on 27.11.19.
//

#pragma once

#include "KeyFrame.h"
#include "Diff.h"
#include "ChainRule.h"
#include "Reduction.h"
#include "RenderPass.h"

#include <Magnum/Magnum.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/TextureArray.h>
#include <Magnum/GL/Mesh.h>

#include <ceres/sized_cost_function.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

class PhotometricCost : public ceres::SizedCostFunction<1,3,3> {
public:

    explicit PhotometricCost(RenderPass&, KeyFrame&);

    bool Evaluate(double const* const* parameters,
                          double* residuals,
                          double** jacobians) const override;

private:

    RenderPass& m_renderPass;
    KeyFrame& m_kf;
};


