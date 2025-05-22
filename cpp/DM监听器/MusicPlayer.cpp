#include "MusicPlayer.h"
#include <windows.h>
#include <shlwapi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <curl/curl.h>
#include"Global.h"
#include <algorithm>  // for std::sort
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <commctrl.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Comctl32.lib")
void DebugMsg(const std::wstring& msg) {
    if (debug == 1) {
        MessageBoxW(NULL, msg.c_str(), L"调试信息", MB_OK | MB_ICONINFORMATION);
    }
}


#define ID_PLAY_BUTTON     2002
#define ID_PAUSE_BUTTON    3001
#define ID_VOLUME_SLIDER   3002
#define ID_PROGRESS_SLIDER 3003
HWND hListBox = nullptr; // Declare hListBox globally or within the appropriate function scope.


HWND hShowHiddenCheckbox = nullptr;
std::vector<Music> visibleMusicList; // 当前显示在 UI 上的歌曲
std::vector<Music> musicList;
std::atomic<bool> isPaused = false;
std::atomic<int> currentVolume = 64;
std::atomic<bool> isDownloading = false;
std::string currentTempFile = "";
Mix_Music* currentTrack = nullptr;
HWND hProgressSlider = nullptr;
std::wstring utf8ToWide(const std::string& str);
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* data = static_cast<std::string*>(userp);
    data->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

static std::string trim(const std::string& s) {
    auto l = s.find_first_not_of(" \t\r\n");
    if (l == std::string::npos) return "";
    auto r = s.find_last_not_of(" \t\r\n");
    return s.substr(l, r - l + 1);
}
void applyProxySettings(CURL* curl) {
    HKEY hKey;
    DWORD proxyEnable = 0;
    DWORD bufSize = sizeof(proxyEnable);
    char proxyServer[256] = { 0 };
    bool isProxyOn = false;

    // 1. 读取系统代理设置
    if (RegOpenKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueExA(hKey, "ProxyEnable", nullptr, nullptr,
            reinterpret_cast<LPBYTE>(&proxyEnable), &bufSize) == ERROR_SUCCESS
            && proxyEnable == 1)
        {
            bufSize = sizeof(proxyServer);
            if (RegQueryValueExA(hKey, "ProxyServer", nullptr, nullptr,
                reinterpret_cast<LPBYTE>(proxyServer), &bufSize) == ERROR_SUCCESS)
            {
                isProxyOn = true;
            }
        }
        RegCloseKey(hKey);
    }

    if (isProxyOn && curl) {
        std::string proxyStr(proxyServer);
        std::string selectedProxy;

        // 2. 判断当前 URL 是 http 还是 https
        char* url = nullptr;
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
        if (!url) {
            // 如果 EFFECTIVE_URL 获取不到，就使用 CURLOPT_URL（libcurl 没开始请求前）
            curl_easy_getinfo(curl, CURLINFO_PRIVATE, &url); // 可设置为你自己传入的 URL
        }

        bool isHttps = false;
        if (url != nullptr && _strnicmp(url, "https://", 8) == 0) {
            isHttps = true;
        }

        // 3. 解析 ProxyServer 字符串
        if (proxyStr.find('=') != std::string::npos) {
            // 多协议代理格式
            if (isHttps) {
                auto pos = proxyStr.find("https=");
                if (pos != std::string::npos) {
                    auto semi = proxyStr.find(';', pos);
                    selectedProxy = proxyStr.substr(pos + 6, semi - pos - 6);
                }
            }

            if (selectedProxy.empty()) {
                auto pos = proxyStr.find("http=");
                if (pos != std::string::npos) {
                    auto semi = proxyStr.find(';', pos);
                    selectedProxy = proxyStr.substr(pos + 5, semi - pos - 5);
                }
            }
        }
        else {
            // 单一代理，直接使用
            selectedProxy = proxyStr;
        }

        // 4. 加上前缀
        if (!selectedProxy.empty() && selectedProxy.find("://") == std::string::npos) {
            selectedProxy = "http://" + selectedProxy;
        }

        // 5. 应用到 curl
        if (!selectedProxy.empty()) {
            curl_easy_setopt(curl, CURLOPT_PROXY, selectedProxy.c_str());
            curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);

            if (debug == 1) {
                std::wstring wmsg = L"使用系统代理: ";
                wmsg += utf8ToWide(selectedProxy);
                MessageBoxW(NULL, wmsg.c_str(), L"代理设置", MB_OK | MB_ICONINFORMATION);
            }
        }
    }
    else {
        if (debug == 1) {
            MessageBoxW(NULL,
                L"未检测到系统代理，所有请求将直连。",
                L"代理设置",
                MB_OK | MB_ICONINFORMATION);
        }
    }
}
void buildMusicListUI() {
    SendMessageW(hListBox, LB_RESETCONTENT, 0, 0);
    BOOL showHidden = SendMessage(hShowHiddenCheckbox, BM_GETCHECK, 0, 0);

    visibleMusicList.clear();

    for (const auto& music : musicList) {
        if (music.status == Music::DEV) {
            continue; // dev 永远不显示
        }

        if (music.status == Music::HIDE && !showHidden) {
            continue; // hide 受复选框控制
        }

        visibleMusicList.push_back(music);

        std::wstring wname = utf8ToWide(music.name);  // 保证字符串变量的生命周期
        SendMessageW(hListBox, LB_ADDSTRING, 0, (LPARAM)wname.c_str());
    }

    InvalidateRect(hListBox, NULL, TRUE);

    if (debug == 1) {
        DebugMsg(L"[buildMusicListUI] 显示了 " + std::to_wstring(visibleMusicList.size()) + L" 首歌曲。");
    }
}


bool downloadFile(const std::string& url, const std::string& filename) {
    Mix_HaltMusic();           // 停止播放
    Mix_FreeMusic(currentTrack);  // 释放音乐资源
    currentTrack = nullptr;
    SDL_Delay(100);            // 延迟等待句柄释放（必要）
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string buffer;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    applyProxySettings(curl);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return false;

    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    return ofs.good();
}

std::wstring utf8ToWide(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    wstr.resize(wcslen(wstr.c_str()));
    return wstr;
}

bool isSupportedFormat(const std::string& url) {
    auto ext = url.substr(url.find_last_of('.') + 1);
    for (auto& c : ext) c = static_cast<char>(tolower(c));
    return (ext == "mp3" || ext == "ogg" || ext == "wav" || ext == "m4a" || ext == "flac");
}

std::string getTempFilePath(const std::string& extension) {
    char tempPath[MAX_PATH] = { 0 };
    GetTempPathA(MAX_PATH, tempPath);
    std::string filename = "temp_music_file." + extension;
    char fullPath[MAX_PATH] = { 0 };
    strcpy_s(fullPath, MAX_PATH, tempPath);
    if (!PathAppendA(fullPath, filename.c_str())) return "";
    return std::string(fullPath);
}

bool fetchMusicList(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string rawData;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawData);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    applyProxySettings(curl);

    if (curl_easy_perform(curl) != CURLE_OK) {
        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_cleanup(curl);

    musicList.clear();

    std::istringstream stream(rawData);
    std::string line;
    Music current;
    int step = 0;

    while (std::getline(stream, line)) {
        auto t = trim(line);
        if (t.empty()) continue;

        switch (step) {
        case 0:
            if (isdigit(t[0]) && t.back() == ':') {
                current = Music();
                current.id = std::stoi(t.substr(0, t.size() - 1));
                step = 1;
            }
            break;
        case 1:
            if (t.rfind("- ", 0) == 0) {
                current.name = t.substr(2);
                step = 2;
            }
            break;
        case 2:
            if (t.rfind("- ", 0) == 0) {
                current.url = t.substr(2);
                step = 3;
            }
            break;
        case 3:
            if (t.rfind("- ", 0) == 0) {
                std::string flag = t.substr(2);
                if (flag == "hide") { current.status = Music::HIDE; }
                else if (flag == "dev") { current.status = Music::DEV; }
                else current.status = Music::NORMAL;
            }
            else {
                current.status = Music::NORMAL;
            }
            musicList.push_back(current);
            step = 0;
            break;
        }
    }

    // 如果文件末尾漏了状态行，补上最后一个
    if (step == 3) {
        current.status = Music::NORMAL;
        musicList.push_back(current);
    }

    std::sort(musicList.begin(), musicList.end(), [](const Music& a, const Music& b) {
        return a.id < b.id;
        });

    buildMusicListUI();
    return true;
}



LRESULT CALLBACK MusicPlayerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hPlayButton, hPauseButton, hVolumeSlider;
    static std::vector<std::wstring> musicNames;

    switch (msg) {
    case WM_CREATE: {
        InitCommonControls();
        curl_global_init(CURL_GLOBAL_DEFAULT);
        SDL_Init(SDL_INIT_AUDIO);
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC);

        hListBox = CreateWindowW(L"LISTBOX", nullptr,
            WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
            10, 10, 460, 200, hwnd, (HMENU)2001, nullptr, nullptr);

        hShowHiddenCheckbox = CreateWindowW(L"BUTTON", L"显示CFG SONG",
            WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
            350, 220, 120, 30, hwnd, (HMENU)4001, nullptr, nullptr);

        hPlayButton = CreateWindowW(L"BUTTON", L"播放",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 220, 100, 30, hwnd, (HMENU)ID_PLAY_BUTTON, nullptr, nullptr);
        hPauseButton = CreateWindowW(L"BUTTON", L"暂停",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            180, 220, 100, 30, hwnd, (HMENU)ID_PAUSE_BUTTON, nullptr, nullptr);
        hVolumeSlider = CreateWindowW(TRACKBAR_CLASS, nullptr,
            WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
            10, 260, 460, 30, hwnd, (HMENU)ID_VOLUME_SLIDER, nullptr, nullptr);
        hProgressSlider = CreateWindowW(TRACKBAR_CLASS, nullptr,
            WS_CHILD | WS_VISIBLE | TBS_NOTICKS,
            10, 300, 460, 20, hwnd, (HMENU)ID_PROGRESS_SLIDER, nullptr, nullptr);

        SendMessage(hVolumeSlider, TBM_SETRANGE, TRUE, MAKELPARAM(0, 128));
        SendMessage(hVolumeSlider, TBM_SETPOS, TRUE, currentVolume);

        fetchMusicList("https://tomh500.github.io/Square/MusicList.txt");
        SendMessageW(hListBox, LB_RESETCONTENT, 0, 0);
        for (const auto& music : musicList) {
            auto wname = utf8ToWide(music.name);
            SendMessageW(hListBox, LB_ADDSTRING, 0, (LPARAM)wname.c_str());
        }
        buildMusicListUI();

        // 播放进度刷新线程
        std::thread([hwnd]() {
            while (true) {
                if (Mix_PlayingMusic()) {
                    int pos = Mix_GetMusicPosition(currentTrack);
                    int total = Mix_MusicDuration(currentTrack);
                    if (total > 0)
                        PostMessage(hwnd, WM_USER + 100, pos, total);
                }
                Sleep(500);
            }
            }).detach();

        break;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_PLAY_BUTTON: {
            if (isDownloading) break;
           
            int index = (int)SendMessageW(hListBox, LB_GETCURSEL, 0, 0);
            if (index == LB_ERR || index < 0 || index >= (int)visibleMusicList.size()) break;

            const auto& sel = visibleMusicList[index];

            if (!isSupportedFormat(sel.url)) {
                MessageBoxA(hwnd, "不支持的音频格式", "错误", MB_OK | MB_ICONERROR);
                break;
            }

            isDownloading = true;
            std::thread([hwnd, sel]() {
                Mix_HaltMusic();
                if (!currentTempFile.empty()) std::remove(currentTempFile.c_str());

                std::string ext = sel.url.substr(sel.url.find_last_of('.') + 1);
                currentTempFile = getTempFilePath(ext);
                if (!downloadFile(sel.url, currentTempFile)) {
                    MessageBoxA(hwnd, "下载失败", "错误", MB_OK | MB_ICONERROR);
                    isDownloading = false;
                    return;
                }

                currentTrack = Mix_LoadMUS(currentTempFile.c_str());
                if (!currentTrack) {
                    MessageBoxA(hwnd, Mix_GetError(), "音频加载失败", MB_OK | MB_ICONERROR);
                    isDownloading = false;
                    return;
                }

                Mix_VolumeMusic(currentVolume);
                Mix_PlayMusic(currentTrack, 1);
                isPaused = false;
                SetWindowTextW(GetDlgItem(hwnd, ID_PAUSE_BUTTON), L"暂停");

                isDownloading = false;
                }).detach();
            break;
        }
        case 4001: {
            if (HIWORD(wParam) == BN_CLICKED) {
                buildMusicListUI();  // 重新过滤歌曲并刷新
            }
            break;
        }

        case ID_PAUSE_BUTTON:
            if (isPaused) {
                Mix_ResumeMusic();
                SetWindowTextW(hPauseButton, L"暂停");
            }
            else {
                Mix_PauseMusic();
                SetWindowTextW(hPauseButton, L"继续");
            }
            isPaused = !isPaused;
            break;
        }
        break;

    case WM_HSCROLL:
        if ((HWND)lParam == hVolumeSlider) {
            currentVolume = SendMessage(hVolumeSlider, TBM_GETPOS, 0, 0);
            Mix_VolumeMusic(currentVolume);
        }
        else if ((HWND)lParam == hProgressSlider) {
            if (LOWORD(wParam) == TB_THUMBTRACK || LOWORD(wParam) == TB_ENDTRACK) {
                int seekPos = SendMessage(hProgressSlider, TBM_GETPOS, 0, 0);
                if (currentTrack && !isDownloading) {
                    Mix_SetMusicPosition(static_cast<double>(seekPos));
                }
            }
        }
        break;

    case WM_USER + 100: {
        int pos = (int)wParam;
        int total = (int)lParam;
        if (total > 0)
            SendMessage(hProgressSlider, TBM_SETRANGE, TRUE, MAKELPARAM(0, total));
        SendMessage(hProgressSlider, TBM_SETPOS, TRUE, pos);
        break;
    }

    case WM_DESTROY:
        Mix_HaltMusic();
        if (!currentTempFile.empty()) std::remove(currentTempFile.c_str());
        // 清空列表，释放内存
        SendMessageW(hListBox, LB_RESETCONTENT, 0, 0);
        musicList.clear();
        musicNames.clear();
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
        curl_global_cleanup();
        
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
