#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <map>
#include <chrono>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <regex>
#include <mutex>
#include <iostream>
std::mutex coutMutex; 

// RTSS 提供的核心功能头文件
#include "RTSSApi/rtss-core.h"

// CS2 Console Watcher 项目头文件
#include "ReadConsole/RegistryReader.h"
#include "ReadConsole/VdfParser.h"
#include "ReadConsole/LogWatcher.h"

#define YAML_CPP_STATIC_DEFINE





namespace RTSSController {

    std::vector<std::thread> threads;

    static bool ReadConsole = false; 
    static std::atomic<bool> running{ true };

    struct KeyBind {
        int vk = 0;
        int fps = 0;
        bool hold = false;
        bool active = false;
    };

    static std::map<std::string, std::vector<KeyBind>> profileBinds;
    static std::map<std::string, int> keyMap = {
        {"SPACE", VK_SPACE}, {"ESCAPE", VK_ESCAPE}, {"F1", VK_F1}, {"F2", VK_F2},
        {"F3", VK_F3}, {"F4", VK_F4}, {"F5", VK_F5}, {"F6", VK_F6},
        {"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'}, {"Q", 'Q'}
    };

    int GetKeyVCode(const std::string& keyName) {
        auto it = keyMap.find(keyName);
        return (it != keyMap.end()) ? it->second : 0;
    }

    void CreateDefaultConfig(const std::string& configPath) {
        std::ofstream fout(configPath);
        fout << "ReadConsole: true\n";
        fout << "cs2.exe:\n";
        fout << "  binds:\n";
        fout << "    - key: SPACE\n";
        fout << "      fps: 64\n";
        fout << "      hold: true\n";
        fout.close();
        std::cout << "[INFO] 未找到配置，已生成默认配置 " << configPath << std::endl;
    }

    void LoadConfig(const std::string& configPath) {
        try {
            YAML::Node config = YAML::LoadFile(configPath);

            if (config["ReadConsole"]) {
                ReadConsole = config["ReadConsole"].as<bool>();
            }

            for (auto it = config.begin(); it != config.end(); ++it) {
                std::string profileName = it->first.as<std::string>();
                if (profileName == "ReadConsole") continue;

                YAML::Node profileNode = it->second;
                if (profileNode["binds"]) {
                    for (const auto& bindNode : profileNode["binds"]) {
                        KeyBind b;
                        b.vk = GetKeyVCode(bindNode["key"].as<std::string>());
                        b.fps = bindNode["fps"].as<int>();
                        b.hold = bindNode["hold"].as<bool>();
                        if (b.vk != 0) profileBinds[profileName].push_back(b);
                    }
                }
            }

            std::cout << "[INFO] 配置已加载，profile 数量: "
                << profileBinds.size() << std::endl;
        }
        catch (const YAML::Exception& e) {
            std::cerr << "[ERROR] 加载配置失败: " << e.what() << std::endl;
        }
    }

    int CountAllBinds() {
        int total = 0;
        for (auto& entry : profileBinds) total += static_cast<int>(entry.second.size());
        return total;
    }

    void KeyListener() {
        while (running) {
            for (auto& profileEntry : profileBinds) {
                const std::string& profileName = profileEntry.first;
                auto& binds = profileEntry.second;

                for (auto& b : binds) {
                    if (b.hold) {
                        if (GetAsyncKeyState(b.vk) & 0x8000) {
                            if (!b.active) {
                                SetProperty(profileName, "FramerateLimit", b.fps);
                                b.active = true;

         
                                {
                                    std::lock_guard<std::mutex> lock(coutMutex);
                                    std::cout << "[DEBUG] " << profileName
                                        << " 按住键触发: 锁帧 " << b.fps << " FPS"
                                        << std::endl;
                                }
                            }
                        }
                        else if (b.active) {
                            SetProperty(profileName, "FramerateLimit", 0);
                            b.active = false;

                            {
                                std::lock_guard<std::mutex> lock(coutMutex);
                                std::cout << "[DEBUG] " << profileName
                                    << " 松开键: 解锁帧率"
                                    << std::endl;
                            }
                        }
                    }
                    else {
                        if (GetAsyncKeyState(b.vk) & 0x0001) {
                            SetProperty(profileName, "FramerateLimit", b.active ? 0 : b.fps);
                            b.active = !b.active;

                            {
                                std::lock_guard<std::mutex> lock(coutMutex);
                                std::cout << "[DEBUG] " << profileName
                                    << " 按键触发: "
                                    << (b.active ? "锁帧 " + std::to_string(b.fps) + " FPS" : "解锁帧率")
                                    << std::endl;
                            }

                            std::this_thread::sleep_for(std::chrono::milliseconds(250));
                        }
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }



    // -------------------- 控制台日志解析 /fps_set --------------------
    void ConsoleWatcherThread() {
        using namespace CS2ConsoleWatcher;
        try {
            std::wstring steamPath = RegistryReader::GetSteamPath();
            std::wstring logPath = VdfParser::FindCSLogPath(steamPath);

            std::ifstream file;
            std::streampos lastSize = 0;
            static std::regex logPrefix(R"(^\d{2}/\d{2}\s+\d{2}:\d{2}:\d{2}\s+)");

            while (RTSSController::running) {
                if (std::filesystem::exists(logPath)) {
                    auto fileSize = std::filesystem::file_size(logPath);

                    if (fileSize < lastSize) lastSize = 0; // 文件被重置

                    if (fileSize > lastSize) {
                        file.open(logPath, std::ios::binary);
                        if (file) {
                            file.seekg(lastSize);
                            std::string line;
                            while (std::getline(file, line)) {
                                if (!line.empty() && (unsigned char)line[0] == 0xEF &&
                                    line.size() >= 3 &&
                                    (unsigned char)line[1] == 0xBB &&
                                    (unsigned char)line[2] == 0xBF) {
                                    line = line.substr(3); // 去 BOM
                                }

                                std::string rawLine = std::regex_replace(line, logPrefix, "");
                                // 打印日志到控制台
                                std::cout << rawLine << std::endl;

                                // 解析 /fps_set <数字>;
                                const std::string cmd = "/fps_set ";
                                auto pos = rawLine.find(cmd);
                                if (pos != std::string::npos) {
                                    try {
                                        std::string numStr = rawLine.substr(pos + cmd.length());
                                        size_t end = numStr.find(";");
                                        if (end != std::string::npos) numStr = numStr.substr(0, end);
                                        int fps = std::stoi(numStr);
                                        SetProperty("cs2.exe", "FramerateLimit", fps);
                                        std::cout << "[INFO] 控制台命令生效: /fps_set " << fps << std::endl;
                                    }
                                    catch (...) {}
                                }
                            }
                            lastSize = file.tellg();
                            if (lastSize == -1) lastSize = fileSize;
                            file.close();
                        }
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "[ERROR] 控制台监视失败: " << ex.what() << std::endl;
        }
    }


    void RunThreaded(const std::string& configPath) {
        if (!std::filesystem::exists(configPath)) CreateDefaultConfig(configPath);
        LoadConfig(configPath);

        int totalBinds = CountAllBinds();
        if (totalBinds == 0 && !ReadConsole) return;

       
        threads.emplace_back(KeyListener);

        if (ReadConsole) threads.emplace_back(ConsoleWatcherThread); 

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        for (auto& t : threads) if (t.joinable()) t.join();
    }

} // namespace RTSSController



// -------------------- main --------------------
int main() {


    std::thread mainThread(RTSSController::RunThreaded, "rtss_config.yml");

    while (true) Sleep(1000); 
    return 0;
}
