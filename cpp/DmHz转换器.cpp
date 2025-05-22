// convert.cpp
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <Windows.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

constexpr double sensitivity = 2.52;
constexpr double m_yaw_pitchvalue = 0.022;

// 重命名备份函数
void backup_file_if_exists(const std::string& filename) {
    if (!fs::exists(filename)) return;

    int index = 1;
    std::string backup_name;
    do {
        backup_name = filename + ".backup" + std::to_string(index);
        index++;
    } while (fs::exists(backup_name));

    try {
        fs::rename(filename, backup_name);
        std::cout << "已将原文件 " << filename << " 备份为 " << backup_name << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "备份文件失败: " << e.what() << std::endl;
    }
}

void json_to_yaml(const std::string& in, const std::string& out) {
    if (!fs::exists(in)) {
        std::cerr << "输入文件不存在: " << in << std::endl;
        return;
    }

    backup_file_if_exists(out);

    std::ifstream fin(in);
    if (!fin) {
        std::cerr << "无法打开输入文件: " << in << std::endl;
        return;
    }
    json j;
    fin >> j;

    YAML::Emitter em;
    em << YAML::BeginMap;
    for (auto& g : j["grenades"]) {
        std::string id = g["id"];
        std::string zh = g["name_zh_cn"];
        std::string en = g["name_en_us"];
        double pitch = g["angle"]["pitch"];
        double yaw = g["angle"]["yaw"];
        double p_conv = pitch / (sensitivity * m_yaw_pitchvalue);
        double y_conv = -yaw / (sensitivity * m_yaw_pitchvalue);
        std::string throw_cmd = g.value("throw_command", "");
        std::string mode;

        if (throw_cmd == "hzNade_jumpthrow_L") {
            mode = "Jump";
        }
        else if (throw_cmd == "hzNade_throw_L") {
            mode = "Normal";
        }
        else if (throw_cmd == "hzNade_Wjumpthrow_L") {
            mode = "ForwardJump";
        }
        else {
            mode = "Custom"; // 默认
        }

        std::string mapname = g.value("map", "");
        std::string type = g.value("type", "");
        if (type == "HE") type = "grenade";

        em << YAML::Key << id << YAML::Value << YAML::BeginMap;
        em << YAML::Key << "filename" << YAML::Value << id + ".cfg";
        em << YAML::Key << "displayname" << YAML::Value << (zh + " " + en);
        em << YAML::Key << "map" << YAML::Value << mapname;
        em << YAML::Key << "sensitivity" << YAML::Value << sensitivity;
        em << YAML::Key << "yaw" << YAML::Value << y_conv;
        em << YAML::Key << "pitch" << YAML::Value << p_conv;
        em << YAML::Key << "type" << YAML::Value << type;
        em << YAML::Key << "throwmode" << YAML::Value << mode;
        em << YAML::Key << "extra" << YAML::Value << YAML::BeginSeq
            << g.value("pre_throw_command", "") << g.value("post_throw_command", "") << YAML::EndSeq;
        em << YAML::Key << "select" << YAML::Value << YAML::BeginSeq;
        em << YAML::BeginMap << YAML::Key << "page" << YAML::Value << "" << YAML::EndMap;
        em << YAML::BeginMap << YAML::Key << "slot" << YAML::Value << "" << YAML::EndMap;
        em << YAML::BeginMap << YAML::Key << "command" << YAML::Value << "" << YAML::EndMap;
        em << YAML::BeginMap << YAML::Key << "bind" << YAML::Value << "None" << YAML::EndMap;
        em << YAML::EndSeq;
        em << YAML::Key << "setpos" << YAML::Value << YAML::BeginSeq;
        em << YAML::BeginMap << YAML::Key << "x" << YAML::Value << g["position"]["x"].get<double>() << YAML::EndMap;
        em << YAML::BeginMap << YAML::Key << "z" << YAML::Value << g["position"]["y"].get<double>() << YAML::EndMap;
        em << YAML::BeginMap << YAML::Key << "y" << YAML::Value << 100 << YAML::EndMap;
        em << YAML::EndSeq;
        em << YAML::EndMap;
    }
    em << YAML::EndMap;

    if (!em.good()) {
        std::cerr << "YAML 写出错误: " << em.GetLastError() << std::endl;
        return;
    }

    std::ofstream fout(out);
    if (!fout) {
        std::cerr << "无法打开输出文件: " << out << std::endl;
        return;
    }
    fout << em.c_str();
}

void yaml_to_json(const std::string& in, const std::string& out) {
    if (!fs::exists(in)) {
        std::cerr << "输入文件不存在: " << in << std::endl;
        return;
    }

    backup_file_if_exists(out);

    YAML::Node root;
    try {
        root = YAML::LoadFile(in);
    }
    catch (const std::exception& e) {
        std::cerr << "读取YAML失败: " << e.what() << std::endl;
        return;
    }
    json j;
    j["grenades"] = json::array();

    for (auto it : root) {
        std::string id = it.first.as<std::string>();
        auto v = it.second;
        std::string dsp = v["displayname"].as<std::string>();
        size_t pos = dsp.rfind(' ');
        std::string zh = dsp.substr(0, pos);
        std::string en = dsp.substr(pos + 1);
        double pitch = v["pitch"].as<double>() * sensitivity * m_yaw_pitchvalue;
        double yaw = -v["yaw"].as<double>() * sensitivity * m_yaw_pitchvalue;
        std::string mode = v["throwmode"].as<std::string>();

        // 根据 throwmode 反推 throw_command
        std::string cmd;
        if (mode == "Jump")
            cmd = "hzNade_jumpthrow_L";
        else if (mode == "ForwardJump")
            cmd = "hzNade_Wjumpthrow_L";
        else
            cmd = "hzNade_throw_L";

        json g;
        g["id"] = id;
        g["name_zh_cn"] = zh;
        g["name_en_us"] = en;
        g["map"] = v["map"].as<std::string>();
        g["type"] = (v["type"].as<std::string>() == "grenade" ? "HE" : v["type"].as<std::string>());
        g["angle"] = { {"pitch", pitch}, {"yaw", yaw} };
        auto sp = v["setpos"];
        g["position"] = { {"x", sp[0]["x"].as<double>()}, {"y", sp[1]["z"].as<double>()} };
        g["pre_throw_command"] = v["extra"][0].as<std::string>();
        g["post_throw_command"] = v["extra"][1].as<std::string>();
        g["throw_command"] = cmd;
        j["grenades"].push_back(g);
    }

    std::ofstream fout(out);
    if (!fout) {
        std::cerr << "无法打开输出文件: " << out << std::endl;
        return;
    }

    fout << j.dump(2);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif // _WIN32

    std::cout << "1: Custom.yml→list.json  \n 2: list.json→Custom.yml\n请选择：";
    int c;
    if (!(std::cin >> c)) {
        std::cerr << "输入无效\n";
        return 1;
    }

    if (c == 2) json_to_yaml("list.json", "Custom.yml");
    else if (c == 1) yaml_to_json("Custom.yml", "list.json");
    else {
        std::cerr << "无效选项\n";
        return 1;
    }

    std::cout << "\n转换完成，按回车键退出...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 吃掉之前输入留下的换行符
    std::cin.get();

    return 0;
}
