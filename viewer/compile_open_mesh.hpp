//
// Created by janos on 31.01.20.
//

#pragma once

#include "../mesh.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/MeshTools/Compile.h>

Magnum::GL::Mesh compileOpenMesh(const Mesh& mesh, Magnum::MeshTools::CompileFlags flags = {});