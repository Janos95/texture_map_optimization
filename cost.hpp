//
// Created by janos on 27.11.19.
//

#pragma once

#include "interpolation.hpp"
#include "quadrature_ref_triangle.hpp"

#include <igl/grad.h>
#include <igl/doublearea.h>

#include <ceres/dynamic_cost_function.h>

class InterfaceEnergy : public ceres::CostFunction
{
public:

    InterfaceEnergy(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const double epsilon):
            m_epsilon(epsilon),
            m_F(F),
            m_V(V),
            m_dblA(F.rows())
    {
        igl::doublearea(m_V, m_F, m_dblA);

        Eigen::SparseMatrix<double> G;
        igl::grad(m_V,m_F,G);
        const auto & T = (m_dblA.replicate(3,1) * .5).asDiagonal();
        m_GSQ = G.transpose() * T * G;


        mutable_parameter_block_sizes()->push_back(m_V.rows());
        set_num_residuals(1);
    }

    bool Evaluate(double const* const* parameters,
                  double* residuals,
                  double** jacobians) const override
    {
        Eigen::Map<Eigen::VectorXd> U((double*)*parameters, m_V.rows()); //TODO: dont const cast

        Eigen::VectorXd intermResult = m_GSQ * U;
        residuals[0] = .5 * m_epsilon * U.transpose() * intermResult;

        if(jacobians && *jacobians)
        {
            for (int i = 0; i < intermResult.rows(); ++i) {
                jacobians[0][i] = m_epsilon * intermResult[i];
            }
        }

        return true;
    }

private:

    const Eigen::MatrixXd& m_V;
    const Eigen::MatrixXi& m_F;

    Eigen::SparseMatrix<double> m_GSQ;
    Eigen::VectorXd m_dblA;

    double m_epsilon;
};


class PotentialEnergy : public ceres::CostFunction
{
public:

    PotentialEnergy(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const double epsilon):
            m_epsilon(epsilon),
            m_F(F),
            m_V(V),
            m_dblA(F.rows())
    {
        igl::doublearea(m_V, m_F, m_dblA);

        mutable_parameter_block_sizes()->push_back(m_V.rows());
        set_num_residuals(1);
    }

    bool Evaluate(double const* const* parameters,
                  double* residuals,
                  double** jacobians) const override
    {
        Eigen::Map<Eigen::VectorXd> U((double*)*parameters, m_V.rows()); //TODO: dont const cast

        if(jacobians && jacobians[0])
            std::fill(jacobians[0], jacobians[0] + U.rows(), 0);

        QuadratureRefTriangle<double> quad;
        double residual = 0;

        for (int i = 0; i < m_F.rows(); ++i) {
            auto f = m_F.row(i);
            DoubleWellPotential<double> pot{U[f[0]],U[f[1]],U[f[2]]};
            residual += m_dblA[i] * quad.integrate(pot);

            if(jacobians && jacobians[0])
            {
                DoubleWellPotentialGrad<double> potGrad{U[f[0]],U[f[1]],U[f[2]]};
                for (int j = 0; j < 3; ++j) {
                    potGrad.i = j;
                    jacobians[0][f[j]] += m_dblA[i] * quad.integrate(potGrad);
                }
            }
        }

        residuals[0] = .5 * residual / m_epsilon;

        if(jacobians && jacobians[0])
        {
            Eigen::Map<Eigen::VectorXd> J((double*)*jacobians, m_V.rows());
            J *= 1./(2. * m_epsilon);
        }

        return true;
    }

private:

    const Eigen::MatrixXd& m_V;
    const Eigen::MatrixXi& m_F;

    Eigen::VectorXd m_dblA;

    double m_epsilon;
};


class AreaRegularizer : public ceres::CostFunction
{

public:

    AreaRegularizer(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F):
            m_F(F),
            m_V(V),
            m_dblA(F.rows())
    {
        igl::doublearea(m_V, m_F, m_dblA);
        m_area = m_dblA.sum() / 2.;

        mutable_parameter_block_sizes()->push_back(m_V.rows());
        set_num_residuals(1);
    }

    bool Evaluate(double const* const* parameters,
                  double* residuals,
                  double** jacobians) const override
    {
        Eigen::Map<Eigen::VectorXd> U((double*)*parameters, m_V.rows()); //TODO: dont const cast

        if(jacobians && jacobians[0])
            std::fill(jacobians[0], jacobians[0] + U.rows(), 0);

        QuadratureRefTriangle<double> quad;
        double residual = 0;

        for (int i = 0; i < m_F.rows(); ++i) {
            auto f = m_F.row(i);
            IndicatorFunction<double> ind{U[f[0]],U[f[1]],U[f[2]]};
            residual += m_dblA[i] * quad.integrate(ind);

            if(jacobians && jacobians[0])
            {
                IndicatorFunctionGrad<double> interp{U[f[0]],U[f[1]],U[f[2]]};
                for (int j = 0; j < 3; ++j) {
                    interp.i = j;
                    jacobians[0][f[j]] += m_dblA[i] * quad.integrate(interp);
                }
            }
        }

        residuals[0] = residual - m_area / 2.;

        return true;
    }


private:

    const Eigen::MatrixXd& m_V;
    const Eigen::MatrixXi& m_F;

    Eigen::VectorXd m_dblA;
    double m_area;
};
