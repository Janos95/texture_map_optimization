//
// Created by janos on 03.02.20.
//


#pragma once

#include "viewer.hpp"
#include "cost.hpp"
#include "render_pass.hpp"
#include "render_target.hpp"
#include "../mesh.hpp"

#include <Corrade/Containers/ArrayView.h>

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>
#include <Magnum/PixelFormat.h>

#include <ceres/problem.h>
#include <ceres/solver.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <thread>

using namespace Magnum;
using namespace Corrade;

struct Frame
{
    Frame(cv::Mat_<cv::Vec3f> img, Matrix4 t): image(img), tf(t)
    {
        //some blur to remove noise
        cv::GaussianBlur(img, img, cv::Size(3,3), 0, 0, cv::BORDER_DEFAULT);

        //compute numeric gradients using scharr filter
        std::vector<cv::Mat_<float>> rgb, channelsDx(3), channelsDy(3);
        cv::split(img, rgb);
        for(int i = 0; i < 3; ++i){
            cv::Mat_<float> cdx, cdy;
            cv::Scharr(rgb[i], channelsDx[i], -1, 1, 0);
            cv::Scharr(rgb[i], channelsDy[i], -1, 0, 1);
        }
        cv::merge(channelsDx, xderiv);
        cv::merge(channelsDy, yderiv);

        //compress rigid transformation to 6 degrees of freedom
        cv::Matx33d rot;
        std::copy_n(tf.rotation().data(), 9, rot.val);
        cv::Rodrigues(rot, rvec);
        std::copy_n(tf.translation().data(), 3, tvec.val);

    }
    cv::Mat_<cv::Vec3f> image, xderiv, yderiv;
    Matrix4 tf;
    cv::Vec3d rvec, tvec;
};

cv::Vec4d compressCameraMatrix(Matrix3& cameraMatrix)
{
    cv::Vec4d compressed;
    compressed[0] = cameraMatrix[0][0];
    compressed[1] = cameraMatrix[1][1];
    compressed[2] = cameraMatrix[2][0];
    compressed[3] = cameraMatrix[2][1];
    return compressed;
}





void filterDepthDiscontinuities(MutableImageView2D img, ImageView2D depth, float threshold);
cv::Mat_<cv::Vec3f> computeInterpolatedMeshVertices();

class TextureMapOptimization
{
    TextureMapOptimization(Mesh&, std::vector<Frame>&, Matrix3&, Vector2i, float);

    cv::Mat run(Vector2i res, bool vis = false){

        ceres::Solver::Options options;
        options.linear_solver_type = ceres::SPARSE_SCHUR;
        options.minimizer_progress_to_stdout = true;
        options.update_state_every_iteration = true;
        options.callbacks.push_back(m_updateTexture.get());
        ceres::Solver::Summary summary;

        std::thread t([&] {
            ceres::Solve(options, &m_problem, &summary);
        });

        if(vis){
            m_scene.reset(new Scene)
            m_viewer = std::make_unique<Viewer>(m_scene);
            m_viewer.callbacks.emplace_back(UpdateScene{m_texture});
            m_viewer.exec();
        }
        t.join();

        return m_texture.clone();
    }


private:

    struct VisibilityInformation
    {
        int x,y;
        std::vector<PhotometricCost> photometricCosts;
    };

    ceres::Problem m_problem;
    std::vector<VisibilityInformation> m_visibility;
    cv::Mat_<cv::Vec3d> m_texture;
    std::vector<Frame> m_frames;
    cv::Vec4d m_camera;

    std::unique_ptr<Viewer> m_viewer;
    Scene m_scene;

    struct UpdateTexture : public ceres::IterationCallback
    {
        cv::Mat_<cv::Vec3f> tex;
        std::vector<Frame>& frames;
        std::vector<VisibilityInformation>& vis;

        ceres::CallbackReturnType operator()(const ceres::IterationSummary& summary) override{

            for(const auto& [x, y, visibleImages] : vis){
                tex(y,x) = cv::Vec3f(.0f,.0f,.0f);
                for(const int idx : visibleImages){
                    tex(y,x) += frames[idx].image(y,x);
                }
                tex(y,x) *= 1.f/visibleImages.size();
            }
        }
    };


    struct UpdateScene
    {
        cv::Mat_<cv::Vec3f> tex;

        void operator()(Scene& scene) const{
            auto* obj = scene.getObject("mesh");
            CORRADE_INTERNAL_ASSERT(obj != nullptr);

            int H = tex.rows, W = tex.cols;
            ImageView2D image(PixelFormat::RGB32F, {H, W}, Containers::ArrayView<float>((float*)tex.data, 3*H*W));

            GL::Texture2D texture;
            texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
                    .setMagnificationFilter(GL::SamplerFilter::Linear)
                    .setMinificationFilter(GL::SamplerFilter::Linear)
                    .setStorage(1, GL::textureFormat(image.format()), image.size())
                    .setSubImage(0, {}, image);
            obj->texture = texture;
        }
    };

    std::unique_ptr<UpdateTexture> m_updateTexture;
};

