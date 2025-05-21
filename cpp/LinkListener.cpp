#include <windows.h>
#include <shobjidl.h>  // For IShellLink
#include <shlguid.h>   // For IID_IShellLink
#include <objbase.h>
#include <shlobj.h>    // For SHGetKnownFolderPath
#include <iostream>

int main() {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        std::cerr << "COM 初始化失败！" << std::endl;
        return 1;
    }

    // 获取当前目录路径
    wchar_t currentDir[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, currentDir);

    // 构造 .bat 文件路径
    std::wstring batFile = std::wstring(currentDir) + L"\\CFG启动入口.bat";
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
    std::wstring shortcutPath = std::wstring(desktopPath) + L"\\CFG启动入口.lnk";
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
