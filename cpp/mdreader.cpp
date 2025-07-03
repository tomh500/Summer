#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <cmark.h>

#ifdef _WIN32
#include <windows.h>
#include <codecvt>
#include <locale>
#else
#include <unistd.h>
#endif

std::string read_file_utf8(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::string get_temp_path() {
#ifdef _WIN32
    wchar_t tmp_path[MAX_PATH];
    GetTempPathW(MAX_PATH, tmp_path);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(tmp_path);
#else
    const char* tmp = std::getenv("TMPDIR");
    if (!tmp) tmp = "/tmp";
    return std::string(tmp) + "/";
#endif
}

void open_in_browser(const std::string& path) {
#ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    std::wstring wpath = conv.from_bytes(path);
    ShellExecuteW(NULL, L"open", wpath.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
    std::string cmd = "xdg-open \"" + path + "\"";
    system(cmd.c_str());
#endif
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: mdread <markdown-file>" << std::endl;
        return 1;
    }

    std::string md_path = argv[1];
    std::string md_content = read_file_utf8(md_path);
    if (md_content.empty()) return 1;

    char* html_c = cmark_markdown_to_html(md_content.c_str(), md_content.size(), CMARK_OPT_DEFAULT);
    std::string html_body(html_c);
    free(html_c);

    std::string full_html = "<html><head><meta charset='utf-8'><style>body{font-family:sans-serif;margin:40px;line-height:1.6;}code{background:#f0f0f0;padding:2px 4px;border-radius:4px;font-family:monospace;}pre code{display:block;padding:10px;overflow-x:auto;}h1,h2,h3{border-bottom:1px solid #eee;}</style></head><body>" + html_body + "</body></html>";

    std::string tmp_path = get_temp_path() + "md_preview.html";

    std::ofstream out(tmp_path);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << tmp_path << std::endl;
        return 1;
    }
    out << full_html;
    out.close();

    open_in_browser(tmp_path);

    return 0;
}
