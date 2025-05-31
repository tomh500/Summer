//用于检查是否存在注入情况
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>

DWORD GetProcessIdByName(const wchar_t* procName) {
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32 pe = { sizeof(pe) };
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, procName) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return pid;
}

void CheckRemoteThreads(DWORD listenerPid) {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        std::wcerr << L"无法创建线程快照" << std::endl;
        return;
    }

    THREADENTRY32 te = { sizeof(te) };
    if (Thread32First(hThreadSnap, &te)) {
        do {
            if (te.th32OwnerProcessID != listenerPid) {

            }
        } while (Thread32Next(hThreadSnap, &te));
    }

    CloseHandle(hThreadSnap);
}

int wmain() {
    const wchar_t* targetProc = L"Listener.exe";
    DWORD listenerPid = 0;

    std::wcout << L"开始监控 " << targetProc << L" 是否发起注入..." << std::endl;

    while (true) {
        if (listenerPid == 0) {
            listenerPid = GetProcessIdByName(targetProc);
            if (listenerPid != 0) {
                std::wcout << L"找到 Listener.exe，PID=" << listenerPid << std::endl;
            }
        } else {
            CheckRemoteThreads(listenerPid);
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, listenerPid);
            if (!hProc) {
                std::wcout << L"Listener.exe 进程结束，重置PID" << std::endl;
                listenerPid = 0;
            } else {
                CloseHandle(hProc);
            }
        }
        Sleep(2000);
    }
    return 0;
}
