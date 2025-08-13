import yaml
import os
import shutil
import re
from pathlib import Path

class UniqueKeyLoader(yaml.SafeLoader):
    pass

def construct_mapping(loader, node, deep=False):
    mapping = {}
    for key_node, value_node in node.value:
        key = loader.construct_object(key_node, deep=deep)
        if key in mapping:
            raise ValueError(f"重复的道具唯一标识符（item_id）: '{key}'，请检查 custom.yml")
        value = loader.construct_object(value_node, deep=deep)
        mapping[key] = value
    return mapping

UniqueKeyLoader.add_constructor(
    yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
    construct_mapping
)

script_dir = Path(__file__).resolve().parent
config_path = script_dir / "custom.yml"

execute_dir = script_dir
output_root = script_dir.parent
actions_root = output_root / "actions"
overlay_path = output_root / "Overlay.cfg"
ui_root = output_root / "UI"

with open(config_path, "r", encoding="utf-8") as f:
    raw_data = yaml.load(f, Loader=UniqueKeyLoader) or {}

data = {item_id: {k.lower(): v for k, v in props.items()} for item_id, props in raw_data.items()}

VALID_MAPS = {m.lower() for m in ("dust2", "inferno", "mirage", "ancient", "nuke", "anubis", "overpass", "vertigo")}
VALID_TYPES = {t.lower() for t in ("smoke", "molo", "flash", "grenade", "decoy")}
VALID_THROWMODES = {m.lower() for m in ("normal", "jump", "forwardjump", "custom")}
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
        return "exec Summer/src/CS2/main/SQ/4items/tools/itemthrow;"
    elif mode == "normal":
        return "exec Summer/src/CS2/main/SQ/4items/tools/itemthrow_withoutjump;"
    elif mode == "forwardjump":
        return "+forward;exec Summer/src/CS2/main/SQ/4items/tools/itemthrow;"
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
    if path.exists():
        content = path.read_text(encoding="utf-8")
        content = re.sub(r'//custom define[\s\S]*', '', content, flags=re.MULTILINE)
    else:
        content = ""
    new_content = content.strip() + "\n\n//custom define\n" + "\n".join(lines) + "\n"
    path.write_text(new_content, encoding="utf-8")
    

if not data:
    overlay_path.write_text("", encoding="utf-8")
    if ui_root.exists():
        for map_dir in ui_root.iterdir():
            if map_dir.is_dir():
                for fname in ["InterfaceCMD.cfg", "InterfaceTEXT.cfg"]:
                    fpath = map_dir / fname
                    if fpath.exists():
                        content = fpath.read_text(encoding="utf-8")
                        content = re.sub(r'//custom define[\s\S]*', '', content, flags=re.MULTILINE)
                        fpath.write_text(content.strip() + "\n", encoding="utf-8")
    resource_dir = execute_dir.parent.parent.parent.parent / "resources"
    resource_dir.mkdir(exist_ok=True)
    shutil.copy(output_root / "base/base_keybindings_schinese.txt", resource_dir / "keybindings_schinese.txt")
    shutil.copy(output_root / "base/base_keybindings_english.txt", resource_dir / "keybindings_english.txt")
    shutil.copy(output_root / "base/base_keybindings_russian.txt", resource_dir / "keybindings_russian.txt")
    shutil.copy(output_root / "base/base_keybindings_japanese.txt", resource_dir / "keybindings_japanese.txt")
    exit(0)

overlay_lines = []
bind_lines = []
display_mappings = []
interface_cmd_lines = {}
interface_text_lines = {}

def normalize_to_dict(raw, key_name):
    if isinstance(raw, dict):
        return {k.lower(): v for k, v in raw.items()}
    if isinstance(raw, list):
        out = {}
        for entry in raw:
            if isinstance(entry, dict):
                out.update({k.lower(): v for k, v in entry.items()})
            else:
                raise ValueError(f"{key_name} 列表项不是字典：{entry}")
        return out
    raise ValueError(f"{key_name} 字段类型不支持：{type(raw)}")

for item_id, props in data.items():
    item_l = item_id.lower()
    map_l = str(props.get("map", "")).strip().lower()
    filename = props.get("filename", "")
    sensitivity = props.get("sensitivity", "")
    displayname = props.get("displayname", "")
    yaw = props.get("yaw", "")
    pitch = props.get("pitch", "")
    throwmode = str(props.get("throwmode", "")).strip().lower()
    extra = props.get("extra", ["", ""])
    item_type = str(props.get("type", "")).strip().lower()

    select_dict = normalize_to_dict(props.get("select", {}), "select")
    setpos_dict = normalize_to_dict(props.get("setpos", {}), "setpos")

    bind_key = str(select_dict.get("bind", "")).strip().lower()
    command_name = str(select_dict.get("command", "")).strip()
    page = str(select_dict.get("page", "")).strip()
    slot = str(select_dict.get("slot", "")).strip()

    x = setpos_dict.get("x", "")
    y = setpos_dict.get("y", "")
    z = setpos_dict.get("z", "")
    if bool(x) + bool(y) + bool(z) not in (0, 3):
        raise ValueError(f"Setpos 必须全部填写或全部为空，item_id: {item_id}")

    if map_l not in VALID_MAPS:
        raise ValueError(f"Invalid map: {map_l}")
    if item_type not in VALID_TYPES:
        raise ValueError(f"Invalid type: {item_type}")
    if throwmode not in VALID_THROWMODES:
        raise ValueError(f"Invalid throwmode: {throwmode}")

    filepath = f"Summer/src/CS2/main/SQ/4items/actions/{map_l}/{filename}"

    overlay_lines.extend([
        f'alias {item_l} "items_{item_l};alias sq_14"',
        f'alias items_{item_l}_set "alias -LSquare_bind_items {item_l}"',
        f'alias items_{item_l} "sensitivity {sensitivity};exec {filepath}"'
    ])
    if command_name:
        overlay_lines.append(f"alias {command_name} items_{item_l}_set")
    if bind_key and bind_key.lower() != "none":
        bind_lines.append(f'bind {bind_key} items_{item_l}_set')

    map_dir = actions_root / map_l
    map_dir.mkdir(parents=True, exist_ok=True)
    item_lines = []
    if x and y and z:
        item_lines.append(f"setpos {x} {z} {y}")
    item_lines.extend([
        f"yaw {yaw} 1 1",
        f"pitch {pitch} 1 1",
        TYPE_SLOT_MAP[item_type],
        f'alias +Square_action_itemthrow "{extra[0]};{get_throwmode_exec(throwmode)}alias sq_19"',
        f'alias -Square_action_itemthrow "{extra[1]};{get_throwmode_cleanup(throwmode)}"',
        'alias +Square_bind_itemsthrow "alias sq_19 +Square_action_itemthrow"',
        'alias -Square_bind_itemsthrow "alias sq_19 -Square_action_itemthrow"',
    ])
    (map_dir / filename).write_text("\n".join(item_lines), encoding="utf-8")

    display_mappings.append((map_l.upper(), item_l.upper(), displayname))

if page or slot:

    # 如果 page 和 slot 都填了，再写入 UI 圆盘文件
    if page and slot:
        page_idx = abs(int(page) - 3)
        slot_idx = int(slot) + 2
        cmd_line = f'cl_radial_radio_tab_{page_idx}_text_{slot_idx} cmd";items_{item_l}_set;'
        text_line = f'cl_radial_radio_tab_{page_idx}_text_{slot_idx} #{map_l.upper()}_{item_l.upper()};'
        interface_cmd_lines.setdefault(map_l, []).append(cmd_line)
        interface_text_lines.setdefault(map_l, []).append(text_line)


overlay_path.write_text("\n".join(overlay_lines + bind_lines), encoding="utf-8")

for m, cmds in interface_cmd_lines.items():
    out_dir = ui_root / m
    out_dir.mkdir(parents=True, exist_ok=True)
    clear_and_insert_custom_define(out_dir / "InterfaceCMD.cfg", cmds)
    clear_and_insert_custom_define(out_dir / "InterfaceTEXT.cfg", interface_text_lines[m])

def update_keybinding_file(base_path, output_path, mappings):
    content = Path(base_path).read_text(encoding="utf-8")
    content = re.sub(
        r'\n?"注释" "以下内容是自定义道具"\n?(?:".*?" ".*?"\n?)*',
        '', content, flags=re.MULTILINE
    )
    insert = ['"注释" "以下内容是自定义道具"'] + [f'"{mp}_{ip}" "{dp}"' for mp, ip, dp in mappings]
    content = content.strip()
    if content.endswith("}"):
        content = content[:-1].rstrip() + "\n" + "\n".join(insert) + "\n}"
    Path(output_path).write_text(content, encoding="utf-8")

resources_dir = execute_dir.parent.parent.parent.parent / "resources"
resources_dir.mkdir(exist_ok=True)
update_keybinding_file(output_root / "base/base_keybindings_schinese.txt", resources_dir / "keybindings_schinese.txt", display_mappings)
update_keybinding_file(output_root / "base/base_keybindings_english.txt", resources_dir / "keybindings_english.txt", display_mappings)
update_keybinding_file(output_root / "base/base_keybindings_russian.txt", resources_dir / "keybindings_russian.txt", display_mappings)
update_keybinding_file(output_root / "base/base_keybindings_japanese.txt", resources_dir / "keybindings_japanese.txt", display_mappings)