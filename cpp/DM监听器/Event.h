#pragma once
#include<iostream>
#include "framework.h"
#include "DM¼àÌýÆ÷.h"

void quitDM(HWND hWnd);
void createSetupCfg();
bool CheckRunPath();
bool FolderExists(const std::wstring& folderPath);
bool IsProcessRunning(const std::wstring& processName);
void KillProcess(const std::wstring& processName);
void HandleKillSound(HWND hWnd);
void CloseKillSound(HWND hWnd);
void ClearAndResetBindings(HWND hWnd);
void MusicPlayerExit(HWND hWnd);