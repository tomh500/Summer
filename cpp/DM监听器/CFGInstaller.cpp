#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "Tools.h"
#include "Global.h"
using namespace std;
using namespace std::filesystem;

void AppendIfMissing(const path& filePath, const string& lineToAdd)
{
    string content;
    ifstream in(filePath, ios::binary);
    if (in) {
        content.assign((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        in.close();
    }

    if (content.find(lineToAdd) == string::npos) {
        ofstream out(filePath, ios::app | ios::binary);
        if (!out) {
            MessageBoxW(nullptr, L"无法打开 autoexec.cfg 进行写入", L"错误", MB_OK | MB_ICONERROR);
            return;
        }

        // 可选：自动补换行
        if (!content.empty() && content.back() != '\n') {
            out << "\n";
        }

        out << lineToAdd << "\n";
        MessageBoxW(nullptr, L"已成功添加 exec DearMoments/Setup 到 autoexec.cfg", L"完成", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBoxW(nullptr, L"autoexec.cfg 已包含 exec DearMoments/Setup，无需添加", L"信息", MB_OK | MB_ICONINFORMATION);
    }
}

int CFGInstaller()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    path CFGRootPath = path(exePath).parent_path(); // 当前目录
    CFGRootPath = CFGRootPath.parent_path();        // 上一级
    CFGRootPath = CFGRootPath.parent_path();        // 再上一级
    CFGRootPath = CFGRootPath.parent_path();        // 第三级父目录

    path srcRoot = CFGRootPath / L"src" / L"resources";

    // 文件复制操作，一旦失败立即返回 1
    if (!CopyFile(
        (srcRoot / L"DearMoments_Installed.cfg").wstring(),
        (CFGRootPath.parent_path().parent_path().parent_path() / L"cfg" / L"DearMoments_Installed.cfg").wstring()
    )) return 1;

    if (!CopyFile(
        (srcRoot / L"keybindings_schinese.txt").wstring(),
        (CFGRootPath.parent_path().parent_path() / L"resource" / L"keybindings_schinese.txt").wstring()
    )) return 1;

    if (!CopyFile(
        (srcRoot / L"keybindings_english.txt").wstring(),
        (CFGRootPath.parent_path().parent_path() / L"resource" / L"keybindings_english.txt").wstring()
    )) return 1;

    if (!CopyFile(
        (srcRoot / L"gamestate_integration_dearmoments.cfg").wstring(),
        (CFGRootPath.parent_path() / L"gamestate_integration_dearmoments.cfg").wstring()
    )) return 1;

    //在Sounds下创建目录
    path soundsDir = CFGRootPath.parent_path().parent_path() / L"sounds" / L"DearMoments";
    path musicModeDir = soundsDir / L"musicmode";
    path tempDir = soundsDir / L"temp";

    try {
        create_directories(musicModeDir);
        create_directories(tempDir);
    }
    catch (const filesystem_error& e) {
        if (debug==1) {
            wstring err = L"创建目录失败：\n" + soundsDir.wstring() +
                L"\n错误信息：" + String2WString(e.what());
            MessageBoxW(nullptr, err.c_str(), L"创建目录失败", MB_OK | MB_ICONERROR);
        }
        return 3;
    }
	if (StartApps(L"LinkListener.exe", L"", false) != 0)
	{
		MessageBoxW(nullptr, L"启动 LinkListener.exe 失败", L"错误", MB_OK | MB_ICONERROR);
		return 4;
	}
    // 复制成功后询问是否添加到 autoexec.cfg
    int response = MessageBoxW(nullptr, L"全部文件已成功复制，是否添加到 autoexec.cfg？", L"操作确认", MB_YESNO | MB_ICONQUESTION);
    if (response == IDYES) {
        path autoexecPath = CFGRootPath.parent_path() / L"autoexec.cfg";

        try {
            // 若不存在则创建
            if (!exists(autoexecPath)) {
                ofstream create(autoexecPath); // UTF-8 默认打开
                if (!create) throw runtime_error("创建 autoexec.cfg 失败");
                create.close();
            }

            AppendIfMissing(autoexecPath, "exec DearMoments/Setup");
        }
        catch (...) {
            MessageBoxW(nullptr, L"写入 autoexec.cfg 失败", L"错误", MB_OK | MB_ICONERROR);
            return 2;
        }
    }

    return 0;
}