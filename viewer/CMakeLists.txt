
corrade_add_resource(viewer_RESOURCES resources.conf)
add_library(viewer SHARED
        viewer.cpp
        viewer.h
        arc_ball.cpp
        arc_ball.h
        drawables.cpp
        drawables.h
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
            MagnumPlugins::StbImageConverter
            tmo::shaders
            tmo::io
            scoped_timer::scoped_timer
        )

add_library(tmo::viewer ALIAS viewer)