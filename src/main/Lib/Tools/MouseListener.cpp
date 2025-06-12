#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>

HHOOK hMouseHook;
bool running = true;

// 处理鼠标事件
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        if (pMouseStruct) {
            std::cout << "鼠标点击位置: (" << pMouseStruct->pt.x << ", " << pMouseStruct->pt.y << ")\n";
        }
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

// 设置鼠标钩子
void SetMouseHook() {
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, nullptr, 0);
    if (!hMouseHook) {
        std::cerr << "设置鼠标钩子失败！\n";
    }
}

// 释放鼠标钩子
void ReleaseMouseHook() {
    UnhookWindowsHookEx(hMouseHook);
}

// 主循环：使用 PeekMessage 以减少 CPU 负载
void MessageLoop() {
    MSG msg;
    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 休眠减少 CPU 占用
    }
}

int main() {
    SetMouseHook();
    std::cout << "正在监听鼠标点击事件...\n按下 Ctrl+C 退出\n";

    // 使用线程处理消息循环，减少影响主线程
    std::thread msgThread(MessageLoop);
    msgThread.join();

    ReleaseMouseHook();
    return 0;
}
