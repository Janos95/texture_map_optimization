//
// Created by janos on 26.05.20.
//

#pragma once

#include <Magnum/SceneGraph/SceneGraph.h>
#include <Corrade/Containers/Containers.h>

namespace Cr = Corrade;
namespace Mg = Magnum;
namespace Containers = Corrade::Containers;

using Object = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
using Scene = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;
using Drawable = Magnum::SceneGraph::Drawable3D;
using DrawableGroup = Magnum::SceneGraph::DrawableGroup3D;
