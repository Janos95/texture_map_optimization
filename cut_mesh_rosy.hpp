//
// Created by jmeny on 04.12.19.
//


#pragma once

#include <igl/avg_edge_length.h>
#include <igl/barycenter.h>
#include <igl/comb_cross_field.h>
#include <igl/comb_frame_field.h>
#include <igl/compute_frame_field_bisectors.h>
#include <igl/cross_field_mismatch.h>
#include <igl/cut_mesh_from_singularities.h>
#include <igl/find_cross_field_singularities.h>
#include <igl/local_basis.h>
#include <igl/rotate_vectors.h>
#include <igl/copyleft/comiso/miq.h>
#include <igl/copyleft/comiso/nrosy.h>
#include <igl/PI.h>
#include <igl/cut_mesh.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/writePLY.h>


void cut_mesh_rosy(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, Eigen::MatrixXd& Vcut, Eigen::MatrixXi& Fcut)
{
    // Face barycenters
    Eigen::MatrixXd B;


// Cross field
    Eigen::MatrixXd X1,X2;

// Bisector field
    Eigen::MatrixXd BIS1, BIS2;

// Combed bisector
    Eigen::MatrixXd BIS1_combed, BIS2_combed;

// Per-corner, integer mismatches
    Eigen::Matrix<int, Eigen::Dynamic, 3> MMatch;

// Field singularities
    Eigen::Matrix<int, Eigen::Dynamic, 1> isSingularity, singularityIndex;

// Per corner seams
    Eigen::Matrix<int, Eigen::Dynamic, 3> Seams;

// Combed field
    Eigen::MatrixXd X1_combed, X2_combed;

    // Compute face barycenters
    igl::barycenter(V, F, B);

    // Compute scale for visualizing fields
    auto global_scale =  .5*igl::avg_edge_length(V, F);

    // Contrain one face
    Eigen::VectorXi b(1);
    b << 0;
    Eigen::MatrixXd bc(1, 3);
    bc << 1, 0, 0;

    // Create a smooth 4-RoSy field
    Eigen::VectorXd S;
    igl::copyleft::comiso::nrosy(V, F, b, bc, Eigen::VectorXi(), Eigen::VectorXd(), Eigen::MatrixXd(), 4, 0.5, X1, S);

    // Find the orthogonal vector
    Eigen::MatrixXd B1, B2, B3;
    igl::local_basis(V, F, B1, B2, B3);
    X2 = igl::rotate_vectors(X1, Eigen::VectorXd::Constant(1, igl::PI / 2), B1, B2);

    // Always work on the bisectors, it is more general
    igl::compute_frame_field_bisectors(V, F, X1, X2, BIS1, BIS2);

    // Comb the field, implicitly defining the seams
    igl::comb_cross_field(V, F, BIS1, BIS2, BIS1_combed, BIS2_combed);

    // Find the integer mismatches
    igl::cross_field_mismatch(V, F, BIS1_combed, BIS2_combed, true, MMatch);

    // Find the singularities
    igl::find_cross_field_singularities(V, F, MMatch, isSingularity, singularityIndex);

    // Cut the mesh, duplicating all vertices on the seams
    igl::cut_mesh_from_singularities(V, F, MMatch, Seams);

    igl::cut_mesh(V, F, Seams, Vcut, Fcut);

    //igl::writePLY("/home/jmeny/texture_map_optimization/crane_cut.ply", Vcut, Fcut);

}
