
add_library(scoped_timer STATIC include/ScopedTimer/ScopedTimer.cpp)
target_include_directories(scoped_timer PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_library(scoped_timer::scoped_timer ALIAS scoped_timer)