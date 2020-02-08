//
// Created by janos on 2/8/20.
//

#pragma once

#include <Magnum/Trade/MeshData3D.h>

#include <string>

Corrade::Containers::Optional<Magnum::Trade::MeshData3D> loadMeshData(const std::string& path);

