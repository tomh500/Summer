// DM监听器.cpp : 定义应用程序的入口点。
//
//#define SDL_STATIC 
#define SDL_MAIN_HANDLED
#define _WIN32_WINNT 0x0600


#define _HAS_STD_BYTE 0
#include "framework.h"
#include "DM监听器.h"
#include <thread>
#include <chrono>
#include <Windows.h>
#include <string>
#include <fstream> 
#include "Event.h"
#include "Ticker.h"
#include "Tools.h"
#include "Global.h"
#include "OutputRedirector.h"
#include <commctrl.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <curl/curl.h>
#include "UpdateChecker.h"
#include <shellapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <uxtheme.h>
#pragma comment(lib, "uxtheme.lib")

#include "MusicPlayer.h"
#define MAX_LOADSTRING 100

#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

int debug = 0;
//int LocalVersion = 14500;  // 你的当前版本号

// 全局变量:
HINSTANCE hInst;
ULONG_PTR gdiplusToken;
ULONG_PTR g_diplusToken;
HBITMAP   g_hBackgroundBitmap = nullptr;
HDC       g_hdcBackgroundMem = nullptr;
WNDPROC   g_OldEditProc = nullptr;

WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HANDLE hMutex = NULL;


LRESULT CALLBACK MusicPlayerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MusicPlayerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK EditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;  // 阻止系统背景擦除
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);
        // 计算相对父窗口位置
        POINT pt = { 0,0 };
        MapWindowPoints(hWnd, GetParent(hWnd), &pt, 1);
        // 背景图贴图
        if (g_hdcBackgroundMem) {
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, g_hdcBackgroundMem, pt.x, pt.y, SRCCOPY);
        }
        else {
            FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
        }
        // GDI+ 半透明背景 + 边框
        Gdiplus::Graphics g(hdc);
        Gdiplus::SolidBrush brush(Gdiplus::Color(120, 50, 50, 50));
        g.FillRectangle(&brush, 0, 0, rc.right, rc.bottom);
        Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255));
        g.DrawRectangle(&pen, 0.5f, 0.5f, rc.right - 1.0f, rc.bottom - 1.0f);
        EndPaint(hWnd, &ps);
        return 0;
    }


    default:
        if (g_OldEditProc)
            return CallWindowProc(g_OldEditProc, hWnd, msg, wParam, lParam);
        else
            return DefWindowProc(hWnd, msg, wParam, lParam);
    
    }
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);


    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
  //SetWorkingDirectory(L"..\\..\\..");

    // 创建一个唯一的互斥体名称
    const std::wstring mutexName = L"Global\\DearMomentsAppMutex";

    // 尝试创建一个名为 "DearMomentsAppMutex" 的互斥体
    hMutex = CreateMutexW(NULL, TRUE, mutexName.c_str());

    if (hMutex == NULL)
    {
        // 创建互斥体失败，说明可能没有足够的权限或者系统出了问题
        MessageBoxW(NULL, L"创建互斥体失败！", L"错误", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // 检查互斥体是否已经存在，意味着程序已经在运行
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxW(NULL, L"DearMoments已经在运行！", L"错误", MB_OK | MB_ICONERROR);
        CloseHandle(hMutex); // 关闭互斥体句柄
        return FALSE; // 退出程序，避免重复运行
    }

    // 程序正常启动，继续执行原有的代码
    if (debug != 1)
    {
        CheckRunPath(); // 检查运行路径
    }
    createSetupCfg();   // 创建cfg文件

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    // 注册音乐播放器窗口类
    WNDCLASS wc = {};
    wc.lpfnWndProc = MusicPlayerWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MusicPlayerWindowClass";
    RegisterClass(&wc);


    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DM));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 退出时关闭互斥体句柄
    CloseHandle(hMutex);

    return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DM);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

    hInst = hInstance; // 将实例句柄存储在全局变量中

    // 计算屏幕分辨率
    int screenWidth = GetSystemMetrics(SM_CXSCREEN); // 屏幕宽度
    int screenHeight = GetSystemMetrics(SM_CYSCREEN); // 屏幕高度

    // 创建窗口
    HWND hWnd = CreateWindowW(szWindowClass, szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 350,
        nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) {
        return FALSE;  // 如果窗口创建失败，返回 FALSE
    }

    // 获取窗口的宽度和高度
    RECT rcWindow;
    GetWindowRect(hWnd, &rcWindow);  // 获取窗口的边界

    int windowWidth = rcWindow.right - rcWindow.left;
    int windowHeight = rcWindow.bottom - rcWindow.top;

    // 计算窗口左上角的坐标，使窗口居中
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    // 设置窗口位置
    SetWindowPos(hWnd, HWND_TOP, posX, posY, 0, 0, SWP_NOSIZE);

    // 显示窗口
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 获取窗口的高度
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    // 动态调整按钮位置，使其位于窗口底部
    int buttonHeight = 60;
    int buttonY = rcClient.bottom - buttonHeight - 50;

    CreateWindowW(L"BUTTON", L"启用击杀音效替换",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
        30, buttonY, 150, buttonHeight, hWnd, (HMENU)IDC_ENABLE_KILL_SOUND, hInstance, nullptr);

    CreateWindowW(L"BUTTON", L"启动 CS2",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
        200, buttonY, 150, buttonHeight, hWnd, (HMENU)IDC_TOGGLE_CS2, hInstance, nullptr);

    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    SetWindowLongPtr(hWnd, GWL_STYLE, style | WS_CLIPCHILDREN);


    // 输入框位置调整为按钮上方
    int inputY = buttonY - 40;
    HWND hEdit = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        20, inputY, 250, 25, hWnd, (HMENU)1002, hInstance, nullptr);
    SetWindowTheme(hEdit, L"", L"");  // 禁用主题，防止背景重绘

    CheckForUpdate(hWnd);



    // 添加运行命令按钮，放在调试复选框右边
    CreateWindowW(L"BUTTON", L"运行命令",
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        280, inputY, 100, 25, hWnd, (HMENU)IDC_RUN_COMMAND_BUTTON, hInstance, nullptr);

    // 添加调试模式复选框，放在输入框右侧
    HWND hCheck = CreateWindowW(L"BUTTON", L"调试模式",
        WS_CHILD | BS_AUTOCHECKBOX,  // 去掉 WS_VISIBLE
        280, inputY, 100, 25, hWnd, (HMENU)IDC_DEBUG_CHECKBOX, hInstance, nullptr);


    if (debug == 1) {
        SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
    }
    else {
        SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
    }
    extern WNDPROC g_OldEditProc; // 声明全局原窗口过程指针
    g_OldEditProc = (WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)EditProc);

    if (CheckLcfgExecuted() !=0)
    {
        DisableSmartActiveCmdFiles();
    }
    CleanLockFiles();

    std::thread([=]() {
        while (true) {
            CheckCS2RunningAndUpdateButton(hWnd);
            std::this_thread::sleep_for(std::chrono::seconds(3));  // 每3秒检查一次
        }
        }).detach();

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//

//音乐播放器


//主窗口
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if (dis->CtlType == ODT_BUTTON)
        {
            int w = dis->rcItem.right - dis->rcItem.left;
            int h = dis->rcItem.bottom - dis->rcItem.top;
            HDC hdcBtn = dis->hDC;

            // ————— 1. 计算按钮在主窗口客户区的位置 —————
            // dis->rcItem 是相对于按钮父窗口的坐标，先取左上角
            POINT pt = { dis->rcItem.left, dis->rcItem.top };
            // 将这个点从按钮父窗口(client)坐标系，映射到主窗口(client)坐标系
            MapWindowPoints(dis->hwndItem, hWnd, &pt, 1);

            // ————— 2. 从全局背景位图 DC 中截取子图 —————
            if (g_hdcBackgroundMem)
            {
                BitBlt(
                    hdcBtn,         // 目标：按钮 DC
                    0, 0, w, h,     // 目标区域：按钮大小
                    g_hdcBackgroundMem, // 源：全局背景 DC
                    pt.x, pt.y,     // 源点：映射后的按钮左上角
                    SRCCOPY
                );
            }
            else
            {
                // 如未成功创建全局背景 DC，再退回拷贝父窗口背景
                HWND hwndParent = GetParent(dis->hwndItem);
                HDC  hdcParent = GetDC(hwndParent);
                POINT pt2 = { dis->rcItem.left, dis->rcItem.top };
                MapWindowPoints(dis->hwndItem, hwndParent, &pt2, 1);
                BitBlt(hdcBtn, 0, 0, w, h, hdcParent, pt2.x, pt2.y, SRCCOPY);
                ReleaseDC(hwndParent, hdcParent);
            }

            // ————— 3. 半透明叠加 —————
            Gdiplus::Graphics graphics(hdcBtn);
            Gdiplus::Color overlay(120, 0, 0, 139);
            if (dis->itemState & ODS_SELECTED)
                overlay = Gdiplus::Color(120, 169, 169, 169);
            else if (dis->itemState & ODS_DISABLED)
                overlay = Gdiplus::Color(120, 169, 169, 169);
            // Hotlight 同样需要你在 WM_MOUSEMOVE/WM_MOUSELEAVE 中设置状态并 Invalidate

            if (overlay.GetA() > 0)
            {
                Gdiplus::SolidBrush b(overlay);
                graphics.FillRectangle(&b, 0, 0, w, h);
            }

            // ————— 4. 边框 —————
            Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));
            graphics.DrawRectangle(&pen, 0.5f, 0.5f, w - 1.0f, h - 1.0f);

            // ————— 5. 文本 —————
// ————— 5. 文本 —————
            WCHAR textBuffer[256] = { 0 };
            GetWindowText(dis->hwndItem, textBuffer, 256);
            const wchar_t* text = textBuffer;

            // 构造 GDI+ Font，失败时回退
            Gdiplus::Font* pFont = nullptr;
            {
                Gdiplus::FontFamily ff(L"Segoe UI");
                pFont = new Gdiplus::Font(&ff, 16, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
                if (pFont->GetLastStatus() != Gdiplus::Ok) {
                    delete pFont;
                    Gdiplus::FontFamily fb(L"Arial");
                    pFont = new Gdiplus::Font(&fb, 16, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                }
            }
            if (pFont && pFont->GetLastStatus() == Gdiplus::Ok)
            {
                Gdiplus::SolidBrush txtBrush(Gdiplus::Color(255, 255, 182, 193));
                Gdiplus::RectF layout(0, 0, (Gdiplus::REAL)w, (Gdiplus::REAL)h);
                Gdiplus::StringFormat fmt;
                fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
                fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
                graphics.DrawString(text, -1, pFont, layout, &fmt, &txtBrush);
            }
            delete pFont;


            return TRUE;
        }
        break;
    }




    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        // 分析菜单选择和按钮点击
        switch (wmId)
        {
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            HWND hEdit = (HWND)lParam;

            // 设置透明背景 + 白色文字
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));

            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        case IDC_RUN_COMMAND_BUTTON:
        {
            WCHAR cmdBuffer[512] = { 0 };
            HWND hEdit = GetDlgItem(hWnd, 1002);
            GetWindowTextW(hEdit, cmdBuffer, 512);

            std::wstring cmdStr(cmdBuffer);

            if (cmdStr.empty())
            {
                MessageBoxW(hWnd, L"命令不能为空。", L"运行失败", MB_OK | MB_ICONERROR);
                break;
            }

            // 判断是否是 /debug 命令
            if (cmdStr.find(L"/debug") == 0)
            {
                if (cmdStr.length() < 8) {
                    MessageBoxW(hWnd, L"无效的 /debug 命令。格式：/debug 0 或 /debug 1", L"错误", MB_OK | MB_ICONERROR);
                    break;
                }

                WCHAR arg = cmdStr[7]; // 取第8个字符

                if (arg == L'1')
                {
                    // 开启 debug 需检测文件是否存在
                    fs::path keyPath = RootPath.parent_path() / L"MoClient_development.key";
                    if (fs::exists(keyPath))
                    {
                        debug = 1;
                        MessageBoxW(hWnd, L"开发者权限已启用。", L"成功", MB_OK | MB_ICONINFORMATION);
                    }
                    else
                    {
                        MessageBoxW(hWnd, L"没有开发者权限，无法启用。", L"权限不足", MB_OK | MB_ICONERROR);
                    }
                }
                else if (arg == L'0')
                {
                    debug = 0;
                    MessageBoxW(hWnd, L"开发者权限已禁用。", L"成功", MB_OK | MB_ICONINFORMATION);
                }
                else
                {
                    MessageBoxW(hWnd, L"无效的 /debug 参数，只能是0或1。", L"错误", MB_OK | MB_ICONERROR);
                }
                break;
            }

            // 执行 /dos 命令，必须以 /dos 开头且 debug == 1
            if (cmdStr.find(L"/dos ") == 0)
            {
                if (debug != 1)
                {
                    MessageBoxW(hWnd, L"无开发者权限，无法执行该命令。", L"权限不足", MB_OK | MB_ICONERROR);
                    break;
                }

                std::wstring realCmd = cmdStr.substr(5); // 去除 "/dos "
                if (realCmd.empty())
                {
                    MessageBoxW(hWnd, L"命令不能为空。", L"运行失败", MB_OK | MB_ICONERROR);
                    break;
                }

                std::wstring fullCmd = L"cmd.exe /C \"" + realCmd + L"\"";

                STARTUPINFOW si = { sizeof(si) };
                PROCESS_INFORMATION pi;
                std::wstring workingDir = RootPath.wstring();
                WCHAR cmdLine[600];
                wcscpy_s(cmdLine, fullCmd.c_str());

                BOOL result = CreateProcessW(
                    nullptr, cmdLine, nullptr, nullptr, FALSE,
                    CREATE_NO_WINDOW, nullptr,
                    workingDir.c_str(), &si, &pi
                );

                if (!result)
                {
                    DWORD err = GetLastError();
                    std::wstring errMsg = L"启动失败，错误代码：" + std::to_wstring(err);
                    MessageBoxW(hWnd, errMsg.c_str(), L"运行失败", MB_OK | MB_ICONERROR);
                }
                else
                {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                    MessageBoxW(hWnd, L"命令已执行。", L"成功", MB_OK | MB_ICONINFORMATION);
                }
            }
            else
            {
                MessageBoxW(hWnd, L"无效命令，必须以 /dos 开头或 /debug 开头。", L"错误", MB_OK | MB_ICONERROR);
            }
            break;
        }
        case WM_CHECK_FOR_UPDATE: {// 处理更新检查请求
            CheckForUpdate(hWnd);
            return 0;
        }
        case IDM_OPEN_MOD_MARKET:  // 用户点击了“模组市场”菜单项
        {
            StartAppsNew(RootPath / "src" / "main" / "Lib" / "ModsMarket.exe", L"", true);


            break;
        }
        case IDM_USERSPACE_FOLDER:
        {

           wstring userspacePath =  L"..\\..\\..\\userspace";
           ShellExecuteW(NULL, L"open", userspacePath.c_str(), NULL, NULL, SW_SHOW);
            break;
        }
        case IDM_INSTALL_CFG:
        {
			if (!CFGInstaller()==0)
			{
				MessageBoxW(hWnd, L"安装失败", L"提示", MB_OK | MB_ICONERROR);
			}
            break;

        }
        case IDC_TOGGLE_CS2: {
            WCHAR buttonText[100];
            GetDlgItemText(hWnd, IDC_TOGGLE_CS2, buttonText, 100);

            if (wcscmp(buttonText, L"启动 CS2") == 0) {
                StartCS2();  // 启动 CS2
                SetDlgItemText(hWnd, IDC_TOGGLE_CS2, L"关闭 CS2");  // 更新按钮文本
            }
            else {
                KillProcess(L"cs2.exe");  // 关闭 CS2
                SetDlgItemText(hWnd, IDC_TOGGLE_CS2, L"启动 CS2");  // 更新按钮文本
            }
            break;
        }
        case IDM_CLEAR_AUTOEXEC:  // 清空autoexec
            ClearAutoexec(hWnd);
            break;
        case IDM_UPDATE_CHECK:
        {
            CheckForUpdate(hWnd);
            break;
        }
        case IDM_CLEAR_AND_RESET_BINDINGS:
        {
            // 清空autoexec并恢复默认按键绑定
            ClearAndResetBindings(hWnd);
            break;
        }
        case IDC_ENABLE_KILL_SOUND: {
            // 获取按钮文本，判断按钮状态
            WCHAR buttonText[100];
            GetDlgItemText(hWnd, IDC_ENABLE_KILL_SOUND, buttonText, 100);

            // 根据按钮文本执行不同的操作
            if (wcscmp(buttonText, L"启用击杀音效替换") == 0) {
                // 获取用户输入的 SteamID
                WCHAR steamIDBuffer[100];
                GetDlgItemTextW(hWnd, 1002, steamIDBuffer, 100);

                // 仅在非空时保存
                if (wcslen(steamIDBuffer) > 0) {
                    // 创建目录
                    CreateDirectoryW(L"..\\..\\..\\userspace", NULL);
                    CreateDirectoryW(L"..\\..\\..\\userspace\\gsi", NULL);

                    // 写入文件
                    std::wofstream outFile(L"..\\..\\..\\userspace\\gsi\\steamid.txt", std::ios::trunc);
                    if (outFile) {
                        outFile << steamIDBuffer;
                        outFile.close();
                    }
                }

                HandleKillSound(hWnd);  // 启动击杀音效替换

            }
            else if (wcscmp(buttonText, L"关闭击杀音效替换") == 0) {
                CloseKillSound(hWnd);   // 关闭击杀音效替换
            }
            break;
        }
        case IDC_DEBUG_CHECKBOX:
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            debug = (state == BST_CHECKED) ? 1 : 0;
            break;
        }

        case IDM_MUSICPLAYER_INIT: {
            MusicPlayerExit(hWnd);
            break;
        }

        case IDM_ITEMS_THROW_FOLDER:
        {
            wstring userspacePath = L"..\\..\\..\\src\\legacy";
            ShellExecuteW(NULL, L"open", userspacePath.c_str(), NULL, NULL, SW_SHOW);
            break;
        }

        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;

        case IDM_EXIT:
            quitDM(hWnd);

            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_USER + 1:
    {
        // 更新按钮文本
        WCHAR buttonText[100];
        GetDlgItemText(hWnd, IDC_ENABLE_KILL_SOUND, buttonText, 100);
        if (wcscmp(buttonText, L"启用击杀音效替换") == 0) {
            SetDlgItemText(hWnd, IDC_ENABLE_KILL_SOUND, L"关闭击杀音效替换");
        }
        else {
            SetDlgItemText(hWnd, IDC_ENABLE_KILL_SOUND, L"启用击杀音效替换");
        }
        break;
    }
    case WM_USER + 2: {
        // 更新 CS2 按钮文本
        WCHAR buttonText[100];
        GetDlgItemText(hWnd, IDC_TOGGLE_CS2, buttonText, 100);
        if (isCS2Running) {
            // 如果 CS2 正在运行
            if (wcscmp(buttonText, L"启动 CS2") == 0) {
                SetDlgItemText(hWnd, IDC_TOGGLE_CS2, L"关闭 CS2");
                cout << "CS2已启动" << endl;
            }
        }
        else {
            // 如果 CS2 没有运行
            if (wcscmp(buttonText, L"关闭 CS2") == 0) {
                SetDlgItemText(hWnd, IDC_TOGGLE_CS2, L"启动 CS2");
				cout << "CS2已关闭" << endl;
            }
        }
        break;
    }



    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        // 从资源加载位图

        HBITMAP hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BACKGROUND));
        if (hBitmap)
        {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

            BITMAP bmp;
            GetObject(hBitmap, sizeof(BITMAP), &bmp);

            // 绘制背景图像
            BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);
            g_hBackgroundBitmap = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BACKGROUND));
            if (g_hBackgroundBitmap) {
                HDC hdc = GetDC(hWnd);
                g_hdcBackgroundMem = CreateCompatibleDC(hdc);
                SelectObject(g_hdcBackgroundMem, g_hBackgroundBitmap);
                ReleaseDC(hWnd, hdc);
            }

            // 清理资源
            SelectObject(hdcMem, hOldBitmap);
            DeleteDC(hdcMem);
            DeleteObject(hBitmap);
        }
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        if (g_hdcBackgroundMem) DeleteDC(g_hdcBackgroundMem);
        if (g_hBackgroundBitmap) DeleteObject(g_hBackgroundBitmap);
        PostQuitMessage(0);
        break;

    case WM_CLOSE:
        quitDM(hWnd);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
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
    Gdiplus::GdiplusShutdown(gdiplusToken);  // 在程序退出前调用
    exit(0);
    // }


}