import os

def num_to_letter(n):
    return chr(ord('a') + (n - 10))  # 10 -> 'a', 11 -> 'b', ...

# 处理 sq_6ms_10.cfg 到 sq_6ms_20.cfg
for i in range(10, 21):
    filename = f"sq_6ms_{i}.cfg"
    if not os.path.exists(filename):
        print(f"❌ 跳过：{filename}（文件不存在）")
        continue

    try:
        with open(filename, 'r', encoding='utf-8') as f:
            content = f.read()
    except UnicodeDecodeError:
        print(f"⚠️ 无法读取文件编码：{filename}")
        continue

    lines = content.splitlines()  # 不保留换行符

    if len(lines) < 3:
        print(f"⚠️ 文件格式错误（少于3行）：{filename}")
        continue

    letter_current = num_to_letter(i)
    letter_prev = num_to_letter(i - 1)

    # 替换第二行和第三行
    lines[1] = f"alias sq_6ms!{letter_prev}"
    lines[2] = f"alias sq_6ms!{letter_current}  sq_6ms"

    # 替换全文中所有的 sq_6ms!<i> 为 sq_6ms!<letter_current>
    new_lines = [line.replace(f"sq_6ms!{i}", f"sq_6ms!{letter_current}") for line in lines]

    # 写入文件（使用 LF 换行）
    with open(filename, 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(new_lines) + '\n')

    print(f"✔ 已处理：{filename}")
