import os
import subprocess

def ask_conversion_direction():
    print("请选择转换方向：")
    print("1. ogg → wav")
    print("2. wav → ogg")
    choice = input("请输入数字（1或2）：").strip()
    if choice == '1':
        return '.ogg', '.wav'
    elif choice == '2':
        return '.wav', '.ogg'
    else:
        print("无效选择，程序退出。")
        exit(1)

def convert_audio(src_ext, dst_ext):
    for root, _, files in os.walk('.'):
        for file in files:
            if file.lower().endswith(src_ext):
                src_path = os.path.join(root, file)
                dst_file = file[:-len(src_ext)] + dst_ext
                dst_path = os.path.join(root, dst_file)

                if os.path.exists(dst_path):
                    print(f"[跳过] 目标已存在: {dst_path}")
                    continue

                try:
                    subprocess.run([
                        'ffmpeg', '-y',
                        '-i', src_path,
                        dst_path
                    ], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                    print(f"[完成] {src_path} → {dst_path}")
                except subprocess.CalledProcessError:
                    print(f"[错误] 转换失败: {src_path}")

if __name__ == "__main__":
    src_ext, dst_ext = ask_conversion_direction()
    convert_audio(src_ext, dst_ext)
