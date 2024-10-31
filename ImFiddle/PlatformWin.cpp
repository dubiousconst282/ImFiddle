#include "Module.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

void xplat::RunProcess(const std::string& args, std::string* stdOut, int32_t* exitCode)
{
    HANDLE pipeR, pipeW;
    SECURITY_ATTRIBUTES saAttr = { .nLength = sizeof(SECURITY_ATTRIBUTES), .bInheritHandle = TRUE };
    CreatePipe(&pipeR, &pipeW, &saAttr, 0);
    SetHandleInformation(pipeR, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFO startInfo = {
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_USESTDHANDLES,
        .hStdOutput = pipeW,
        .hStdError = pipeW,
    };
    PROCESS_INFORMATION procInfo;

    if (!CreateProcessA(
        NULL, (char*)args.c_str(),
        NULL, NULL, TRUE, 0, NULL, NULL,
        &startInfo, &procInfo))
    {
        throw std::system_error(GetLastError(), std::system_category(), "Failed to launch process");
    }

    // Close handles to the stdin and stdout pipes no longer needed by the child process.
    // If they are not explicitly closed, there is no way to recognize that the child process has ended.
    CloseHandle(pipeW);

    char buffer[1024];
    DWORD dwRead = 0;

    while (ReadFile(pipeR, buffer, 1024, &dwRead, NULL) && dwRead > 0) {
        stdOut->append(buffer, dwRead);
    }
    
    CloseHandle(pipeR);

    WaitForSingleObject(procInfo.hProcess, INFINITE);
    GetExitCodeProcess(procInfo.hProcess, (DWORD*)exitCode);

    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);
}

uintptr_t xplat::LoadModule(const fs::path& path) {
    return (uintptr_t)LoadLibraryW(fs::absolute(path).c_str());
}
void xplat::CloseModule(uintptr_t moduleHandle) {
    if (moduleHandle != 0) {
        FreeLibrary((HMODULE)moduleHandle);
    }
}
uintptr_t xplat::FindSym(uintptr_t moduleHandle, const char* name) {
    return (uintptr_t)GetProcAddress((HMODULE)moduleHandle, name);
}

fs::path xplat::GetCurrentExePath() {
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return fs::path(buf, buf + len);
}
