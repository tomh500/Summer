import os

# 复原替换规则（反向）
restore_map = {
    'alias autostop_forward_5 "alias autostop_forward_start autostop_forward_6"':
        'alias autostop_forward_5 "CatchVelStop_F_Pkg;"',
    'alias autostop_back_5 "alias autostop_back_start autostop_back_6"':
        'alias autostop_back_5 "CatchVelStop_B_Pkg;"',
    'alias autostop_right_5 "alias autostop_right_start autostop_right_6"':
        'alias autostop_right_5 "CatchVelStop_R_Pkg;"',
    'alias autostop_left_5 "alias autostop_left_start autostop_left_6"':
        'alias autostop_left_5 "CatchVelStop_L_Pkg;"',

    'alias autostop_forward_3 "CatchVelStop_F_Pkg;"':
        'alias autostop_forward_3 "alias autostop_forward_start autostop_forward_4"',
    'alias autostop_back_3 "CatchVelStop_B_Pkg;"':
        'alias autostop_back_3 "alias autostop_back_start autostop_back_4"',
    'alias autostop_right_3 "CatchVelStop_R_Pkg;"':
        'alias autostop_right_3 "alias autostop_right_start autostop_right_4"',
    'alias autostop_left_3 "CatchVelStop_L_Pkg;"':
        'alias autostop_left_3 "alias autostop_left_start autostop_left_4"'
}

# 遍历并还原
for root, dirs, files in os.walk("."):
    for file in files:
        if file.endswith(".cfg"):
            path = os.path.join(root, file)
            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()
            original_content = content

            for old, new in restore_map.items():
                content = content.replace(old, new)

            if content != original_content:
                with open(path, "w", encoding="utf-8") as f:
                    f.write(content)
                print(f"复原文件: {path}")
