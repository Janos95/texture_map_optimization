//
// Created by jmeny on 05.12.19.
//

#pragma once

#include "cut_mesh_rosy.hpp"

#include <igl/arap.h>
#include <igl/boundary_loop.h>
#include <igl/harmonic.h>
#include <igl/map_vertices_to_circle.h>


class UVUnwrap
{
public:

    UVUnwrap(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F)
    {
        cut_mesh_rosy(V, F, m_V, m_F);
    }


    auto arap()
    {
        Eigen::VectorXi bnd;
        Eigen::MatrixXd V_uv;
        Eigen::MatrixXd initial_guess;

        igl::boundary_loop(m_F,bnd);
        Eigen::MatrixXd bnd_uv;
        igl::map_vertices_to_circle(m_V,bnd,bnd_uv);

        igl::harmonic(m_V,m_F,bnd,bnd_uv,1,initial_guess);

        // Add dynamic regularization to avoid to specify boundary conditions
        igl::ARAPData arap_data;
        arap_data.with_dynamics = true;
        Eigen::VectorXi b  = Eigen::VectorXi::Zero(0);
        Eigen::MatrixXd bc = Eigen::MatrixXd::Zero(0,0);

        // Initialize ARAP
        arap_data.max_iter = 100;
        // 2 means that we're going to *solve* in 2d
        igl::arap_precomputation(m_V,m_F,2,b,arap_data);

        // Solve arap using the harmonic map as initial guess
        V_uv = initial_guess;

        igl::arap_solve(bc,arap_data,V_uv);

        // Scale UV to make the texture more clear
        return V_uv;
    }

private:
    Eigen::MatrixXd m_V;
    Eigen::MatrixXi m_F;
};
