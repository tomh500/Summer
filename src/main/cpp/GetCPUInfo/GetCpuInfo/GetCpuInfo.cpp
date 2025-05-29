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

#pragma comment(lib, "wininet.lib")

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "shell32.lib")

using namespace std;
namespace fs = std::filesystem;

int vendor_c = 0;

//------------------------
// 辅助函数：优化文件复制
bool CopyFileOptimized(const std::string& src, const std::string& dest) {
    try {
        fs::path srcPath(src);
        fs::path destPath(dest);
        fs::path destDir = destPath.parent_path();
        if (!fs::exists(destDir)) {
            fs::create_directories(destDir);
        }
        fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);
        return true;
    }
    catch (const fs::filesystem_error& e) {
        cerr << "复制文件失败: " << e.what() << endl;
        return false;
    }
}

//------------------------
// 辅助函数：添加启动项到 autoexec.cfg
bool AddStartupToAutoexec(const std::string& autoexecPath) {
    try {
        fs::path filePath(autoexecPath);
        fs::path parentDir = filePath.parent_path();
        if (!fs::exists(parentDir)) {
            fs::create_directories(parentDir);
        }
        // 以追加模式打开文件（不存在则创建）
        ofstream ofs(autoexecPath, ios::app);
        if (!ofs.is_open()) {
            cerr << "打开 autoexec.cfg 失败。" << endl;
            return false;
        }
        // 写入前先添加换行符
        ofs << "\r\nexec Square/setup\r\n";
        ofs.close();
        return true;
    }
    catch (const fs::filesystem_error& e) {
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
// 获取 GPU 信息（WMI查询）
wstring GetGPUInfo() {
    IWbemLocator* pLocator = nullptr;
    IWbemServices* pService = nullptr;
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        wcout << L"COM 初始化失败" << endl;
        return L"无法获取 GPU 信息";
    }
    hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (FAILED(hres)) {
        wcerr << L"安全初始化失败" << endl;
        CoUninitialize();
        return L"无法获取 GPU 信息";
    }
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLocator);
    if (FAILED(hres)) {
        wcerr << L"创建 IWbemLocator 对象失败" << endl;
        CoUninitialize();
        return L"无法获取 GPU 信息";
    }
    hres = pLocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pService);
    if (FAILED(hres)) {
        wcerr << L"连接到 WMI 服务失败" << endl;
        pLocator->Release();
        CoUninitialize();
        return L"无法获取 GPU 信息";
    }
    IEnumWbemClassObject* pEnumerator = nullptr;
    hres = pService->ExecQuery(bstr_t(L"SELECT * FROM Win32_VideoController"),
        bstr_t(L"WQL"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator);
    if (FAILED(hres)) {
        wcerr << L"WMI 查询失败" << endl;
        pLocator->Release();
        pService->Release();
        CoUninitialize();
        return L"无法获取 GPU 信息";
    }
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;
    wstring gpuInfo = L"未检测到 GPU";
    while (pEnumerator) {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn)
            break;
        VARIANT vtProp;
        hres = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres)) {
            gpuInfo = vtProp.bstrVal;
        }
        VariantClear(&vtProp);
        pclsObj->Release();
    }
    pEnumerator->Release();
    pService->Release();
    pLocator->Release();
    CoUninitialize();
    return gpuInfo;
}

bool DeleteFileInCurrentDirectory(const std::string& filename) {
    char currentDir[MAX_PATH];
    if (!GetCurrentDirectoryA(MAX_PATH, currentDir))
        return false;
    string filePath = string(currentDir) + "\\" + filename;
    return (DeleteFileA(filePath.c_str()) != 0);
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
        if (line.find("alias Square_Fps_Default") != string::npos) {
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
        if (brand.find("Intel(R) Core(TM) i5-12400F") != string::npos ||
            brand.find("Intel(R) Core(TM) i5-12600KF") != string::npos) {
          //  newAliasCommand = "alias Square_Fps_Default \"fps_max 1009\"";
            fps = "1009";
        }
        else {
           // newAliasCommand = "alias Square_Fps_Default \"fps_max 1009\"";
            fps = "1009";
        }
    }
    else if (vendor == "AuthenticAMD") {
        newAliasCommand = "//AMD";
        vendor_c = 2;
        if (brand.find("AMD Ryzen 7500F") != string::npos) {
         //   newAliasCommand = "alias Square_Fps_Default \"fps_max 1009\"";
            fps = "1009";
        }
        else {
           // newAliasCommand = "alias Square_Fps_Default \"fps_max 1009\"";
            fps = "1009";
        }
    }
    else {
        MessageBox(NULL, L"检测到未知的CPU厂商，程序无法运行。", L"错误", MB_ICONERROR);
        exit(1);
    }
    string currentAliasCommand;
    if (CheckForExistingAlias(filePath, currentAliasCommand)) {
        if (currentAliasCommand != newAliasCommand) {
            cout << "配置文件中已存在别名命令，但与检测到的CPU信息不一致。正在更新..." << endl;
            ifstream inputFile(filePath);
            ofstream tempFile("temp.cfg");
            string line;
            bool commandDeleted = false;
            while (getline(inputFile, line)) {
                if (line.find("alias Square_Fps_Default") != string::npos) {
                    commandDeleted = true;
                }
                else {
                    tempFile << line << endl;
                }
            }
            if (commandDeleted) {
                tempFile << newAliasCommand << endl;
            }
            inputFile.close();
            tempFile.close();
            remove(filePath.c_str());
            rename("temp.cfg", filePath.c_str());
        }
        else {
            cout << "配置文件中的命令与检测到的CPU型号一致，未做更改。" << endl;
        }
    }
    else {
        AppendToFile(filePath, newAliasCommand);
    }
}

void ShowRegionMessage(int regionCode) {
    wstring message;
    switch (regionCode) {
    case 1:
        message = L"本CFG保证不卖，仅供受信人员有限使用，您的地区：中国大陆";
        break;
    case 2:
        message = L"本CFG保证不卖，仅供受信人员有限使用，您的地区：台湾地区";
        break;
    case 3:
        message = L"本CFG保证不卖，仅供受信人员有限使用，您的地区：日本";
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

// wstring 转 UTF-8 string
string WStringToString(const wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), &str[0], size_needed, NULL, NULL);
    return str;
}


int main() {
    system("@echo off");
    system("chcp 65001");
    fs::path current_path = fs::current_path();
    fs::path target_path = L"Counter-Strike Global Offensive\\game\\csgo\\cfg\\Square";
    if (current_path.filename() != target_path.filename()) {
        MessageBoxW(NULL, L"运行目录不正确，请仔细观看教程", L"错误", MB_OK | MB_ICONERROR);
    }
    else {
        int regionCode = GetRegionCode();  
        ShowRegionMessage(regionCode);     

        if (!InitSDL()) {
            return -1;
        }

        UINT bgmResourceId = 0;
        if (regionCode == 1) {
            bgmResourceId = IDR_BGM_CN;
        }
        else if (regionCode == 2) {
            bgmResourceId = IDR_BGM;
        }
        thread musicThread;
        if (bgmResourceId != 0) {
            musicThread = thread(PlayBGMFromMemory, bgmResourceId);
        }

       

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

        string filePath = "src\\main\\cfg\\cn\\luotiany1\\SquareNextgen\\Square.cfg";
        string brand = GetCpuBrand();
        string fps;
        HandleCpuTypeAndAppend(vendor, brand, filePath, fps);

        wstring messageCPU = L"CPU 厂商: " + wstring(vendor.begin(), vendor.end()) + L"\n" +
            L"CPU 型号: " + wstring(brand.begin(), brand.end()) + L"\n" +
            L"因为你的CPU已经为你设定帧率： " + wstring(fps.begin(), fps.end());
        MessageBox(NULL, messageCPU.c_str(), L"帧率设置", MB_OK | MB_ICONINFORMATION);

        wstring gpuInfo = GetGPUInfo();
        wstring messageG = L"检测到 GPU: " + gpuInfo;
        //MessageBox(NULL, messageG.c_str(), L"GPU 信息", MB_OK | MB_ICONINFORMATION);

       // system("color 0F");
 

        // 保存其他资源文件
        wchar_t resourceFile[] = L"完全免费如果你买到的你就被骗了.free";
        if (!SaveResourceToFile(IDR_TEST_FREE, resourceFile)) {
            cerr << "保存资源失败。" << endl;
            return 1;
        }
        ifstream checkFile(resourceFile);
        if (checkFile.good()) {
        }
        else {
            MessageBox(NULL, L"文件保存失败，未能创建文件。", L"错误", MB_ICONERROR);
        }
        // 询问是否打开模组市场
        int modResponse = MessageBox(NULL, L"是否打开模组市场？如果你需要自动急停、在官匹游玩，可能需要安装一些模组", L"模组市场", MB_YESNO | MB_ICONQUESTION);
        if (modResponse == IDYES) {
            STARTUPINFO si = { sizeof(si) };
            PROCESS_INFORMATION pi;

            if (CreateProcess(L"ModsMarket.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            else {
                MessageBox(NULL, L"无法启动模组市场！", L"错误", MB_OK | MB_ICONERROR);
            }
        }


     
        system("cls");
        // 是否替换游戏启动画面和音乐
        /*
        string insteadboot;
        cout << "是否允许替换游戏启动画面和音乐，如果不允许，请输入0，否则将默认替换：";
        cin >> insteadboot;
        if (insteadboot != "0") {
            CopyFileOptimized("src\\main\\resources\\sounds\\bootsounds.vsnd_c", "..\\..\\sounds\\bootsounds.vsnd_c");
            CopyFileOptimized("src\\main\\resources\\intro.webm", "..\\..\\panorama\\videos\\intro.webm");
            CopyFileOptimized("src\\main\\resources\\intro720p.webm", "..\\..\\panorama\\videos\\intro720p.webm");
            CopyFileOptimized("src\\main\\resources\\intro-perfectworld.webm", "..\\..\\panorama\\videos\\intro-perfectworld.webm");
            CopyFileOptimized("src\\main\\resources\\intro-perfectworld720p.webm", "..\\..\\panorama\\videos\\intro-perfectworld720p.webm");
        }*/


        // 复制 Square_Installed.cfg 文件
        CopyFileOptimized("src\\main\\resources\\sounds\\disable_a.vsnd_c", "..\\..\\sounds\\disable_a.vsnd_c");
        CopyFileOptimized("src\\main\\resources\\sounds\\enable_a.vsnd_c", "..\\..\\sounds\\enable_a.vsnd_c");
        CopyFileOptimized("src\\main\\resources\\keybindings_schinese.txt", "..\\..\\resource\\keybindings_schinese.txt");
        CopyFileOptimized("src\\main\\resources\\Square_Installed.cfg", "..\\..\\..\\cfg\\Square_Installed.cfg");

        cout << "所有文件均复制完成！\n";

        // 无论是否打开市场，都创建模组市场快捷方式
        {
            // 获取当前目录完整路径
            fs::path curDir = fs::current_path();
            wstring currentDirW = curDir.wstring();
            wstring targetMarket = currentDirW + L"\\ModsMarket.exe";
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
   
        if (musicThread.joinable()) {
            musicThread.join();
        }
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
        // 不再需要删除临时文件
    }
    return 0;
}
