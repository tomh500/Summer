#include "OutputRedirector.h"
#include <Windows.h>
#include <codecvt>
#include <locale>
#include <iostream>
using namespace std;
//------------------------------------------------------------------------------
// OutputRedirector 构造/析构
//------------------------------------------------------------------------------
OutputRedirector::OutputRedirector(HWND hEdit)
    : m_narrowBuf(new NarrowBuf(hEdit))
    , m_wideBuf(new WideBuf(hEdit))
    , m_oldCoutBuf(cout.rdbuf(m_narrowBuf))
    , m_oldWcoutBuf(wcout.rdbuf(m_wideBuf))
{}

OutputRedirector::~OutputRedirector() {
    // 恢复原来的缓冲区
    cout.rdbuf(m_oldCoutBuf);
    wcout.rdbuf(m_oldWcoutBuf);
    delete m_narrowBuf;
    delete m_wideBuf;
}

//------------------------------------------------------------------------------
// NarrowBuf 实现
//------------------------------------------------------------------------------
OutputRedirector::NarrowBuf::NarrowBuf(HWND hEdit)
    : m_hEdit(hEdit)
{}

streambuf::int_type OutputRedirector::NarrowBuf::overflow(streambuf::int_type ch) {
    if (ch == traits_type::eof()) return ch;
    char c = static_cast<char>(ch);
    buffer.push_back(c);
    if (c == '\n') {
        // 缩短示例：假设 buffer 是 UTF-8，可以直接转宽字符
        wstring w;
        w.assign(buffer.begin(), buffer.end());
        append(w);
        buffer.clear();
    }
    return ch;
}

int OutputRedirector::NarrowBuf::sync() {
    if (!buffer.empty()) {
        wstring w(buffer.begin(), buffer.end());
        append(w);
        buffer.clear();
    }
    return 0;
}

void OutputRedirector::NarrowBuf::append(const wstring& text) {
    int len = GetWindowTextLengthW(m_hEdit);
    SendMessageW(m_hEdit, EM_SETSEL, len, len);
    SendMessageW(m_hEdit, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
}

//------------------------------------------------------------------------------
// WideBuf 实现
//------------------------------------------------------------------------------
OutputRedirector::WideBuf::WideBuf(HWND hEdit)
    : m_hEdit(hEdit)
{}

wstreambuf::int_type OutputRedirector::WideBuf::overflow(wstreambuf::int_type ch) {
    if (ch == traits_type::eof()) return ch;
    wchar_t wch = static_cast<wchar_t>(ch);
    buffer.push_back(wch);
    if (wch == L'\n') {
        append(buffer);
        buffer.clear();
    }
    return ch;
}

int OutputRedirector::WideBuf::sync() {
    if (!buffer.empty()) {
        append(buffer);
        buffer.clear();
    }
    return 0;
}

void OutputRedirector::WideBuf::append(const wstring& text) {
    int len = GetWindowTextLengthW(m_hEdit);
    SendMessageW(m_hEdit, EM_SETSEL, len, len);
    SendMessageW(m_hEdit, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
}
