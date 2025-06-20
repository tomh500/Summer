# 尝试导入必要的库，如果没有安装，则自动安装
try:
    import tkinter as Tkinter
    from tkinter import filedialog as fD, messagebox as mB
    import ffmpeg as fF
    import os
    import threading as t
except ImportError as e:
    import pip
    missing_lib = str(e).split("'")[1]  # 获取缺少的库名
    print(f"Missing library {missing_lib}. Installing...")
    pip.main(['install', missing_lib])
    import tkinter as Tkinter
    from tkinter import filedialog as fD, messagebox as mB
    import ffmpeg as fF
    import os
    import threading as t

# 选择文件
def a1():
    b1 = fD.askopenfilename(filetypes=[("Audio Files", "*.mp3;*.wav;*.flac")])
    if b1:
        b2.set(b1)  

# 音量转换函数
def a2(d1, d2, d3):
    try:
        fF.input(d1).output(d2, filter='volume={}'.format(d3 / 100.0)).run()
        mB.showinfo("Success", "Audio conversion complete!")
        os.remove(d1)
        os.rename(d2, d1)
    except Exception as e:
        mB.showerror("Error", f"An error occurred: {e}")

# 转换按钮的回调
def b2():
    d1 = b2.get()
    if not d1:
        mB.showwarning("Warning", "Please select an audio file first!")
        return

    d3 = a3.get()

    d2 = d1 + ".tmp"

    t.Thread(target=a2, args=(d1, d2, d3)).start()

# 创建主窗口
root = Tkinter.Tk()
root.title("Audio Volume Converter")

# 文件选择按钮
b2 = Tkinter.StringVar()

b1 = Tkinter.Button(root, text="Select File", command=a1)
b1.pack(pady=10)

# 显示选择的文件名
b3 = Tkinter.Label(root, textvariable=b2, wraplength=400)
b3.pack(pady=10)

# 音量滑块
a3 = Tkinter.Scale(root, from_=0, to=200, orient="horizontal", label="Volume (%)")
a3.set(100)  # 默认音量为100%
a3.pack(pady=20)

# 转换按钮
b4 = Tkinter.Button(root, text="Convert Audio", command=b2)
b4.pack(pady=20)

# 调整窗口尺寸为450x300
root.geometry("450x300")  

# 启动 GUI
root.mainloop()
