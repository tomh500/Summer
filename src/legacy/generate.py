import yaml
import os
import shutil
import re

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

# 有效值定义
VALID_MAPS = {"dust2", "inferno", "mirage", "ancient", "nuke", "anubis", "overpass", "vertigo"}
VALID_TYPES = {"smoke", "molo", "flash", "grenade", "decoy"}
VALID_THROWMODES = {"Normal", "Jump", "ForwardJump"}

TYPE_SLOT_MAP = {
    "grenade": "slot6",
    "flash": "slot7",
    "smoke": "slot8",
    "decoy": "slot9",
    "molo": "slot10"
}

def get_throwmode_exec(mode):
    if mode in {"Normal", "Jump"}:
        return "exec DearMoments/src/legacy/4items/tools/itemthrow;"
    elif mode == "ForwardJump":
        return "+forward;exec DearMoments/src/legacy/4items/tools/itemthrow;"
    else:
        raise ValueError(f"Unknown throwmode: {mode}")

def get_throwmode_cleanup(mode):
    return "-forward;rec_sensitivity;alias sq_19" if mode == "ForwardJump" else "rec_sensitivity;alias sq_19"

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

with open("Custom.yml", "r", encoding="utf-8") as f:
    data = yaml.load(f, Loader=UniqueKeyLoader) or {}


output_root = "4items"
overlay_path = os.path.join(output_root, "Overlay.cfg")
actions_root = os.path.join(output_root, "actions")
ui_root = os.path.join(output_root, "UI")
os.makedirs(output_root, exist_ok=True)

# 如果 YML 是空的
if not data:
    # 1. 清空 Overlay.cfg
    with open(overlay_path, "w", encoding="utf-8") as f:
        f.write("")

    # 2. 清空所有已存在 UI 的 custom define 段
    if os.path.exists(ui_root):
        for map_dir in os.listdir(ui_root):
            full_map_path = os.path.join(ui_root, map_dir)
            if os.path.isdir(full_map_path):
                for file in ["InterfaceCMD.cfg", "InterfaceTEXT.cfg"]:
                    full_path = os.path.join(full_map_path, file)
                    if os.path.exists(full_path):
                        # 清空 custom define 段
                        with open(full_path, "r", encoding="utf-8") as f:
                            content = f.read()
                        content = re.sub(r'//custom define[\s\S]*', '', content, flags=re.MULTILINE)
                        with open(full_path, "w", encoding="utf-8") as f:
                            f.write(content.strip() + "\n")

    # 3. 拷贝并覆盖默认 keybindings 文件
    script_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(script_dir)
    resource_dir = os.path.join(parent_dir, "resources")

    os.makedirs(resource_dir, exist_ok=True)
    shutil.copyfile("4items/base/base_keybindings_schinese.txt", os.path.join(resource_dir, "keybindings_schinese.txt"))
    shutil.copyfile("4items/base/base_keybindings_english.txt", os.path.join(resource_dir, "keybindings_english.txt"))

    exit(0)  # 提前退出脚本


overlay_lines = []
bind_lines = []
display_mappings = []

# map -> list of interface cmd/text lines
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
    setpos_values = props.get("setpos", [{"x": ""}, {"y": ""}])
    item_type = props["type"]

    select = props.get("select", [])
    select_dict = {k: v for d in select for k, v in d.items()} if isinstance(select, list) else select
    bind_key = select_dict.get("bind")
    command_name = select_dict.get("command")
    page = select_dict.get("page")
    slot = select_dict.get("slot")


    if command_name:
        if not isinstance(command_name, str):
            raise ValueError(f"{item_id} 的 command 值不是字符串")
        if not command_name.startswith("/"):
            raise ValueError(f"{item_id} 的 select.command 字段必须以 '/' 开头：当前值为 '{command_name}'")

    if map_name not in VALID_MAPS:
        raise ValueError(f"Invalid map: {map_name}")
    if item_type not in VALID_TYPES:
        raise ValueError(f"Invalid type: {item_type}")
    if throwmode not in VALID_THROWMODES:
        raise ValueError(f"Invalid throwmode: {throwmode}")
    setpos_dict = {k: v for d in setpos_values for k, v in d.items()}
    x = setpos_dict.get("x", "")
    y = setpos_dict.get("y", "")
    z = setpos_dict.get("z", "")

    if bool(x) + bool(y) + bool(z) not in (0, 3):
        raise ValueError(f"Setpos 必须全部填写或全部为空，item_id: {item_id}")


    filepath = f"DearMoments/src/legacy/4items/actions/{map_name}/{filename}"
    slot_cmd = TYPE_SLOT_MAP[item_type]
    throw_exec = get_throwmode_exec(throwmode)
    throw_cleanup = get_throwmode_cleanup(throwmode)

    # === Overlay additions ===
    overlay_lines.extend([
        f'alias {item_id_l} "items_{item_id_l};alias sq_14"',
        f'alias items_{item_id_l}_set "alias -LSquare_bind_items {item_id_l}"',
        f'alias items_{item_id_l} "sensitivity {sensitivity};exec {filepath} "\n'
    ])
    if command_name:
        overlay_lines.append(f"alias {command_name} items_{item_id_l}_set")
    if bind_key and bind_key.lower() != "none":
        bind_lines.append(f'bind {bind_key} items_{item_id_l}_set')

    # === Action config ===
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

    # === UI definitions ===
    if command_name and page and slot and page != "" and slot != "":
        try:
            page_index = abs(int(page) - 3)
            slot_index = int(slot) + 2
        except ValueError:
            raise ValueError(f"Invalid page or slot format for item {item_id}")

        cmd_line = f'cl_radial_radio_tab_{page_index}_text_{slot_index} cmd";items_{item_id_l}_set;'
        text_line = f'cl_radial_radio_tab_{page_index}_text_{slot_index} #{map_name.upper()}_{item_id.upper()};'


        if map_name not in interface_cmd_lines:
            interface_cmd_lines[map_name] = []
            interface_text_lines[map_name] = []

        interface_cmd_lines[map_name].append(cmd_line)
        interface_text_lines[map_name].append(text_line)

# === Save Overlay.cfg ===
with open(overlay_path, "w", encoding="utf-8") as f:
    f.write("\n".join(overlay_lines + bind_lines))

# === Save UI files ===
for map_name in interface_cmd_lines:
    map_ui_dir = os.path.join(ui_root, map_name)
    os.makedirs(map_ui_dir, exist_ok=True)

    cmd_path = os.path.join(map_ui_dir, "InterfaceCMD.cfg")
    text_path = os.path.join(map_ui_dir, "InterfaceTEXT.cfg")

    clear_and_insert_custom_define(cmd_path, interface_cmd_lines[map_name])
    clear_and_insert_custom_define(text_path, interface_text_lines[map_name])

# === Update Keybinding TXT ===
def update_keybinding_file(base_path, output_path, mappings):
    with open(base_path, "r", encoding="utf-8") as f:
        content = f.read()

    content = re.sub(r'\n?"注释" "以下内容是自定义道具"\n?(?:".*?" ".*?"\n?)*', '', content, flags=re.MULTILINE)

    insert_lines = ['"注释" "以下内容是自定义道具"']
    for map_upper, item_id_upper, dispname in mappings:
        insert_lines.append(f'"{map_upper}_{item_id_upper}" "{dispname}"')

    content = content.strip()
    if content.endswith("}"):
        content = content[:-1].rstrip() + "\n" + "\n".join(insert_lines) + "\n}"

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(content)

script_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(script_dir)
resource_dir = os.path.join(parent_dir, "resources")
os.makedirs(resource_dir, exist_ok=True)
output_schinese = os.path.join(resource_dir, "keybindings_schinese.txt")
output_english = os.path.join(resource_dir, "keybindings_english.txt")

update_keybinding_file("4items/base/base_keybindings_schinese.txt", output_schinese, display_mappings)
update_keybinding_file("4items/base/base_keybindings_english.txt", output_english, display_mappings)
