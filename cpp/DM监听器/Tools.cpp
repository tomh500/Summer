#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include<iostream>
#include "framework.h"
#include "DM监听器.h"
#include <windows.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <locale>
#include <codecvt>
#include <thread>
#include <chrono>
#include <tlhelp32.h>
#include <sstream>
#include <algorithm> 
#include <cctype> 
#include <psapi.h>
#include "Global.h"
#include <Shlwapi.h>

#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Shlwapi.lib")

using namespace std;
using namespace filesystem;

bool SetWorkingDirectory(LPCWSTR path) {
    wchar_t newDir[MAX_PATH] = { 0 };

    if (path == nullptr || wcslen(path) == 0) {
        // 获取程序自身所在目录
        wchar_t exePath[MAX_PATH];
        if (GetModuleFileNameW(NULL, exePath, MAX_PATH) == 0) {
            return false;
        }

        // 去掉文件名，保留目录
        if (!PathRemoveFileSpecW(exePath)) {
            return false;
        }

        wcscpy_s(newDir, exePath); // 设置为程序所在目录
    }
    else {
        // 将相对路径转为绝对路径
        if (GetFullPathNameW(path, MAX_PATH, newDir, NULL) == 0) {
            return false;
        }
    }

    // 设置当前工作目录
    return SetCurrentDirectoryW(newDir) != 0;
}
bool CheckRunPath()
{
    wchar_t exePath[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    path exeDir = path(exePath).parent_path();
    wstring exeDirStr = exeDir.wstring();
    transform(exeDirStr.begin(), exeDirStr.end(), exeDirStr.begin(), ::towlower);
    wstring expectedSuffix = L"\\counter-strike global offensive\\game\\csgo\\cfg\\dearmoments\\src\\main\\Lib";
    transform(expectedSuffix.begin(), expectedSuffix.end(), expectedSuffix.begin(), ::towlower);
    if (exeDirStr.size() >= expectedSuffix.size() &&
        exeDirStr.compare(exeDirStr.size() - expectedSuffix.size(), expectedSuffix.size(), expectedSuffix) == 0)
    {
        return true;
    }
    else
    {
        MessageBoxW(nullptr, L"路径错误，请阅读教程", L"路径错误", MB_ICONERROR | MB_OK);
        exit(0);
        return false;
    }
}
string WString2String(const wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), &str[0], size_needed, NULL, NULL);
    return str;
}

wstring String2WString(const string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
    wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], size_needed);
    return wstr;
}


bool FolderExists(const wstring& folderPath) {
    DWORD fileType = GetFileAttributesW(folderPath.c_str());
    return (fileType != INVALID_FILE_ATTRIBUTES) && (fileType & FILE_ATTRIBUTE_DIRECTORY);
}



// 复制文件并自动创建路径
bool CopyFile(const wstring& src, const wstring& dst)
{
    try {
        path dstPath(dst);
        create_directories(dstPath.parent_path()); // 自动创建目标文件夹
        copy_file(src, dst, copy_options::overwrite_existing);

        wstring successMsg = L"成功复制文件：\n" + src + L"\n到\n" + dst;

          if(debug==1)  MessageBoxW(nullptr, successMsg.c_str(), L"复制成功", MB_OK | MB_ICONINFORMATION);
        
       
        return true;
    }
    catch (const filesystem_error& e) {
        wstring errorMsg = L"复制文件失败：\n" + src + L"\n到\n" + dst +
            L"\n错误信息：" + String2WString(e.what());
        if(debug==1)MessageBoxW(nullptr, errorMsg.c_str(), L"错误", MB_OK | MB_ICONERROR);
        return false;
    }
}


int StartApps(const std::wstring& exePath, const std::wstring& arguments, bool showWindow) {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = showWindow ? SW_SHOWNORMAL : SW_HIDE;

    // 拼接命令行
    std::wstring cmdLine = L"\"" + exePath + L"\" " + arguments;
    LPWSTR cmdLineBuffer = &cmdLine[0];

    // 可选调试信息（如果需要调试输出，可以自行临时加上）
    if (debug == 1)
    {

    std::wstringstream ss;
    ss << L"即将执行的命令：\n" << cmdLine;
    MessageBoxW(nullptr, ss.str().c_str(), L"调试信息", MB_OK | MB_ICONINFORMATION);
    }


    if (!CreateProcessW(nullptr, cmdLineBuffer, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        DWORD err = GetLastError();

        std::wstringstream ss;
        ss << L"无法启动程序！错误代码：" << err;
        MessageBoxW(nullptr, ss.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}


bool IsProcessRunning(const std::wstring& processName) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return false;
    }
    cProcesses = cbNeeded / sizeof(DWORD);
    for (unsigned int i = 0; i < cProcesses; i++) {
        if (aProcesses[i] == 0) continue;

        TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
        if (hProcess != NULL) {
            HMODULE hMod;
            DWORD cbNeeded;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
            }
            CloseHandle(hProcess);
        }

        // 如果找到进程名为 cs2.exe，则返回 true
        if (_wcsicmp(szProcessName, processName.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

// 关闭进程
void KillProcess(const wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 将传入的进程名转换为小写
    wstring lowerProcessName = processName;
    transform(lowerProcessName.begin(), lowerProcessName.end(), lowerProcessName.begin(), ::towlower);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            // 将进程名转换为小写并进行比较
            wstring lowerExeName = pe32.szExeFile;
            transform(lowerExeName.begin(), lowerExeName.end(), lowerExeName.begin(), ::towlower);

            if (lowerProcessName == lowerExeName) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
}

void ClearAutoexec(HWND hWnd)
{
    // 弹出确认框，询问用户是否确认清空 autoexec.cfg
    int msgBoxResult = MessageBoxW(hWnd, L"确定要清空 autoexec.cfg 吗？", L"确认操作", MB_YESNO | MB_ICONQUESTION);

    if (msgBoxResult == IDYES)  // 用户点击了“是”
    {
        // 获取autoexec.cfg的路径
        std::wstring filePath = L"..\\..\\..\\..\\autoexec.cfg";

        // 打开文件并清空它
        std::ofstream file(filePath, std::ios::trunc);  // 以截断模式打开文件（即清空内容）
        if (file.is_open())
        {
            // 文件已清空
            MessageBoxW(hWnd, L"autoexec.cfg已清空", L"操作成功", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            // 文件打开失败
            MessageBoxW(hWnd, L"无法打开autoexec.cfg文件", L"错误", MB_OK | MB_ICONERROR);
        }
    }
    else
    {
        // 用户点击了“否”，不执行清空操作
        MessageBoxW(hWnd, L"操作已取消", L"取消操作", MB_OK | MB_ICONINFORMATION);
    }
}