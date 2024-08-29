#include "Module.h"

#include <dlfcn.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <regex>
#include <fstream>

static std::string ReadFile(const fs::path& path) {
    std::string buf;
    buf.resize(fs::file_size(path));

    std::ifstream(path, std::ios::binary).read(buf.data(), (std::streamsize)buf.size());
    return buf;
}

static std::string ParseCompileCmd(const fs::path& compileCmdJsonPath) {
    // Quick and dirty regex parsing
    // TODO: consider using some actual json parser
    std::string json = ReadFile(compileCmdJsonPath);

    auto re = std::regex(R"!("command": "(.+?)",\s*"file": ".+?[\/\\]Sketches/Template.cpp")!");

    auto m = std::sregex_iterator(json.begin(), json.end(), re);
    if (m == std::sregex_iterator()) return "";

    return std::regex_replace(m->str(1), std::regex(R"!(-o .+ -c .+)!"), "");
}

FiddleModule::FiddleModule(const fs::path& sourcePath, const fs::path& compileCmdJsonPath) {
    _sourcePath = sourcePath;
    _compileCmd = ParseCompileCmd(compileCmdJsonPath);

    if (_compileCmd.empty()) {
        throw std::runtime_error("Failed to parse compile command");
    }

    std::error_code errc;
    fs::remove_all("/tmp/CppFiddle2D/module_bin/", errc);
    fs::create_directories("/tmp/CppFiddle2D/module_bin/");

    SetSourcePath(sourcePath);
}

FiddleModule::~FiddleModule() {
    close(_watcher);
}

void FiddleModule::SetSourcePath(const fs::path& path) {
    _sourcePath = path;

    if (_watcher != 0) {
        close(_watcher);
        _watcher = 0;
    }
    _watcher = inotify_init1(IN_NONBLOCK);
    inotify_add_watch(_watcher, path.c_str(), IN_MODIFY);
    fn_Paint = nullptr;
}

void FiddleModule::InvokePaint(ImDrawList* drawList) {
    char buffer[512];

    if (!fn_Paint || (read(_watcher, buffer, sizeof(buffer)) > 0 && ImGui::GetTime() >= _lastCompileTime + 0.5)) {
        _lastCompileTime = ImGui::GetTime(); // throttle a bit because inotify notifies twice in a row
        _binPath = "/tmp/CppFiddle2D/module_bin/build_" + std::to_string(++_compileId) + ".so";

        std::string cmd = _compileCmd;
        cmd += "-shared -fPIC -o \"";
        cmd += _binPath;
        cmd += "\" \"";
        cmd += _sourcePath;
        cmd += "\"";

        auto proc = popen(cmd.c_str(), "r");
        std::string compileLog = "";
        while (fgets(buffer, sizeof(buffer), proc)) {
            compileLog += buffer;
        }
        int status = pclose(proc);

        if (status != 0) {
            printf("Compile error: %s\n", compileLog.c_str());
            fn_Paint = nullptr;
        } else {
            if (_modHandle) {
                dlclose(_modHandle);
            }
            _modHandle = dlopen(fs::absolute(_binPath).c_str(), RTLD_NOW | RTLD_GLOBAL);
            printf("Load: %s\n", dlerror());
            fn_Paint = (PaintFn)dlsym(_modHandle, "ImFiddle_ModulePaint");
        }
        if (!fn_Paint) fn_Paint = [](ImDrawList* drawList) { drawList->AddText(ImVec2(0, 0), 0xFF0000FF, "No module loaded"); };
    }

    if (fn_Paint) {
        fn_Paint(drawList);
    }
}