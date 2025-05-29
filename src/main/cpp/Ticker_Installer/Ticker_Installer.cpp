#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <Windows.h>

namespace fs = std::filesystem;

void download_and_extract(const std::string& url, const std::string& output_dir, const std::string& extract_dir) {
    // 使用 wget 下载文件
    std::string command = "src\\main\\lib\\wget.exe " + url + " -P " + output_dir;
    std::cout << "Executing: " << command << std::endl;
    system(command.c_str());

    // 使用 7z 解压文件
    std::string extract_command = "src\\main\\lib\\7z.exe x " + output_dir + "\\"
        + fs::path(url).filename().string() + " -o" + extract_dir + " -aoa";
    std::cout << "Executing: " << extract_command << std::endl;
    system(extract_command.c_str());
}

void clear_temp_files(const std::string& temp_dir) {
    std::cout << "Clearing temporary files in " << temp_dir << std::endl;
    for (const auto& entry : fs::directory_iterator(temp_dir)) {
        if (fs::is_regular_file(entry)) {
            fs::remove(entry);  // 删除文件
        }
    }
}


int main() {
    // 设置控制台为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "请选择增量包:" << std::endl;
    std::cout << "1. 增量包1（延长至600分钟）" << std::endl;
    std::cout << "2. 增量包2（延长至1000分钟，需安装增量包A）" << std::endl;
    std::cout << "3. 增量包3（延长至1200分钟，需安装增量包A、B）" << std::endl;

    int choice = 0;
    std::cout << "请输入选项（1、2或3）: ";
    std::cin >> choice;

    // 创建临时文件夹
    std::string temp_dir = "src\\temp";
    fs::create_directory(temp_dir);  // 创建临时目录

    if (choice == 1) {
        std::cout << "你选择了增量包1" << std::endl;

        std::cout << "正在下载增量包1..." << std::endl;
        download_and_extract("https://github.com/tomh500/SquareModsMarket/releases/download/Ticker_extra/add6hour_1ms_p1.zip", temp_dir, "src\\main\\cfg\\cn\\luotiany1\\SquareNextgen\\Utilities\\Ticker\\180fps\\1ms");
        download_and_extract("https://github.com/tomh500/SquareModsMarket/releases/download/Ticker_extra/6ms_extra.zip", temp_dir, "src\\main\\cfg\\cn\\luotiany1\\SquareNextgen\\Utilities\\Ticker\\180fps\\6ms");
        download_and_extract("https://github.com/tomh500/SquareModsMarket/releases/download/Ticker_extra/66ms_extra.zip", temp_dir, "src\\main\\cfg\\cn\\luotiany1\\SquareNextgen\\Utilities\\Ticker\\180fps\\66ms");

        // 清理临时文件夹中的压缩包
        clear_temp_files(temp_dir);
    }
    else if (choice == 2) {
        std::cout << "你选择了增量包2" << std::endl;

        std::cout << "正在下载增量包2..." << std::endl;
        download_and_extract("https://github.com/tomh500/SquareModsMarket/releases/download/Ticker_extra/add6hour_1ms_p2.zip", temp_dir, "src\\main\\cfg\\cn\\luotiany1\\SquareNextgen\\Utilities\\Ticker\\180fps\\1ms");

        // 清理临时文件夹中的压缩包
        clear_temp_files(temp_dir);
    }
    else if (choice == 3) {
        std::cout << "你选择了增量包3" << std::endl;

        std::cout << "正在下载增量包3..." << std::endl;
        download_and_extract("https://github.com/tomh500/SquareModsMarket/releases/download/Ticker_extra/add3hour_1ms_p3.zip", temp_dir, "src\\main\\cfg\\cn\\luotiany1\\SquareNextgen\\Utilities\\Ticker\\180fps\\1ms");

        // 清理临时文件夹中的压缩包
        clear_temp_files(temp_dir);
    }
    else {
        std::cout << "无效的选择，请重新运行程序并选择 1、2 或 3。" << std::endl;
    }

    return 0;
}
