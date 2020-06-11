//
// Created by janos on 11.06.20.
//


#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

struct BackgroundShader : public Mg::GL::AbstractShaderProgram {

    explicit BackgroundShader();
    explicit BackgroundShader(Mg::NoCreateT) : Mg::GL::AbstractShaderProgram{Mg::NoCreate} {}

    BackgroundShader& setTransformationMatrix(Mg::Matrix4 const& tf){
        setUniform(transformationMatrixUniform, tf);
        return *this;
    }

    BackgroundShader& setProjectionMatrix(Mg::Matrix4 const& proj){
        setUniform(projectionMatrixUniform, proj);
        return *this;
    }

    BackgroundShader& bindCubeMapTexture(Mg::GL::CubeMapTexture& texture){
        texture.bind(cubeMapTextureUnit);
        return *this;
    }

    Mg::Int projectionMatrixUniform;
    Mg::Int transformationMatrixUniform;

    Mg::Int cubeMapTextureUnit = 0;
};
