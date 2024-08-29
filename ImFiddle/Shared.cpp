#include <cstdint>
#include <unordered_map>

#define IMGUI_IMPL_API
#include <imgui.h>

std::unordered_map<size_t, std::pair<void*, size_t>> g_SessionData;

extern std::pair<void*, size_t>& ImFiddle_GetPersistData(size_t id) {
    return g_SessionData[id];
}