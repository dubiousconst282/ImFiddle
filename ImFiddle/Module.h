#pragma once

#include <imgui.h>

#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

struct FiddleModule {
    FiddleModule(const fs::path& compileCmdJsonPath);
    ~FiddleModule();

    void InvokePaint(ImDrawList* drawList);

    fs::path SourcePath;
    bool EnableOpts = false;

private:
    using PaintFn = void (*)(ImDrawList* drawList);

    fs::path _binPath;
    std::string _compileCmd;

    uintptr_t _modHandle = 0;
    PaintFn fn_Paint = nullptr;
    
    fs::file_time_type _sourceLastWriteTime = {};
};

namespace xplat {
    void RunProcess(const std::string& args, std::string* stdOut, int32_t* exitCode);
    
    uintptr_t LoadModule(const fs::path& path);
    void CloseModule(uintptr_t moduleHandle);
    uintptr_t FindSym(uintptr_t moduleHandle, const char* name);

    fs::path GetCurrentExePath();
};