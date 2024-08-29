add_executable(
    Fiddle2D

    Main.cpp
    Module.cpp
    Sketches/Template.cpp
)

# dlsym() only works if we make a shared library, dunno why
add_library(ImFiddleSharedDeps SHARED Shared.cpp Main.cpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

target_link_libraries(ImFiddleSharedDeps PUBLIC
    glfw
    imgui::imgui
    glad::glad
    glm::glm
)
target_compile_definitions(ImFiddleSharedDeps PUBLIC
   # GLM_FORCE_DEPTH_ZERO_TO_ONE
   # GLM_FORCE_LEFT_HANDED
    GLM_FORCE_XYZW_ONLY
)

target_link_libraries(Fiddle2D PUBLIC ImFiddleSharedDeps)
