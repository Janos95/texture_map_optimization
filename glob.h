//
// Created by jmeny on 08.11.19.
//

#pragma once

#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

std::vector<fs::path> glob(const fs::path& path, const std::string& regex);
