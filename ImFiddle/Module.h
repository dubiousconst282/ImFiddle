#pragma once

#include <imgui.h>

#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

struct FiddleModule {
    FiddleModule(const fs::path& sourcePath, const fs::path& compileCmdJsonPath);
    ~FiddleModule();

    void InvokePaint(ImDrawList* drawList);

    const fs::path& GetSourcePath() const { return _sourcePath; }
    void SetSourcePath(const fs::path& path);

private:
    using PaintFn = void (*)(ImDrawList* drawList);

    fs::path _sourcePath;
    fs::path _binPath;
    std::string _compileCmd;

    int _watcher = 0;  // TODO: reuse havk::FileWatcher
    void* _modHandle = nullptr;
    PaintFn fn_Paint = nullptr;
    uint32_t _compileId = 0;
    double _lastCompileTime = 0;
};