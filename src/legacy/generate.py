import yaml
import os

# 道具类型映射为 slot 编号
TYPE_SLOT_MAP = {
    "grenade": "slot6",
    "flash": "slot7",
    "smoke": "slot8",
    "decoy": "slot9",
    "molo": "slot10"
}

# 投掷方式映射
def get_throwmode_exec(mode):
    if mode == "Normal":
        return "exec DearMoments/src/legacy/4items/tools/itemthrow_withoutjump;"
    elif mode == "Jump":
        return "exec DearMoments/src/legacy/4items/tools/itemthrow;"
    elif mode == "ForwardJump":
        return "+forward;exec DearMoments/src/legacy/4items/tools/itemthrow;"
    else:
        raise ValueError(f"Unknown throwmode: {mode}")

def get_throwmode_cleanup(mode):
    return "-forward;rec_sensitivity;alias sq_19" if mode == "ForwardJump" else "rec_sensitivity;alias sq_19"

# 读取 Custom.yml
with open("Custom.yml", "r", encoding="utf-8") as f:
    data = yaml.safe_load(f)

# 设置生成目录
output_root = "4items"
overlay_path = os.path.join(output_root, "Overlay.cfg")
actions_root = os.path.join(output_root, "actions")

os.makedirs(output_root, exist_ok=True)

overlay_lines = []
bind_lines = []

# 遍历所有道具项
for item_id, props in data.items():
    item_id_l = item_id.lower()
    map_name = props["map"].lower()
    filename = props["filename"]
    sensitivity = props["sensitivity"]
    filepath = f"DearMoments/src/legacy/4items/actions/{map_name}/{filename}"

    # 添加到 Overlay.cfg 的 alias 部分
    overlay_lines.extend([
        f'alias {item_id_l} "items_{item_id_l};alias sq_14"',
        f'alias items_{item_id_l}_set "alias -LSquare_bind_items {item_id_l}"',
        f'alias items_{item_id_l} "sensitivity {sensitivity};exec {filepath} "\n'
    ])

    # 添加 bind 指令
    bind_key = props.get("bind")
    if bind_key:
        bind_lines.append(f'bind {bind_key} items_{item_id_l}_set')

    # 获取 slot 和投掷指令
    slot = TYPE_SLOT_MAP.get(props["type"])
    if not slot:
        raise ValueError(f"Unsupported type: {props['type']}")

    throw_exec = get_throwmode_exec(props["throwmode"])
    throw_cleanup = get_throwmode_cleanup(props["throwmode"])

    # 生成动作文件
    map_dir = os.path.join(actions_root, map_name)
    os.makedirs(map_dir, exist_ok=True)
    cfg_path = os.path.join(map_dir, filename)

    with open(cfg_path, "w", encoding="utf-8") as f:
        f.write(
f"""yaw {props["yaw"]} 1 1
pitch {props["pitch"]} 1 1
{slot}
alias +Square_action_itemthrow "{throw_exec}alias sq_19"
alias -Square_action_itemthrow "{throw_cleanup}"
alias +Square_bind_itemsthrow "alias sq_19 +Square_action_itemthrow"
alias -Square_bind_itemsthrow "alias sq_19 -Square_action_itemthrow"
""")

# 写入 Overlay.cfg 文件
with open(overlay_path, "w", encoding="utf-8") as f:
    f.write("\n".join(overlay_lines + bind_lines))
