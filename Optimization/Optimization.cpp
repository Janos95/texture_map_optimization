//
// Created by janos on 05.02.20.
//

#include "Optimization.h"
#include "Cost.h"
#include "ScopedTimer.h"
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
        for(auto& kf : kfs)
            kf.uncompressPose();

        renderPass.averagingPass();
        if(cb()){
            return ceres::CallbackReturnType::SOLVER_CONTINUE;
        } else {
            return ceres::CallbackReturnType::SOLVER_ABORT;
        }
    }};

    ceres::Problem problem;

    for(std::size_t i = 0; i < kfs.size(); ++i) {
        problem.AddResidualBlock(new PhotometricCost{i, renderPass}, new ceres::TrivialLoss{}, kfs[i].tf6D.data());
    }

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::SPARSE_SCHUR;
    options.minimizer_progress_to_stdout = true;
    options.update_state_every_iteration = true;
    options.callbacks.push_back(&callback);

    ceres::Solver::Summary summary;

    ceres::Solve(options, &problem, &summary);

    Debug{} << summary.BriefReport().c_str();

    return summary.termination_type != ceres::USER_FAILURE;
}

}

