chcp 65001
@echo off
:loop
cls
echo ==================================================
echo 请选择你要操作的文件：
echo ==================================================
set /p filename="请输入文件名（包括路径）："

if not exist "%filename%" (
    echo 文件 %filename% 不存在，请检查文件路径。
    pause
    goto loop
)

:choose_action
echo ==================================================
echo 请选择操作：
echo 1. 为文件加上隐藏和系统属性
echo 2. 去除文件的隐藏和系统属性
echo 3. 退出程序
echo ==================================================
set /p action="请输入你的选择（1/2/3）："

if "%action%"=="1" (
    attrib +h +s "%filename%"
    echo 文件 "%filename%" 已经被标记为隐藏和系统文件。
)

if "%action%"=="2" (
    attrib -h -s "%filename%"
    echo 文件 "%filename%" 已经去除了隐藏和系统文件属性。
)

if "%action%"=="3" (
    echo 退出程序。
    exit /b
)

if "%action%" neq "1" if "%action%" neq "2" if "%action%" neq "3" (
    echo 无效的选择，请重新选择。
)

pause
goto loop
