

add_library(pointcloud
        pointcloud.cpp
        pointcloud.hpp
        )


target_include_directories(pointcloud INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(pointcloud
        PUBLIC
            Magnum::GL
            Magnum::Magnum
        )


add_library(tmo::pointcloud ALIAS pointcloud)