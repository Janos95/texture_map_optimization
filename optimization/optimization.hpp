//
// Created by janos on 03.02.20.
//


#pragma once

#include "cost.hpp"

#include <Corrade/Containers/ArrayView.h>

#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>
#include <Magnum/PixelFormat.h>

#include <ceres/problem.h>
#include <ceres/solver.h>

#include <thread>

using namespace Magnum;
using namespace Corrade;

cv::Vec4d compressCameraMatrix(Matrix3& cameraMatrix);

Matrix4 projectionMatrixFromCameraMatrix(const Matrix3& cameraMatrix, float W, float H);

cv::Mat_<cv::Vec3f> computeInterpolatedMeshVertices(Trade::MeshData3D& meshData, const int H, const int W);

void visibleTextureCoords(GL::Mesh& mesh, const Matrix4& tf, const Matrix4& proj, const float threshold, cv::Mat_<cv::Vec2i>& coords);

class TextureMapOptimization
{
    TextureMapOptimization(Trade::MeshData3D& meshData, std::vector<Frame>&, Matrix3&, Vector2i, float);

    cv::Mat run(Vector2i res, bool vis = false);


private:

    struct VisibilityInformation
    {
        int x,y;
        std::vector<PhotometricCost*> photometricCosts;
    };

    ceres::Problem m_problem;
    std::vector<VisibilityInformation> m_visibility;
    cv::Mat_<cv::Vec3d> m_texture;
    std::vector<Frame> m_frames;
    cv::Vec4d m_camera;

    std::unique_ptr<Viewer> m_viewer;

    //This is weird. Ideally we would store the compiled mesh
    //but this is a bit difficult since we need to share it
    //between differen gl contexts, so for now we just compile
    //the mesh data twice.
    Trade::MeshData3D& m_meshData;

    struct UpdateTexture : public ceres::IterationCallback
    {
        cv::Mat_<cv::Vec3f> tex;
        std::vector<Frame>& frames;
        std::vector<VisibilityInformation>& vis;

        ceres::CallbackReturnType operator()(const ceres::IterationSummary& summary) override{

            for(const auto& [x, y, visibleImages] : vis){
                tex(y,x) = cv::Vec3f(.0f,.0f,.0f);
                for(const auto& photoCost : visibleImages){
                    tex(y,x) += frames[photoCost->idx].image(y,x);
                }
                tex(y,x) *= 1.f/visibleImages.size();
            }
            return ceres::CallbackReturnType::SOLVER_CONTINUE;
        }
    };


    struct UpdateScene
    {
        cv::Mat_<cv::Vec3f> tex;

        void operator()(Scene& scene) const{
            auto* obj = scene.getObject("mesh");
            CORRADE_INTERNAL_ASSERT(obj != nullptr);

            int H = tex.rows, W = tex.cols;
            Containers::Array<Vector3> imageData(W*H); //TODO: pretty unnecessary copy...
            std::transform(tex.begin(), tex.end(), imageData.begin()
                    , [](const auto& v){ return Vector3(v[0],v[1], v[2]); });
            ImageView2D image(PixelFormat::RGB32F, {H, W}, imageData);

            GL::Texture2D texture;
            texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
                    .setMagnificationFilter(GL::SamplerFilter::Linear)
                    .setMinificationFilter(GL::SamplerFilter::Linear)
                    .setStorage(1, GL::textureFormat(image.format()), image.size())
                    .setSubImage(0, {}, image);
            obj->texture = std::move(texture);
        }
    };

    std::unique_ptr<UpdateTexture> m_updateTexture;
};

