import os
import re

# 当前目录
root_dir = os.getcwd()

# 匹配模式
pattern = re.compile(
    r'alias (halfstop_(forward|back|left|right)_1) "([+-]?\w+ -2 0 0;alias \w+_start \w+_2)"'
)

# 遍历子目录
for dirpath, dirnames, filenames in os.walk(root_dir):
    if "_init_.cfg" in filenames:
        cfg_path = os.path.join(dirpath, "_init_.cfg")
        print(f"[INFO] Processing: {cfg_path}")

        with open(cfg_path, "r", encoding="utf-8") as f:
            content = f.read()

        # 替换
        def repl(match):
            alias_name = match.group(1)
            rest = match.group(3)
            # 如果已经加过 +smartpack_pack，就不要重复加
            if rest.startswith("+smartpack_pack;"):
                return match.group(0)
            return f'alias {alias_name} "+smartpack_pack;{rest}"'

        new_content = pattern.sub(repl, content)

        # 写回文件
        with open(cfg_path, "w", encoding="utf-8") as f:
            f.write(new_content)

        print(f"[INFO] Updated: {cfg_path}")
