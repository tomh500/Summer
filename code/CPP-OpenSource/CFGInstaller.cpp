#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "Tools.h"
#include "Global.h"
using namespace std;
using namespace std::filesystem;

void AppendIfMissing(const path& filePath, const string& lineToAdd,HWND hWnd)
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
            MessageBoxW(hWnd, L"无法打开 autoexec.cfg 进行写入", L"错误", MB_OK | MB_ICONERROR);
            return;
        }
        if (!content.empty() && content.back() != '\n') {
            out << "\n";
        }

        out << lineToAdd << "\n";
        MessageBoxW(hWnd, L"已成功添加 exec Autumn/Setup 到 autoexec.cfg", L"完成", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBoxW(hWnd, L"autoexec.cfg 已包含 exec Autumn/Setup，无需添加", L"信息", MB_OK | MB_ICONINFORMATION);
    }
}



int CFGInstaller(HWND hWnd)
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    path CFGRootPath = path(exePath).parent_path(); // 当前目录
    CFGRootPath = CFGRootPath.parent_path();        // 上一级
    CFGRootPath = CFGRootPath.parent_path();        // 再上一级

    path srcRoot = CFGRootPath / L"extra" / L"resources";

    // 文件复制操作，一旦失败立即返回 1
    if (!CopyFile(
        (srcRoot / L"Autumn_Installed.cfg").wstring(),
        (CFGRootPath.parent_path().parent_path().parent_path() / L"cfg" / L"Autumn_Installed.cfg").wstring()
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
        (srcRoot / L"keybindings_tchinese.txt").wstring(),
        (CFGRootPath.parent_path().parent_path() / L"resource" / L"keybindings_tchinese.txt").wstring()
    )) return 1;

    if (!CopyFile(
        (srcRoot / L"keybindings_japanese.txt").wstring(),
        (CFGRootPath.parent_path().parent_path() / L"resource" / L"keybindings_japanese.txt").wstring()
    )) return 1;

    if (!CopyFile(
        (srcRoot / L"keybindings_russian.txt").wstring(),
        (CFGRootPath.parent_path().parent_path() / L"resource" / L"keybindings_russian.txt").wstring()
    )) return 1;


    if (!CopyFile(
        (srcRoot / L"gamestate_integration_autumn.cfg").wstring(),
        (CFGRootPath.parent_path() / L"gamestate_integration_autumn.cfg").wstring()
    )) return 1;

    //在Sounds下创建目录
    path soundsDir = CFGRootPath.parent_path().parent_path() / L"sounds" / L"Autumn";
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
            MessageBoxW(hWnd, err.c_str(), L"创建目录失败", MB_OK | MB_ICONERROR);
        }
        return 3;
    }
	if (StartApps(L"LinkListener.exe", L"", false) != 0)
	{
		MessageBoxW(hWnd, L"启动 LinkListener.exe 失败", L"错误", MB_OK | MB_ICONERROR);
		return 4;   
	}
    int response = MessageBoxW(hWnd, L"全部文件已成功复制，是否添加到 autoexec.cfg？", L"操作确认", MB_YESNO | MB_ICONQUESTION);
    if (response == IDYES) {
        path autoexecPath = CFGRootPath.parent_path() / L"autoexec.cfg";

        try {
            // 若不存在则创建
            if (!exists(autoexecPath)) {
                ofstream create(autoexecPath); // UTF-8 默认打开
                if (!create) throw runtime_error("创建 autoexec.cfg 失败");
                create.close();
            }

            AppendIfMissing(autoexecPath, "exec Autumn/Setup",hWnd);
        }
        catch (...) {
            MessageBoxW(hWnd, L"写入 autoexec.cfg 失败", L"错误", MB_OK | MB_ICONERROR);
            return 2;
        }
    }
   
    int result = MessageBoxW(
        hWnd,
        L"是否需要现在编辑用户空间，如果选否，需要稍后自行配置CFG的风格和绑定",
        L"操作确认",
        MB_YESNO | MB_ICONQUESTION
    );

    if (result == IDYES) {
        StartAppsNew( L"Asul_Editor.exe", L"-asulink ..\\asulproject\\fastconfig.asulink", false);
    }

    

    return 0;
}


int CFGInstaller_EN(HWND hWnd)
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    path CFGRootPath = path(exePath).parent_path(); // 当前目录
    CFGRootPath = CFGRootPath.parent_path();        // 上一级
    CFGRootPath = CFGRootPath.parent_path();        // 再上一级

    path srcRoot = CFGRootPath / L"extra" / L"resources";

    // 文件复制操作，一旦失败立即返回 1
    if (!CopyFile(
        (srcRoot / L"Autumn_Installed.cfg").wstring(),
        (CFGRootPath.parent_path().parent_path().parent_path() / L"cfg" / L"Autumn_Installed.cfg").wstring()
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
        (srcRoot / L"keybindings_tchinese.txt").wstring(),
        (CFGRootPath.parent_path().parent_path() / L"resource" / L"keybindings_tchinese.txt").wstring()
    )) return 1;

    if (!CopyFile(
        (srcRoot / L"keybindings_japanese.txt").wstring(),
        (CFGRootPath.parent_path().parent_path() / L"resource" / L"keybindings_japanese.txt").wstring()
    )) return 1;

    if (!CopyFile(
        (srcRoot / L"keybindings_russian.txt").wstring(),
        (CFGRootPath.parent_path().parent_path() / L"resource" / L"keybindings_russian.txt").wstring()
    )) return 1;


    if (!CopyFile(
        (srcRoot / L"gamestate_integration_autumn.cfg").wstring(),
        (CFGRootPath.parent_path() / L"gamestate_integration_autumn.cfg").wstring()
    )) return 1;

    //在Sounds下创建目录
    path soundsDir = CFGRootPath.parent_path().parent_path() / L"sounds" / L"Autumn";
    path musicModeDir = soundsDir / L"musicmode";
    path tempDir = soundsDir / L"temp";

    try {
        create_directories(musicModeDir);
        create_directories(tempDir);
    }
    catch (const filesystem_error& e) {
        if (debug == 1) {
            wstring err = L"创建目录失败：\n" + soundsDir.wstring() +
                L"\n错误信息：" + String2WString(e.what());
            MessageBoxW(hWnd, err.c_str(), L"Create folder failed", MB_OK | MB_ICONERROR);
        }
        return 3;
    }
    if (StartApps(L"LinkListener.exe", L"", false) != 0)
    {
        MessageBoxW(hWnd, L"unbale to link desktop", L"错误", MB_OK | MB_ICONERROR);
        return 4;
    }
    int response = MessageBoxW(hWnd, L"Copiled! write autoexec.cfg？", L"操作确认", MB_YESNO | MB_ICONQUESTION);
    if (response == IDYES) {
        path autoexecPath = CFGRootPath.parent_path() / L"autoexec.cfg";

        try {
            // 若不存在则创建
            if (!exists(autoexecPath)) {
                ofstream create(autoexecPath); // UTF-8 默认打开
                if (!create) throw runtime_error("create autoexec.cfg failed");
                create.close();
            }

            AppendIfMissing(autoexecPath, "exec Autumn/Setup", hWnd);
        }
        catch (...) {
            MessageBoxW(hWnd, L"failed to write autoexec.cfg ", L"错误", MB_OK | MB_ICONERROR);
            return 2;
        }
    }

    int result = MessageBoxW(
        hWnd,
        L"Edit CFG setting right now?",
        L"操作确认",
        MB_YESNO | MB_ICONQUESTION
    );

    if (result == IDYES) {
        StartAppsNew(L"Asul_Editor.exe", L"-asulink ..\\asulproject\\fastconfig.asulink", false);
    }



    return 0;
}

