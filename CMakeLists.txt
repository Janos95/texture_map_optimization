cmake_minimum_required(VERSION 3.13)
project(TextureOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Corrade REQUIRED PluginManager Containers TestSuite Utility)
find_package(Magnum REQUIRED GL MeshTools Sdl2Application Shaders Primitives DebugTools)
find_package(MagnumPlugins REQUIRED AssimpImporter)
find_package(MagnumIntegration REQUIRED ImGui)
find_package(Ceres)
find_package(MagnumPlugins REQUIRED AssimpImporter)

add_subdirectory(ScopedTimer)
add_subdirectory(Shaders)
add_subdirectory(Viewer)
add_subdirectory(Optimization)
add_subdirectory(Utilities)

add_executable(App main.cpp)

target_link_libraries(App PUBLIC
        Magnum::Magnum
        Tmo::Viewer
        Tmo::Optimization)

