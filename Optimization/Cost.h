//
// Created by janos on 27.11.19.
//

#pragma once

#include "RenderPass.h"

#include <ceres/sized_cost_function.h>

namespace TextureMapOptimization {

class PhotometricCost : public ceres::SizedCostFunction<1, 6> {
public:

    explicit PhotometricCost(size_t idx, RenderPass& renderPass) : m_kfIdx(idx), m_renderPass(renderPass) {}

    bool Evaluate(double const* const* parameters,
                  double* residuals,
                  double** jacobians) const override {
        double* grad = jacobians ? *jacobians : nullptr ;
        m_renderPass.optimizationPass(m_kfIdx, *parameters, *residuals, grad);
        return true;
    }

private:

    size_t m_kfIdx;
    RenderPass& m_renderPass;
};

}


