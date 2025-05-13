import os

# 要替换的字符串映射表
replace_map = {
    "forward -1009 0 0": "forward -2 0 0",
    "back -1009 0 0": "back -2 0 0",
    "left -1009 0 0": "left -2 0 0",
    "right -1009 0 0": "right -2 0 0"
}

def process_subfolders(base_path):
    for item in os.listdir(base_path):
        item_path = os.path.join(base_path, item)

        # 只处理当前目录下的文件夹（不包括当前目录下的文件）
        if os.path.isdir(item_path):
            for foldername, subfolders, filenames in os.walk(item_path):
                for filename in filenames:
                    file_path = os.path.join(foldername, filename)
                    try:
                        with open(file_path, 'r', encoding='utf-8') as f:
                            content = f.read()

                        new_content = content
                        for old, new in replace_map.items():
                            new_content = new_content.replace(old, new)

                        if new_content != content:
                            with open(file_path, 'w', encoding='utf-8') as f:
                                f.write(new_content)
                            print(f"Updated: {file_path}")
                    except Exception as e:
                        print(f"Failed to process {file_path}: {e}")

if __name__ == "__main__":
    current_dir = os.path.dirname(os.path.abspath(__file__))
    process_subfolders(current_dir)
