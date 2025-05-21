#include <iostream>
#include <filesystem>
#include <string>
#include <Windows.h>
#include "sw.h"
namespace fs = std::filesystem;



int CreatFolder()
{
    // ʹ�����·�� "..\..\sounds"
    std::wstring basePath = L"..\\..\\sounds";

    // ���� "DearMoments" �ļ���
    fs::path DearMomentsPath = basePath;
    DearMomentsPath.append(L"\\DearMoments");

    // ��� "DearMoments" �ļ��в����ڣ��򴴽�
    if (!fs::exists(DearMomentsPath)) {
        if (fs::create_directory(DearMomentsPath)) {
            std::cout << "created: " << WStringToString(DearMomentsPath.wstring()) << std::endl;
        }
        else {
            std::cerr << "create failed: " << WStringToString(DearMomentsPath.wstring()) << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "neednt create: " << WStringToString(DearMomentsPath.wstring()) << std::endl;
    }

    // ���� "DearMoments/musicmode" �ļ���
    fs::path musicModePath = DearMomentsPath;
    musicModePath.append(L"\\musicmode");

    if (!fs::exists(musicModePath)) {
        if (fs::create_directory(musicModePath)) {
            std::cout << "�ɹ������ļ���: " << WStringToString(musicModePath.wstring()) << std::endl;
        }
        else {
            std::cerr << "�����ļ���ʧ��: " << WStringToString(musicModePath.wstring()) << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "�ļ����Ѿ�����: " << WStringToString(musicModePath.wstring()) << std::endl;
    }

    // ���� "DearMoments/temp" �ļ���
    fs::path tempPath = DearMomentsPath;
    tempPath.append(L"\\temp");

    if (!fs::exists(tempPath)) {
        if (fs::create_directory(tempPath)) {
            std::cout << "�ɹ������ļ���: " << WStringToString(tempPath.wstring()) << std::endl;
        }
        else {
            std::cerr << "�����ļ���ʧ��: " << WStringToString(tempPath.wstring()) << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "�ļ����Ѿ�����: " << WStringToString(tempPath.wstring()) << std::endl;
    }

    return 0;
}
