//
// Created by janos on 2/8/20.
//

#pragma once

#include "Types.h"
#include <Magnum/Trade/Trade.h>

namespace Mg = Magnum;

namespace TextureMapOptimization {

Optional<Mg::Trade::MeshData>
loadMeshData(const StringView& path);

Array<Mg::Trade::ImageData2D>
loadImages(const StringView& dir);

Array<Matrix4> loadPoses(const StringView& path);

}

