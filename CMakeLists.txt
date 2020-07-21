cmake_minimum_required(VERSION 3.13)
project(TextureOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Corrade CONFIG REQUIRED PluginManager Containers TestSuite Utility)
find_package(Magnum CONFIG REQUIRED GL SceneGraph MeshTools Sdl2Application Shaders Primitives DebugTools)
find_package(MagnumPlugins CONFIG REQUIRED AssimpImporter)
find_package(MagnumIntegration CONFIG REQUIRED ImGui)

find_package(MagnumPlugins CONFIG REQUIRED AssimpImporter)

add_subdirectory(scoped_timer)
add_subdirectory(io)
add_subdirectory(shaders)
add_subdirectory(viewer)
add_subdirectory(optimization)


add_executable(tmo main.cpp)

target_link_libraries(tmo PUBLIC
        Magnum::Magnum
        tmo::io
        tmo::viewer
        tmo::optimization)

