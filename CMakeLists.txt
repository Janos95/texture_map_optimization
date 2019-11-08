cmake_minimum_required(VERSION 3.10)
project(TextureOptimization)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


set(WITH_ANYSCENEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_ASSIMPIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_ANYIMAGEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)

add_subdirectory(external/corrade EXCLUDE_FROM_ALL)
add_subdirectory(external/magnum EXCLUDE_FROM_ALL)
add_subdirectory(external/magnum-plugins EXCLUDE_FROM_ALL)


add_custom_target(plugins ALL DEPENDS
        Magnum::AnySceneImporter
        Magnum::AnyImageImporter
        MagnumPlugins::AssimpImporter)

add_executable(texture_map_optimization
        main.cpp
        viewer.h
        viewer.cpp
        vertex_color_drawable.cpp
        vertex_colored_drawable.h
        glob.cpp
        glob.h)

target_link_libraries(texture_map_optimization PRIVATE
        Corrade::Main
        Magnum::Sdl2Application
        Magnum::GL
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::SceneGraph
        Magnum::Shaders
        Magnum::Trade
        stdc++fs
        CONAN_PKG::opencv)


find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(texture_map_optimization PUBLIC OpenMP::OpenMP_CXX)
endif()