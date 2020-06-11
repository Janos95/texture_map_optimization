
corrade_add_resource(viewer_RESOURCES resources.conf)
add_library(viewer SHARED
        viewer.cpp
        viewer.hpp
        arc_ball.cpp
        arc_ball.hpp
        drawables.cpp
        drawables.hpp
        arc_ball_camera.hpp
        ${viewer_RESOURCES}
        )

target_include_directories(viewer PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(viewer
        PUBLIC
            Magnum::GL
            Magnum::SceneGraph
            Magnum::Magnum
            Magnum::MeshTools
            Magnum::SceneGraph
            Magnum::Application
            Magnum::Sdl2Application
            Magnum::Shaders
            Magnum::Primitives
            Magnum::DebugTools
            MagnumIntegration::ImGui
            tmo::shaders
            tmo::io
        )

add_library(tmo::viewer ALIAS viewer)