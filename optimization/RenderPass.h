//
// Created by janos on 7/23/20.
//

#ifndef TEXTUREOPTIMIZATION_RENDERPASS_H
#define TEXTUREOPTIMIZATION_RENDERPASS_H

#include "KeyFrame.h"
#include "Diff.h"
#include "ChainRule.h"
#include "Reduction.h"
#include "TextureCoordinates.h"
#include "Remap.h"
#include "Combine.h"

#include <Magnum/Magnum.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/TextureArray.h>
#include <Magnum/GL/Mesh.h>

namespace Mg = Magnum;
namespace Cr = Corrade;

struct RenderPass {
public:

    enum class FramebufferMode : Mg::UnsignedInt {
        OptimizationTexture,
        GroundTruthImage
    };

    RenderPass(Mg::GL::Mesh&, Mg::GL::Texture2D&, Mg::Vector2i);

    struct GradientResult {
        Mg::Vector3 rotationGrad;
        Mg::Vector3 translationGrad;
        float cost;
    };

    GradientResult computeGradient(KeyFrame&, Mg::Vector3 const&, Mg::Vector3 const&);

    void averageImagesIntoTexture(Cr::Containers::Array<KeyFrame>&);

private:

    Mg::GL::Framebuffer m_fb{Mg::NoCreate};

    Mg::GL::Mesh& m_mesh;
    Mg::GL::Texture2D& m_texture;

    shaders::Diff m_diff;
    shaders::ChainRule m_chainRule;
    shaders::Reduction m_reduction{3};

    shaders::TextureCoordinates m_texCoordsShader;
    shaders::Remap m_remap;
    shaders::Combine m_combine;

    Mg::GL::Texture2D m_depth, m_cost, m_color;
    Mg::GL::Texture2D m_gradRot, m_gradTrans;

    Mg::GL::Texture2D m_mipMapRotations;
    Mg::GL::Texture2D m_mipMapTranslations;
    Mg::GL::Texture2D m_mipMapCost;
    Mg::GL::Texture3D m_jacs;

    Mg::GL::Texture2D m_red;
    Mg::GL::Texture2D m_green;
    Mg::GL::Texture2D m_blue;
    Mg::GL::Texture2D m_count;
    Mg::GL::Texture2D m_texCoords;

    Mg::Vector2i m_imageSize;
    Mg::Vector2i m_textureSize;

    FramebufferMode m_fbMode = FramebufferMode::GroundTruthImage;
};

#endif //TEXTUREOPTIMIZATION_RENDERPASS_H
