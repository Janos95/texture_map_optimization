cmake_minimum_required(VERSION 3.13)
project(TextureOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)

find_package(Eigen3 CONFIG REQUIRED)
find_package(Corrade CONFIG REQUIRED PluginManager Containers TestSuite)
find_package(Magnum CONFIG REQUIRED GL SceneGraph MeshTools Sdl2Application Shaders Primitives DebugTools)
find_package(MagnumPlugins CONFIG REQUIRED AssimpImporter StbImageImporter StbImageConverter)
find_package(MagnumIntegration CONFIG REQUIRED ImGui Eigen)
find_package(ImGui REQUIRED COMPONENTS SourcesMiscCpp)

find_package(MagnumPlugins CONFIG REQUIRED AssimpImporter)

add_subdirectory(scoped_timer)
add_subdirectory(io)
add_subdirectory(shaders)
add_subdirectory(viewer)
#add_subdirectory(optimization)


add_executable(texture_map_optimization
        main.cpp generate_trivial_texture.hpp)

set_property(TARGET texture_map_optimization PROPERTY CXX_STANDARD 17)

target_link_libraries(texture_map_optimization PUBLIC
        Magnum::Magnum
        tmo::io
        tmo::viewer
        #tmo::optimization
        Eigen3::Eigen
        ceres)

