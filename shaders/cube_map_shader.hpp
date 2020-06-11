//
// Created by janos on 10.06.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

struct CubeMapShader : public Mg::GL::AbstractShaderProgram {

    explicit CubeMapShader();
    explicit CubeMapShader(Mg::NoCreateT) : Mg::GL::AbstractShaderProgram{Mg::NoCreate} {}

    CubeMapShader& setTransformationMatrix(Mg::Matrix4 const& tf){
        setUniform(transformationMatrixUniform, tf);
        return *this;
    }

    CubeMapShader& setProjectionMatrix(Mg::Matrix4 const& proj){
        setUniform(projectionMatrixUniform, proj);
        return *this;
    }

    CubeMapShader& bindEquirectangularTexture(Mg::GL::Texture2D& texture){
        texture.bind(equirectanguleTextureUnit);
        return *this;
    }

    Mg::Int projectionMatrixUniform;
    Mg::Int transformationMatrixUniform;

    Mg::Int equirectanguleTextureUnit = 0;
};
