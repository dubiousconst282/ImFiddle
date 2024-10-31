#pragma once

#include <imgui.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/compatibility.hpp>

using namespace glm;

using uint2 = uvec2;
using uint3 = uvec3;
using uint4 = uvec4;

#include <cstdio>
#include <vector>
#include <typeinfo>

static ImDrawList* g_DrawList;
static float3x3 g_Transform = identity<float3x3>();

static std::vector<float3x3> g_TransformStack;
static ImU32 g_FillColor = 0xFF'000000, g_StrokeColor = 0;

void Save() { g_TransformStack.push_back(g_Transform); }
void Restore() {
    g_Transform = g_TransformStack.back();
    g_TransformStack.pop_back();
}
void SetFillColor(float r, float g, float b, float a = 1.0f) { g_FillColor = ImGui::ColorConvertFloat4ToU32({ r, g, b, a }); }
void SetFillColor(float3 color, float a = 1.0f) { SetFillColor(color.x, color.y, color.z, a); }
void SetStrokeColor(float r, float g, float b, float a = 1.0f) { g_StrokeColor = ImGui::ColorConvertFloat4ToU32({ r, g, b, a }); }
void SetStrokeColor(float3 color, float a = 1.0f) { SetStrokeColor(color.x, color.y, color.z, a); }
void NoFill() { g_FillColor = 0; }
void NoStroke() { g_StrokeColor = 0; }

void Translate(float x, float y) { g_Transform = glm::translate(g_Transform, float2(x, y)); }
void Rotate(float ang) { g_Transform = glm::rotate(g_Transform, ang); }
void Scale(float x, float y) { g_Transform = glm::scale(g_Transform, float2(x, y)); }
void Scale(float v) { g_Transform = glm::scale(g_Transform, float2(v)); }
float2 GetScale(const float3x3& m) {
    float sx = copysignf(sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0]), m[0][0]);
    float sy = copysignf(sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1]), m[0][1]);
    return float2(sx, sy);
}
float2 GetTransform(float x, float y) { return g_Transform * float3(x, y, 1); }
float2 GetInvTransform(float x, float y) { return glm::inverse(g_Transform) * float3(x, y, 1); }

void Text(float x, float y, const char* format, auto... args) {
    char buffer[2048];
    int len = snprintf(buffer, sizeof(buffer), format, args...);

    float2 pos = GetTransform(x, y);
    g_DrawList->AddText(ImVec2(pos.x, pos.y), g_FillColor, buffer, buffer + len);
}
void Text(float2 pos, const char* format, auto... args) { return Text(pos.x, pos.y, format, args...); }

void Line(float2 a, float2 b, float thickness = 1.0f) {
    a = GetTransform(a.x, a.y);
    b = GetTransform(b.x, b.y);
    g_DrawList->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), g_FillColor, thickness);
}
void Rect(float x, float y, float w, float h, float rounding = 0, float thickness = 1.0f) {
    // FIXME: this won't work with rotatations
    ImVec2 a = GetTransform(x, y);
    ImVec2 b = GetTransform(x + w, y + h);

    if (g_FillColor != 0) {
        g_DrawList->AddRectFilled(a, b, g_FillColor);
    }
    if (g_StrokeColor != 0) {
        g_DrawList->AddRect(a, b, g_StrokeColor, rounding, ImDrawFlags_None, thickness);
    }
}
void Rect(float2 pos, float2 size, float rounding = 0, float thickness = 1.0f) { Rect(pos.x, pos.y, size.x, size.y, rounding, thickness); }

void TexturedRect(float x, float y, float w, float h, ImTextureID tex) {
    ImVec2 a = GetTransform(x, y);
    ImVec2 b = GetTransform(x + w, y + h);
    g_DrawList->AddImage(tex, a, b);
}
void TexturedRect(float2 pos, float2 size, ImTextureID tex) { TexturedRect(pos.x, pos.y, size.x, size.y, tex); }

void Circle(float x, float y, float radius) {
    float2 pos = GetTransform(x, y);
    float2 rad = radius * GetScale(g_Transform);

    if (g_FillColor != 0) {
        g_DrawList->AddEllipseFilled(ImVec2(pos.x, pos.y), ImVec2(rad.x, rad.y), g_FillColor);
    }
    if (g_StrokeColor != 0) {
        g_DrawList->AddEllipse(ImVec2(pos.x, pos.y), ImVec2(rad.x, rad.y), g_StrokeColor);
    }
}
void Circle(float2 pos, float radius) { Circle(pos.x, pos.y, radius); }


float2 GetMousePos() {
    auto pos = ImGui::GetMousePos();
    return GetInvTransform(pos.x, pos.y);
}
bool IsMouseDragging(ImGuiMouseButton button = ImGuiMouseButton_Left, float lock_threshold = -1.0f) {
    return ImGui::IsMouseDragging(button, lock_threshold) && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}
bool IsMouseDown(ImGuiMouseButton button = ImGuiMouseButton_Left) {
    return ImGui::IsMouseDown(button) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}
bool IsMouseClicked(ImGuiMouseButton button = ImGuiMouseButton_Left) {
    return ImGui::IsMouseClicked(button) && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

float2 GetCanvasSize() {
    auto rectMin = g_DrawList->GetClipRectMin();
    auto rectMax = g_DrawList->GetClipRectMax();
    return float2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);
}

void Paint();

// Creates a temporary texture from a RGBA32 pixel buffer (that is automatically destroyed after painting).
extern ImTextureID CreateTexture(uint2 size, const uint32_t* pixels, uint32_t pixelsPerRow = 0, bool nolerp = false);

#if _WIN32
__declspec(dllexport)
#endif
extern "C" void ImFiddle_ModulePaint(ImDrawList* drawList) {
    g_DrawList = drawList;
    g_TransformStack.clear();
    g_Transform = identity<float3x3>();
    Paint();
}

extern std::pair<void*, size_t>& ImFiddle_GetPersistData(size_t id);

// Gets global data that persists module reloads.
// This function can lead to crashes or heap corruption if the struct layout changes.
template<typename T> requires std::is_trivial_v<T>
T& GetPersistData() {
    // TODO: speciaize for std::string maybe
    auto& slot = ImFiddle_GetPersistData(typeid(T).hash_code());
    if (slot.first == nullptr) {
        slot.first = new T();
        slot.second = sizeof(T);
    }
    assert(sizeof(T) == slot.second);
    return *(T*)slot.first;
}