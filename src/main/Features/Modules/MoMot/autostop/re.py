import os

# 替换映射（反向）
replace_map = {
    "-forward": "forward -1009 0 0",
    "-back": "back -1009 0 0",
    "-left": "left -1009 0 0",
    "-right": "right -1009 0 0"
}

def restore_commands_in_halfstop_lines(base_path):
    for item in os.listdir(base_path):
        item_path = os.path.join(base_path, item)

        # 只处理当前目录下的子文件夹
        if os.path.isdir(item_path):
            for foldername, subfolders, filenames in os.walk(item_path):
                for filename in filenames:
                    file_path = os.path.join(foldername, filename)
                    try:
                        with open(file_path, 'r', encoding='utf-8') as f:
                            lines = f.readlines()

                        modified = False
                        new_lines = []
                        for line in lines:
                            if "halfstop" in line:
                                original_line = line
                                for short, full in replace_map.items():
                                    line = line.replace(short, full)
                                if line != original_line:
                                    modified = True
                            new_lines.append(line)

                        if modified:
                            with open(file_path, 'w', encoding='utf-8') as f:
                                f.writelines(new_lines)
                            print(f"Restored in: {file_path}")
                    except Exception as e:
                        print(f"Failed to process {file_path}: {e}")

if __name__ == "__main__":
    current_dir = os.path.dirname(os.path.abspath(__file__))
    restore_commands_in_halfstop_lines(current_dir)
