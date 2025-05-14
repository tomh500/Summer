import os

# 要注释掉的关键词列表
keywords = [
    "//autostop_right_last_delay",
    "//autostop_left_last_delay",
    "//autostop_forward_last_delay",
    "//autostop_back_last_delay"
]

# 获取当前脚本所在目录
base_dir = os.path.dirname(os.path.abspath(__file__))

# 遍历所有子文件夹及文件
for root, dirs, files in os.walk(base_dir):
    for filename in files:
        file_path = os.path.join(root, filename)
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                lines = f.readlines()
        except (UnicodeDecodeError, PermissionError):
            continue  # 跳过非文本文件或无权限文件

        changed = False
        new_lines = []
        for line in lines:
            stripped = line.lstrip()
            for keyword in keywords:
                if keyword in stripped and not stripped.startswith("//"):
                    # 只对未被注释的目标行添加注释
                    line = line.replace(keyword, f"//{keyword}")
                    changed = True
            new_lines.append(line)

        if changed:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.writelines(new_lines)
            print(f"Updated: {file_path}")
