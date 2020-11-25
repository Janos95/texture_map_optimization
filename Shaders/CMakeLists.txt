
corrade_add_resource(TMO_Rcs resources.conf)

add_library(Shaders SHARED
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
        Reduction.cpp
        Reduction.h
        #DepthFilter.cpp
        #DepthFilter.h
        ${TMO_Rcs} )


target_include_directories(Shaders PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Shaders
        PUBLIC
            Magnum::Magnum
            Magnum::Shaders
            Tmo::Utilities
            Tmo::ScopedTimer
        )

add_library(Tmo::Shaders ALIAS Shaders)
