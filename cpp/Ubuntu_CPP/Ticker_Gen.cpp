#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// 生成 35 进制别名后缀
string generate_35base_suffix(int index) {
    const string charset = "123456789abcdefghijklmnopqrstuvwxyz";
    string suffix = "";
    do {
        suffix = charset[index % 35] + suffix;
        index = index / 35 - 1;
    } while (index >= 0);
    return suffix;
}

// 将路径标准化
string normalize_path(const string& path) {
    string normalized = path;
    replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

// 清除字符串尾部空白字符（特别是 \n 和 \r）
void trim_trailing_whitespace(string& s) {
    while (!s.empty() && isspace(s.back())) {
        s.pop_back();
    }
}

int main(int argc, char* argv[]) {
    int debugmode = 0;
    ifstream debugFile("debug.gen");
    if (debugFile.is_open()) {
        debugmode = 1;
        debugFile.close();
    }

    #ifdef _WIN32
    system("chcp 65001");
    #endif

    if (debugmode) {
        cout << "DebugMode Enable!" << endl;
    }

    // 输入变量
    string base_alias;
    int n, frame_rate, k, t;
    string setup_directory;

    // 读取参数
    if (argc == 3 && string(argv[1]) == "-f") {
        ifstream infile(argv[2]);
        if (!infile.is_open()) {
            cerr << "无法打开输入文件: " << argv[2] << endl;
            return 1;
        }

        getline(infile, base_alias);
        trim_trailing_whitespace(base_alias);  // 关键修复行

        string line;
        getline(infile, line); n = stoi(line);
        getline(infile, line); frame_rate = stoi(line);
        getline(infile, line); k = stoi(line);
        getline(infile, line); t = stoi(line);
        getline(infile, setup_directory);
        trim_trailing_whitespace(setup_directory);

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

    // 去除 setup 目录路径尾部多余斜杠
    if (!setup_directory.empty() && setup_directory.back() == '/') {
        setup_directory.pop_back();
    }

    vector<string> filenames;
    vector<string> alias_variants;

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
        ofstream file(filename);  // 不用 binary，避免 Linux 下问题

        if (!file.is_open()) {
            cerr << "无法打开文件: " << filename << endl;
            continue;
        }

        // Debug 开始标志
        if (debugmode) {
            file << "say_team ticker_" << (i + 1) << " start\n";
        }

        // sleep 延迟
        if (i > 0) {
            double sleep_time = time_per_file_ms * i;
            file << "sleep " << static_cast<int>(round(sleep_time)) << "\n";
        }

        // alias 定义
        if (i == 0) {
            file << "alias " << current_alias << " " << base_alias << "\n";
        } else {
            string prev_alias = alias_variants[i - 1];
            file << "alias " << prev_alias << ";\n";
            file << "alias " << current_alias << " " << base_alias << "\n";
        }

        // 主体命令组
        for (int j = 0; j < k; ++j) {
            file << current_alias << "\n";
            if (t > 0) {
                file << "sleep " << t << "\n";
            }
        }

        // Debug 结束标志
        if (debugmode || i == n - 1) {
            file << "say_team ticker " << (i + 1) << " died\n";
        }

        file.close();
    }

    // 写入 Ticker_setup.cfg
    ofstream setup_file("Ticker_setup.cfg");
    setup_file << "sv_cheats 1\n";

    for (const auto& fname : filenames) {
        setup_file << "exec_async " << setup_directory << "/" << fname << "\n";
    }

    setup_file.close();

    cout << "生成完成！" << endl;
    return 0;
}
