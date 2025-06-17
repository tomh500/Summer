import sys
import os
import tempfile
import webbrowser
from markdown_it import MarkdownIt

def md_to_html(md_text):
    md = MarkdownIt()
    body = md.render(md_text)
    # 最基础样式，仅支持标题、粗体、代码块等
    minimal_css = """
    <style>
        body { font-family: sans-serif; margin: 40px; line-height: 1.6; }
        code { background: #f0f0f0; padding: 2px 4px; border-radius: 4px; font-family: monospace; }
        pre code { display: block; padding: 10px; overflow-x: auto; }
        h1, h2, h3 { border-bottom: 1px solid #eee; }
    </style>
    """
    return f"<html><head><meta charset='utf-8'>{minimal_css}</head><body>{body}</body></html>"

def main():
    if len(sys.argv) < 2:
        print("Usage: mdread.exe <markdown-file>")
        sys.exit(1)

    md_path = sys.argv[1]
    if not os.path.isfile(md_path):
        print(f"File not found: {md_path}")
        sys.exit(1)

    with open(md_path, "r", encoding="utf-8") as f:
        md_content = f.read()

    html_content = md_to_html(md_content)

    # 写入临时 HTML 文件
    with tempfile.NamedTemporaryFile('w', delete=False, suffix=".html", encoding="utf-8") as tmp:
        tmp.write(html_content)
        tmp_path = tmp.name

    webbrowser.open(f"file://{tmp_path}")

if __name__ == "__main__":
    main()
