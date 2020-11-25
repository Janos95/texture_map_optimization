add_library(Utilities INTERFACE)

target_sources(Utilities INTERFACE
        UniqueFunction.h
        Utilities.h
        Types.h
        RemoveIf.h
        Allocate.h
        Utilities.h)

target_include_directories(Utilities INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Utilities INTERFACE Corrade::Containers)
add_library(Tmo::Utilities ALIAS Utilities)
