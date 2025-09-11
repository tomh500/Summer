 # 🌌 CS2 RTSS 配置文件使用说明


 **📂 配置文件路径**：  
 `<你的安装目录>\setting\RTSS_mortis.yml`
 
 > ⚠️ 注意： RTSS_Mortis控制器已经集成到Autumn客户端，**当且仅当你正确安装了RTSS且正常运行本产品才有效**
 
 ---
 
 ## 🗂 文件结构总览
 
 `RTSS_mortis.yml` 是 YAML 格式的文本文件，主要包含：
 
 1. 🔧 **全局设置**  
 2. 🎛 **多个 Profile，每个 Profile 可以设置多个按键绑定**
 
 ---
 
 ### 1. 🔧 全局设置
 
 ```yaml
 ReadConsole: true
 ```
 
 - **ReadConsole** ⚡
   - 类型：`bool`（true/false）  
   - 功能：控制是否开启 **CS2 控制台读取** 👀  
   - 注意事项：开启此开关会增加 **高频硬盘 IO** 🛠️  
   - 需要添加启动项：`-condebug` 才会生效 🔑  
   - 使用场景：在 CS2 控制台输入：
     ```text
     echoln /fps_set <value>
     ```
     可以让 RTSS **锁帧到指定帧数** 🎯  
     - `<value>` = 0 → 解除锁帧 ❌  
     - 其他整数 → 锁到指定帧数 ⚡
   - 建议：默认 `true`
 
 ---
 
 ### 2. 🎛 Profile 配置
 
 每个 Profile 是一个独立的键值，包含 `binds` 列表。例如：
 
 ```yaml
 Profile1:
   binds:
     - key: "Space"
       fps: 64
       hold: true
     - key: "F"
       fps: 32
       hold: false
 
 Profile2:
   binds:
     - key: "Q"
       fps: 128
       hold: true
 ```
 
 每个 bind 的含义：
 - **key** ⌨️：按键名称（字符串）  
 - **fps** 🎮：锁帧值（整数）  
 - **hold** 🔒：是否按住按键  
   - `true` → 按键会一直按住  
   - `false` → 只触发一次  
 
 ---
 
 ### ⚡ 使用示例
 
 1. 打开 `rtss_config.yml` 文件（文本编辑器，如 VSCode、记事本）  
 2. 修改 `ReadConsole` 为 `true` 或 `false`  
 3. 增加或修改 Profile 的 `binds`  
 4. 保存文件，程序会自动读取新的配置  
 5. 在 CS2 控制台使用 `echoln /fps_set <value>` 即可生效 🎉
 
 ---
 
 **💡 注意事项**：
 - 按键名称必须有效，否则会被忽略 ⚠️  
 - `fps` 建议设置为常用帧率，如 32、64、128 ⏱️  
 - `hold` 设置为 true 时，按键会一直按住，否则只触发一次 🔄  
 - 开启 ReadConsole 会增加硬盘 IO 🌀  
 - 可使用在线编辑器：[RTSS Editor](https:tomh500.github.io/Square/RTSS_Editor.html) ✨
 
 ---
 
