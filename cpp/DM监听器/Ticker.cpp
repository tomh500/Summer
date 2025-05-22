#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <thread>
#include "Tools.h"
#include "Global.h" 
#include <sstream>

bool isCS2Running = false;


// 启动 CS2
void StartCS2() {
    const wchar_t* steamUri = L"steam://rungameid/730";

    // 如果处于调试模式，弹窗显示启动内容
    if (debug == 1) {
        MessageBoxW(nullptr, L"通过 Steam URI 启动 CS2: steam://rungameid/730", L"调试信息", MB_OK | MB_ICONINFORMATION);
    }

    HINSTANCE result = ShellExecuteW(nullptr, L"open", steamUri, nullptr, nullptr, SW_SHOWNORMAL);

    if ((int)result <= 32) {
        std::wstringstream ss;
        ss << L"无法通过 Steam 启动 CS2。\n错误代码：" << (int)result;
        MessageBoxW(nullptr, ss.str().c_str(), L"启动失败", MB_OK | MB_ICONERROR);
    }
}


// 监听 CS2 进程是否存在的线程函数
void CheckCS2RunningAndUpdateButton(HWND hWnd) {
    bool previousCS2Running = isCS2Running;  // 使用全局变量

    while (true) {
        bool currentCS2Running = IsProcessRunning(L"cs2.exe");

        // 如果 CS2 进程状态发生变化，发送消息更新按钮文本
        if (currentCS2Running != previousCS2Running) {
            previousCS2Running = currentCS2Running;
            isCS2Running = currentCS2Running;  // 更新全局状态

            // 通过 PostMessage 发送消息更新按钮文本
            PostMessage(hWnd, WM_USER + 2, 0, 0);  // 更新按钮文本为“关闭 CS2”或“启动 CS2”
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));  // 每3秒检查一次
    }
}