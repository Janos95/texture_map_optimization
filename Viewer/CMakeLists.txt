
corrade_add_resource(Viewer_RESOURCES resources.conf)
add_library(Viewer SHARED
        Viewer.cpp
        Viewer.h
        ArcBall.cpp
        ArcBall.h
        IO.cpp
        IO.h
        ${Viewer_RESOURCES}
        )

target_include_directories(Viewer PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Viewer
        PUBLIC
            Magnum::GL
            Magnum::Magnum
            Magnum::MeshTools
            Magnum::SceneGraph
            Magnum::Application
            Magnum::Sdl2Application
            Magnum::Shaders
            Magnum::Primitives
            Magnum::DebugTools
            MagnumIntegration::ImGui
            Tmo::Shaders
            Tmo::Optimization
            Tmo::ScopedTimer
        )

add_library(Tmo::Viewer ALIAS Viewer)