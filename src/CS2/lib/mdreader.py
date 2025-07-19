import sys
import os
import tempfile
import webbrowser

import markdown
from markdown.extensions.codehilite import CodeHiliteExtension

def read_file_utf8(path):
    try:
        with open(path, 'r', encoding='utf-8') as f:
            return f.read()
    except Exception as e:
        print(f"❌ Failed to read file: {path}\n{e}")
        return ""

def get_temp_path():
    return os.path.join(tempfile.gettempdir(), 'md_preview.html')

def render_html(md_content):
    html_body = markdown.markdown(md_content, extensions=['extra', CodeHiliteExtension()])
    style = """
    <style>
    body {
        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", sans-serif;
        margin: 40px;
        line-height: 1.6;
        font-size: 16px;
        background-color: #fff;
        color: #24292e;
    }
    h1, h2, h3 {
        font-weight: bold;
        border-bottom: 1px solid #eaecef;
        padding-bottom: .3em;
        margin-top: 1em;
    }
    code {
        background-color: #f6f8fa;
        padding: .2em .4em;
        margin: 0;
        font-size: 85%;
        border-radius: 6px;
    }
    pre {
        background-color: #f6f8fa;
        padding: 1em;
        overflow: auto;
        border-radius: 6px;
    }
    pre code {
        background: none;
        font-size: 100%;
    }
    </style>
    """
    return f"<html><head><meta charset='utf-8'>{style}</head><body>{html_body}</body></html>"

def open_in_browser(html_path):
    webbrowser.open('file://' + html_path)

def main():
    if len(sys.argv) < 2:
        print("📖 用法: python mdreader.py <markdown文件路径>")
        return 1

    md_path = sys.argv[1]
    if not os.path.exists(md_path):
        print("❌ 文件不存在:", md_path)
        return 1

    md_content = read_file_utf8(md_path)
    if not md_content:
        return 1

    full_html = render_html(md_content)
    html_path = get_temp_path()

    with open(html_path, 'w', encoding='utf-8') as f:
        f.write(full_html)

    print(f"✅ 已生成预览文件：{html_path}")
    open_in_browser(html_path)

if __name__ == "__main__":
    main()
