

add_library(viewer
        smart_drawable.hpp
        viewer.cpp
        viewer.hpp
        )

target_include_directories(viewer PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(viewer
        PUBLIC
            Magnum::GL
            Magnum::Magnum
            Magnum::MeshTools
            Magnum::SceneGraph
            Magnum::Application
            Magnum::Shaders
           # tmo::shaders
        )


add_library(tmo::viewer ALIAS viewer)