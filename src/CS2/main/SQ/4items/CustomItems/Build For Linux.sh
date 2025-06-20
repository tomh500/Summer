#!/bin/bash

# 设置UTF-8编码
export LANG=C.UTF-8

echo "检查 Python 是否已安装..."
python --version > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "未检测到 Python，请先安装 Python 并配置环境变量。"
    exit 1
fi

echo "检查是否已安装 PyYAML..."
python -c "import yaml" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "未安装 PyYAML，正在安装..."
    pip install pyyaml
else
    echo "已安装 PyYAML"
fi

echo "正在执行 generate.py 脚本..."
python generate.py

# 复制文件到目标目录
echo "请重新运行CFG安装器！以便刷新轮盘文字！"

echo "脚本执行完毕。"
