//
// Created by janos on 12.01.20.
//

#include "pointcloud.hpp"

#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Utility/Assert.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/AbstractShaderProgram.h>

using namespace Magnum;

PointCloud::PointCloud(const Magnum::Trade::MeshData3D& meshData) {
    CORRADE_CONSTEXPR_ASSERT(meshData.positionArrayCount() == 1, "only one mesh suppprted");
    auto& vertices = meshData.positions(0);
    CORRADE_CONSTEXPR_ASSERT(meshData.colorArrayCount() == 1, "at most one color vector");
    auto& colors = meshData.colors(0);
    m_count = vertices.size();
    m_points.setData(vertices, Magnum::GL::BufferUsage::StaticDraw);
    m_colors.setData(colors, Magnum::GL::BufferUsage::StaticDraw);
}


PointCloud::setUniformColor(const Math::Color4<float>& color)
{
    m_colors.setData(std::vector{m_count, color}, Magnum::GL::BufferUsage::StaticDraw);
}



PointCloud& PointCloud::draw(Magnum::GL::AbstractShaderProgram& shader)
{
    if(!m_count)
        return *this;

    shader.use();

}
