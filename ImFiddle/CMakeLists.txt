CPMAddPackage("gh:glfw/glfw#3.4")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:ocornut/imgui#v1.91.4")

add_executable(
    Fiddle2D

    Main.cpp
    Module.cpp
    
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ../deps/glad/src/glad.c
)

if(CMAKE_HOST_WIN32)
    target_sources(Fiddle2D PRIVATE "PlatformWin.cpp")
else()
    target_sources(Fiddle2D PRIVATE "PlatformLinux.cpp")
endif()

target_link_libraries(Fiddle2D PUBLIC
    glfw
    glm::glm
)
target_include_directories(Fiddle2D PRIVATE 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends/
    ../deps/glad/include
)

target_compile_definitions(Fiddle2D PUBLIC
   # GLM_FORCE_DEPTH_ZERO_TO_ONE
   # GLM_FORCE_LEFT_HANDED
    GLM_FORCE_XYZW_ONLY
    IMGUI_USER_CONFIG="${CMAKE_CURRENT_LIST_DIR}/ImConfig.h"
)
set_target_properties(Fiddle2D PROPERTIES 
    ENABLE_EXPORTS ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

add_library(
    TemplateSketch SHARED

    Sketches/Template.cpp
)
target_link_libraries(TemplateSketch PUBLIC Fiddle2D)

target_include_directories(TemplateSketch PRIVATE ${imgui_SOURCE_DIR})
target_compile_definitions(TemplateSketch PRIVATE FIDDLE_SKETCH)