#include "Module.h"

#include <dlfcn.h>
#include <sys/inotify.h>

void xplat::RunProcess(const std::string& args, std::string* stdOut, int32_t* exitCode) {
    auto proc = popen(args.data(), "r");
    char buffer[1024];

    while (char* bufferEnd = fgets(buffer, sizeof(buffer), proc)) {
        stdOut->append(buffer, bufferEnd);
    }
    *exitCode = pclose(proc);
}

uintptr_t xplat::LoadModule(const fs::path& path) {
    return (uintptr_t)dlopen(fs::absolute(path).c_str(), RTLD_NOW | RTLD_GLOBAL);
}
void xplat::CloseModule(uintptr_t moduleHandle) {
    if (moduleHandle != 0) {
        dlclose((void*)moduleHandle);
    }
}
uintptr_t xplat::FindSym(uintptr_t moduleHandle, const char* name) {
    return (uintptr_t)dlsym((void*)moduleHandle, name);
}

fs::path xplat::GetCurrentExePath() {
    return fs::canonical("/proc/self/exe");
}
