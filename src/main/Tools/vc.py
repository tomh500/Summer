import os

def convert_crlf_to_lf(file_path):
    with open(file_path, 'rb') as f:
        content = f.read()
    # 只在存在 CRLF 的情况下转换
    if b'\r\n' in content:
        content = content.replace(b'\r\n', b'\n')
        with open(file_path, 'wb') as f:
            f.write(content)
        print(f"Converted: {file_path}")
    else:
        print(f"Already LF: {file_path}")

def process_directory(root_dir):
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.lower().endswith('.cfg'):
                file_path = os.path.join(dirpath, filename)
                convert_crlf_to_lf(file_path)

if __name__ == "__main__":
    root_directory = os.getcwd()  # 当前目录，也可以换成其他路径
    process_directory(root_directory)

