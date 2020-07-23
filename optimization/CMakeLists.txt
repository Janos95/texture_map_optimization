add_library(optimization
        Cost.h
        Cost.cpp
        Optimization.h
        Optimization.cpp
        KeyFrame.h
        KeyFrame.cpp
        RenderPass.h
        RenderPass.cpp
        )

target_include_directories(optimization INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(optimization
        PUBLIC
            Magnum::Magnum
            Magnum::GL
            tmo::utilities
        PRIVATE
            tmo::shaders
            ceres
        )

add_library(tmo::optimization ALIAS optimization)