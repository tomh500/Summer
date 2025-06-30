 # 🎮 DearMoments-X 2.1 使用说明

 ## ❓ 提问须知  
 在群里提问之前，请确保你已经完整阅读以下文件：  
 - 📄 本教程  
 - 🔧 UserKeyBinds  
 - ⚙️ UserSetting  
 - 🔢 UserValue  
 - 🎮 UserKZMode  
 - 🎵 config.json  
 - 🍎 Custom.yml
 - 🎯 道具轮盘站位图.exe  

 并且请声明你已完整阅读，提问时附带截图或视频，确保 CFG 开启 Debug 模式，否则你的提问将被忽略。

 ---

 ## 🆕 本次更新日志  
 - [+] ```单发武器自动化```更名为```虚拟撞火枪托```
 - [+] ```双键急停```更名为```半自动急停（双键）```
 - [+] ```单键急停```更名为```全自动急停```
 - [+] 2.1预览版的全部更新功能
 - [+] 混合移动api发送
 - [+] 新增了3个移动api
 - [+] 更改项目结构，```移动模式```更名为```移动风格```，独立于移动api
 - [+] 回滚```虚拟撞火枪托```代码，要求锁定到低帧
 - [+] 遵循OpenCFG及AsulProject精神，允许用户移除```AntiHorizon```模块的注册
 - 🐞 修复：开启自动连跳会锁帧的问题 
 - 🐞 修复：AsulProject配置问题

 ---

 ## 🐧 关于 Linux 用户  
 2.1 开发版加入跨平台支持（测试），安装器文件为 `CFG_Installer_Linux`  
 ⚠️ 注意：Linux 路径区分大小写，部分模块将不可用  
 遇到问题请到 Discord 频道反馈并附带相关信息  

 ---

 ## 🕹️ 关于半条命与 CS1.6  
 - 用户空间路径：`DearMoments/Userspace/HL/Userspace.cfg`  
 - 启动项设置：`+exec DearMoments/Setup_HL.cfg`  
 ⚠️ 此为 CS1.6 使用方式，CS2 用户请继续阅读下文  

 ---

 ## 🔫 GSI-击杀音效替换使用方式  
 请阅读：`Userspace/CS2/gsi/config.json`

 ---

 ## 💾 安装方式  
 1. 将项目放入 CS2 的 CFG 文件夹中，文件夹名为 `DearMoments`  
 2. 确保复制的是整个项目文件夹而非内部文件  
 3. 运行 `CFG监听器主程序.bat`  
 4. 点击左上角 → 文件 → 更新并安装 CFG  
 5. 选择构建计时器，并在用户空间配置锁帧  
 6. 配置完成后点击打开用户空间并保存  

 ⚠️ 游玩时请勿关闭监听器程序

 启动项示例：  
 ```bash
 +exec DearMoments/setup
 ```
 或由安装器自动添加到 `autoexec`（注意只能加载一次）

 ---

 ## ⚙️ 更改偏好设置  
 编辑 `Userspace` 文件夹内的以下配置文件：  
 - UserKeyBinds  
 - UserSetting  
 - UserValue  
 - UserKZMode  

 ---

 ## 📝 用户空间命令列表  
 | 命令                      | 功能描述                 |  
 |---------------------------|--------------------------|  
 | `/fps_recovery`            | 解锁 FPS 上限            |  
 | `/fps_low`                 | 锁定 FPS 到 64 帧        |  
 | `/reload_userspace`        | 重置用户空间             |  
 | `/reload_command`          | 重新注册命令             |  
 | `/crosshair_color_recovery`| 准星颜色设为默认         |  
 | `/crosshair_color_recoil`  | 准星颜色随后座力         |  
 | `/s_1` ～ `/s_5`           | 快捷灵敏度（最低~最高）  |  
 | `/RapidFire`               | 虚拟撞火枪托开关             |  
 | `/sma`                    | 自动身法菜单             |  
 | `/music`                  | 音乐菜单                 |  
 | `/autostop_all`           | 自动急停开关             |  
 | `/KZmode`                 | 进入 KZ 模式             |  
 | `/reload`                 | 重载 CFG                 |  

 ---

 ## 🗑️ 如何卸载 CFG  
 1. 删除 `autoexec.cfg` 中的 `exec DearMoments/setup`  
 2. 删除游戏启动项中的 `+exec DearMoments/setup`  
 3. 控制台输入 `binddefaults` 重置绑定  

 ---

 ## ❓ 常见问题解答

 ### ⚠️ 游戏卡死或自动退出  
 请勿删除正版验证文件 `"免费发布.lty"`  
 请运行监听器主程序 `.bat`  

 ### 🔄 视角无法转动  
 请运行 `CFG监听器主程序.bat` 并打开购买界面，或重启游戏  

 ### ⏳ 为什么锁帧  
 锁帧设置在 `UserValue.cfg`，帧率不得超过 1009，确保正确构建 ticker  

 ### ❌ 是否兼容 Horizon  
 不兼容  

 ### 🕒 游玩一段时间后失效  
 请检查锁帧设置不超过 1009，建议数小时后重启游戏  

 ---

 ## 📜 使用范围与授权说明  
 - 仅限个人使用  
 - 非官方版本须免费、开源  
 - 团队不对衍生产品提供支持  

 ---

 ## 💰 关于倒卖  
 - 可自由倒卖，只需保留著作权  
 - 是否被骂不负责  
 - 欢迎尝试破解正版验证  

 ---

 ## 🎁 关于赞助与定制  
 - 使用免费  
 - 可自愿赞助  
 - 定制请私信 B站 Luotiany1_Mar 或 Discord，含商业条款  

 ---

 ## ❓ Q&A

 **Q：是外挂吗？**  
 A：不是，仅使用 CS2 官方指令，纯 CFG 项目  

 **Q：为什么项目里有 .exe？**  
 A：用于安装/卸载/验证，不操作游戏进程，安全无毒  

 **Q：为什么有 dll？**  
 A：用于安装模块调用，与 CS 游戏无关  

 **Q：第三方社区服说 CFG 会封号？**  
 A：仅适用于特定社区服，非 Valve 官方。建议咨询管理员  

 **Q：如何使用自动身法？**  
 A：需要使用 lcfg 编译 lua 文件  

 ---

 ## 🛠️ 自定义 CFG 方法  

 1. 打开 `DearMoments` 文件夹 → `Userspace` 文件夹  
 2. 编辑以下四个配置文件：  
    - UserKeyBinds  
    - UserKZMode  
    - UserSetting  
    - UserValue  

 ### 🔑 设置按键示例（UserKeyBinds）  
 ```cfg
 bind w +DM_bind_forward     前进  
 bind s +DM_bind_back        后退  
 bind a +DM_bind_left        左移  
 bind d +DM_bind_right       右移  
 ```  
 > 若出现 `bind key +DM_bind_radialradio`，请将 `key` 替换为实际按键  

 ### 🖥️ 设置输出方式示例（UserSetting）  
 ```cfg
 DearMoments_TextOutput_CW        
 DearMoments_TextOutput_Team      
 DearMoments_TextOutput_All       
 DearMoments_TextOutput_Echo      
 DearMoments_TextOutput_Null      
 DearMoments_TextOutput_CW
 ```  
 取消想要的输出方式注释并保存  

 ### 🎯 设置灵敏度（UserValue）  
 ```cfg
 alias my_sensitivity  sensitivity   1            视角灵敏度  
 alias fire_sensitivity sensitivity 0.834        开火灵敏度  
 alias my_yaw m_yaw 0.022                       yaw 值，一般为 0.022  
 ```  
 > 使用项目内计算器计算旋转值即可  

 ---

 **最后修订 by Luotiany1_Mar at Jul 1th 2025**  
