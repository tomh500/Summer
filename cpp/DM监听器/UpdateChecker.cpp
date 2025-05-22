#include "UpdateChecker.h"
#include <windows.h>
#include <fstream>
#include <string>
#include <thread>
#include <iostream>
#include <wininet.h>
#include <shlwapi.h>
#include "Global.h"
#include <string>
#include "Resource.h"
#pragma comment(lib, "wininet.lib")
#include <filesystem>
using namespace std;
// 外部声明版本号变量（你需在主程序中定义）
extern int LocalVersion;
wchar_t title[256];


using namespace std;
using namespace filesystem;
namespace fs = std::filesystem;




void CheckForUpdate(HWND hWnd) {
    std::thread([hWnd]() {
        const wchar_t* url = L"https://tomh500.github.io/Square/version.txt";

        if (debug == 1) {
            MessageBoxW(hWnd, L"[调试] 开始执行 CheckForUpdate", L"调试", MB_OK);
        }

        // 打开网络连接
        HINTERNET hInternet = InternetOpenW(L"UpdateChecker", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if (!hInternet) {
            if (debug == 1) {
                MessageBoxW(hWnd, L"[调试] InternetOpenW 失败", L"调试", MB_ICONERROR);
            }
            return;
        }

        // 获取远程版本
        HINTERNET hFile = InternetOpenUrlW(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hFile) {
            if (debug == 1) {
                MessageBoxW(hWnd, L"[调试] InternetOpenUrlW 失败", L"调试", MB_ICONERROR);
            }
            InternetCloseHandle(hInternet);
            return;
        }

        char buffer[32] = { 0 };
        DWORD bytesRead = 0;
        if (!InternetReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead) || bytesRead == 0) {
            if (debug == 1) {
                MessageBoxW(hWnd, L"[调试] InternetReadFile 失败", L"调试", MB_ICONERROR);
            }
            InternetCloseHandle(hFile);
            InternetCloseHandle(hInternet);
            return;
        }

        buffer[bytesRead] = '\0';
        int remoteVersion = atoi(buffer); // 网络版本

        InternetCloseHandle(hFile);
        InternetCloseHandle(hInternet);

        // 读取本地版本文件 src/resources/version.txt
        int localVersion = -1;
        try {
            fs::path relativePath = L"..\\..\\..\\src\\resources\\version.txt";

            // 获取当前工作目录
            fs::path currentPath = fs::current_path();

            // 计算绝对路径
            fs::path absolutePath = currentPath / relativePath;

            // 打印绝对路径，调试用
            std::wcout << L"Absolute path: " << absolutePath << std::endl;

            // 使用绝对路径打开文件
            std::ifstream localFile(absolutePath);
            if (!localFile.is_open()) {
                MessageBoxW(hWnd, L"无法读取本地版本文件", L"错误", MB_ICONERROR);
                return;
            }

            std::string line;
            std::getline(localFile, line);
            localVersion = std::stoi(line);
            localFile.close();
        }
        catch (...) {
            MessageBoxW(hWnd, L"读取本地版本时发生异常", L"错误", MB_ICONERROR);
            return;
        }

        if (debug == 1) {
            wchar_t msg[128];
            swprintf_s(msg, 128, L"[调试] 本地版本：%d，远程版本：%d", localVersion, remoteVersion);
            MessageBoxW(hWnd, msg, L"调试", MB_OK);
        }

        // 判断版本
        if (remoteVersion > localVersion) {
            SetWindowTextW(hWnd, L"挚爱的时刻CFG-Listener （有新版本可用！）");
            if (debug == 1) {
                MessageBoxW(hWnd, L"[调试] 检测到新版本可用", L"更新提示", MB_ICONINFORMATION);
            }
        }
        else if (remoteVersion == localVersion) {
            SetWindowTextW(hWnd, L"挚爱的时刻CFG-Listener （最新版）");
            if (debug == 1) {
                MessageBoxW(hWnd, L"[调试] 当前版本已是最新", L"调试", MB_OK);
            }
        }
        else {
            // 异常情况：远程版本小于本地版本
            path devKeyPath = L"..\\..\\..\\..\\MoClient_development.key";
            if (!exists(devKeyPath)) {
                MessageBoxW(hWnd, L"版本号异常，请咨询开发者！", L"错误", MB_ICONERROR | MB_OK);
                ExitProcess(1);
            }
            else {
                MessageBoxW(hWnd, L"版本号异常！但处于开发者模式", L"警告", MB_ICONWARNING | MB_OK);
            }
        }
        }).detach();
}
