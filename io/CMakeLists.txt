


add_library(io
        glob.cpp
        glob.hpp
        load_poses.hpp)


target_include_directories(io PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(io
        PUBLIC
            Corrade::Utility
        )

add_library(tmo::io ALIAS io)