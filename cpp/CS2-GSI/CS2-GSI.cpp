#define SDL_MAIN_HANDLED
#include <ws2tcpip.h>
#include <iostream>
#include "cpp-httplib/httplib.h"
#include <nlohmann/json.hpp>
#include <SDL.h>
#include <SDL_mixer.h>
#include <thread>
#include <atomic>
#include <winsock2.h>
#include <queue>
#include <mutex>
#include <shlwapi.h>
#include <chrono>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")
using json = nlohmann::json;
std::wstring target_steamid = L"";  // 用于命令行指定目标 SteamID

// 播放击杀提示音效的控制线程
std::atomic<bool> is_playing(false);  // 用于标记是否有音效在播放
bool is_sdl_initialized = false;

// 用于保存本地存储的击杀数，避免重复播放音效
//int roundkill_local = -1;  // 初始化为一个不可能的数值
//bool is_first_data_received = true;  // 标记是否是第一次接收到数据
bool wait_for_kill_reset = false;    // << 新增标志变量：是否等待回合开始（即 round_kills == 1）
static bool round_death_occurred = false;
static bool mvp_flag = false;
static bool mvp_played = false;  // 避免重复播放
static int mvp_candidate_kills = 0;

bool IsPortInUse(int port) {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in service;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return true; // 认为初始化失败也不能继续
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return true;
    }

    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(port);

    // 如果绑定失败说明端口已被占用
    int result = bind(sock, (SOCKADDR*)&service, sizeof(service));

    closesocket(sock);
    WSACleanup();

    return result == SOCKET_ERROR;
}

void ReinitializeSDL() {
    if (Mix_Init(MIX_INIT_MP3) == 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }
    is_sdl_initialized = false; // 重新标记 SDL 未初始化
}


void PlayKillSound(int kills_count, bool match, float vol) {
    // 如果有音效正在播放，则先停止它
    if (is_playing.load()) {
        Mix_HaltMusic();  // 停止当前音效的播放
        while (Mix_PlayingMusic()) {
            SDL_Delay(100);  // 等待音效停止
        }
    }

    // 启动新的音效播放线程
    std::thread([kills_count, match, vol]() {
        is_playing.store(true);  // 标记音效正在播放

        // 如果没有初始化 SDL，尝试初始化
        if (!is_sdl_initialized) {
            if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
                std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
                is_playing.store(false);
                return;
            }
            is_sdl_initialized = true;  // 标记已初始化
        }

        std::string sound_file;

        // 如果 match 为 true，根据击杀数播放音效，否则播放 extra.mp3
        if (match) {
            switch (kills_count) {
            case 0: break;
            case 1:
                sound_file = "Userspace/gsi/killing_sound/1.mp3";
                break;
            case 2:
                sound_file = "Userspace/gsi/killing_sound/2.mp3";
                break;
            case 3:
                sound_file = "Userspace/gsi/killing_sound/3.mp3";
                break;
            case 4:
                sound_file = "Userspace/gsi/killing_sound/4.mp3";
                break;
            case 5:
                sound_file = "Userspace/gsi/killing_sound/5.mp3";
                break;
            case 99:
                sound_file = "Userspace/gsi/killing_sound/mvp.mp3";
                break;
            default:
                sound_file = "Userspace/gsi/killing_sound/extra.mp3";
                break;
            }
        }
        else {
            // match 为 false，播放 extra.mp3
            sound_file = "Userspace/gsi/killing_sound/extra.mp3";
        }

        // 加载并播放音效
        Mix_Music* music = Mix_LoadMUS(sound_file.c_str());
        if (music == nullptr) {
            std::cerr << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
            is_playing.store(false);
            return;
        }

        if (Mix_PlayMusic(music, 1) == -1) {
            std::cerr << "Mix_PlayMusic failed! SDL_mixer Error: " << Mix_GetError() << std::endl;
            is_playing.store(false);
            Mix_FreeMusic(music);  // 释放资源
            return;
        }

        // 将 vol 转换为 SDL 音量，音量范围是 0 到 MIX_MAX_VOLUME
        int sdl_volume = static_cast<int>(std::round(vol * MIX_MAX_VOLUME));
        if (sdl_volume > MIX_MAX_VOLUME) {
            sdl_volume = MIX_MAX_VOLUME;  // 确保音量不超过最大值
        }

        // 调整音量
        Mix_VolumeMusic(sdl_volume);  // 设置音量大小，vol 是浮动值

        while (Mix_PlayingMusic()) {
            SDL_Delay(100);  // 等待音效播放完成
        }

        Mix_FreeMusic(music);  // 释放资源
        is_playing.store(false);  // 音效播放完成，更新标记
        }).detach();  // 使用 detach 启动异步线程
}

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

void RunBatchFile(const std::wstring& exePath, const std::wstring& arguments, bool showWindow) {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = showWindow ? SW_SHOWNORMAL : SW_HIDE;

    // 获取目标 exe 的目录
    wchar_t exeDir[MAX_PATH] = { 0 };
    wcscpy_s(exeDir, exePath.c_str());
    PathRemoveFileSpecW(exeDir);  // 获取 exe 的目录

    // 设置工作目录
    if (!SetWorkingDirectory(exeDir)) {
        DWORD err = GetLastError();
        std::wstringstream ss;
        ss << L"无法设置工作目录！错误代码：" << err;
        MessageBoxW(nullptr, ss.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
        return;
    }

    // 拼接命令行
    std::wstring cmdLine = L"\"" + exePath + L"\" " + arguments;
    LPWSTR cmdLineBuffer = &cmdLine[0];

    // 可选调试信息（如果需要调试输出，可以自行临时加上）


    if (!CreateProcessW(nullptr, cmdLineBuffer, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        DWORD err = GetLastError();

        std::wstringstream ss;
        ss << L"无法启动程序！错误代码：" << err;
        MessageBoxW(nullptr, ss.str().c_str(), L"错误", MB_OK | MB_ICONERROR);
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


// 定义击杀播报的函数
void ReportKill(int kills_count) {
    switch (kills_count) {
    case 0: break;
    case 1:
        std::cout << "One kill!" << std::endl;
        break;
    case 2:
        std::cout << "Double kill!" << std::endl;
        break;
    case 3:
        std::cout << "Triple kill!" << std::endl;
        break;
    case 4:
        std::cout << "Quadra kill!" << std::endl;
        break;
    case 5:
        std::cout << "ACE!" << std::endl;
        break;
    default:
        std::cout << "Multiple kills!" << std::endl;
        break;
    }
}

// 线程安全队列
std::queue<int> kills_queue;
std::mutex queue_mutex;

// 用于保存本地存储的击杀数，避免重复播放音效
int roundkill_local = -1;  // 初始化为一个不可能的数值
bool is_first_data_received = true;  // 标记是否是第一次接收到数据

// 从 killing_sound.json 文件读取配置
bool LoadKillingSoundConfig(bool& match, float& vol) {
    std::ifstream config_file("Userspace/gsi/killing_sound/killing_sound.json");
    if (!config_file.is_open()) {
        std::cerr << "Failed to open killing_sound.json file!" << std::endl;
        return false;
    }

    json config;
    config_file >> config;

    if (config.contains("match")) {
        match = config["match"].get<bool>();
    }

    if (config.contains("vol")) {
        vol = config["vol"].get<float>();
    }

    return true;
}

// 接收数据的线程
// 全局状态变量

void ReceiveData(httplib::Server* svr, bool match, float vol, bool debug_mode) {
    svr->Post("/", [debug_mode, match, vol](const httplib::Request& req, httplib::Response& res) {
        static std::string last_phase = "";
        static int last_kills = -1;
        static bool dead_muted = false;
        static bool wait_for_kill_reset = false;

        // 解析 JSON
        json j = json::parse(req.body);
        std::string phase = j["round"]["phase"];
        const auto& state = j["player"]["state"];
        int health = state["health"];
        int round_kills = state["round_kills"];

        bool is_mvp = j.contains("mvp") && j["mvp"].get<bool>();
        if (is_mvp) {
            mvp_flag = true;
        }

        // 阶段变更
        if (phase != last_phase) {
            if (debug_mode)
                std::cout << "[PHASE] " << last_phase << " → " << phase << "\n";

            if (phase == "over") {
                wait_for_kill_reset = true;

                // 播放 mvp 音效
                if (mvp_flag && mvp_candidate_kills > 3 && !mvp_played) {
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    kills_queue.push(-2);  // -2 表示播放 mvp.mp3
                    mvp_played = true;
                    if (debug_mode)
                        std::cout << "[MVP] MVP Achieved with " << mvp_candidate_kills << " kills\n";
                }

                if (debug_mode)
                    std::cout << "[MARK] round over, waiting for reset\n";
            }
            else if (phase == "live" && wait_for_kill_reset) {
                // 回合重置
                mvp_flag = false;
                mvp_played = false;
                mvp_candidate_kills = 0;
                round_death_occurred = false;
                dead_muted = false;
                last_kills = -1;
                wait_for_kill_reset = false;
                if (debug_mode)
                    std::cout << "[RESET] new round, re-enable sound\n";
            }

            last_phase = phase;
        }

        // 非 live 或 over 阶段忽略
        if (phase != "live" && phase != "over") {
            if (debug_mode) std::cout << "[SKIP] not in live/over\n";
            res.set_content("IGNORED (not live/over)", "text/plain");
            return;
        }

        // 死亡竞赛模式直接处理击杀
        bool is_deathmatch = (j["map"]["mode"] == "deathmatch");
        if (is_deathmatch) {
            if (round_kills > last_kills) {
                std::lock_guard<std::mutex> lock(queue_mutex);
                kills_queue.push(-1); // -1 = extra.mp3
                last_kills = round_kills;
                if (debug_mode)
                    std::cout << "[DM-KILL] pushed extra.mp3 for kill #" << round_kills << "\n";
            }
        }
        else {
            // 普通模式死亡处理
            if (health <= 0) {
                dead_muted = true;
                round_death_occurred = true;
                if (debug_mode)
                    std::cout << "[MUTED] Player is dead\n";
            }

            // 记录击杀（如果未死亡）
            if (!dead_muted && round_kills > last_kills) {
                {
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    kills_queue.push(round_kills);
                }

                if (round_kills > mvp_candidate_kills) {
                    mvp_candidate_kills = round_kills;
                }

                last_kills = round_kills;

                if (debug_mode)
                    std::cout << "[KILL] pushed " << round_kills << "\n";
            }
        }

        res.set_content("OK", "text/plain");
        });

    svr->listen("127.0.0.1", 1009);
}




int main(int argc, char* argv[]) {
    SetWorkingDirectory(L"..\\..\\..");
    if (IsPortInUse(1009)) {
        MessageBoxA(nullptr, "端口 1009 已被占用，程序将退出。", "端口占用", MB_ICONERROR | MB_OK);
        return -1;
    }
    httplib::Server svr;

    bool match = true;
    float vol = 1.0f;
    bool debug_mode = false;  // 用于标记是否处于调试模式

    // 检查是否有 -debug 参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-steamid" && i + 1 < argc) {
            std::string steamid_arg = argv[i + 1];
            target_steamid = std::wstring(steamid_arg.begin(), steamid_arg.end());
            ++i;
        }
        else if (arg == "-debug") {
            debug_mode = true;
        }
    }

    // 加载配置
    if (!LoadKillingSoundConfig(match, vol)) {
        return -1;
    }


    // 启动接收数据的线程
    std::thread receive_thread(ReceiveData, &svr, match, vol, debug_mode);

    
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::lock_guard<std::mutex> lock(queue_mutex);  // 加锁
        if (!kills_queue.empty()) {
            int kills_count = kills_queue.front();
            kills_queue.pop();

            if (kills_count == -1) {
                ReportKill(0);
                PlayKillSound(0, false, vol);  // 播 extra.mp3
            }
            else if (kills_count == -2) {
                ReportKill(0);
                PlayKillSound(99, false, vol);  // 播 mvp.mp3
            }
            else {
                ReportKill(kills_count);
                PlayKillSound(kills_count, match, vol);
            }
        }

    }

    receive_thread.join();  // 等待接收线程结束

    
    return 0;
}
