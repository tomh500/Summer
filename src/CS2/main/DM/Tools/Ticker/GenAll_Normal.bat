@echo off
setlocal

:: 获取当前批处理文件所在的路径
set "current_dir=%~dp0"

:: 运行 NAfps\1ms\Gen_Normal.bat
pushd "%current_dir%NAfps\1ms"
call Gen_Normal.bat
popd

:: 运行 NAfps\4ms\Gen_Normal.bat
pushd "%current_dir%NAfps\4ms"
call Gen_Normal.bat
popd

:: 运行 NAfps\66ms\Gen_Normal.bat
pushd "%current_dir%NAfps\66ms"
call Gen_Normal.bat
popd

echo 所有批处理文件已执行完毕！
pause
