add_library(utilities INTERFACE)

target_sources(utilities INTERFACE
        UniqueFunction.hpp
        Utilities.h)

target_include_directories(utilities INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(utilities INTERFACE Corrade::Containers)
add_library(tmo::utilities ALIAS utilities)
