cmake_minimum_required(VERSION 3.13)
project(TextureOptimization LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)
find_package(OpenMesh CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED core imgproc calib3d)
find_package(Ceres CONFIG REQUIRED)
find_package(Folly CONFIG REQUIRED)


find_package(Corrade CONFIG REQUIRED
        Main
        Containers)
find_package(Magnum CONFIG REQUIRED
        GL
        MeshTools
        Shaders
        SceneGraph
        Trade
        AnySceneImporter
        AnyImageImporter
        Sdl2Application
        WindowlessEglApplication
        EglContext)

find_package(MagnumPlugins CONFIG REQUIRED AssimpImporter)


add_subdirectory(io)
add_subdirectory(shaders)
add_subdirectory(viewer)
add_subdirectory(optimization)


add_executable(texture_map_optimization
        main.cpp generate_trivial_texture.hpp)

set_property(TARGET texture_map_optimization PROPERTY CXX_STANDARD 17)

target_link_libraries(texture_map_optimization PRIVATE
        Corrade::Containers
        Magnum::GL
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::Trade
        Magnum::AnySceneImporter
        Magnum::AnyImageImporter
        OpenMeshCore
        OpenMeshTools
        tmo::io
        tmo::viewer
        tmo::optimization
        opencv_core
        opencv_highgui)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(texture_map_optimization PUBLIC OpenMP::OpenMP_CXX)
endif()