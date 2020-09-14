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

#include <ceres/loss_function.h>

using namespace Magnum;
using namespace Corrade;

namespace TextureMapOptimization {

struct Callback : public ceres::IterationCallback {

    explicit Callback(UniqueFunction<ceres::CallbackReturnType()> cb_) : cb(std::move(cb_)) {}

    ceres::CallbackReturnType operator()(ceres::IterationSummary const& summary) override {
       return cb();
    }

    UniqueFunction<ceres::CallbackReturnType()> cb;
};

Optimization::Optimization(Cr::Containers::Array<KeyFrame>& kfs, Mg::Trade::MeshData& meshData) :
    m_kfs(kfs), m_renderPass(meshData, kfs)
{
}

ceres::TerminationType Optimization::run(UniqueFunction<bool()> vis) {

    //m_renderPass.setTexture(*m_texture);

    Callback callback{[this, &vis]{
        m_renderPass.averagingPass();
        if(vis()){
            return ceres::CallbackReturnType::SOLVER_CONTINUE;
        } else {
            return ceres::CallbackReturnType::SOLVER_ABORT;
        }
    }};
    std::vector<double*> parameterBlocks(m_kfs.size());
    for(std::size_t i = 0; i < m_kfs.size(); ++i) {
        m_kfs[i].compressPose();
        parameterBlocks[i] = m_kfs[i].pose6D.data();
    }

    ceres::Problem problem;
    problem.AddResidualBlock(new PhotometricCost{m_renderPass}, new ceres::TrivialLoss{}, parameterBlocks);

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::SPARSE_SCHUR;
    options.minimizer_progress_to_stdout = true;
    options.update_state_every_iteration = true;
    options.callbacks.push_back(&callback);

    ceres::Solver::Summary summary;

    ceres::Solve(options, &problem, &summary);

    return summary.termination_type;
}

}

