//
// Created by jmeny on 05.12.19.
//

#include "glob.h"
#include "trajectory.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <fmt/format.h>


struct Filter
{
    bool operator()(const int i)
    {
        return i % 10 == 0;
    }
};


template<typename R, typename F>
auto filter_by_index(const R& range, F f)
{
    R filtered;
    int count = 0;
    for (auto it = range.begin(); it != range.end(); ++it) {
        if(f(count++))
            filtered.push_back(*it);
    }
    return filtered;
}



template<class F>
std::vector<cv::Mat> loadImages(const std::string& path, F f)
{
    auto imagePaths = glob(path, ".*\\.png");
    std::sort(imagePaths.begin(), imagePaths.end());
    imagePaths = filter_by_index(imagePaths, f);

    std::vector<cv::Mat> images(imagePaths.size());

#pragma omp parallel for
    for(std::size_t i = 0; i < imagePaths.size(); ++i)
    {
        images[i] = cv::imread(imagePaths[i].string());
    }

    return images;
}

template<class F>
std::vector<Eigen::Affine3d> loadPoses(const std::string& path, F f)
{

    Trajectory trajectory;
    trajectory.LoadFromFile(path);

    std::vector<Eigen::Affine3d> poses; poses.reserve(trajectory.data_.size());

    fmt::print("Loading Poses");

    for(const auto& framedTf: trajectory.data_)
    {
        Eigen::Affine3d tf;
        tf.matrix() = framedTf.transformation_;

        poses.push_back(tf);
    }

    fmt::print("\rLoading Poses done!\n");

    return filter_by_index(poses, f);
}

