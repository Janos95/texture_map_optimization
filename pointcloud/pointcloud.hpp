//
// Created by janos on 12.01.20.
//

#pragma once

#include <Magnum/GL/AbstractObject.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/AbstractShaderProgram.h>

class PointCloud : public Magnum::GL::AbstractObject
{
    friend Magnum::GL::AbstractShaderProgram;
public:
    explicit PointCloud(const Magnum::Trade::MeshData3D& meshData);

    PointCloud& draw(Magnum::GL::AbstractShaderProgram& shader);


private:

    Magnum::GL::Buffer m_points;
    Magnum::GL::Buffer m_colors;

    std::size_t m_count;
};

