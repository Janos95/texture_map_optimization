//
// Created by janos on 05.02.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>

using namespace Magnum;

class InterpolateVerticesShader: public GL::AbstractShaderProgram {
public:
    using Position = GL::Attribute<0, Vector2>;
    using Vertex = GL::Attribute<0, Vector3>;

    explicit InterpolateVerticesShader();
};