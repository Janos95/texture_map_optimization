

corrade_add_resource(TMO_Rcs resources.conf)

add_library(shaders SHARED
        Diff.h
        Diff.cpp
        TextureCoordinates.h
        TextureCoordinates.cpp
        ${TMO_Rcs} TextureCoordinates.cpp TextureCoordinates.h ScreenTriangle.cpp ScreenTriangle.h Remap.cpp Remap.h Combine.cpp Combine.h)


target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(shaders
        PUBLIC
            Magnum::Magnum
            Magnum::Shaders
        )

set_property(TARGET shaders PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(tmo::shaders ALIAS shaders)
