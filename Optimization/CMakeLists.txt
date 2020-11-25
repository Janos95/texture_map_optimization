add_library(Optimization
        Cost.h
        Optimization.h
        Optimization.cpp
        KeyFrame.h
        KeyFrame.cpp
        RenderPass.h
        RenderPass.cpp
        )

target_include_directories(Optimization INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Optimization
        PUBLIC
            Magnum::Magnum
            Magnum::GL
            Tmo::Utilities
            Ceres::ceres
        PRIVATE
            Tmo::Shaders
        )

add_library(Tmo::Optimization ALIAS Optimization)