//
// Created by janos on 11/15/19.
//

#pragma once

#include <Corrade/Containers/GrowableArray.h>

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>

#include <fstream>

using namespace Magnum;
using namespace Corrade;

auto loadPoses(const std::string& path)
{
    std::ifstream file(path);
    Containers::Array<Matrix4d> tfs;
    int dummy;
    while(file >> dummy >> dummy >> dummy)  {
        Matrix4d tf;
        file >> tf[0][0] >> tf[1][0] >> tf[2][0] >> tf[3][0] >>
                tf[0][1] >> tf[1][1] >> tf[2][1] >> tf[3][1] >>
                tf[0][2] >> tf[1][2] >> tf[2][2] >> tf[3][2] >>
                tf[0][3] >> tf[1][3] >> tf[2][3] >> tf[3][3];
        Containers::arrayAppend(tfs, tf);
    }

    return tfs;
}