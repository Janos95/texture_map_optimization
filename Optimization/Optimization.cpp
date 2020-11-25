//
// Created by janos on 05.02.20.
//

#include "Optimization.h"
#include "Cost.h"
#include "ScopedTimer/ScopedTimer.h"
#include "UniqueFunction.h"

#include <Magnum/Math/Color.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/PixelFormat.h>

#include <ceres/loss_function.h>
#include <ceres/problem.h>
#include <ceres/solver.h>

namespace TextureMapOptimization {

struct Callback : public ceres::IterationCallback {

    explicit Callback(UniqueFunction<ceres::CallbackReturnType()> cb_) : cb(std::move(cb_)) {}

    ceres::CallbackReturnType operator()(ceres::IterationSummary const& summary) override {
       return cb();
    }

    UniqueFunction<ceres::CallbackReturnType()> cb;
};

bool runOptimization(
            Array<KeyFrame>& kfs,
            GL::Texture2D& texture,
            RenderPass& renderPass,
            UniqueFunction<bool()>&& cb) {

    //m_renderPass.setTexture(*m_texture);

    Callback callback{[&]{
        renderPass.averagingPass();
        if(cb()){
            return ceres::CallbackReturnType::SOLVER_CONTINUE;
        } else {
            return ceres::CallbackReturnType::SOLVER_ABORT;
        }
    }};

    std::vector<double*> parameterBlocks(kfs.size());
    for(std::size_t i = 0; i < kfs.size(); ++i) {
        kfs[i].compressPose();
        parameterBlocks[i] = kfs[i].pose6D.data();
    }

    ceres::Problem problem;
    problem.AddResidualBlock(new PhotometricCost{renderPass}, new ceres::TrivialLoss{}, parameterBlocks);

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::SPARSE_SCHUR;
    options.minimizer_progress_to_stdout = true;
    options.update_state_every_iteration = true;
    options.callbacks.push_back(&callback);

    ceres::Solver::Summary summary;

    ceres::Solve(options, &problem, &summary);

    return summary.termination_type != ceres::USER_FAILURE;
}

}

