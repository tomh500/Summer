#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <direct.h> // 用于创建目录

using namespace std;

// 解析命令行参数的函数
void parseArguments(int argc, char* argv[], string& alias_name, int& n, int& frame_rate, int& k, int& t, string& setup_directory, string& output_path, int& debugmode) {
    // 默认值
    alias_name = "";
    n = 1;
    frame_rate = 60;
    k = 1;
    t = 100;
    setup_directory = "";
    output_path = "";  // 默认输出路径为空
    debugmode = 0;

    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-alias") == 0 && i + 1 < argc) {
            alias_name = argv[++i];
        }
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            n = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-fr") == 0 && i + 1 < argc) {
            frame_rate = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            k = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            t = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-setup") == 0 && i + 1 < argc) {
            setup_directory = argv[++i];
        }
        else if (strcmp(argv[i], "-path") == 0 && i + 1 < argc) {
            output_path = argv[++i];  // 获取文件输出路径
        }
        else if (strcmp(argv[i], "-debug") == 0) {
            debugmode = 1;
        }
        else {
            cerr << "未知的参数: " << argv[i] << endl;
            exit(1);
        }
    }

    // 如果没有提供 alias_name，给出提示
    if (alias_name.empty()) {
        cerr << "错误: 请提供命令的别名 (-alias)" << endl;
        exit(1);
    }

    // 如果没有提供 setup_directory，给出提示
    if (setup_directory.empty()) {
        cerr << "错误: 请提供 setup 目录路径 (-setup)" << endl;
        exit(1);
    }
}

// 检查并创建文件夹
void createDirectory(const string& path) {
    if (_mkdir(path.c_str()) != 0) {
        // 如果目录已存在，不做任何操作
        if (errno != EEXIST) {
            cerr << "无法创建目录: " << path << endl;
            exit(1);
        }
    }
}

int main(int argc, char* argv[]) {
    system("chcp 65001");
    // 调用解析函数
    string alias_name;
    int n, frame_rate, k, t;
    string setup_directory;
    string output_path;
    int debugmode;

    parseArguments(argc, argv, alias_name, n, frame_rate, k, t, setup_directory, output_path, debugmode);

    // 检测 debug.gen 文件是否存在
    ifstream debugFile("debug.gen");

    // 如果文件存在，则将 debugmode 设置为 1
    if (debugFile.is_open()) {
        debugmode = 1;
        debugFile.close();
    }

    system("chcp 65001"); // 设置字符集为 UTF-8
    if (debugmode == 1) {
        cout << "DebugMode Enable!" << endl;
    }

    // 每行命令的执行时间（毫秒），忽略小数部分
    int cycle_time_per_line = 1000 / frame_rate;  // 单行命令的执行时间（毫秒）

    // 每个文件包含 k 组命令，每组包含 2 行（sq 和 sleep t），总共 2k 行命令
    // 计算每个文件的执行时间
    int total_time_per_file = k * (2 * cycle_time_per_line + t);  // 每个文件的总执行时间（毫秒）

    // 记录前面文件已经消耗的时间
    int accumulated_time = 0;

    // 生成一个存储文件名的 vector
    vector<string> filenames;

    // 如果指定了 output_path，首先创建目标文件夹
    if (!output_path.empty()) {
        createDirectory(output_path);
    }

    // 循环生成 n 个 cfg 文件
    for (int i = 1; i <= n; ++i) {
        // 生成文件名并存入 vector
        string filename = alias_name + "_" + to_string(i) + ".cfg";  // 使用别名作为文件名前缀
        filenames.push_back(filename);

        // 拼接当前文件的完整路径（如果有指定 output_path）
        string full_path = output_path.empty() ? filename : output_path + "/" + filename;
        cout << "生成文件: " << full_path << endl;  // 输出生成文件的路径，进行调试

        // 打开一个文件来写入
        ofstream file(full_path);
        if (!file.is_open()) {
            cerr << "无法打开文件: " << full_path << endl;
            continue;  // 如果文件无法打开，跳过该文件
        }

        // 如果是后续文件，首先添加 sleep 累计时间
        if (i > 1) {
		if(t!=0){
            file << "sleep " << accumulated_time << endl;}
else { file << "sleep " << accumulated_time/2 << endl;}
		
        }

        // 在文件开头添加 say_team ticker_<i> start
        if (debugmode == 1) {
            file << "say_team ticker_" << i << " start" << endl;
        }

        // 生成 k 组命令，每组包含 别名 和 "sleep t"（如果 t > 0）
        for (int j = 0; j < k; ++j) {
            file << alias_name << endl;  // 使用别名替换原来的 "sq"
            if (t > 0) {
                file << "sleep " << t << endl;  // 只有当 t > 0 时才添加 sleep
            }
	
        }

        // 更新当前文件执行完所需的时间
        accumulated_time += total_time_per_file;

        // 在文件末尾添加 say_team ticker <i> died
        if (debugmode == 1 || i == n) {
            file << "say_team ticker " << i << " died" << endl;
        }

        // 关闭当前文件
        file.close();
    }

    // 生成 Ticker_setup.cfg 文件
    string setup_file_path = output_path.empty() ? "Ticker_setup.cfg" : output_path + "/Ticker_setup.cfg";  // 生成路径上的 setup 文件
    cout << "正在生成 Ticker_setup.cfg 文件: " << setup_file_path << endl;
    ofstream setup_file(setup_file_path);

    // 写入 Ticker_setup.cfg 的内容
    setup_file << "sv_cheats 1" << endl;

    // 添加 alias <别名> 到 Ticker_setup.cfg
    setup_file << "alias " << alias_name << endl;

    // 为每个文件生成 exec_async 调用
    for (int i = 0; i < n; ++i) {
        setup_file << "exec_async " << setup_directory + "/" + filenames[i] << endl;
    }

    // 关闭 Ticker_setup.cfg 文件
    setup_file.close();

    cout << "生成完成！" << endl;
    return 0;
}
