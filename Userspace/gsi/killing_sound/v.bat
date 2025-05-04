@echo off
setlocal enabledelayedexpansion

:: 遍历 1.mp3 到 5.mp3 文件
for %%i in (1 2 3 4 5) do (
    :: 转换文件，使用 ffmpeg 将 input.mp3 转换为 temp.mp3（128k 比特率）
    ffmpeg -i %%i.mp3 -b:a 128k temp.mp3

    :: 删除原始的 MP3 文件
    del /f %%i.mp3

    :: 将转换后的 temp.mp3 重命名为原始文件名
    ren temp.mp3 %%i.mp3
)

echo 转换完成！
pause
