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

// 检测是否有线程是由listenerPid发起远程线程注入
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
                // 打开线程句柄查询创建者信息（需要权限，且普通进程无法获取“线程创建者PID”，但我们可以尝试用NtQueryInformationThread）
                // 这里简单示例：只监控非listenerPid的线程

                // 简单思路：判断线程是否为远程线程（复杂，需要驱动支持）
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
            // TODO: 这里放检测listenerPid是否创建远程线程的逻辑
            CheckRemoteThreads(listenerPid);

            // 检查进程是否还存在
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
