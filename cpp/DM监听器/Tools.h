#pragma once
#include "framework.h"
#include "DM¼àÌýÆ÷.h"
#include <windows.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <tlhelp32.h>
#pragma comment(lib, "Kernel32.lib")

using namespace std;
using namespace filesystem;

bool CheckRunPath();
bool FolderExists(const wstring& folderPath);
int StartApps(const std::wstring& exePath, const std::wstring& arguments, bool showWindow);
bool IsProcessRunning(const wstring& processName);
void KillProcess(const wstring& processName);
void ClearAutoexec(HWND hWnd);
bool SetWorkingDirectory(LPCWSTR path);
string WString2String(const wstring& wstr);
wstring String2WString(const std::string& str);
bool CopyFile(const wstring& src, const wstring& dst);
int CFGInstaller();