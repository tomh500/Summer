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
#include <shlwapi.h>
#include <sstream>
#include <algorithm> 
#include <cctype> 
#include "Tools.h"
#include "Global.h"
#include <aclapi.h>
#include <sddl.h>
#include <vector>
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Shlwapi.lib")
namespace fs = std::filesystem;

std::vector<HANDLE> lockedFileHandles;

fs::path RootPath = []() {
    fs::path p = std::filesystem::current_path();
    for (int i = 0; i < 3; ++i)
    {
        p = p.parent_path();
    }
    return p;
    }();


int CheckLcfgExecuted()
{
    const wchar_t* subkey = L"Software\\LCFG";
    const wchar_t* valueName = L"LCFGHasRun";
    DWORD expectedValue = 1;
    DWORD existingValue = 0;
    DWORD dataSize = sizeof(DWORD);
    DWORD type = 0;

    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        subkey,
        0,
        KEY_QUERY_VALUE,
        &hKey);

    if (result != ERROR_SUCCESS)
    {
        return 2; // 读取失败
    }

    result = RegQueryValueExW(
        hKey, valueName, nullptr, &type,
        reinterpret_cast<BYTE*>(&existingValue),
        &dataSize);

    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS || type != REG_DWORD)
    {
        return 2; // 读取失败
    }

    return (existingValue == expectedValue) ? 0 : 1;
}

void DisableSmartActiveCmdFiles()
{
    int msgboxflag = 0;
    fs::path basePath = RootPath / L"src" / L"main" / L"Features" / L"Modules" / L"SmartActive";

    if (!fs::exists(basePath) || !fs::is_directory(basePath))
        return;
    for (const auto& dir1 : fs::directory_iterator(basePath))
    {
        if (!dir1.is_directory()) continue;

        for (const auto& dir2 : fs::directory_iterator(dir1.path()))
        {
            if (!dir2.is_directory()) continue;
            std::vector<std::wstring> filenames = { L"cmd_1.cfg", L"_init_.cfg" };      //检查是否存在这些文件
            for (const auto& filename : filenames)
            {
                fs::path cfgPath = dir2.path() / filename;

                if (fs::exists(cfgPath))
                {
                    fs::path newPath = cfgPath;
                    newPath.replace_extension(L".lockfile");

                    int suffix = 1;
                    while (fs::exists(newPath))
                    {
                        std::wstring suffixStr = L".lockfile" + std::to_wstring(suffix);
                        newPath = cfgPath;
                        newPath.replace_extension(suffixStr);
                        suffix++;
                        if (suffix > 1000) break; 
                    }

                    std::error_code ec;
                    fs::rename(cfgPath, newPath, ec);
                    if (ec)
                    {
                        std::wstringstream ss;
                        ss << L"重命名失败:\n" << cfgPath.wstring() << L"\n错误: " << ec.message().c_str();
                        MessageBoxW(nullptr, ss.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
                        continue;
                    }

                    // 设置隐藏属性
                    DWORD attr = GetFileAttributesW(newPath.c_str());
                    if (attr != INVALID_FILE_ATTRIBUTES)
                    {
                        SetFileAttributesW(newPath.c_str(), attr | FILE_ATTRIBUTE_HIDDEN);
                    }
                    if (msgboxflag != 1)
                    {
                        MessageBoxW(NULL,L"你的电脑未运行过LCFG但是存在自动身法文件，已禁用相关功能，请遵守用户协议。",L"警告",MB_OK | MB_ICONWARNING);
                        msgboxflag = 1;
                    }
                }
            }
        }
    }
}



void CleanLockFiles()
{
    fs::path basePath = RootPath / L"src" / L"main" / L"Features" / L"Modules" / L"SmartActive";

    if (!fs::exists(basePath) || !fs::is_directory(basePath)) {
        std::wstringstream ss;
        ss << L"[错误] SmartActive 目录不存在:\n" << basePath.wstring();
        MessageBoxW(nullptr, ss.str().c_str(), L"清理失败", MB_OK | MB_ICONERROR);
        return;
    }

    int deleted_count = 0;
    std::wstringstream deleted_files;

    for (const auto& dir1 : fs::directory_iterator(basePath)) {
        if (!dir1.is_directory()) continue;

        for (const auto& dir2 : fs::directory_iterator(dir1.path())) {
            if (!dir2.is_directory()) continue;

            for (const auto& file : fs::directory_iterator(dir2.path())) {
                if (!file.is_regular_file()) continue;

                fs::path filePath = file.path();
                std::wstring ext = filePath.extension().wstring();

                // 判断是否以 .lockfile 开头
                if (ext.find(L".lockfile") == 0) {
                    std::error_code ec;
                    fs::remove(filePath, ec);
                    if (ec) {
                        std::wstringstream err;
                        err << L"删除失败:\n" << filePath.wstring()
                            << L"\n错误: " << ec.message().c_str();
                        MessageBoxW(nullptr, err.str().c_str(), L"删除错误", MB_OK | MB_ICONERROR);
                    }
                    else {
                        deleted_count++;
                        deleted_files << L"删除: " << filePath.wstring() << L"\n";
                    }
                }
            }
        }
    }

    std::wstringstream result;
    result << L"[完成] 已删除 " << deleted_count << L" 个 .lockfile 文件。\n";
    if (deleted_count > 0)
        result << L"\n删除文件列表:\n" << deleted_files.str();
    else
        result << L"\n没有发现任何 .lockfile 文件。";

    if (debug == 1)
    {
 MessageBoxW(nullptr, result.str().c_str(), L"清理完成", MB_OK | MB_ICONINFORMATION);
    }
   
}

void createSetupCfg()
{
    const wchar_t* filePath = L"..\\..\\..\\src\\.listener.lty";

    if (fs::exists(filePath)) {
        fs::remove(filePath);
    }

    std::wofstream file(filePath, std::ios::out | std::ios::binary);
    if (file.is_open()) {
        file.put(0xEF); file.put(0xBB); file.put(0xBF);
        file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

        file << L"alias DMListenerBoot\n";
        file << L"alias DMListenerRunning\n";
        file << L"exec DearMoments/src/main/Tools/define_mousexy;\n";

        file.close();
    }
}


void HandleKillSound(HWND hWnd) {
    StartApps(L"DM-GSI.exe", L"", FALSE);
    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (IsProcessRunning(L"DM-GSI.exe")) {
            PostMessageW(hWnd, WM_USER + 1, 0, 0);
        }
        // 启动失败时，不弹窗，根据需要可恢复
        }).detach();
}



void CloseKillSound(HWND hWnd) {
    KillProcess(L"DM-GSI.exe");
    PostMessage(hWnd, WM_USER + 1, 0, 0);  // 通知主线程更新按钮文本
}




void ClearAndResetBindings(HWND hWnd)
{
    // 获取autoexec.cfg的路径
    std::wstring filePath = L"..\\..\\..\\..\\autoexec.cfg";

    // 询问用户是否确定操作
    int result = MessageBoxW(hWnd, L"确定清空autoexec并恢复默认按键绑定吗？", L"确认操作", MB_YESNO | MB_ICONQUESTION);
    if (result == IDNO)
    {
        return;  // 用户选择取消，退出函数
    }

    // 使用宽字符流以UTF-8编码写入文件
    std::wofstream file(filePath, std::ios::out | std::ios::trunc);  // 以截断模式打开文件（即清空内容）

    // 设置UTF-8编码
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>));

    if (file.is_open())
    {
        // 写入新的默认按键绑定内容（转换为UTF-8编码）
        file << L"bind mouse_x yaw;\n"
            << L"bind mouse_y pitch;\n"
            << L"unbindall;\n"
            << L"binddefaults;\n"
            << L"binddefaults;\n"
            << L"joy_response_move 1;\n"
            << L"joy_side_sensitivity 1.000000;\n"
            << L"joy_forward_sensitivity 1.000000;\n"
            << L"cl_scoreboard_mouse_enable_binding +attack2;\n"
            << L"cl_quickinventory_filename radial_quickinventory.txt;\n"
            << L"host_writeconfig\n";

        file.close();  // 关闭文件

        // 弹出提示，告知用户需要启动一次游戏
        MessageBoxW(hWnd, L"启动一次游戏后再清空一次autoexec即可！", L"操作提示", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        // 文件打开失败
        MessageBoxW(hWnd, L"无法打开autoexec.cfg文件", L"错误", MB_OK | MB_ICONERROR);
    }
}

void MusicPlayerExit(HWND hWnd)
{
    HWND hMusicWnd = CreateWindowEx(0,L"MusicPlayerWindowClass",L"音乐播放器",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,hWnd, nullptr, hInst, nullptr);

    if (hMusicWnd) {
        ShowWindow(hMusicWnd, SW_SHOW);
        UpdateWindow(hMusicWnd);
    }
}

void LockSmartActiveFolder()
{
    if (debug != 1)
        return;

    fs::path folderPath = RootPath / L"src" / L"main" / L"Features" / L"Modules" / L"SmartActive";

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath))
    {
        if (debug == 1)
            MessageBoxW(nullptr, L"[错误] SmartActive 目录不存在，无法锁定。", L"锁定失败", MB_OK | MB_ICONERROR);
        return;
    }

    int locked_count = 0;

    for (const auto& entry : fs::recursive_directory_iterator(folderPath))
    {
        if (!entry.is_regular_file()) continue;

        HANDLE hFile = CreateFileW(
            entry.path().c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,  // 禁止写、删
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );

        if (hFile != INVALID_HANDLE_VALUE)
        {
            lockedFileHandles.push_back(hFile);
            locked_count++;
        }
    }

    if (debug == 1)
    {
        std::wstringstream ss;
        ss << L"[锁定完成] 已锁定 " << locked_count << L" 个文件。\n"<< L"SmartActive 目录现已被本程序占用，防止外部修改。";
        MessageBoxW(nullptr, ss.str().c_str(), L"锁定成功", MB_OK | MB_ICONINFORMATION);
    }
}

PSID GetCurrentUserSid()
{
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return nullptr;

    DWORD length = 0;
    GetTokenInformation(hToken, TokenUser, nullptr, 0, &length);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        CloseHandle(hToken);
        return nullptr;
    }

    TOKEN_USER* tokenUser = (TOKEN_USER*)malloc(length);
    if (!GetTokenInformation(hToken, TokenUser, tokenUser, length, &length)) {
        CloseHandle(hToken);
        free(tokenUser);
        return nullptr;
    }

    DWORD sidLength = GetLengthSid(tokenUser->User.Sid);
    PSID pSid = (PSID)malloc(sidLength);
    CopySid(sidLength, pSid, tokenUser->User.Sid);

    CloseHandle(hToken);
    free(tokenUser);
    return pSid;
}


void UnlockSmartActiveFolder()
{
    for (HANDLE hFile : lockedFileHandles)
    {
        if (hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
    }

    lockedFileHandles.clear();

    if (debug == 1)
    {
        MessageBoxW(nullptr, L"[解锁完成] 所有 SmartActive 文件占用已解除。", L"解锁成功", MB_OK | MB_ICONINFORMATION);
    }
}

void LockSmartActiveFolderNTFS()
{
    if (debug != 1)
        return;

    fs::path folderPath = RootPath / L"src" / L"main" / L"Features" / L"Modules" / L"SmartActive";

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        MessageBoxW(nullptr, L"[错误] SmartActive 目录不存在，无法锁定。", L"锁定失败", MB_OK | MB_ICONERROR);
        return;
    }

    PSID pSid = GetCurrentUserSid();
    if (!pSid) {
        MessageBoxW(nullptr, L"无法获取当前用户 SID。", L"权限设置错误", MB_OK | MB_ICONERROR);
        return;
    }

    // 设置拒绝写入权限
    EXPLICIT_ACCESSW ea = {};
    ea.grfAccessPermissions = FILE_GENERIC_WRITE | DELETE;
    ea.grfAccessMode = DENY_ACCESS;
    ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.ptstrName = (LPWSTR)pSid;

    PACL pNewDACL = nullptr;
    DWORD result = SetEntriesInAclW(1, &ea, nullptr, &pNewDACL);
    if (result != ERROR_SUCCESS) {
        MessageBoxW(nullptr, L"无法创建访问控制列表 (ACL)。", L"权限设置错误", MB_OK | MB_ICONERROR);
        free(pSid);
        return;
    }

    result = SetNamedSecurityInfoW(
        (LPWSTR)folderPath.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr, nullptr,
        pNewDACL,
        nullptr
    );

    if (result == ERROR_SUCCESS) {
        MessageBoxW(nullptr, L"SmartActive 文件夹已被锁定（禁止写入和删除）。", L"锁定成功", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBoxW(nullptr, L"设置文件夹权限失败，可能需要管理员权限。", L"权限设置失败", MB_OK | MB_ICONERROR);
    }

    // 清理资源
    if (pNewDACL) LocalFree(pNewDACL);
    if (pSid) free(pSid);
}


void UnlockSmartActiveFolderNTFS()
{
    if (debug != 1)
        return;

    fs::path folderPath = RootPath / L"src" / L"main" / L"Features" / L"Modules" / L"SmartActive";

    DWORD result = SetNamedSecurityInfoW(
        (LPWSTR)folderPath.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr, nullptr,
        nullptr, nullptr // 清除 DACL → 恢复默认权限
    );

    if (result == ERROR_SUCCESS) {
        MessageBoxW(nullptr, L"SmartActive 文件夹权限已恢复为默认状态。", L"解锁成功", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBoxW(nullptr, L"解锁权限失败，可能需要管理员权限。", L"解锁失败", MB_OK | MB_ICONERROR);
    }
}
