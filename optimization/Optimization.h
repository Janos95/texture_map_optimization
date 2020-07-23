//
// Created by janos on 03.02.20.
//


#pragma once

#include "KeyFrame.h"

#include "UniqueFunction.h"

#include <Corrade/Containers/Containers.h>

#include <ceres/problem.h>
#include <ceres/solver.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

class Viewer;

class TextureMapOptimization
{
public:
    TextureMapOptimization(Cr::Containers::Array<KeyFrame>&, Mg::GL::Mesh&);

    void setTexture(Mg::GL::Texture2D& texture) { m_texture = &texture; }
    void run(UniqueFunction<void()>&&);

private:

    void averageColors();




    Mg::GL::Mesh& m_mesh;
    Cr::Containers::Array<KeyFrame>& m_kfs;

    Mg::GL::Texture2D* m_texture = nullptr;
};

