#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <intrin.h>
#include <algorithm>
#include "resource.h"
#include <Wbemidl.h>
#include <atlbase.h>
#include <thread>
#include <filesystem>
#include <comutil.h>
#include <shlobj.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <wininet.h>
#include "CreatFolder.h"
#include "sw.h"

#pragma comment(lib, "wininet.lib")

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "shell32.lib")

using namespace std;
using namespace std::filesystem;
namespace fs = std::filesystem;

int vendor_c = 0;

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

//------------------------
// 辅助函数：优化文件复制
std::wstring Utf8ToWstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
    return wstrTo;
}



//------------------------
// 辅助函数：添加启动项到 autoexec.cfg
bool AddStartupToAutoexec(const std::string& autoexecPath) {
    try {
        path filePath(autoexecPath);
        path parentDir = filePath.parent_path();
        if (!exists(parentDir)) {
            create_directories(parentDir);
        }
        // 以追加模式打开文件（不存在则创建）
        ofstream ofs(autoexecPath, ios::app);
        if (!ofs.is_open()) {
            cerr << "打开 autoexec.cfg 失败。" << endl;
            return false;
        }
        // 写入前先添加换行符
        ofs << "\r\nexec DearMoments/setup\r\n";
        ofs.close();
        return true;
    }
    catch (const filesystem_error& e) {
        cerr << "修改 autoexec.cfg 失败: " << e.what() << endl;
        return false;
    }
}

//------------------------
// 初始化 SDL2 和 SDL_mixer
bool InitSDL() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        cerr << "SDL 初始化失败: " << SDL_GetError() << endl;
        return false;
    }
    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        cerr << "SDL_mixer 初始化失败: " << Mix_GetError() << endl;
        SDL_Quit();
        return false;
    }
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        cerr << "音频设备打开失败: " << Mix_GetError() << endl;
        Mix_Quit();
        SDL_Quit();
        return false;
    }
    return true;
}

//------------------------
// 从内存播放 MP3（BGM）
void PlayBGMFromMemory(UINT resourceID) {
    HRSRC hRes = FindResourceW(NULL, MAKEINTRESOURCEW(resourceID), RT_RCDATA);
    if (!hRes) {
        cerr << "找不到资源" << endl;
        return;
    }
    HGLOBAL hData = LoadResource(NULL, hRes);
    if (!hData) {
        cerr << "加载资源失败" << endl;
        return;
    }
    DWORD dwSize = SizeofResource(NULL, hRes);
    void* pData = LockResource(hData);
    if (!pData) {
        cerr << "锁定资源失败" << endl;
        return;
    }
    SDL_RWops* rw = SDL_RWFromConstMem(pData, dwSize);
    if (!rw) {
        cerr << "创建 RWops 失败: " << SDL_GetError() << endl;
        return;
    }
    Mix_Music* music = Mix_LoadMUS_RW(rw, 1);  // 自动释放 rw
    if (!music) {
        cerr << "加载音乐失败: " << Mix_GetError() << endl;
        return;
    }
    if (Mix_PlayMusic(music, 1) == -1) {
        cerr << "播放音乐失败: " << Mix_GetError() << endl;
        Mix_FreeMusic(music);
        return;
    }
    while (Mix_PlayingMusic()) {
        SDL_Delay(100);
    }
    Mix_FreeMusic(music);
}

//------------------------
// 获取系统国家/地区代码
int GetRegionCode() {
    char region[256];
    int len = GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, region, sizeof(region));
    if (len > 0) {
        string regionStr(region);
        if (regionStr == "CN") {
            return 1;
        }
        else if (regionStr == "TW") {
            return 2;
        }
        else if (regionStr == "JP") {
            return 3;
        }
    }
    return 0; // 未知
}

//------------------------
// 获取 CPU 品牌信息
string GetCpuBrand() {
    int cpuInfo[4] = { 0 };
    char brand[0x40] = { 0 };
    __cpuid(reinterpret_cast<int*>(cpuInfo), 0x80000002);
    memcpy(brand, cpuInfo, sizeof(cpuInfo));
    __cpuid(reinterpret_cast<int*>(cpuInfo), 0x80000003);
    memcpy(brand + 16, cpuInfo, sizeof(cpuInfo));
    __cpuid(reinterpret_cast<int*>(cpuInfo), 0x80000004);
    memcpy(brand + 32, cpuInfo, sizeof(cpuInfo));
    return string(brand);
}

void GetCPUInfo() {
    int cpuInfo[4] = { 0 };
    char cpuVendor[13] = { 0 };
    __cpuid(cpuInfo, 0);
    memcpy(cpuVendor, &cpuInfo[1], 4);
    memcpy(cpuVendor + 4, &cpuInfo[3], 4);
    memcpy(cpuVendor + 8, &cpuInfo[2], 4);
    string vendor(cpuVendor);
    cout << "CPU 厂商: " << vendor << endl;
    string brand = GetCpuBrand();
    cout << "CPU 型号: " << brand << endl;
}

void SetBackgroundColor(const string& vendor) {
    if (vendor == "GenuineIntel")
        system("color CE");
    else if (vendor == "AuthenticAMD")
        system("color 9F");
    else
        system("color 0F");
}

bool CheckForExistingAlias(const string& filePath, string& aliasCommand) {
    ifstream file(filePath);
    string line;
    bool aliasFound = false;
    while (getline(file, line)) {
        if (line.find("alias DearMoments_Fps_Default") != string::npos) {
            aliasFound = true;
            size_t fpsPos = line.find("fps_max");
            if (fpsPos != string::npos) {
                aliasCommand = line.substr(fpsPos);
            }
            break;
        }
    }
    return aliasFound;
}

void AppendToFile(const string& filePath, const string& content) {
    ofstream file(filePath, ios::app);
    if (file.is_open()) {
        file << content << endl;
        file.close();
    }
    else {
        MessageBox(NULL, L"追加失败。", L"错误", MB_ICONERROR);
    }
}

void HandleCpuTypeAndAppend(const string& vendor, const string& brand, const string& filePath, string& fps) {
    string newAliasCommand;
    if (vendor == "GenuineIntel") {
        newAliasCommand = "//Intel";
        vendor_c = 1;
    }
    else if (vendor == "AuthenticAMD") {
        newAliasCommand = "//AMD";
        vendor_c = 2;
    }
    else {
        MessageBox(NULL, L"检测到未知的CPU厂商，程序无法运行。", L"错误", MB_ICONERROR);
        exit(1);
    }
}



void ShowRegionMessage(int regionCode) {
    wstring message;
    switch (regionCode) {
    case 1:
        message = L"DearMoments公开版，完全免费发布，您的地区：中国大陆";
        break;
    case 2:
        message = L"DearMoments公开版，完全免费发布，您的地区：台湾地区";
        break;
    case 3:
        message = L"DearMoments公开版，完全免费发布，您的地区：日本";
        break;
    default:
        message = L"非中国、日本地区，可能不适配您的语言信息，本CFG完全免费！";
        break;
    }
    MessageBoxW(NULL, message.c_str(), L"地区信息", MB_OK | MB_ICONINFORMATION);
}

// 创建快捷方式（快捷方式文件将放置在当前目录，目标为当前目录下的 ModsMarket.exe）
bool CreateShortcut(const wchar_t* shortcutPath, const wchar_t* targetPath, const wchar_t* workingDir) {
    CoInitialize(NULL);
    IShellLink* pShellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
    if (SUCCEEDED(hr)) {
        pShellLink->SetPath(targetPath);
        pShellLink->SetWorkingDirectory(workingDir);
        IPersistFile* pPersistFile = nullptr;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
        if (SUCCEEDED(hr)) {
            hr = pPersistFile->Save(shortcutPath, TRUE);
            pPersistFile->Release();
            pShellLink->Release();
            CoUninitialize();
            return SUCCEEDED(hr);
        }
        pShellLink->Release();
    }
    CoUninitialize();
    return false;
}

bool SaveResourceToFile(UINT resourceID, const wchar_t* fileName) {
    HRSRC hResInfo = FindResourceW(NULL, MAKEINTRESOURCEW(resourceID), RT_RCDATA);
    if (hResInfo == NULL) {
        wcerr << L"找不到资源。" << endl;
        return false;
    }

    HGLOBAL hResData = LoadResource(NULL, hResInfo);
    if (hResData == NULL) {
        wcerr << L"加载资源失败。" << endl;
        return false;
    }

    LPVOID pData = LockResource(hResData);
    if (pData == NULL) {
        wcerr << L"锁定资源失败。" << endl;
        return false;
    }

    DWORD dwSize = SizeofResource(NULL, hResInfo);
    if (dwSize == 0) {
        wcerr << L"获取资源大小失败。" << endl;
        return false;
    }

    ofstream outputFile(fileName, ios::binary);
    if (!outputFile.is_open()) {
        wcerr << L"打开文件写入失败: " << fileName << endl;
        return false;
    }

    outputFile.write(reinterpret_cast<const char*>(pData), dwSize);
    outputFile.close();

    wcout << L"资源已保存到文件: " << fileName << endl;
    return true;
}


// 下载文件并返回 UTF-8 编码的 string
string DownloadFile(const wstring& url) {
    HINTERNET hInternet = InternetOpen(L"HTTPGET", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return "";

    HINTERNET hConnect = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[4096];
    DWORD bytesRead;
    string content;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead) {
        content.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return content;
}


bool CopyFileOptimized(const std::wstring& src, const std::wstring& dest) {
    try {
        fs::path srcPath(src);
        fs::path destPath(dest);
        fs::path destDir = destPath.parent_path();

        if (!fs::exists(destDir)) {
            fs::create_directories(destDir);
        }

        fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);

        cout << "文件复制成功: " << WStringToString(src)
            << " -> " << WStringToString(dest) << endl;

        return true;
    }
    catch (const fs::filesystem_error&) {
        cout << "文件复制失败: " << WStringToString(src)
            << " -> " << WStringToString(dest) << endl;
        return false;
    }
}

bool CheckRunPath()
{
    // 获取当前程序的路径
    wchar_t exePath[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    // 获取当前程序的目录
    fs::path exeDir = fs::path(exePath).parent_path();

    // 转换路径为小写进行比较
    wstring exeDirStr = exeDir.wstring();
    transform(exeDirStr.begin(), exeDirStr.end(), exeDirStr.begin(), ::towlower);

    // 目标路径的后缀（以小写形式存储）
    wstring expectedSuffix = L"\\counter-strike global offensive\\game\\csgo\\cfg\\dearmoments\\src\\main\\Lib";
    transform(expectedSuffix.begin(), expectedSuffix.end(), expectedSuffix.begin(), ::towlower);

    // 检查当前路径是否以预期的后缀结尾
    if (exeDirStr.size() >= expectedSuffix.size() &&
        exeDirStr.compare(exeDirStr.size() - expectedSuffix.size(), expectedSuffix.size(), expectedSuffix) == 0)
    {
        return true; // 路径匹配，返回 true
    }
    else
    {
        // 路径不匹配，弹出错误消息框并退出
        MessageBoxW(nullptr, L"运行路径错误，请仔细阅读教程！", L"路径错误", MB_ICONERROR | MB_OK);
        exit(0); // 退出程序
        return false;
    }
}

int main() {
    SetWorkingDirectory(L"..\\..\\..");
    system("@echo off");
    system("chcp 65001");
 //   CreatFolder();

    path basePath = "..\\..\\sounds";
    path current_path = fs::current_path();
   path target_path = L"Counter-Strike Global Offensive\\game\\csgo\\cfg\\DearMoments\\src\\main\\Lib";
   CheckRunPath(); 
  
        create_directory(L"..\\..\\sounds\\DearMoments");
        create_directory(L"..\\..\\sounds\\DearMoments\\musicmode");
        create_directory(L"..\\..\\sounds\\DearMoments\\temp");
        int regionCode = GetRegionCode();  
        ShowRegionMessage(regionCode);     

        if (!InitSDL()) {
            return -1;
        }
        //音乐播放 取消注释然后添加资源文件即可
        /*
        UINT bgmResourceId = 0;
        if (regionCode == 1) {
            bgmResourceId = IDR_BGM_CN;
        }
        else if (regionCode == 2) {
            bgmResourceId = IDR_BGM_CN;
        }
        thread musicThread;
        if (bgmResourceId != 0) {
            musicThread = thread(PlayBGMFromMemory, bgmResourceId);
        }
        
       
       */
        system("cls");
        GetCPUInfo();

        char cpuVendor[13] = { 0 };
        int cpuInfo[4] = { 0 };
        __cpuid(cpuInfo, 0);
        memcpy(cpuVendor, &cpuInfo[1], 4);
        memcpy(cpuVendor + 4, &cpuInfo[3], 4);
        memcpy(cpuVendor + 8, &cpuInfo[2], 4);
        string vendor(cpuVendor);
        SetBackgroundColor(vendor);

        string brand = GetCpuBrand();


        wstring messageCPU = L"CPU 厂商: " + wstring(vendor.begin(), vendor.end()) + L"\n" +
            L"CPU 型号: " + wstring(brand.begin(), brand.end());
        MessageBox(NULL, messageCPU.c_str(), L"GetCpuInfo", MB_OK | MB_ICONINFORMATION);

       wchar_t LinkerCFG[] = L"CFGShortLinker.exe";
        if (!SaveResourceToFile(IDR_CFG_LINKER, LinkerCFG)) {
            cout<< "未能正确创建快捷方式！" << endl;
        }
       ifstream checkFile(LinkerCFG);
       if (checkFile.good()) {
           checkFile.close();
           // 启动外部程序
           system("start CFGShortLinker.exe");
           Sleep(1000);

           // 删除文件
           if (!DeleteFile(LinkerCFG)) {
               MessageBox(NULL, L"临时文件删除失败。", L"警告", MB_ICONWARNING);
           }
       }
       else {
           MessageBox(NULL, L"文件保存失败，未能创建文件。", L"错误", MB_ICONERROR);
       }
        // 询问是否打开模组市场
        int modResponse = MessageBox(NULL, L"是否打开模组市场？", L"模组市场", MB_YESNO | MB_ICONQUESTION);
        if (modResponse == IDYES) {
            STARTUPINFO si = { sizeof(si) };
            PROCESS_INFORMATION pi;

            if (CreateProcess(L"src\\main\\Lib\\ModsMarket.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            else {
                MessageBox(NULL, L"无法启动模组市场！", L"错误", MB_OK | MB_ICONERROR);
            }
        }


        
        system("cls");
        // 复制 DearMoments_Installed.cfg 文件
        CopyFileOptimized(L"src\\resources\\keybindings_schinese.txt", L"..\\..\\resource\\keybindings_schinese.txt");
        CopyFileOptimized(L"src\\resources\\DearMoments_Installed.cfg", L"..\\..\\..\\cfg\\DearMoments_Installed.cfg");
        CopyFileOptimized(L"src\\resources\\gamestate_integration_dearmoments.cfg", L"..\\gamestate_integration_dearmoments.cfg");
        cout << "所有文件均复制完成！\n";

        // 无论是否打开市场，都创建模组市场快捷方式
        {
            // 获取当前目录完整路径
            fs::path curDir = fs::current_path();
            wstring currentDirW = curDir.wstring();
            wstring targetMarket = currentDirW + L"\\src\\main\\Lib\\ModsMarket.exe";
            // 快捷方式将放置在当前目录下，名称为“模组市场.lnk”
            if (!CreateShortcut(L"模组市场.lnk", targetMarket.c_str(), currentDirW.c_str())) {
                MessageBox(NULL, L"创建模组市场快捷方式失败。", L"错误", MB_OK | MB_ICONERROR);
            }
        }


        // 询问是否添加启动项到 autoexec（推荐）
        int autoexecResponse = MessageBox(NULL, L"是否添加启动项到 autoexec（推荐）？", L"启动项", MB_YESNO | MB_ICONQUESTION);
        if (autoexecResponse == IDYES) {
            if (AddStartupToAutoexec("..\\autoexec.cfg")) {
                MessageBox(NULL, L"autoexec 启动项添加成功。", L"提示", MB_OK | MB_ICONINFORMATION);
            }
            else {
                MessageBox(NULL, L"添加 autoexec 启动项失败。", L"错误", MB_OK | MB_ICONERROR);
            }
        }

        MessageBox(NULL, L"现在你可以退出本程序进行下一步配置", L"tips", MB_OK | MB_ICONINFORMATION);

   
     //   if (musicThread.joinable()) {
    //        musicThread.join();
    //    }
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
       
    
    return 0;
}
