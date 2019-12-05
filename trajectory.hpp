//
// Created by jmeny on 05.12.19.
//

#pragma once

#include <Eigen/Geometry>


struct FramedTransformation {
    int id1_;
    int id2_;
    int frame_;
    Eigen::Matrix4d transformation_;

    FramedTransformation(const int id1, const int id2, const int f, const Eigen::Matrix4d& t)
            : id1_(id1), id2_(id2), frame_(f), transformation_(t) {}

};

struct Trajectory {
    std::vector<FramedTransformation> data_;
    int index_;

    void LoadFromFile(const std::string& filename) {
        data_.clear();
        index_ = 0;
        int id1, id2, frame;
        Eigen::Matrix4d trans;
        FILE *f = fopen(filename.c_str(), "r");
        if (f != nullptr) {
            char buffer[1024];
            while (fgets(buffer, 1024, f) != nullptr) {
                if (strlen(buffer) > 0 && buffer[0] != '#') {
                    sscanf(buffer, "%d %d %d", &id1, &id2, &frame);
                    fgets(buffer, 1024, f);
                    sscanf(buffer, "%lf %lf %lf %lf", &trans(0, 0), &trans(0, 1), &trans(0, 2), &trans(0, 3));
                    fgets(buffer, 1024, f);
                    sscanf(buffer, "%lf %lf %lf %lf", &trans(1, 0), &trans(1, 1), &trans(1, 2), &trans(1, 3));
                    fgets(buffer, 1024, f);
                    sscanf(buffer, "%lf %lf %lf %lf", &trans(2, 0), &trans(2, 1), &trans(2, 2), &trans(2, 3));
                    fgets(buffer, 1024, f);
                    sscanf(buffer, "%lf %lf %lf %lf", &trans(3, 0), &trans(3, 1), &trans(3, 2), &trans(3, 3));
                    data_.emplace_back(id1, id2, frame, trans);
                }
            }
            fclose(f);
        }
    }

    void SaveToFile(std::string filename) {
        FILE *f = fopen(filename.c_str(), "w");
        for (auto& framedTF: data_) {
            Eigen::Matrix4d &trans = framedTF.transformation_;
            fprintf(f, "%d\t%d\t%d\n", framedTF.id1_, framedTF.id2_, framedTF.frame_);
            fprintf(f, "%.8f %.8f %.8f %.8f\n", trans(0, 0), trans(0, 1), trans(0, 2), trans(0, 3));
            fprintf(f, "%.8f %.8f %.8f %.8f\n", trans(1, 0), trans(1, 1), trans(1, 2), trans(1, 3));
            fprintf(f, "%.8f %.8f %.8f %.8f\n", trans(2, 0), trans(2, 1), trans(2, 2), trans(2, 3));
            fprintf(f, "%.8f %.8f %.8f %.8f\n", trans(3, 0), trans(3, 1), trans(3, 2), trans(3, 3));
        }
        fclose(f);
    }
};
