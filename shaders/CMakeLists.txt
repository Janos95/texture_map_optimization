

corrade_add_resource(TMO_Rcs resources.conf)

add_library(shaders SHARED
        pbr_shader.hpp
        pbr_shader.cpp
        diff_shader.hpp
        diff_shader.cpp
        cube_map_shader.hpp
        cube_map_shader.cpp
        background_shader.cpp
        background_shader.hpp
        screen_triangle.cpp
        screen_triangle.hpp
        ${TMO_Rcs})


target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(shaders
        PUBLIC
            Magnum::Magnum
            Magnum::Shaders
        )

set_property(TARGET shaders PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(tmo::shaders ALIAS shaders)
