#define _WIN32_WINNT 0x0602 // Windows 8

#include <windows.h>
#include <shobjidl.h>  // For IShellLink
#include <shlguid.h>   // For IID_IShellLink
#include <objbase.h>
#include <shlobj.h>    // For SHGetKnownFolderPath
#include <iostream>
#include <pathcch.h>
#pragma comment(lib, "Pathcch.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")
#include <shlobj.h> 
bool SetWorkingDirectory(LPCWSTR path) {
    wchar_t newDir[MAX_PATH] = { 0 };

    if (path == nullptr || wcslen(path) == 0) {
        // 获取程序自身所在目录
        wchar_t exePath[MAX_PATH];
        if (GetModuleFileNameW(NULL, exePath, MAX_PATH) == 0) {
            return false;
        }

        // 去掉文件名，保留目录
        HRESULT hr = PathCchRemoveFileSpec(exePath, MAX_PATH);
        if (FAILED(hr)) {
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

//------------------------
// 辅助函数：优化文件复制
std::wstring Utf8ToWstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
    return wstrTo;
}


int main() {
    system("chcp 65001>nul");
    SetWorkingDirectory(L"..\\..\\..");
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        std::cerr << "COM 初始化失败！" << std::endl;
        return 1;
    }

    // 获取当前目录路径
    wchar_t currentDir[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, currentDir);

    // 构造 .bat 文件路径
    std::wstring batFile = std::wstring(currentDir) + L"\\CFG监听器主程序.bat";
    std::wstring workingDir = std::wstring(currentDir);

    // 获取桌面路径
    PWSTR desktopPath = NULL;
    hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &desktopPath);
    if (FAILED(hr)) {
        std::cerr << "无法获取桌面路径！" << std::endl;
        CoUninitialize();
        return 1;
    }

    // 快捷方式完整路径
    std::wstring shortcutPath = std::wstring(desktopPath) + L"\\CFG监听器主程序.lnk";
    CoTaskMemFree(desktopPath);

    // 创建 IShellLink 对象
    IShellLinkW* pShellLink = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&pShellLink);
    
    if (SUCCEEDED(hr)) {
        pShellLink->SetPath(batFile.c_str());
        pShellLink->SetWorkingDirectory(workingDir.c_str());
        

        // 获取 IPersistFile 接口来保存 .lnk 文件
        IPersistFile* pPersistFile;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
        if (SUCCEEDED(hr)) {
            hr = pPersistFile->Save(shortcutPath.c_str(), TRUE);
            pPersistFile->Release();
        }

        pShellLink->Release();
    }

    CoUninitialize();

    if (SUCCEEDED(hr)) {
        std::cout << "快捷方式创建成功！" << std::endl;
    } else {
        std::cerr << "创建快捷方式失败！" << std::endl;
    }

    return 0;
}
