//
// Created by janos on 05.02.20.
//

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/Texture.h>

namespace Mg = Magnum;

namespace TextureMapOptimization::Shaders {

class Diff : public Mg::GL::AbstractShaderProgram {
public:

    using Position = Mg::Shaders::Generic3D::Position;
    using TextureCoordinates = Mg::Shaders::Generic3D::TextureCoordinates;
    using TransformationMatrix = Mg::Shaders::Generic3D::TransformationMatrix;

    Diff();

    Diff& setRotation(const Mg::Vector3& rot);

    Diff& setTranslation(Mg::Vector3 const& translation);

    Diff& setProjectionMatrix(Mg::Matrix4 const& projection) {
        setUniform(m_projectionUniform, projection);
        return *this;
    }

    Diff& bindTexture(Mg::GL::Texture2D& texture) {
        texture.bind(0);
        return *this;
    }

private:
    Mg::Int m_rotationUniform;
    Mg::Int m_translationUniform;
    Mg::Int m_projectionUniform;
    Mg::Int m_textureSizeUniform;
};

}
