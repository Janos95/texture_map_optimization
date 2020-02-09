//
// Created by janos on 27.11.19.
//

#pragma once

#include <ceres/sized_cost_function.h>
#include <ceres/cost_function_to_functor.h>
#include <ceres/rotation.h>
#include <ceres/ceres.h>


#include <opencv2/core.hpp>

#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>


using namespace Magnum;


struct PhotometricCost : public ceres::SizedCostFunction<3,1,1>
{
    using ImgT = cv::Mat_<cv::Vec3f>;

    PhotometricCost(const int idx_, const ImgT& img_, const ImgT& dx_, const ImgT& dy_, const cv::Vec3f& c_):
        idx(idx_),
        img(img_),
        xderiv(dx_),
        yderiv(dy_),
        c(c_)
    {
    }

    bool Evaluate(double const* const* parameters,
                          double* residuals,
                          double** jacobians) const override{
        auto x = static_cast<int>(*parameters[0]);
        auto y = static_cast<int>(*parameters[1]);

        auto& r = img(y,x);
        for(int i = 0; i < 3; ++i)
            residuals[i] = r[i] - c[i];

        if (jacobians) {
            auto dx = xderiv(y,x);
            auto dy = yderiv(y,x);
            for(int i = 0; i < 3; ++i){
                jacobians[0][i] = dx[i];
                jacobians[1][i] = dy[i];
            }
        }

        return true;
    }

    int idx;
    const cv::Mat_<cv::Vec3f>& img, xderiv, yderiv;
    const cv::Vec3f& c;
};

struct TexturePixelFunctor
{
    TexturePixelFunctor(const cv::Vec3f& p, PhotometricCost* cost): ip(p), photoCost(cost)
    {
    }

    template <typename T>
    bool operator()(const T* rvec, const T* tvec, const T* camera, T* residuals) const {
        T p[3] = {T(ip[0]), T(ip[1]), T(ip[2])};
        std::fill_n(residuals, 3, T(0));

        ceres::AngleAxisRotatePoint(rvec, p, p);
        auto x = p[0] + tvec[0];
        auto y = p[1] + tvec[1];
        auto z = p[2] + tvec[2];

        auto u = camera[0] * x / z + camera[2];
        auto v = camera[1] * x / z + camera[3];

        T r[3];
        if(!photoCost(&u, &v, r))
            return false;

        residuals[0] += r[0];
        residuals[1] += r[1];
        residuals[2] += r[2];

        return true;
    }

    const cv::Vec3f ip;
    const ceres::CostFunctionToFunctor<3, 1, 1> photoCost;
};


