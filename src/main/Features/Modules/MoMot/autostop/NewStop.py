import os

# 替换规则（原字符串 : 替换后的字符串）
replacements = {
    'alias autostop_forward_5 "alias autostop_forward_start autostop_forward_6"':
        'alias autostop_forward_5 "CatchVelStop_F_Pkg;"',
    'alias autostop_back_5 "alias autostop_back_start autostop_back_6"':
        'alias autostop_back_5 "CatchVelStop_B_Pkg;"',
    'alias autostop_left_5 "alias autostop_left_start autostop_left_6"':
        'alias autostop_left_5 "CatchVelStop_L_Pkg;"',
    'alias autostop_right_5 "alias autostop_right_start autostop_right_6"':
        'alias autostop_right_5 "CatchVelStop_R_Pkg;"'
}

def process_cfg_file(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        original_content = content

        for old, new in replacements.items():
            content = content.replace(old, new)

        if content != original_content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"✔ 已修改: {file_path}")
    except Exception as e:
        print(f"⚠ 无法处理 {file_path}: {e}")

def process_all_cfg_files(root_dir):
    for foldername, subfolders, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.lower().endswith(".cfg"):
                file_path = os.path.join(foldername, filename)
                process_cfg_file(file_path)

if __name__ == "__main__":
    root_directory = os.getcwd()  # 当前工作目录
    process_all_cfg_files(root_directory)
