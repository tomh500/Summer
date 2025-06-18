@echo off
chcp 65001 >nul
echo 检查 Python 是否已安装...
python --version >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo 未检测到 Python，请先安装 Python 并配置环境变量。
    pause
    exit /b
)

echo 检查是否已安装 PyYAML...
python -c "import yaml" 2>nul
IF %ERRORLEVEL% NEQ 0 (
    echo 未安装 PyYAML，正在安装...
    pip install pyyaml
) ELSE (
    echo 已安装 PyYAML
)

echo 正在执行 generate.py 脚本...
python generate.py
xcopy /Y /Q "..\..\..\..\resources\keybindings_schinese.txt" "..\..\..\..\..\..\..\resource\"
xcopy /Y /Q "..\..\..\..\resources\keybindings_english.txt" "..\..\..\..\..\..\..\resource\"

echo 脚本执行完毕。
pause
