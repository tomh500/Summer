import tkinter as Tkinter
from tkinter import filedialog as fD, messagebox as mB
import os
import threading as t

try:
    import ffmpeg
except ImportError:
    import pip
    pip.main(['install', 'ffmpeg-python'])
    import ffmpeg

import subprocess
import json

# 用于获取音频时长（单位：秒）
def get_duration(filename):
    try:
        result = subprocess.run(
            ["ffprobe", "-v", "error", "-show_entries",
             "format=duration", "-of", "json", filename],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            universal_newlines=True
        )
        json_result = json.loads(result.stdout)
        return float(json_result["format"]["duration"])
    except Exception as e:
        mB.showerror("Error", f"获取音频时长失败: {e}")
        return 0

# 选择音频文件
def a1():
    path = fD.askopenfilename(filetypes=[("Audio Files", "*.mp3;*.wav;*.flac")])
    if path:
        file_path.set(path)
        duration = get_duration(path)
        audio_duration.set(duration)
        start_scale.config(to=duration)
        end_scale.config(to=duration)
        end_scale.set(duration)
        update_range_label()

# 音量转换 + 截取
def a2(infile, outfile, volume, start, end):
    try:
        stream = ffmpeg.input(infile, ss=start, to=end)
        stream = ffmpeg.output(stream, outfile, filter=f'volume={volume / 100.0}')
        ffmpeg.run(stream)
        os.remove(infile)
        os.rename(outfile, infile)
        mB.showinfo("Success", f"转换完成，截取：{start:.2f}s ~ {end:.2f}s")
    except Exception as e:
        mB.showerror("Error", f"处理失败: {e}")

def b2():
    infile = file_path.get()
    if not infile:
        mB.showwarning("Warning", "请先选择音频文件")
        return
    outfile = infile + ".tmp"
    volume = volume_scale.get()
    start = start_scale.get()
    end = end_scale.get()
    if start >= end:
        mB.showwarning("Warning", "开始时间必须小于结束时间")
        return
    t.Thread(target=a2, args=(infile, outfile, volume, start, end)).start()

def update_range_label(*args):
    start = start_scale.get()
    end = end_scale.get()
    range_label.config(text=f"截取范围：{start:.2f}s ～ {end:.2f}s")

# 创建主窗口
root = Tkinter.Tk()
root.title("音频音量转换器（含截取功能）")
root.geometry("500x400")

file_path = Tkinter.StringVar()
audio_duration = Tkinter.DoubleVar()

# 文件选择按钮
Tkinter.Button(root, text="选择音频文件", command=a1).pack(pady=8)
Tkinter.Label(root, textvariable=file_path, wraplength=450).pack()

# 音量调节
volume_scale = Tkinter.Scale(root, from_=0, to=200, orient="horizontal", label="音量 (%)")
volume_scale.set(100)
volume_scale.pack(pady=10)

# 截取起始点滑块
start_scale = Tkinter.Scale(root, from_=0, to=100, orient="horizontal", resolution=0.1, label="开始时间 (秒)")
start_scale.pack()

end_scale = Tkinter.Scale(root, from_=0, to=100, orient="horizontal", resolution=0.1, label="结束时间 (秒)")
end_scale.pack()

# 显示截取范围
range_label = Tkinter.Label(root, text="截取范围：0.00s ～ 0.00s")
range_label.pack(pady=5)
start_scale.config(command=update_range_label)
end_scale.config(command=update_range_label)

# 转换按钮
Tkinter.Button(root, text="开始转换", command=b2).pack(pady=20)

root.mainloop()
