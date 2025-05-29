#include <windows.h>

#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cwctype>
#include <algorithm>
#include <fstream>



using namespace std;

wstring StringToWString(const string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
    wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], size_needed);
    return wstr;
}

// wstring 转 UTF-8 string
string WStringToString(const wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), &str[0], size_needed, NULL, NULL);
    return str;
}

void RemoveExistingProxySettings(const string& filePath) {
    ifstream file(filePath);  // 打开文件读取
    if (!file) {
        cerr << "无法打开文件: " << filePath << endl;
        return;
    }

    // 使用一个字符串流来存储所有非代理行
    stringstream buffer;
    string line;
    while (getline(file, line)) {
        // 检查是否是代理配置行（http_proxy、https_proxy 或 all_proxy），如果是就跳过
        if (line.find("http_proxy") == string::npos &&
            line.find("https_proxy") == string::npos &&
            line.find("all_proxy") == string::npos) {
            buffer << line << endl;
        }
    }
    file.close();

    // 重新写回文件，更新代理配置
    ofstream outFile(filePath);
    if (!outFile) {
        cerr << "无法打开文件: " << filePath << endl;
        return;
    }

    outFile << buffer.str();  // 写入新的内容
    outFile.close();

    cout << "代理配置已清除。" << endl;
}

// 用于设置新的代理配置到 wget 配置文件
void SetProxyToWgetRC(const string& proxyAddress) {
    string wgetRCFilePath = "src/main/lib/.wgetrc";  // 假设的 .wgetrc 配置文件路径

    // 删除现有的代理设置
    RemoveExistingProxySettings(wgetRCFilePath);

    // 打开文件（追加模式）
    ofstream file(wgetRCFilePath, ios::app);  // 确保使用 std::ofstream
    if (!file) {
        cerr << "无法打开文件: " << wgetRCFilePath << endl;
        return;
    }

    // 写入代理设置
    file << "# Proxy settings added via C++ program\n";

    // 只设置 http_proxy 和 https_proxy
    if (!proxyAddress.empty()) {
        file << "http_proxy = " << proxyAddress << "\n";
        file << "https_proxy = " << proxyAddress << "\n";
    }
    else {
        // 如果没有代理，清空配置文件中的代理设置
        file << "http_proxy = \n";
        file << "https_proxy = \n";
    }

    file.close();

    cout << "代理设置已成功写入 " << wgetRCFilePath << endl;
}


int main()
{
    system("chcp 65001");
    int choiceProxy;

    // 使用宽字符串进行交互，但通过窄字符串输出
    wstring wmessage = L"是否开启代理？请选择序号：\n";
    wmessage += L"  1. 无需代理\n";
    wmessage += L"  2. 本地 HTTP 代理（127.0.0.1:7890）\n";
    wmessage += L"  3. 自定义代理\n\n";
    wmessage += L"请输入你的选择> ";

    // 将宽字符串转回窄字符串
    cout << WStringToString(wmessage);

    cin >> choiceProxy; // 用户输入选择

    string proxyAddress;

    if (choiceProxy == 1) {
        // 不使用代理
        proxyAddress = "";
        cout << "未开启代理" << endl;
    }
    else if (choiceProxy == 2) {
        // 使用本地 HTTP 代理
        proxyAddress = "http://127.0.0.1:7890";
        cout << "已开启 HTTP 代理，端口：7890" << endl;
    }
    else if (choiceProxy == 3) {
        // 自定义代理
        wstring wcustomMessage = L"请输入代理地址（例如：http://127.0.0.1:8080）：";
        cout << WStringToString(wcustomMessage);
        cin >> proxyAddress;
        cout << "已设置自定义代理：" << proxyAddress << endl;
    }
    else {
        cout << "无效的选择，请重新输入。" << endl;
        return 1;  // 退出程序
    }

    // 将代理设置写入 .wgetrc 文件
    SetProxyToWgetRC(proxyAddress);
    system("src\\main\\lib\\wget.exe -e use_proxy=yes -e https_proxy=127.0.0.1:7890 --no-check-certificate --output-document=teach.zip https://github.com/tomh500/SquareModsMarket/releases/download/ItemsTeachPic/teach.zip");
    system("src\\main\\lib\\7z.exe x -aoa teach.zip -o\"teach\" -y");
    system("del teach.zip");
    system("start teach");
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
