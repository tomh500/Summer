import os
import subprocess

def set_file_attributes(filepath):
    try:
        # 加上 +s +h 属性
        subprocess.check_call(['attrib', '-s', '-h', filepath])
        print(f"属性已设置: {filepath}")
    except subprocess.CalledProcessError as e:
        print(f"设置失败: {filepath} -> {e}")

def main():
    if not os.path.exists('release.txt'):
        print("错误: release.txt 文件未找到")
        return

    with open('release.txt', 'r', encoding='utf-8') as f:
        lines = f.readlines()

    for line in lines:
        line = line.strip().strip('",')
        if not line:
            continue

        # 检查文件是否存在
        if os.path.exists(line):
            set_file_attributes(line)
        else:
            print(f"文件不存在: {line}")

if __name__ == '__main__':
    main()
