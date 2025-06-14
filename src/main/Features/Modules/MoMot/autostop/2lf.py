import os

def convert_to_lf(path):
    with open(path, 'rb') as f:
        content = f.read()
    new_content = content.replace(b'\r\n', b'\n')  # CRLF -> LF

    if new_content != content:
        with open(path, 'wb') as f:
            f.write(new_content)
        print(f"已转换为 LF：{path}")

# 遍历目录
for root, dirs, files in os.walk("."):
    for file in files:
        if file.endswith(".cfg"):
            convert_to_lf(os.path.join(root, file))
