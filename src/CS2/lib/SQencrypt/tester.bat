@echo off
setlocal enabledelayedexpansion

REM 设置要遍历的目录
set "folder=tester"

REM 遍历文件夹下的所有文件
for /R "%folder%" %%f in (*) do (
    echo 正在解密: %%f
    decrypt.exe -p "%%f"
)

echo 所有文件处理完成。
pause
