#include "Module.h"

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

    auto re = std::regex(R"!("command": "(.+?)",\s*"file": ".+?(\/|\\\\)Sketches\2Template\.cpp")!");

    auto m = std::sregex_iterator(json.begin(), json.end(), re);
    if (m == std::sregex_iterator()) return "";

    auto res = std::regex_replace(m->str(1), std::regex(R"!(-o .+ -c .+)!"), "");
    res = std::regex_replace(res, std::regex("\\\\([\\\\|\\\"'])"), "$1"); // unescape shit
    res = std::regex_replace(res, std::regex("-O\\d?"), ""); // strip opt level
    return res;
}

FiddleModule::FiddleModule(const fs::path& compileCmdJsonPath) {
    _compileCmd = ParseCompileCmd(compileCmdJsonPath);

    if (_compileCmd.empty()) {
        throw std::runtime_error("Failed to parse compile command");
    }
}

FiddleModule::~FiddleModule() {
    xplat::CloseModule(_modHandle);
}

void FiddleModule::InvokePaint(ImDrawList* drawList) {
    if (SourcePath.empty()) return;
    auto lastWriteTime = fs::last_write_time(SourcePath);
    
    if (!fn_Paint || lastWriteTime != _sourceLastWriteTime) {
        _sourceLastWriteTime = lastWriteTime;
        _binPath = fs::temp_directory_path() / ("Fiddle2D/build_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".dll");

        std::error_code errc;
        fs::remove_all(_binPath.parent_path(), errc);
        fs::create_directories(_binPath.parent_path());

        std::string cmd = _compileCmd;

    #ifdef _WIN32
        cmd += " -lmsvcrtd";
        cmd += " -l\"" + xplat::GetCurrentExePath().replace_extension(".lib").string() + "\"";
    #endif

        cmd += EnableOpts ? " -O2" : " -O0";

        cmd += " -shared -o \"";
        cmd += _binPath.string();
        cmd += "\" \"";
        cmd += fs::absolute(SourcePath).string();
        cmd += "\"";

        std::string compileLog = "";
        int32_t status;
        xplat::RunProcess(cmd, &compileLog, &status);

        printf("%s\n", compileLog.c_str());

        if (status != 0) {
            fn_Paint = nullptr;
        } else {
            if (_modHandle) {
                xplat::CloseModule(_modHandle);
            }
            _modHandle = xplat::LoadModule(_binPath);
            fn_Paint = (PaintFn)xplat::FindSym(_modHandle, "ImFiddle_ModulePaint");
        }
        if (!fn_Paint) fn_Paint = [](ImDrawList* drawList) { drawList->AddText(ImVec2(0, 0), 0xFF0000FF, "No module loaded"); };
    }

    if (fn_Paint) {
        fn_Paint(drawList);
    }
}