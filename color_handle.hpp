//
// Created by jmeny on 04.12.19.
//

#pragma once

#include <Eigen/Core>

#include <ceres/iteration_callback.h>

#include <mutex>


class ColorHandle : public ceres::IterationCallback
{
public:
    explicit ColorHandle(Eigen::VectorXd& C): m_C(C) {}

    ceres::CallbackReturnType operator()(const ceres::IterationSummary& summary) override
    {
        std::lock_guard lock(m_mutex);
        if(m_update){
            m_C = m_copy;
        }
        else{
            m_copy = m_C;
            m_updated = true;
        }

        return ceres::SOLVER_CONTINUE;
    }

    bool updated() const
    {
        return m_updated;
    }

    Eigen::VectorXd get() const
    {
        std::lock_guard lock(m_mutex);
        m_updated = false;
        return m_copy;
    }

    void set(const Eigen::VectorXd& C)
    {
        std::lock_guard lock(m_mutex);
        m_copy = C;
        m_update = true;
    }

private:

    mutable bool m_updated = false;
    mutable bool m_update = false;

    Eigen::VectorXd& m_C;
    Eigen::VectorXd m_copy;

    mutable std::mutex m_mutex;
};
