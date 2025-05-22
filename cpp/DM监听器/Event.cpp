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

#include <algorithm> 
#include <cctype> 
#include "Tools.h"
#include "Global.h"
#pragma comment(lib, "Kernel32.lib")


namespace fs = std::filesystem;


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
    WCHAR steamID[100];
    HWND hEdit = GetDlgItem(hWnd, 1002);
    GetWindowTextW(hEdit, steamID, 100);

    if (wcslen(steamID) == 0 || wcscmp(steamID, L"请输入你的 Steam ID") == 0) {
        MessageBoxW(hWnd, L"请填写 Steam ID", L"错误", MB_OK | MB_ICONERROR);
        return;
    }

    std::wstring arguments = L"-steamid ";
    arguments += steamID;

    // 只传可执行文件路径，不带参数
   StartApps(L"DM-GSI.exe", arguments,FALSE);

    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (IsProcessRunning(L"DM-GSI.exe")) {
            PostMessageW(hWnd, WM_USER + 1, 0, 0);
        }
        else {
            //MessageBoxW(hWnd, L"启动失败", L"错误", MB_OK | MB_ICONERROR);
        }
        }).detach();
}


void CloseKillSound(HWND hWnd) {
    KillProcess(L"DM-GSI.exe");
    PostMessage(hWnd, WM_USER + 1, 0, 0);  // 通知主线程更新按钮文本
}


void quitDM(HWND hWnd)
{


  //  int result = MessageBox(hWnd, L"确定要退出吗？请确保游戏已经保存！", L"确认退出", MB_YESNO | MB_ICONQUESTION);

 //   if (result == IDYES) {

     //   KillProcess(L"cs2.exe");
    CloseKillSound(hWnd);
        const wchar_t* filePath = L"..\\..\\..\\src\\.listener.lty";
        if (fs::exists(filePath)) {
            fs::remove(filePath);
        }
        exit(0);
   // }


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
    HWND hMusicWnd = CreateWindowEx(
        0,
        L"MusicPlayerWindowClass",
        L"音乐播放器",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
        hWnd, nullptr, hInst, nullptr);

    if (hMusicWnd) {
        ShowWindow(hMusicWnd, SW_SHOW);
        UpdateWindow(hMusicWnd);
    }
}