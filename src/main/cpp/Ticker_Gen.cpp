#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main() {
    // 用户输入
	   int debugmode = 0;

    // 检测 debug.gen 文件是否存在
    ifstream debugFile("debug.gen");

    // 如果文件存在，则将 debugmode 设置为 1
    if (debugFile.is_open()) {
        debugmode = 1;
        debugFile.close();
    }
    system("chcp 65001");
	if(debugmode==1){
		cout<<"DebugMode Enable!"<<endl;}
    string alias_name;  // 新增变量，用于存储别名
    int n, frame_rate, k, t;
    string setup_directory;
    
    cout << "请输入命令的别名: ";
    cin >> alias_name;  // 获取别名
    
    cout << "请输入生成的 cfg 文件数量: ";
    cin >> n;
    
    cout << "请输入游戏的帧率 (如 165): ";
    cin >> frame_rate;
    
    cout << "请输入每个文件的命令组数 (k): ";
    cin >> k;
    
    cout << "请输入每组命令的 sleep 时间 (t) (单位：毫秒): ";
    cin >> t;
    
    cout << "请输入 setup 目录路径 (用于 Ticker_setup.cfg 文件内的 exec_async 路径): ";
    cin >> setup_directory;

    // 每行命令的执行时间（毫秒），忽略小数部分
    int cycle_time_per_line = 1000 / frame_rate;  // 单行命令的执行时间（毫秒）

    // 每个文件包含 k 组命令，每组包含 2 行（sq 和 sleep t），总共 2k 行命令
    // 计算每个文件的执行时间
    int total_time_per_file = k * (2 * cycle_time_per_line + t);  // 每个文件的总执行时间（毫秒）

    // 记录前面文件已经消耗的时间
    int accumulated_time = 0;

    // 生成一个存储文件名的 vector
    vector<string> filenames;

    // 循环生成 n 个 cfg 文件
    for (int i = 1; i <= n; ++i) {
        // 生成文件名并存入 vector
        string filename = alias_name + "_" + to_string(i) + ".cfg";  // 使用别名作为文件名前缀
        filenames.push_back(filename);

        // 拼接当前文件的完整路径（当前工作目录）
        string full_path = filename; // 当前工作目录下创建文件
        cout << "生成文件: " << full_path << endl;  // 输出生成文件的路径，进行调试

        // 打开一个文件来写入
        ofstream file(full_path);
        if (!file.is_open()) {
            cerr << "无法打开文件: " << full_path << endl;
            continue;  // 如果文件无法打开，跳过该文件
        }

        // 如果是后续文件，首先添加 sleep 累计时间
        if (i > 1) {
				if(t==0){
					 file << "sleep " << accumulated_time/2 << endl;
				}else{
            file << "sleep " << accumulated_time << endl;
				}
        }

        // 在文件开头添加 say_team ticker_<i> start
		if(debugmode==1)
		{
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
		if(debugmode==1||i==n)
		{
			file << "say_team ticker " << i << " died" << endl;
		}
		
        // 关闭当前文件
		
        file.close();
    }

    // 生成 Ticker_setup.cfg 文件
    string setup_file_path = "Ticker_setup.cfg";  // 直接在当前工作目录生成 Ticker_setup.cfg
    cout << "正在生成 Ticker_setup.cfg 文件: " << setup_file_path << endl;
    ofstream setup_file(setup_file_path);

    // 写入 Ticker_setup.cfg 的内容
    setup_file << "sv_cheats 1" << endl;

    // 添加 alias <别名> 到 Ticker_setup.cfg
  //  setup_file << "alias " << alias_name << endl;

    // 为每个文件生成 exec_async 调用
    for (int i = 0; i < n; ++i) {
        setup_file << "exec_async " << setup_directory + "/" + filenames[i] << endl;
    }

    // 关闭 Ticker_setup.cfg 文件
    setup_file.close();

    cout << "生成完成！" << endl;
    return 0;
}

