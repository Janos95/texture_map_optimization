//
// Created by janos on 03.02.20.
//

#include "Cost.h"

using namespace Magnum;
using namespace Corrade;



bool PhotometricCost::Evaluate(double const* const* parameters,
        double* residuals,
        double** jacobians) const {

    double const* rotData = parameters[0];
    double const* tranData = parameters[1];

    Vector3 angleAxis(rotData[0], rotData[1], rotData[2]);
    Vector3 translation(tranData[0], tranData[1], tranData[2]);

    auto result = m_renderPass.dispatch(m_kf);

    residuals[0] = result.cost;

    if(jacobians){
        for(int j = 0; j < 3; ++j){
            if(jacobians[0])
                jacobians[0][j] = result.rotationGrad[j];
            if(jacobians[1])
                jacobians[1][j] = result.translationGrad[j];
        }
    }

    return true;
}




