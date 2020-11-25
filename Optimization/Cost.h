//
// Created by janos on 27.11.19.
//

#pragma once

#include "RenderPass.h"

#include <ceres/sized_cost_function.h>

namespace TextureMapOptimization {

class PhotometricCost : public ceres::CostFunction {
public:

    explicit PhotometricCost(RenderPass& renderPass) : m_renderPass(renderPass) {}

    bool Evaluate(double const* const* parameters,
                  double* residuals,
                  double** jacobians) const override {
        m_renderPass.optimizationPass(parameters, residuals, jacobians);
        return true;
    }

private:

    RenderPass& m_renderPass;
};

}


