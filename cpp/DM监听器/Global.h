#pragma once
#include <filesystem>

namespace fs = std::filesystem;
extern bool isCS2Running;
extern int debug;
extern int LocalVersion;
extern HINSTANCE hInst;                              
extern fs::path RootPath;