//
// Created by janos on 2/8/20.
//

#pragma once

#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/ImageData.h>

#include <string>

Corrade::Containers::Optional<Magnum::Trade::MeshData> loadMeshData(const std::string& path);
Corrade::Containers::Optional<Magnum::Trade::ImageData2D> loadHdrImage(const std::string& path);
Corrade::Containers::Array<Magnum::Trade::ImageData2D> loadImages(const std::string& dir);

