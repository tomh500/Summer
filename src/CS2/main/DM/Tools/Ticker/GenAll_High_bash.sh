#!/bin/bash

# 获取当前脚本所在目录
current_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 定义要执行的子目录
subdirs=("NAfps/1ms" "NAfps/4ms" "NAfps/66ms")

if [ -f ./Gen_High.sh ]; then
    chmod +x ./Gen_High.sh
fi


# 循环进入并执行
for dir in "${subdirs[@]}"; do
    echo "正在执行 $dir/Gen_High.sh ..."
    pushd "$current_dir/$dir" > /dev/null

    # 执行对应的 Linux 脚本
    if [ -x ./Gen_Normal.sh ]; then
        ./Gen_Normal.sh
    else
        echo "⚠️ $dir/Gen_High.sh 不存在或没有执行权限"
    fi

    popd > /dev/null
done

echo "✅ 所有脚本执行完毕！"
read -rp "按回车退出..."

