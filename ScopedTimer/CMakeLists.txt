
add_library(ScopedTimer STATIC ScopedTimer.cpp)
target_include_directories(ScopedTimer PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_library(Tmo::ScopedTimer ALIAS ScopedTimer)