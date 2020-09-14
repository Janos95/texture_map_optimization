//
// Created by janos on 03.02.20.
//


#pragma once

#include "KeyFrame.h"
#include "RenderPass.h"
#include "Utilities.h"

#include <Corrade/Containers/Containers.h>

#include <ceres/problem.h>
#include <ceres/solver.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

namespace TextureMapOptimization {

class Optimization {
public:

    Optimization(Cr::Containers::Array<KeyFrame>&, Mg::Trade::MeshData&);

    void setTexture(Mg::GL::Texture2D& texture) { m_renderPass.setTexture(texture); }
    void mapTexture() { m_renderPass.averagingPass(); }

    ceres::TerminationType run(UniqueFunction<bool()>);

private:

    Cr::Containers::Array<KeyFrame>& m_kfs;
    RenderPass m_renderPass;

    //Mg::GL::Texture2D* m_texture = nullptr;
};

}

