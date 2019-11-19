cmake_minimum_required(VERSION 3.13)
project(TextureOptimization)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


find_package(Corrade REQUIRED
        Main
        Containers)
find_package(Magnum REQUIREDb
        GL
        MeshTools
        Shaders
        SceneGraph
        Trade
        AnySceneImporter
        AnyImageImporter
        Sdl2Application)

find_package(scn REQUIRED)

add_subdirectory(scoped_timer)

add_executable(texture_map_optimization
        main.cpp
        viewer.h
        viewer.cpp
        vertex_color_drawable.cpp
        vertex_colored_drawable.h
        glob.cpp
        glob.h
        load_poses.hpp)


set_property(TARGET texture_map_optimization PROPERTY CXX_STANDARD 17)

target_link_libraries(texture_map_optimization PRIVATE
        Corrade::Main
        Corrade::Containers
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::SceneGraph
        Magnum::Shaders
        Magnum::Trade
        Magnum::AnySceneImporter
        Magnum::AnyImageImporter
        scn::scn
        stdc++fs
        scoped_timer::scoped_timer)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(texture_map_optimization PUBLIC OpenMP::OpenMP_CXX)
endif()