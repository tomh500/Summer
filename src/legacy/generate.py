import yaml
import os
import shutil
import re
import json
from pathlib import Path

class UniqueKeyLoader(yaml.SafeLoader):
    pass

def construct_mapping(loader, node, deep=False):
    mapping = {}
    for key_node, value_node in node.value:
        key = loader.construct_object(key_node, deep=deep)
        if key in mapping:
            raise ValueError(f"重复的道具唯一标识符（item_id）: '{key}'，请检查 Custom.yml")
        value = loader.construct_object(value_node, deep=deep)
        mapping[key] = value
    return mapping

UniqueKeyLoader.add_constructor(
    yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
    construct_mapping
)

def normalize_to_dict(raw, key_name):
    if isinstance(raw, dict):
        # convert keys to lowercase for case-insensitive access
        return {k.lower(): v for k, v in raw.items()}
    elif isinstance(raw, list):
        out = {}
        for entry in raw:
            if isinstance(entry, dict):
                out.update({k.lower(): v for k, v in entry.items()})
            else:
                raise ValueError(f"{key_name} 列表项不是字典：{entry}")
        return out
    else:
        raise ValueError(f"{key_name} 字段类型不支持：{type(raw)}")

VALID_MAPS = {m.lower() for m in ("dust2", "inferno", "mirage", "ancient", "nuke", "anubis", "overpass", "vertigo")}
VALID_TYPES = {t.lower() for t in ("smoke", "molo", "flash", "grenade", "decoy")}
VALID_THROWMODES = {m.lower() for m in ("Normal", "Jump", "ForwardJump", "Custom")}

TYPE_SLOT_MAP = {
    "grenade": "slot6",
    "flash": "slot7",
    "smoke": "slot8",
    "decoy": "slot9",
    "molo": "slot10"
}

def get_throwmode_exec(mode):
    mode = mode.lower()
    if mode == "jump":
        return "exec DearMoments/src/legacy/4items/tools/itemthrow;"
    elif mode == "normal":
        return "exec DearMoments/src/legacy/4items/tools/itemthrow_withoutjump;"
    elif mode == "forwardjump":
        return "+forward;exec DearMoments/src/legacy/4items/tools/itemthrow;"
    elif mode == "custom":
        return ""
    else:
        raise ValueError(f"Unknown throwmode: {mode}")

def get_throwmode_cleanup(mode):
    mode = mode.lower()
    if mode == "forwardjump":
        return "-forward;rec_sensitivity;alias sq_19"
    return "rec_sensitivity;alias sq_19"

def clear_and_insert_custom_define(path, lines):
    if os.path.exists(path):
        with open(path, "r", encoding="utf-8") as f:
            content = f.read()
        content = re.sub(r'//custom define[\s\S]*', '', content, flags=re.MULTILINE)
    else:
        content = ""

    content = content.strip() + "\n\n//custom define\n" + "\n".join(lines) + "\n"
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)

# 加载并转换为键全小写
with open("Custom.yml", "r", encoding="utf-8") as f:
    raw_data = yaml.load(f, Loader=UniqueKeyLoader) or {}
# 将外层 props 键也转换为小写
data = {item_id: {k.lower(): v for k, v in props.items()} for item_id, props in raw_data.items()}

output_root = "4items"
overlay_path = os.path.join(output_root, "Overlay.cfg")
actions_root = os.path.join(output_root, "actions")
ui_root = os.path.join(output_root, "UI")
os.makedirs(output_root, exist_ok=True)

if not data:
    with open(overlay_path, "w", encoding="utf-8") as f:
        f.write("")

    if os.path.exists(ui_root):
        for map_dir in os.listdir(ui_root):
            full_map_path = os.path.join(ui_root, map_dir)
            if os.path.isdir(full_map_path):
                for file in ["InterfaceCMD.cfg", "InterfaceTEXT.cfg"]:
                    full_path = os.path.join(full_map_path, file)
                    if os.path.exists(full_path):
                        with open(full_path, "r", encoding="utf-8") as f:
                            content = f.read()
                        content = re.sub(r'//custom define[\s\S]*', '', content, flags=re.MULTILINE)
                        with open(full_path, "w", encoding="utf-8") as f:
                            f.write(content.strip() + "\n")

    script_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(script_dir)
    resource_dir = os.path.join(parent_dir, "resources")
    os.makedirs(resource_dir, exist_ok=True)
    shutil.copyfile("4items/base/base_keybindings_schinese.txt", os.path.join(resource_dir, "keybindings_schinese.txt"))
    shutil.copyfile("4items/base/base_keybindings_english.txt", os.path.join(resource_dir, "keybindings_english.txt"))
    exit(0)

overlay_lines = []
bind_lines = []
display_mappings = []
interface_cmd_lines = {}
interface_text_lines = {}

for item_id, props in data.items():
    item_id_l = item_id.lower()
    map_name = props["map"].lower()
    filename = props["filename"]
    sensitivity = props["sensitivity"]
    displayname = props["displayname"]
    yaw = props["yaw"]
    pitch = props["pitch"]
    throwmode = props["throwmode"]
    extra = props.get("extra", ["", ""])
    item_type = props["type"]

    # 规范 select 和 setpos
    select_dict = normalize_to_dict(props.get("select", {}), "select")
    setpos_dict = normalize_to_dict(props.get("setpos", {}), "setpos")

    bind_key = select_dict.get("bind")
    command_name = select_dict.get("command")
    if command_name and not command_name.startswith('/'):
        raise ValueError(f"command 字段必须以 '/' 开头：{command_name}")
    page = select_dict.get("page")
    slot = select_dict.get("slot")

    x = setpos_dict.get("x", "")
    y = setpos_dict.get("y", "")
    z = setpos_dict.get("z", "")
    if bool(x) + bool(y) + bool(z) not in (0, 3):
        raise ValueError(f"Setpos 必须全部填写或全部为空，item_id: {item_id}")

    if map_name not in VALID_MAPS:
        raise ValueError(f"Invalid map: {map_name}")
    if item_type not in VALID_TYPES:
        raise ValueError(f"Invalid type: {item_type}")
    if throwmode not in VALID_THROWMODES:
        raise ValueError(f"Invalid throwmode: {throwmode}")

    filepath = f"DearMoments/src/legacy/4items/actions/{map_name}/{filename}"
    slot_cmd = TYPE_SLOT_MAP[item_type]
    throw_exec = get_throwmode_exec(throwmode)
    throw_cleanup = get_throwmode_cleanup(throwmode)

    overlay_lines.extend([
        f'alias {item_id_l} "items_{item_id_l};alias sq_14"',
        f'alias items_{item_id_l}_set "alias -LSquare_bind_items {item_id_l}"',
        f'alias items_{item_id_l} "sensitivity {sensitivity};exec {filepath} "\n'
    ])
    if command_name:
        overlay_lines.append(f"alias {command_name} items_{item_id_l}_set")
    if bind_key and bind_key.lower() != "none":
        bind_lines.append(f'bind {bind_key} items_{item_id_l}_set')

    map_dir = os.path.join(actions_root, map_name)
    os.makedirs(map_dir, exist_ok=True)
    cfg_path = os.path.join(map_dir, filename)

    item_lines = []
    if x and y and z:
        item_lines.append(f"setpos {x} {z} {y}")
    item_lines.extend([
        f"yaw {yaw} 1 1",
        f"pitch {pitch} 1 1",
        f"{slot_cmd}",
        f'alias +Square_action_itemthrow "{extra[0]};{throw_exec}alias sq_19"',
        f'alias -Square_action_itemthrow "{extra[1]};{throw_cleanup}"',
        'alias +Square_bind_itemsthrow "alias sq_19 +Square_action_itemthrow"',
        'alias -Square_bind_itemsthrow "alias sq_19 -Square_action_itemthrow"',
    ])
    with open(cfg_path, "w", encoding="utf-8") as f:
        f.write("\n".join(item_lines))

    display_mappings.append((map_name.upper(), item_id.upper(), displayname))

    if command_name and page and slot:
        try:
            page_index = abs(int(page) - 3)
            slot_index = int(slot) + 2
        except ValueError:
            raise ValueError(f"Invalid page or slot for item {item_id}")

        cmd_line = f'cl_radial_radio_tab_{page_index}_text_{slot_index} cmd";items_{item_id_l}_set;'
        text_line = f'cl_radial_radio_tab_{page_index}_text_{slot_index} #{map_name.upper()}_{item_id.upper()};'

        interface_cmd_lines.setdefault(map_name, []).append(cmd_line)
        interface_text_lines.setdefault(map_name, []).append(text_line)

with open(overlay_path, "w", encoding="utf-8") as f:
    f.write("\n".join(overlay_lines + bind_lines))

for map_name, cmd_list in interface_cmd_lines.items():
    map_ui_dir = os.path.join(ui_root, map_name)
    os.makedirs(map_ui_dir, exist_ok=True)

    cmd_path = os.path.join(map_ui_dir, "InterfaceCMD.cfg")
    text_path = os.path.join(map_ui_dir, "InterfaceTEXT.cfg")

    clear_and_insert_custom_define(cmd_path, cmd_list)
    clear_and_insert_custom_define(text_path, interface_text_lines[map_name])

def update_keybinding_file(base_path, output_path, mappings):
    with open(base_path, "r", encoding="utf-8") as f:
        content = f.read()

    content = re.sub(
        r'\n?"注释" "以下内容是自定义道具"\n?(?:".*?" ".*?"\n?)*',
        '', content, flags=re.MULTILINE
    )

    insert_lines = ['"注释" "以下内容是自定义道具"']
    for map_up, item_up, disp in mappings:
        insert_lines.append(f'"{map_up}_{item_up}" "{disp}"')

    content = content.strip()
    if content.endswith("}"):
        content = content[:-1].rstrip() + "\n" + "\n".join(insert_lines) + "\n}"

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(content)

script_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(script_dir)
resource_dir = os.path.join(parent_dir, "resources")
os.makedirs(resource_dir, exist_ok=True)

update_keybinding_file("4items/base/base_keybindings_schinese.txt", os.path.join(resource_dir, "keybindings_schinese.txt"), display_mappings)
update_keybinding_file("4items/base/base_keybindings_english.txt", os.path.join(resource_dir, "keybindings_english.txt"), display_mappings)