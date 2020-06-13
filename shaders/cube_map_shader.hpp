//
// Created by janos on 10.06.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/CubeMapTexture.h>
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

    enum class Phase : Mg::UnsignedInt
    {
        EquirectangularConversion,
        IrradianceConvolution,
        Prefilter
    };

    explicit CubeMapShader(Phase);
    explicit CubeMapShader(Mg::NoCreateT) : Mg::GL::AbstractShaderProgram{Mg::NoCreate} {}

    CubeMapShader& setTransformationMatrix(Mg::Matrix4 const& tf){
        setUniform(transformationMatrixUniform, tf);
        return *this;
    }

    CubeMapShader& setProjectionMatrix(Mg::Matrix4 const& proj){
        setUniform(projectionMatrixUniform, proj);
        return *this;
    }

    CubeMapShader& bindTexture(Mg::GL::Texture2D& texture){
        texture.bind(textureUnit);
        return *this;
    }

    CubeMapShader& bindTexture(Mg::GL::CubeMapTexture& texture){
        texture.bind(textureUnit);
        return *this;
    }

    Mg::Int projectionMatrixUniform;
    Mg::Int transformationMatrixUniform;

    Mg::Int textureUnit = 0;
};
