#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Module.h"

#include <vector>
#include <unordered_map>

std::vector<GLuint> _tempTextures;

int main(int argc, char** args) {
    #if _WIN32
    setvbuf(stdout, NULL, _IONBF, 0);
    #else
    setlinebuf(stdout);
    #endif

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImFiddle", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "priv/imgui.ini";
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.Fonts->AddFontFromFileTTF("assets/Roboto-Medium.ttf", 18.0f);

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    FiddleModule mod("./build/compile_commands.json");
    mod.SourcePath = "./ImFiddle/Sketches/Template.cpp";
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (ImGui::Begin("Fiddle Settings")) {
            if (ImGui::BeginCombo("Module", mod.SourcePath.filename().string().c_str())) {
                for (auto& entry : fs::directory_iterator("./ImFiddle/Sketches/")) {
                    if (!entry.is_regular_file()) continue;
                    
                    if (ImGui::Selectable(entry.path().filename().string().c_str(), mod.SourcePath == entry.path())) {
                        mod.SourcePath = entry.path();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Checkbox("Optimize", &mod.EnableOpts);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        }
        ImGui::End();


        mod.InvokePaint(ImGui::GetBackgroundDrawList());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        if (_tempTextures.size() > 0) {
            glDeleteTextures(_tempTextures.size(), _tempTextures.data());
            _tempTextures.clear();
        }
    }
    glfwTerminate();
    return 0;
}

ImTextureID CreateTexture(glm::uvec2 size, const uint32_t* pixels, uint32_t pixelsPerRow = 0, bool nolerp = false) {
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nolerp ? GL_NEAREST : GL_LINEAR);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, pixelsPerRow);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    _tempTextures.push_back(texId);
    return (ImTextureID)texId;
}

std::unordered_map<size_t, std::pair<void*, size_t>> g_SessionData;

extern std::pair<void*, size_t>& ImFiddle_GetPersistData(size_t id) { return g_SessionData[id]; }
