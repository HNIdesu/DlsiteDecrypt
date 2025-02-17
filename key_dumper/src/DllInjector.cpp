#include <windows.h>
#include <iostream>
using namespace std;

int main(int argc,char** argv){
    if (argc <= 2) {
        cout << "Usage: DllInjector <program_path> <dll_path>" << endl;
        return 0;
    }
    const char* programPath = argv[1];
    const char* dllPath = argv[2];
    STARTUPINFO startupInfo;
    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION procInfo;
    if (CreateProcessA(programPath, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &procInfo)) {
        HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procInfo.dwProcessId);
        HMODULE hModule = GetModuleHandleW(L"KERNEL32.DLL");
        if (!hModule) {
            cout << "Failed to find module" << endl;
            return -1;
        }
        FARPROC lpFunc = GetProcAddress(hModule, "LoadLibraryA");
        uint32_t arglen = strlen(dllPath)+1;
        LPVOID argv = VirtualAllocEx(hProc, NULL, arglen, MEM_COMMIT, PAGE_READWRITE);
        if (!argv)
        {
            cout<<"Failed to allocate memory" <<endl;
            return -1;
        }
        WriteProcessMemory(hProc, argv, dllPath, arglen, NULL);
        DWORD threadId = 0;
        CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)lpFunc, argv, 0, &threadId);
        if (!threadId) {
            cout << "Failed to create thread" <<endl;
            return -1;
        }
        cout << "Thread created successfully. Thread ID: 0x" << std::uppercase << std::hex << threadId << std::dec << std::endl;
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
    }
    else {
        cout<<"Failed to create process"<<endl;
        return -1;
    }
    return 0;
}