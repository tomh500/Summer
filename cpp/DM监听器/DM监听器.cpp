// DM监听器.cpp : 定义应用程序的入口点。
//
//#define SDL_STATIC 
#define SDL_MAIN_HANDLED
#define _WIN32_WINNT 0x0600
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
//#include "GSI.h"
#include <shellapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "MusicPlayer.h"
#define MAX_LOADSTRING 100

int debug = 0;
int LocalVersion = 14500;  // 你的当前版本号

// 全局变量:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HANDLE hMutex = NULL;//互斥体
HWND g_hOutputEdit = nullptr;
OutputRedirector* g_outputRedirector = nullptr;

LRESULT CALLBACK MusicPlayerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MusicPlayerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
   
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
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 350,  // 默认大小
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

    CreateWindowW(L"BUTTON", L"启用击杀音效替换", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        30, buttonY, 150, buttonHeight, hWnd, (HMENU)IDC_ENABLE_KILL_SOUND, hInstance, nullptr);

    CreateWindowW(L"BUTTON", L"启动 CS2", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        200, buttonY, 150, buttonHeight, hWnd, (HMENU)IDC_TOGGLE_CS2, hInstance, nullptr);


    // 输入框位置调整为按钮上方
    int inputY = buttonY - 40;
    HWND hEdit = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
        20, inputY, 250, 25, hWnd, (HMENU)1002, hInstance, nullptr);

    SendMessage(hEdit, EM_SETCUEBANNER, TRUE, (LPARAM)L"请输入你的 Steam ID");
    CheckForUpdate(hWnd);
    //StartUpdateCheck(hWnd);

    /*
    std::wifstream inFile(L"userspace/gsi/steamid.txt");
    if (inFile) {
        std::wstring steamID;
        std::getline(inFile, steamID);
        if (!steamID.empty()) {
            PostMessage(hEdit, WM_SETTEXT, 0, (LPARAM)steamID.c_str());

        }
        inFile.close();

        HWND hListBox = CreateWindowEx(0, L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD,
            10, 10, 300, 300, hWnd, (HMENU)IDC_LISTBOX, GetModuleHandle(NULL), NULL);

    }
    */



    // 添加调试模式复选框，放在输入框右侧
    HWND hCheck = CreateWindowW(L"BUTTON", L"调试模式",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        280, inputY, 100, 25, hWnd, (HMENU)IDC_DEBUG_CHECKBOX, hInstance, nullptr);
    if (debug == 1) {
        SendMessage(hCheck, BM_SETCHECK, BST_CHECKED, 0);
    }
    else {
        SendMessage(hCheck, BM_SETCHECK, BST_UNCHECKED, 0);
    }

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
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        // 分析菜单选择和按钮点击
        switch (wmId)
        {
        case WM_CHECK_FOR_UPDATE: {// 处理更新检查请求
            CheckForUpdate(hWnd);
            return 0;
        }
        case IDM_OPEN_MOD_MARKET:  // 用户点击了“模组市场”菜单项
        {
            // 使用 ShellExecute 打开模组市场.lnk
            ShellExecuteW(hWnd, L"open", L"..\\..\\..\\模组市场.lnk", nullptr, nullptr, SW_SHOWNORMAL);
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

            // 清理资源
            SelectObject(hdcMem, hOldBitmap);
            DeleteDC(hdcMem);
            DeleteObject(hBitmap);
        }
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
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
