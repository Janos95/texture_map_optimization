

corrade_add_resource(TMO_Rcs resources.conf)

add_library(shaders SHARED
        diff.h
        diff.cpp
        average_color.h
        average_color.cpp
        ${TMO_Rcs} average_color.cpp average_color.h)


target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(shaders
        PUBLIC
            Magnum::Magnum
            Magnum::Shaders
        )

set_property(TARGET shaders PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(tmo::shaders ALIAS shaders)
