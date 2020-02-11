file(GLOB_RECURSE scoped_timer_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/scoped_timer *.hpp)
file(GLOB_RECURSE scoped_timer_sources ${CMAKE_CURRENT_SOURCE_DIR}/include/scoped_timer *.cpp)

find_package(fmt)

add_library(scoped_timer STATIC ${scoped_timer_sources} ${scoped_timer_headers})
target_compile_features(scoped_timer PUBLIC cxx_std_17)
target_include_directories(scoped_timer PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(scoped_timer PUBLIC fmt::fmt)

add_library(scoped_timer::scoped_timer ALIAS scoped_timer)