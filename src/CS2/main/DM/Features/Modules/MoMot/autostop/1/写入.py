import os

# 获取脚本所在目录的数字名
current_dir = os.path.basename(os.path.abspath(os.path.dirname(__file__)))

# 构建四行配置
lines = [
    f'alias rec_autostop_F_mode "exec DearMoments/src/CS2/main/DM/Features/Modules/MoMot/autostop/{current_dir}/F.cfg"\n',
    f'alias rec_autostop_B_mode "exec DearMoments/src/CS2/main/DM/Features/Modules/MoMot/autostop/{current_dir}/B.cfg"\n',
    f'alias rec_autostop_R_mode "exec DearMoments/src/CS2/main/DM/Features/Modules/MoMot/autostop/{current_dir}/R.cfg"\n',
    f'alias rec_autostop_L_mode "exec DearMoments/src/CS2/main/DM/Features/Modules/MoMot/autostop/{current_dir}/L.cfg"\n',
    '\n'  # 一个空行
]

cfg_filename = "_init_.cfg"

# 读取旧内容
if os.path.exists(cfg_filename):
    with open(cfg_filename, "r", encoding="utf-8") as f:
        old_content = f.read()
else:
    old_content = ""

# 写入新内容在文件开头
with open(cfg_filename, "w", encoding="utf-8") as f:
    f.writelines(lines)
    f.write(old_content)
