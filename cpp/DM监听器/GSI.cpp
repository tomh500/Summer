// GSI.cpp

#include <iostream>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <SDL.h>
#include <SDL_mixer.h>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <chrono>
#include <fstream>


using json = nlohmann::json;
std::atomic<bool> gsi_thread_running(false);  // 用于检查线程是否正在运行
std::atomic<bool> is_playing(false);  // 用于标记是否有音效在播放
std::thread gsi_thread;  // 后台线程



// 播放击杀提示音效
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

        if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
            std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
            is_playing.store(false);
            return;
        }

        std::string sound_file;

        // 如果 match 为 true，根据击杀数播放音效，否则播放 extra.mp3
        if (match) {
            switch (kills_count) {
            case 1:
                sound_file = "src/resources/1.mp3";
                break;
            case 2:
                sound_file = "src/resources/2.mp3";
                break;
            case 3:
                sound_file = "src/resources/3.mp3";
                break;
            case 4:
                sound_file = "src/resources/4.mp3";
                break;
            case 5:
                sound_file = "src/resources/5.mp3";
                break;
            default:
                sound_file = "src/resources/extra.mp3";
                break;
            }
        }
        else {
            // match 为 false，播放 extra.mp3
            sound_file = "src/resources/extra.mp3";
        }

        Mix_Music* music = Mix_LoadMUS(sound_file.c_str());
        if (music == nullptr) {
            std::cerr << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
            is_playing.store(false);
            return;
        }

        if (Mix_PlayMusic(music, 1) == -1) {
            std::cerr << "Mix_PlayMusic failed! SDL_mixer Error: " << Mix_GetError() << std::endl;
            is_playing.store(false);
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

        Mix_FreeMusic(music);
        Mix_CloseAudio();
        is_playing.store(false);  // 音效播放完成，更新标记
        }).detach();  // 使用 detach 启动异步线程
}

// 定义击杀播报的函数
void ReportKill(int kills_count) {
    switch (kills_count) {
    case 0:
        break;  // 如果是 0 杀，不做任何操作
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
    std::ifstream config_file("src/resources/killing_sound.json");
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
void ReceiveData(httplib::Server& svr, bool match, float vol) {
    svr.Post("/", [&match, &vol](const httplib::Request& req, httplib::Response& res) {
        std::string body = req.body;
        std::cout << "Received body: " << body << std::endl;

        try {
            json j = json::parse(body);
            std::cout << "Received GSI data:" << std::endl;
            std::cout << j.dump(4) << std::endl;

            if (j.contains("player") && j["player"].contains("state") && j["player"]["state"].contains("health")) {
                int health = j["player"]["state"]["health"].get<int>();

                if (health > 0) {  // 玩家仍然活着
                    if (j["player"]["state"].contains("round_kills")) {
                        int round_kills = j["player"]["state"]["round_kills"].get<int>();
                        std::lock_guard<std::mutex> lock(queue_mutex);  // 加锁

                        // 如果是第一次接收到数据且击杀数小于等于5，则播放音效
                        if (is_first_data_received) {
                            if (round_kills <= 5) {
                                roundkill_local = round_kills;
                                kills_queue.push(round_kills);
                                is_first_data_received = false;  // 标记第一次数据已接收
                            }
                            // 如果第一次数据大于5，则不播放音效
                            else {
                                roundkill_local = round_kills;
                                is_first_data_received = false;  // 标记第一次数据已接收
                            }
                        }
                        // 如果当前解析到的击杀数和上次存储的不同，则播放音效并更新 local 变量
                        else if (round_kills != roundkill_local) {
                            roundkill_local = round_kills;  // 更新存储的击杀数
                            kills_queue.push(round_kills);  // 将击杀数放入队列
                        }
                    }
                }
                else {
                    std::cout << "Player is dead and spectating!" << std::endl;
                }
            }

            res.set_content("OK", "text/plain");
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            res.status = 400;
        }
        });

    svr.listen("127.0.0.1", 12345);
}

// 封装程序的函数
void RunGSI() {
    httplib::Server svr;

    bool match = true;
    float vol = 1.0f;

    // 加载配置
    if (!LoadKillingSoundConfig(match, vol)) {
        return;
    }

    // 启动接收数据的线程
    std::thread receive_thread(ReceiveData, std::ref(svr), match, vol);

    // 每 50 毫秒检查一次队列，播放音效
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::lock_guard<std::mutex> lock(queue_mutex);  // 加锁
        if (!kills_queue.empty()) {
            int kills_count = kills_queue.front();
            kills_queue.pop();
            ReportKill(kills_count);  // 播放击杀播报
            PlayKillSound(kills_count, match, vol);  // 播放击杀音效
        }
    }

    receive_thread.join();  // 等待接收线程结束
}

void StartGSIThread() {
    if (!gsi_thread_running.load()) {
        gsi_thread = std::thread(RunGSI);
        gsi_thread_running.store(true);
        std::cout << "GSI thread started." << std::endl;
    }
}

// 停止 GSI 线程
void StopGSIThread() {
    if (gsi_thread_running.load()) {
        // 通常要在后台线程函数内部处理退出标志，优雅地停止线程
        gsi_thread_running.store(false);
        if (gsi_thread.joinable()) {
            gsi_thread.join();  // 等待线程结束
        }
        std::cout << "GSI thread stopped." << std::endl;
    }
}