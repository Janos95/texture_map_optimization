cmake_minimum_required(VERSION 3.10)
project(TextureOptimization)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/modules)

find_package(Eigen3 CONFIG REQUIRED)
#find_package(Ceres CONFIG REQUIRED)

set(LIBIGL_BUILD_TUTORIALS ON)
add_subdirectory(external/libigl)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


add_executable(texture_map_optimization
        main.cpp
        color_handle.hpp
        glob.cpp
        glob.h
        cut_mesh_rosy.hpp
        uv_unwrap.hpp
        load_data.hpp
        load_data.hpp
        trajectory.hpp)

target_link_libraries(texture_map_optimization PRIVATE
        igl::core
        igl::opengl
        igl::opengl_glfw
        igl::opengl_glfw_imgui
        igl::triangle
        igl::comiso
        CONAN_PKG::opencv
        CONAN_PKG::fmt
        stdc++fs)


find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(texture_map_optimization PUBLIC OpenMP::OpenMP_CXX)
endif()