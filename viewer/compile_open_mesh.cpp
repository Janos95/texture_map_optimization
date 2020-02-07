//
// Created by janos on 31.01.20.
//


#include "../mesh.hpp"


#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/MeshTools/GenerateNormals.h>
#include <Magnum/MeshTools/Duplicate.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Shaders/Generic.h>
#include <iostream>

using namespace Magnum;
using namespace Corrade;

GL::Mesh compileOpenMesh(const Mesh& mesh, MeshTools::CompileFlags flags = {}) {
    using MeshTools::CompileFlag;


    GL::Mesh glmesh;
    const auto numVertices = mesh.n_vertices();
    const bool isIndexed = mesh.n_faces() > 0;
    std::vector<UnsignedInt> indices;

    if(isIndexed){
        indices.reserve(mesh.n_faces());
        for(auto it = mesh.faces_begin(); it != mesh.faces_end(); ++it){
            for(const auto& v: it->vertices()) {
                indices.push_back(v.idx());
            }
        }
        glmesh.setPrimitive(MeshPrimitive::Triangles);
    }
    else
        glmesh.setPrimitive(MeshPrimitive::Points);

    const bool generateNormals = flags & (CompileFlag::GenerateFlatNormals|CompileFlag::GenerateSmoothNormals) && isIndexed;

    /* Decide about stride and offsets */
    UnsignedInt stride = sizeof(Shaders::Generic3D::Position::Type);
    const UnsignedInt normalOffset = sizeof(Shaders::Generic3D::Position::Type);
    UnsignedInt textureCoordsOffset = sizeof(Shaders::Generic3D::Position::Type);
    UnsignedInt colorsOffset = sizeof(Shaders::Generic3D::Position::Type);
    if(mesh.has_vertex_normals() || generateNormals) {
        stride += sizeof(Shaders::Generic3D::Normal::Type);
        textureCoordsOffset += sizeof(Shaders::Generic3D::Normal::Type);
        colorsOffset += sizeof(Shaders::Generic3D::Normal::Type);
    }
    if(mesh.has_vertex_texcoords2D()) {
        stride += sizeof(Shaders::Generic3D::TextureCoordinates::Type);
        colorsOffset += sizeof(Shaders::Generic3D::TextureCoordinates::Type);
    }
    if(mesh.has_vertex_colors())
        stride += sizeof(Shaders::Generic3D::Color4::Type);

    /* Create vertex buffer */
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array};
    GL::Buffer vertexBufferRef = GL::Buffer::wrap(vertexBuffer.id(), GL::Buffer::TargetHint::Array);

    /* Indirect reference to the mesh data -- either directly the original mesh
       data or processed ones */
    Containers::StridedArrayView1D<const Mesh::Point> positions;
    Containers::StridedArrayView1D<const Mesh::Point> normals;
    Containers::StridedArrayView1D<const Mesh::TexCoord2D> textureCoords2D;
    Containers::StridedArrayView1D<const Mesh::Color> colors;
    bool useIndices; /**< @todo turn into a view once compressIndices() takes views */

    /* If the mesh has no normals, we want to generate them and the mesh is an
       indexed triangle mesh, duplicate all attributes, otherwise just
       reference the original data */
    Containers::Array<Mesh::Point> positionStorage;
    Containers::Array<Mesh::Point> normalStorage;
    Containers::Array<Mesh::TexCoord2D> textureCoords2DStorage;
    Containers::Array<Mesh::Color> colorStorage;
    if(generateNormals) {
        /* If we want flat normals and the mesh is indexed, duplicate all
           attributes */
        if(flags & CompileFlag::GenerateFlatNormals && isIndexed) {
            positionStorage = MeshTools::duplicate<UnsignedInt, Vector3>(
                    Containers::stridedArrayView<const UnsignedInt>(indices),
                    Containers::ArrayView<const Vector3>(mesh.points(), numVertices));
            positions = Containers::arrayView(positionStorage);
            if(mesh.has_vertex_texcoords2D()) {
                textureCoords2DStorage = MeshTools::duplicate<UnsignedInt, Vector2>(
                        Containers::stridedArrayView<const UnsignedInt>(indices),
                        Containers::ArrayView<const Vector2>(mesh.texcoords2D(), numVertices));
                textureCoords2D = Containers::arrayView(textureCoords2DStorage);
            }
            if(mesh.has_vertex_colors()) {
                colorStorage = MeshTools::duplicate<UnsignedInt, Color4>(
                        Containers::stridedArrayView<const UnsignedInt>(indices),
                        Containers::ArrayView<const Color4>(mesh.vertex_colors(), numVertices));
                colors = Containers::arrayView(colorStorage);
            }
        } else {
            positions = Containers::ArrayView(mesh.points(), numVertices);
            if(mesh.has_vertex_texcoords2D())
                textureCoords2D = Containers::ArrayView(mesh.texcoords2D(), numVertices);
            if(mesh.has_vertex_colors())
                colors = Containers::ArrayView(mesh.vertex_colors(), numVertices);
        }

        if(flags & CompileFlag::GenerateFlatNormals || !isIndexed) {
            normalStorage = MeshTools::generateFlatNormals(positions);
            useIndices = false;
        } else {
            normalStorage = MeshTools::generateSmoothNormals<UnsignedInt>(Containers::stridedArrayView(indices), positions);
            useIndices = true;
        }

        normals = Containers::arrayView(normalStorage);

    } else {
        positions = Containers::ArrayView(mesh.points(), numVertices);
        if(mesh.has_vertex_normals()) normals = Containers::ArrayView(mesh.vertex_normals(), numVertices);
        if(mesh.has_vertex_texcoords2D()) textureCoords2D = Containers::ArrayView(mesh.texcoords2D(), numVertices);
        if(mesh.has_vertex_colors()) colors = Containers::ArrayView(mesh.vertex_colors(), numVertices);
        useIndices = isIndexed;
    }

    /* Interleave positions and put them in with ownership transfer, use the
       ref for the rest */
    Containers::Array<char> data = MeshTools::interleave(
            positions,
            stride - sizeof(Shaders::Generic3D::Position::Type));
    glmesh.addVertexBuffer(std::move(vertexBuffer), 0,
                         Shaders::Generic3D::Position(),
                         stride - sizeof(Shaders::Generic3D::Position::Type));

    /* Add also normals, if present */
    if(normals) {
        MeshTools::interleaveInto(data,
                                  normalOffset,
                                  normals,
                                  stride - normalOffset - sizeof(Shaders::Generic3D::Normal::Type));
        glmesh.addVertexBuffer(vertexBufferRef, 0,
                             normalOffset,
                             Shaders::Generic3D::Normal(),
                             stride - normalOffset - sizeof(Shaders::Generic3D::Normal::Type));
    }

    /* Add also texture coordinates, if present */
    if(textureCoords2D) {
        MeshTools::interleaveInto(data,
                                  textureCoordsOffset,
                                  textureCoords2D,
                                  stride - textureCoordsOffset - sizeof(Shaders::Generic3D::TextureCoordinates::Type));
        glmesh.addVertexBuffer(vertexBufferRef, 0,
                             textureCoordsOffset,
                             Shaders::Generic3D::TextureCoordinates(),
                             stride - textureCoordsOffset - sizeof(Shaders::Generic3D::TextureCoordinates::Type));
    }

    /* Add also colors, if present */
    if(colors) {
        MeshTools::interleaveInto(data,
                                  colorsOffset,
                                  colors,
                                  stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
        glmesh.addVertexBuffer(vertexBufferRef, 0,
                             colorsOffset,
                             Shaders::Generic3D::Color4(),
                             stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
    }

    /* Fill vertex buffer with interleaved data */
    vertexBufferRef.setData(data, GL::BufferUsage::StaticDraw);

    /* If indexed (and the mesh didn't have the vertex data duplicated for flat
       normals), fill index buffer and configure indexed mesh */
    if(useIndices) {
        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(indices);

        GL::Buffer indexBuffer{GL::Buffer::TargetHint::ElementArray};
        indexBuffer.setData(indexData, GL::BufferUsage::StaticDraw);
        glmesh.setCount(indices.size())
                .setIndexBuffer(std::move(indexBuffer), 0, indexType, indexStart, indexEnd);

        /* Else set vertex count */
    } else glmesh.setCount(positions.size());

    return glmesh;
}
