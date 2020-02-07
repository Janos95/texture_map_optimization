

add_library(viewer
        viewer.cpp
        viewer.hpp
        render_target.cpp
        render_target.hpp
        scene_graph_node.cpp
        scene_graph_node.hpp
        compile_open_mesh.cpp
        compile_open_mesh.hpp
        scene.hpp
        scene.cpp
        render_pass.hpp
        object.hpp
        camera.hpp
        render_pass.cpp)

target_include_directories(viewer PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(viewer
        PUBLIC
            Magnum::GL
            Magnum::Magnum
            Magnum::MeshTools
            Magnum::SceneGraph
            Magnum::Application
            Magnum::Shaders
            Magnum::EglContext
            tmo::shaders
        )


add_library(tmo::viewer ALIAS viewer)