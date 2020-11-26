//
// Created by janos on 7/22/20.
//

#pragma once

#include "Types.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>
#include <Corrade/Containers/StaticArray.h>

namespace TextureMapOptimization {

/**
 * Compress a 4x4 rigid transformation matrix into a
 * six dimensional angle axis + translation.
 * @param Transformation to compress
 * @return static array of length 6. First three entries
 * represent the rotation axis and the last three components
 * the translation. The length of the rotation axis encodes
 * the angle of the rotation.
 */
StaticArray<6,double> compressTransformationMatrix(const Matrix4& tf);

/**
 * Does the inverse to compressTransformationMatrix.
 * @return a 4x4 rigid transformation matrix.
 */
Matrix4 uncompress6DTransformation(const StaticArrayView<6, const double>&);

struct KeyFrame {
    GL::Texture2D image;

    Matrix4 tf;
    StaticArray<6, double> tf6D;

    /**
     * shorthand for calling compressTransformationMatrix on member data
     */
    void compressPose();

    /**
     * shorthand for calling uncomporess6DTransformation on member data
     */
    void uncompressPose();
};

}

