#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// 生成 35 进制别名后缀：1 ~ 9, a ~ z, 11, ..., zz, 111...
string generate_35base_suffix(int index) {
    const string charset = "123456789abcdefghijklmnopqrstuvwxyz";
    string suffix = "";

    do {
        suffix = charset[index % 35] + suffix;
        index = index / 35 - 1;
    } while (index >= 0);

    return suffix;
}

// 将 Windows 路径中的 \ 转为 /
string normalize_path(const string& path) {
    string normalized = path;
    replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

int main(int argc, char* argv[]) {
    int debugmode = 0;
    ifstream debugFile("debug.gen");
    if (debugFile.is_open()) {
        debugmode = 1;
        debugFile.close();
    }

    system("chcp 65001");

    if (debugmode) {
        cout << "DebugMode Enable!" << endl;
    }

    // 输入变量
    string base_alias;
    int n, frame_rate, k, t;
    string setup_directory;

    // 读取参数文件
    if (argc == 3 && string(argv[1]) == "-f") {
        ifstream infile(argv[2]);
        if (!infile.is_open()) {
            cerr << "无法打开输入文件: " << argv[2] << endl;
            return 1;
        }

        getline(infile, base_alias);
        string line;

        getline(infile, line); n = stoi(line);
        getline(infile, line); frame_rate = stoi(line);
        getline(infile, line); k = stoi(line);
        getline(infile, line); t = stoi(line);
        getline(infile, setup_directory);
        infile.close();

        setup_directory = normalize_path(setup_directory);
    } else {
        // 手动交互输入
        cout << "请输入命令的别名: "; cin >> base_alias;
        cout << "请输入生成的 cfg 文件数量: "; cin >> n;
        cout << "请输入游戏的帧率 (如 165): "; cin >> frame_rate;
        cout << "请输入每个文件的命令组数 (k): "; cin >> k;
        cout << "请输入每组命令的 sleep 时间 (t) (单位：毫秒): "; cin >> t;
        cout << "请输入 setup 目录路径: "; cin >> setup_directory;

        setup_directory = normalize_path(setup_directory);
    }

    vector<string> filenames;
    vector<string> alias_variants;

    // 预计算单个文件所需时间
    double time_per_file_ms = 0;
    if (t == 0) {
        time_per_file_ms = (k * 1000.0) / frame_rate;
    } else {
        double time_lines = (k * 2 * 1000.0) / frame_rate;
        double time_sleeps = k * t;
        time_per_file_ms = time_lines + time_sleeps;
    }

    for (int i = 0; i < n; ++i) {
        string suffix = generate_35base_suffix(i);
        string current_alias = base_alias + suffix;
        alias_variants.push_back(current_alias);

        string filename = current_alias + ".cfg";
        filenames.push_back(filename);

        cout << "生成文件: " << filename << endl;
        ofstream file(filename, ios::binary); // 防止写入 CRLF

        if (!file.is_open()) {
            cerr << "无法打开文件: " << filename << endl;
            continue;
        }

        string newline = "\n";

        // Debug 开始标志
        if (debugmode) {
            string msg = "say_team ticker_" + to_string(i + 1) + " start" + newline;
            file.write(msg.c_str(), msg.size());
        }

        // sleep 延迟
        if (i > 0) {
            double sleep_time = time_per_file_ms * i;
            string sleep_cmd = "sleep " + to_string(static_cast<int>(round(sleep_time))) + newline;
            file.write(sleep_cmd.c_str(), sleep_cmd.size());
        }

        // alias 定义
        if (i == 0) {
            string alias_def = "alias " + current_alias + " " + base_alias + newline;
            file.write(alias_def.c_str(), alias_def.size());
        } else {
            string prev_alias = alias_variants[i - 1];
            string alias_def = "alias " + prev_alias + ";alias " + current_alias + " " + base_alias + newline;
            file.write(alias_def.c_str(), alias_def.size());
        }

        // 命令主体
        for (int j = 0; j < k; ++j) {
            string cmd = current_alias + newline;
            file.write(cmd.c_str(), cmd.size());

            if (t > 0) {
                string sleep_cmd = "sleep " + to_string(t) + newline;
                file.write(sleep_cmd.c_str(), sleep_cmd.size());
            }
        }

        // Debug 结束标志
        if (debugmode || i == n - 1) {
            string msg = "say_team ticker " + to_string(i + 1) + " died" + newline;
            file.write(msg.c_str(), msg.size());
        }

        file.close();
    }

    // 写入 Ticker_setup.cfg
    ofstream setup_file("Ticker_setup.cfg", ios::binary);
    setup_file.write("sv_cheats 1\n", 12);

    for (const auto& fname : filenames) {
        string exec_line = "exec_async " + setup_directory + "/" + fname + "\n";
        setup_file.write(exec_line.c_str(), exec_line.size());
    }

    setup_file.close();
    cout << "生成完成！" << endl;

    return 0;
}
