import os
import re

# 支持的别名前缀和方向
PREFIXES = ['autostop', 'halfstop']
DIRECTIONS = ['forward', 'back', 'left', 'right']

def process_cfg_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    original_lines = list(lines)
    changed = False

    for prefix in PREFIXES:
        for direction in DIRECTIONS:
            pattern = re.compile(rf'^alias {prefix}_{direction}_(\d+)\s+"(.*)"')
            aliases = []

            for i, line in enumerate(lines):
                match = pattern.match(line.strip())
                if match:
                    number = int(match.group(1))
                    content = match.group(2)
                    aliases.append((i, number, content))

            if not aliases:
                continue

            # 找“最后一行”：必须包含 `alias <prefix>_<direction>_start <prefix>_<direction>_1`
            for i in reversed(range(len(aliases))):
                _, num, content = aliases[i]
                if f'{prefix}_{direction}_start {prefix}_{direction}_1' in content:
                    last_line_index, last_num, last_content = aliases[i]
                    break
            else:
                continue  # 未找到结尾，跳过

            # 插入新跳转行
            new_num = last_num
            next_num = new_num + 1

            new_jump_line = (
                f'alias {prefix}_{direction}_{new_num} '
                f'"alias {prefix}_{direction}_start {prefix}_{direction}_{next_num}"\n'
            )
            new_stop_line = (
                f'alias {prefix}_{direction}_{next_num} "{last_content}"\n'
            )

            # 替换原 stop 行为跳转行，插入新 stop 行
            lines[last_line_index] = new_jump_line
            lines.insert(last_line_index + 1, new_stop_line)

            changed = True
            print(f"{prefix}_{direction}: 添加 {prefix}_{direction}_{next_num} 于 {file_path}")

    if changed and lines != original_lines:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.writelines(lines)

def process_all_cfg_files(folder_path):
    for filename in os.listdir(folder_path):
        if filename.endswith(".cfg"):
            full_path = os.path.join(folder_path, filename)
            process_cfg_file(full_path)

if __name__ == "__main__":
    folder = "."  # 当前目录
    process_all_cfg_files(folder)
