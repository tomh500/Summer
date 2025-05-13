import os

# 设置根目录
root_dir = '.'  # 可以改成你需要遍历的根目录

# 输出文件对应的关键词
keyword_to_file = {
    'autostop_forward': 'F.cfg',
    'halfstop_forward': 'F.cfg',
    'autostop_back': 'B.cfg',
    'halfstop_back': 'B.cfg',
    'right': 'R.cfg',
    'left': 'L.cfg',
}

# 创建输出文件对象，追加写入
output_files = {filename: open(filename, 'a', encoding='utf-8') for filename in set(keyword_to_file.values())}

try:
    # 遍历目录及子目录
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename == '_init_.cfg':
                file_path = os.path.join(dirpath, filename)
                with open(file_path, 'r', encoding='utf-8') as f:
                    for line in f:
                        for keyword, out_filename in keyword_to_file.items():
                            if keyword in line:
                                output_files[out_filename].write(line)
                                break  # 避免同一行被写入多个文件
finally:
    # 关闭所有输出文件
    for f in output_files.values():
        f.close()

print("处理完成。")
