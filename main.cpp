#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

bool InjectDLL(DWORD pid, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) return false;

    void* pAlloc = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pAlloc, dllPath, strlen(dllPath) + 1, nullptr);

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"),
        pAlloc, 0, nullptr);

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return true;
}

int main() {
    DWORD pid;
    char dllPath[MAX_PATH];

    std::cout << "Enter target PID: ";
    std::cin >> pid;
    std::cout << "Enter full DLL path: ";
    std::cin >> dllPath;

    if (InjectDLL(pid, dllPath)) {
        std::cout << "[+] Injection successful.\n";
    } else {
        std::cerr << "[-] Injection failed.\n";
    }

    return 0;
}
