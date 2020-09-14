
corrade_add_resource(TMO_Rcs resources.conf)

add_library(shaders SHARED
        Diff.h
        Diff.cpp
        TextureCoordinates.h
        TextureCoordinates.cpp
        TextureCoordinates.cpp
        TextureCoordinates.h
        FullScreenTriangle.cpp
        FullScreenTriangle.h
        Remap.cpp
        Remap.h
        Combine.cpp
        Combine.h
        DepthFilter.cpp
        DepthFilter.h
        Reduction.cpp
        Reduction.h
        DepthFilter.cpp
        DepthFilter.h
        ${TMO_Rcs} )


target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(shaders
        PUBLIC
            Magnum::Magnum
            Magnum::Shaders
            tmo::utilities
            scoped_timer::scoped_timer
        )

set_property(TARGET shaders PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(tmo::shaders ALIAS shaders)
