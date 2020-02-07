


corrade_add_resource(TMO_Rcs resources.conf)

add_library(shaders
    coords_filter.cpp
    coords_filter.hpp
    interpolated_vertices.hpp
    interpolated_vertices.cpp
    visible_texture.hpp
    visible_texture.cpp
    ${TMO_Rcs})


target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(shaders
        PUBLIC
            Magnum::Magnum
            Magnum::Shaders
        )

add_library(tmo::shaders ALIAS shaders)
