cmake_minimum_required(VERSION 3.13)
project(TextureOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)

find_package(MagnumPlugins CONFIG REQUIRED)
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
        Sdl2Application)

find_package(scn CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

add_subdirectory(io)
add_subdirectory(shaders)
add_subdirectory(viewer)


add_executable(texture_map_optimization
        main.cpp)

set_property(TARGET texture_map_optimization PROPERTY CXX_STANDARD 17)

target_link_libraries(texture_map_optimization PRIVATE
        Corrade::Containers
        Magnum::GL
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::Trade
        Magnum::AnySceneImporter
        Magnum::AnyImageImporter
        tmo::io
        tmo::viewer)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(texture_map_optimization PUBLIC OpenMP::OpenMP_CXX)
endif()