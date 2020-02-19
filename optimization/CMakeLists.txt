

add_library(optimization
        cost.cpp
        cost.hpp
        optimization.hpp
        optimization.cpp)


target_include_directories(optimization INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(optimization
        PUBLIC
            Magnum::Magnum
            Magnum::WindowlessGlxApplication
            Magnum::GL
            tmo::viewer
            tmo::shaders
            opencv_core
            opencv_highgui
            ceres
        )


add_library(tmo::optimization ALIAS optimization)