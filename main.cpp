
#include "color_handle.hpp"
#include "cut_mesh_rosy.hpp"



//
//#include <igl/readOFF.h>
//#include <igl/readPLY.h>
//#include <igl/writePLY.h>
//#include <igl/opengl/glfw/Viewer.h>
//#include <igl/jet.h>
//#include <igl/doublearea.h>
//#include <igl/decimate.h>
//
//
//#include <ceres/problem.h>
//#include <ceres/solver.h>
//#include <ceres/numeric_diff_options.h>
//#include <ceres/gradient_checker.h>
//#include <ceres/loss_function.h>
//
//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
//
//#include <thread>
//#include <algorithm>
//#include <iostream>
//#include <mutex>
//
//
//
//int main(int argc, char** argv) {
//
//    Eigen::MatrixXd V;
//    Eigen::MatrixXi F;
//
//    Eigen::VectorXi J,I;
//
//    igl::readPLY("/home/jmeny/texture_map_optimization/crane_simple.ply", V, F);
//
//    igl::decimate(V,F,10000,V,F,J,I);
//
//    std::cout << V.rows() << std::endl;
//    std::cout << F.rows() << std::endl;
//
//
//    //Eigen::VectorXd U = initPhaseField(V, F);
//    Eigen::MatrixXd C(V.rows(), 3);
//
//
////    ceres::Problem problem;
////
////    auto* interface = new PhotometricCost(V, F);
////
////    Eigen::VectorXd A;
////    igl::doublearea(V, F, A);
////
////
////    problem.AddResidualBlock(interface, new ceres::CauchyLoss(0.1), C.data());
////
////
////    ColorHandle handle(C);
////
////    // Run the solver!
////    ceres::Solver::Options options;
////    options.linear_solver_type = ceres::ITERATIVE_SCHUR;
////    options.minimizer_progress_to_stdout = true;
////    options.callbacks.push_back(&handle);
////    options.max_num_iterations = 5000;
////    options.update_state_every_iteration = true;
////    ceres::Solver::Summary summary;
////
////    std::thread t([&]{
////        Solve(options, &problem, &summary);
////        std::cout << summary.BriefReport() << "\n";
////    });
//
//
//    // Plot the mesh
//    igl::opengl::glfw::Viewer viewer;
////    viewer.callback_post_draw = [&](igl::opengl::glfw::Viewer& viewer)
////    {
////        if(handle.updated()){
////            Eigen::VectorXd Z = handle.get();
////
////            std::cout << Z.minCoeff() << " " << Z.maxCoeff() << std::endl;
////
////            igl::jet(Z,false,C);
////            viewer.data().set_colors(C);
////            return true;
////        }
////        return false;
////    };
//
//    Eigen::VectorXd Norm = V.rowwise().norm();
//    viewer.data().set_mesh(V, F);
//    igl::jet(Norm,true,C);
//    viewer.data().set_colors(C);
//    viewer.launch();
//
//    //t.join();
//}



#include <igl/arap.h>
#include <igl/boundary_loop.h>
#include <igl/harmonic.h>
#include <igl/map_vertices_to_circle.h>
#include <igl/readPLY.h>
#include <igl/opengl/glfw/Viewer.h>


Eigen::MatrixXd V, Vcut;
Eigen::MatrixXi F, Fcut;
Eigen::MatrixXd V_uv;
Eigen::MatrixXd initial_guess;
double uv_scale = 200.;
bool show_uv = false;

bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier)
{
    if (key == '1')
        show_uv = false;
    else if (key == '2')
        show_uv = true;

    if (key == 'q')
        V_uv = initial_guess;

    if (show_uv)
    {
        viewer.data().set_mesh(V_uv,Fcut);
        viewer.core().align_camera_center(V_uv,Fcut);
    }
    else
    {
        viewer.data().set_mesh(Vcut,Fcut);
        viewer.core().align_camera_center(Vcut,Fcut);
    }

    viewer.data().compute_normals();

    return false;
}

int main(int argc, char *argv[])
{
    using namespace std;
    // Load a mesh in OFF format
    igl::readPLY("/home/jmeny/texture_map_optimization/crane.ply", V, F);
    cut_mesh_rosy(V,F,Vcut,Fcut);

    // Compute the initial solution for ARAP (harmonic parametrization)
    Eigen::VectorXi bnd;
    igl::boundary_loop(Fcut,bnd);
    Eigen::MatrixXd bnd_uv;
    igl::map_vertices_to_circle(Vcut,bnd,bnd_uv);

    igl::harmonic(Vcut,Fcut,bnd,bnd_uv,1,initial_guess);

    // Add dynamic regularization to avoid to specify boundary conditions
    igl::ARAPData arap_data;
    arap_data.with_dynamics = true;
    Eigen::VectorXi b  = Eigen::VectorXi::Zero(0);
    Eigen::MatrixXd bc = Eigen::MatrixXd::Zero(0,0);

    // Initialize ARAP
    arap_data.max_iter = 100;
    // 2 means that we're going to *solve* in 2d
    arap_precomputation(Vcut,Fcut,2,b,arap_data);


    // Solve arap using the harmonic map as initial guess
    V_uv = initial_guess;

    arap_solve(bc,arap_data,V_uv);


    // Scale UV to make the texture more clear
    V_uv *= uv_scale;

    // Plot the mesh
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(Vcut, Fcut);
    viewer.data().set_uv(V_uv);
    viewer.callback_key_down = &key_down;

    // Disable wireframe
    viewer.data().show_lines = false;

    // Draw checkerboard texture
    viewer.data().show_texture = true;

    // Launch the viewer
    viewer.launch();
}
