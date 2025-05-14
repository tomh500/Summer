import os

# 获取当前脚本所在的目录
base_dir = os.path.dirname(os.path.abspath(__file__))

# 映射文件名到要追加的文本
cfg_suffixes = {
    'F.cfg': 'alias autostop_forward_start autostop_forward_last',
    'B.cfg': 'alias autostop_back_start autostop_back_last',
    'R.cfg': 'alias autostop_right_start autostop_right_last',
    'L.cfg': 'alias autostop_left_start autostop_left_last',
}

# 遍历所有子文件夹
for folder_name in os.listdir(base_dir):
    folder_path = os.path.join(base_dir, folder_name)
    if os.path.isdir(folder_path):
        # 遍历每种配置文件
        for cfg_name, line_to_add in cfg_suffixes.items():
            cfg_path = os.path.join(folder_path, cfg_name)
            if os.path.isfile(cfg_path):
                with open(cfg_path, 'a', encoding='utf-8') as f:
                    f.write(f'\n{line_to_add}')
                print(f'已修改: {cfg_path}')
