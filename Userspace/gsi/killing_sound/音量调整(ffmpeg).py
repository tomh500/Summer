import sys
import subprocess

# 自动安装缺失库
def ensure_package(package):
    try:
        __import__(package)
    except ImportError:
        print(f"\n❗ 检测到缺少依赖库：{package}")
        choice = input(f"是否尝试自动为您安装 {package}? [y/N]: ").strip().lower()
        if choice == 'y':
            try:
                subprocess.check_call([sys.executable, "-m", "pip", "install", package])
                print(f"✅ 成功安装 {package}！\n")
            except subprocess.CalledProcessError:
                print(f"❌ 安装 {package} 失败，请手动运行：pip install {package}")
                sys.exit(1)
        else:
            print(f"⚠️ 请先手动运行：pip install {package}")
            sys.exit(1)

# 检查必要的库
ensure_package("prompt_toolkit")

# 正常导入
import os
import glob
import shutil
from prompt_toolkit import prompt
from prompt_toolkit.completion import WordCompleter

# 构建文件补全器
audio_files = [f for f in os.listdir('.') if f.endswith(('.mp3', '.ogg'))]
file_completer = WordCompleter(audio_files, ignore_case=True)

# 输入文件
while True:
    filename = prompt("请输入要调整音量的音频文件（支持tab自动补全）：", completer=file_completer).strip()
    if os.path.isfile(filename) and filename.lower().endswith(('.mp3', '.ogg')):
        break
    print("❌ 文件无效，请重试。")

# 选择操作
print("\n请选择操作：")
print("1. 增大音量")
print("2. 减小音量")

while True:
    choice = input("输入序号 [1/2]: ").strip()
    if choice in ('1', '2'):
        break
    print("❌ 无效选项，请输入 1 或 2。")

# 输入分贝数
while True:
    try:
        db = float(input("请输入要调整的分贝值（如 3.5 或 -2.0）：").strip())
        break
    except ValueError:
        print("❌ 输入的不是有效数字，请重试。")

if choice == '2':
    db = -abs(db)
else:
    db = abs(db)

# 构建输出文件名
name, ext = os.path.splitext(filename)
out_file = "out_conv" + ext

# 构建并执行 ffmpeg 命令
cmd = [
    "ffmpeg",
    "-y",
    "-i", filename,
    "-filter:a", f"volume={db}dB",
    out_file
]

print(f"\n🚀 正在执行: {' '.join(cmd)}\n")
ret = subprocess.call(cmd)

if ret != 0:
    print("❌ ffmpeg 处理失败。")
    sys.exit(1)

# 备份旧文件并替换
lockfile_name = filename + ".Lockfile"
os.rename(filename, lockfile_name)
shutil.move(out_file, filename)

print(f"✅ 音量调整完成！原文件已重命名为 {lockfile_name}，新文件保存为 {filename}")
