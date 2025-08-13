

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>  // transform
#include <cctype>     // tolower
#include <cstdlib>    // system

using namespace std;
namespace fs = std::filesystem;
#ifdef __linux__
void grant_exec_permission(const fs::path& file) {
    try {
        fs::permissions(file,
            fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
            fs::perm_options::add);
        cout << "已赋予执行权限: " << file << endl;
    } catch (const fs::filesystem_error& e) {
        cerr << "赋予执行权限失败: " << e.what() << endl;
    }
}
#endif
void backup_autoexec_case_variants(const fs::path& dir, const string& canonical_name = "autoexec.cfg") {
    string canonical_lower = canonical_name;
    // 确保比较时用小写
    transform(canonical_lower.begin(), canonical_lower.end(), canonical_lower.begin(), ::tolower);

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;

        string fname = entry.path().filename().string();
        string fname_lower = fname;
        transform(fname_lower.begin(), fname_lower.end(), fname_lower.begin(), ::tolower);

        if (fname_lower == canonical_lower) {
            // 是 autoexec.cfg 的大小写变体
            // 但排除全小写的那个
            if (fname != canonical_name) {
                // 重命名
                fs::path backup_path = entry.path();
                backup_path += ".backup";

                cout << "发现 autoexec.cfg 大小写变体文件: " << fname
                     << "，重命名为 " << backup_path.filename() << endl;

                fs::rename(entry.path(), backup_path);
            }
        }
    }
}

bool iequals(const string& a, const string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (tolower(a[i]) != tolower(b[i])) return false;
    }
    return true;
}

bool contains_exec_setup(const fs::path& file) {
    ifstream in(file);
    if (!in.is_open()) {
        cerr << "无法打开文件检测内容: " << file << endl;
        return false;
    }
    string line;
    while (getline(in, line)) {
        if (line.find("exec Summer/Setup") != string::npos) {
            return true;
        }
    }
    return false;
}

void backup_similar_cfg_files(const fs::path& dir, const string& keep_filename) {
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        auto fname = entry.path().filename().string();
        if (iequals(fname, keep_filename)) continue;

        // 判断是否是类似 *.cfg 文件（简单判断后缀）
        if (fname.size() >= 4 && 
            fname.substr(fname.size() - 4) == ".cfg") {
            // 重命名加后缀
            fs::path new_name = entry.path();
            new_name += ".backup";
            cout << "检测到同目录其他配置文件: " << fname << "，重命名为 " << new_name.filename() << endl;
            fs::rename(entry.path(), new_name);
        }
    }
}

void append_exec_setup_if_missing(const fs::path& file) {
    // 先判断是否以换行符结尾
    ifstream in(file, ios::binary);
    if (!in.is_open()) {
        cerr << "无法打开文件追加内容: " << file << endl;
        return;
    }
    in.seekg(0, ios::end);
    if (in.tellg() == 0) {
        // 空文件，直接写入
        in.close();
        ofstream out(file, ios::app);
        out << "exec Summer/Setup\n";
        cout << "文件为空，写入 exec Summer/Setup" << endl;
        return;
    }
    in.seekg(-1, ios::end);
    char last_char;
    in.get(last_char);
    in.close();

    bool ends_with_newline = (last_char == '\n');

    if (contains_exec_setup(file)) {
        cout << "autoexec.cfg 已包含 exec Summer/Setup，不需要重复添加" << endl;
        return;
    }

    ofstream out(file, ios::app);
    if (!ends_with_newline) out << '\n';
    out << "exec Summer/Setup\n";
    cout << "追加写入 exec Summer/Setup 到 autoexec.cfg 文件末尾" << endl;
}

int main() {
    // --- 之前你的写文件和复制文件逻辑 --- 
    fs::path dir = "src";
    fs::create_directories(dir);

    fs::path filePath = dir / "listener.lty";

    ofstream outfile(filePath, ios::out | ios::trunc);
    if (!outfile.is_open()) {
        cerr << "无法打开文件 " << filePath << " 进行写入" << endl;
        return 1;
    }

    outfile << "alias DMListenerBoot\n";
    outfile << "alias DMListenerRunning\n";
    outfile << "exec Summer/src/CS2/main/DM/Tools/define_mousexy;\n";
    outfile.close();

    cout << "文件写入完成: " << filePath << endl;

  try {
        // 1. src/CS2/resources/Summer_Installed.cfg -> ../../../cfg
        fs::path src1 = "src/CS2/resources/Summer_Installed.cfg";
        fs::path dst1 = fs::path("../../../cfg");
        fs::create_directories(dst1);
        fs::copy_file(src1, dst1 / src1.filename(), fs::copy_options::overwrite_existing);
        cout << "复制完成: " << src1 << " -> " << dst1 << endl;

        // 2. src/CS2/resources/gamestate_integration_Summer.cfg -> ..
        fs::path src2 = "src/CS2/resources/gamestate_integration_Summer.cfg";
        fs::path dst2 = fs::path("..");
        fs::create_directories(dst2);
        fs::copy_file(src2, dst2 / src2.filename(), fs::copy_options::overwrite_existing);
        cout << "复制完成: " << src2 << " -> " << dst2 << endl;

        // 3. src/CS2/resources/keybindings_english.txt 和 keybindings_english.txt -> ../../resource
        fs::path dst3 = fs::path("../../resource");
        fs::create_directories(dst3);

        fs::path src3a = "src/CS2/resources/keybindings_english.txt";
        fs::copy_file(src3a, dst3 / src3a.filename(), fs::copy_options::overwrite_existing);
        cout << "复制完成: " << src3a << " -> " << dst3 << endl;

        fs::path src3b = "src/CS2/resources/keybindings_schinese.txt";  // 当前目录下的文件
        fs::copy_file(src3b, dst3 / src3b.filename(), fs::copy_options::overwrite_existing);
        cout << "复制完成: " << src3b << " -> " << dst3 << endl;
    }
    catch (const fs::filesystem_error& e) {
        cerr << "文件复制失败: " << e.what() << endl;
        return 1;
    }
    // --- 新增询问是否写入 autoexec.cfg ---
    cout << "是否写入 ../autoexec.cfg 文件？(y/n): ";
    string ans;
    getline(cin, ans);
    transform(ans.begin(), ans.end(), ans.begin(), ::tolower);
    if (ans == "y" || ans == "yes") {
        fs::path autoexec_dir = "..";
        fs::path autoexec_file = autoexec_dir / "autoexec.cfg";

        backup_autoexec_case_variants(autoexec_dir, "autoexec.cfg");


        // 确保 autoexec.cfg 存在
        if (!fs::exists(autoexec_file)) {
            ofstream tmp(autoexec_file);
            tmp.close();
            cout << "创建新文件: " << autoexec_file << endl;
        }

        append_exec_setup_if_missing(autoexec_file);
    }

    // --- 询问是否锁帧高于 540 fps ---
    cout << "期望锁帧在 540fps 以上吗？(y/n): ";
    getline(cin, ans);
    transform(ans.begin(), ans.end(), ans.begin(), ::tolower);

    fs::path ticker_dir = "src/CS2/main/DM/Tools/Ticker";
    if (ans == "y" || ans == "yes") {
        cout << "执行 GenAll_High_bash.sh ..." << endl;
        // cd 进入目录执行脚本
        string cmd = "cd " + ticker_dir.string() + " && ./GenAll_High_bash.sh";
        #ifdef __linux__
grant_exec_permission(ticker_dir / "GenAll_High_bash.sh");
grant_exec_permission(ticker_dir / "GenAll_Normal_bash.sh");
#endif

        int ret = system(cmd.c_str());
        if (ret != 0) {
            cerr << "执行 GenAll_High_bash.sh 失败，返回码: " << ret << endl;
        }
    } else {
        cout << "执行 GenAll_Normal_bash.sh ..." << endl;
        string cmd = "cd " + ticker_dir.string() + " && ./GenAll_Normal_bash.sh";
        #ifdef __linux__
grant_exec_permission(ticker_dir / "GenAll_High_bash.sh");
grant_exec_permission(ticker_dir / "GenAll_Normal_bash.sh");
#endif

        int ret = system(cmd.c_str());
        if (ret != 0) {
            cerr << "执行 GenAll_Normal_bash.sh 失败，返回码: " << ret << endl;
        }
    }
    system("cd ../../../../../../..");
    return 0;
}
