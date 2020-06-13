//
// Created by janos on 05.02.20.
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

struct PbrShader: public Mg::GL::AbstractShaderProgram {

    explicit PbrShader(uint32_t lightCount = 1);
    explicit PbrShader(Mg::NoCreateT) : Mg::GL::AbstractShaderProgram{Mg::NoCreate} {}

    PbrShader& setMetallness(float metallness){
        setUniform(metallicUniform, metallness);
        return *this;
    }
    PbrShader& setRoughness(float roughness){
        setUniform(roughnessUniform, roughness);
        return *this;
    }
    PbrShader& setAlbedo(Mg::Vector3 albedo){
        setUniform(albedoUniform, albedo);
        return *this;
    }
    PbrShader& setAmbientOcclusion(float ao){
        setUniform(ambientOcclusionUniform, ao);
        return *this;
    }
    PbrShader& setLightPositions(Cr::Containers::ArrayView<const Mg::Vector3> const& lightPos){
        setUniform(lightPositionUniform, lightPos);
        return *this;
    }
    PbrShader& setLightColors(Cr::Containers::ArrayView<const Mg::Vector3> const& lightColors){
        setUniform(lightColorsUniform, lightColors);
        return *this;
    }

    PbrShader& setProjectionMatrix(Mg::Matrix4 const& proj){
        setUniform(projectionMatrixUniform, proj);
        return *this;
    }
    PbrShader& setTransformationMatrix(Mg::Matrix4 const& view){
        setUniform(transformationMatrixUniform, view);
        return *this;
    }

    PbrShader& setNormalMatrix(Mg::Matrix3 const& normalMatrix){
        setUniform(normalMatrixUniform, normalMatrix);
        return *this;
    }

    PbrShader& bindIrradianceMap(Mg::GL::CubeMapTexture& colorMap){
        colorMap.bind(irradianceTextureUnit);
        return *this;
    }


    Mg::Int irradianceTextureUnit = 0;

    Mg::Int albedoUniform;
    Mg::Int metallicUniform;
    Mg::Int roughnessUniform;
    Mg::Int ambientOcclusionUniform;
    Mg::Int lightPositionUniform;
    Mg::Int lightColorsUniform;
    Mg::Int projectionMatrixUniform;
    Mg::Int transformationMatrixUniform;
    Mg::Int normalMatrixUniform;
};