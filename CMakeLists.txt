cmake_minimum_required(VERSION 3.10)
project(segmenation)

set(WITH_ANYSCENEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_ASSIMPIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_ANYIMAGEIMPORTER ON CACHE BOOL "" FORCE)

add_subdirectory(external/corrade)
add_subdirectory(external/magnum EXCLUDE_FROM_ALL)
add_subdirectory(external/magnum-plugins EXCLUDE_FROM_ALL)

add_custom_target(plugins ALL DEPENDS
        Magnum::AnySceneImporter
        Magnum::AnyImageImporter
        MagnumPlugins::AssimpImporter)

add_executable(segmentation main.cpp viewer.h viewer.cpp)

target_link_libraries(segmentation PRIVATE
        Corrade::Main
        Magnum::Magnum
        Magnum::Shaders
        Magnum::Trade)