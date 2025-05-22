import os

def get_conversion_type():
    while True:
        choice = input("选择转换类型 (lf2crlf / crlf2lf): ").strip().lower()
        if choice in ["lf2crlf", "crlf2lf"]:
            return choice
        print("无效输入，请输入 lf2crlf 或 crlf2lf")

def get_directory():
    while True:
        path = input("请输入目标目录路径: ").strip()
        if os.path.isdir(path):
            return os.path.abspath(path)
        print("无效目录，请重新输入")

def get_extension():
    ext = input("请输入要转换的文件扩展名（例如 .txt）: ").strip()
    if not ext.startswith('.'):
        ext = '.' + ext
    return ext.lower()

def convert_file(path, mode):
    try:
        with open(path, 'rb') as f:
            content = f.read()

        if mode == "lf2crlf":
            new_content = content.replace(b'\r\n', b'\n').replace(b'\n', b'\r\n')
        else:  # crlf2lf
            new_content = content.replace(b'\r\n', b'\n')

        with open(path, 'wb') as f:
            f.write(new_content)

        print(f"转换成功: {path}")
    except Exception as e:
        print(f"转换失败: {path}，错误: {e}")

def main():
    conversion = get_conversion_type()
    directory = get_directory()
    extension = get_extension()

    print(f"\n开始在 {directory} 中将 *{extension} 文件执行 {conversion} 转换...\n")

    for root, _, files in os.walk(directory):
        for filename in files:
            if filename.lower().endswith(extension):
                convert_file(os.path.join(root, filename), conversion)

    print("\n全部处理完成。")

if __name__ == "__main__":
    main()
