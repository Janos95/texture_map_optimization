

add_library(viewer
        viewer.cpp
        viewer.hpp
        scene_graph_node.cpp
        scene_graph_node.hpp
        arc_ball.cpp
        arc_ball.hpp
        arc_ball_camera.hpp
        scene.hpp
        scene.cpp
        object.hpp
        camera.hpp)

target_include_directories(viewer PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(viewer
        PUBLIC
            Magnum::GL
            Magnum::SceneGraph
            Magnum::Magnum
            Magnum::MeshTools
            Magnum::SceneGraph
            Magnum::Application
            Magnum::Shaders
            Folly::folly
            tmo::shaders
        )


add_library(tmo::viewer ALIAS viewer)