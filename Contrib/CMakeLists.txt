set(IMGUI_SOURCES
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h)

add_library(imgui SHARED ${IMGUI_SOURCES})
target_include_directories(imgui SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
target_compile_options(imgui PRIVATE -w)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui ${CORRADE_INCLUDE_DIR})
add_library(ImGui::ImGui ALIAS imgui)

add_library(ImGui::Sources INTERFACE IMPORTED)
set_property(TARGET ImGui::Sources APPEND PROPERTY
    INTERFACE_LINK_LIBRARIES ImGui::ImGui)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui" CACHE "" INTERNAL)
set(WITH_IMGUI ON CACHE "" INTERNAL)

add_subdirectory(magnum-integration)

