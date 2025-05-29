<div align="right">

**简体中文** | [繁体中文](./README.zh-TW.md) | [English](./README.en_US.md)

</div>

<div align="center">
    <img src="https://github.com/Yunkong-ouo/CS2Konc_CFG/blob/main/github/image/CS2.jpg" alt="image" width="800">
</div>

<p align="center">
    <a href="https://github.com/Yunkong-ouo/Cs2CfgHighlighterExtension">
        <img alt="CS2 Visual Studio Code 高亮扩展" src="https://img.shields.io/badge/CS2%20VSCode%20%E9%AB%98%E4%BA%AE%E6%93%B4%E5%B1%95-blue?style=for-the-badge&logo=github">
    </a>
    <a href="https://github.com/tomh500/SquareConfig_Nextgen">
        <img alt="Square下一代" src="https://img.shields.io/badge/Square%20Nextgen%20Config-green?style=for-the-badge&logo=github">
    </a>
</p>


## 💼 Square 介绍
**Square_Nextgen** 全新的SquareConfig，本次的革新带来了超多劲爆的功能，保留了功能模组化的同时，让玩家可以使用 **更强大的** 的功能，且正在持续更新，超过20+的功能让你获取游戏的自豪感。
**Square_Lite** 本质上是Square_Legacy，是旧时代的遗留产物，通过一次更新，让它摇身一变，能够继续提供服务，玩家可以轻松通过生成器制作一个单文件的cfg，且所有功能均合法，轻量化，性能++，完全绕过屏幕共享。
在大多数情况喜爱，使用 **Square** 不会导致 **VAC** 封禁。这仅仅是一个CFG项目。官方提供的功能👍

## ✨ 特性
- 🔥 配置迅速
- 💤 界面简洁
- 📦 开箱即用
- 🧹 优雅设计
- 🚀 慢如乌龟

## 📝 功能介紹
| 名字                      | 介紹                                   |
|------------------------- |----------------------------------------|
| 移动方案                  | 提供超过3种的移动方案，随心所欲辅助你急停
| 跳跃等行为动作            | 提供跳投等行为绑定，道具扔得更准了
| 空格大跳                  | 按下空白鍵自動大跳
| 开关提示音                | 开关提示音，让你知道你是否开启
| 准星功能                  | 准星跟随等使用快速绑定
| 滚轮跳                    | 提供了多种滚轮跳模式，让你kz快人一步
| 自动连跳                  | 你无需担心，按住空格键可以自动连跳了
| 连跳增强                  | 让你连跳快人一步
| 自动手枪                  | 拿起你的手枪，化身马可波罗吧
## 🔥 特别说明
**！此 CFG 会覆盖原来的设置，最好备份一下！**

## 🚀 安裝说明
到 Releases 或者 QQ 群获取最新版，解压后在CFG的相对路径：
```
*\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\cfg

将项目文件夹复制到里面，并更名为 Square
确保项目文件夹为根目录，然后运行GetCpuInfo.exe并运行OpenConfigSettingFolder.bat 配置 setting.cfg 和 keybinds.cfg
```

## 安装方式
**方式1**  
在 Steam 启动项中添加：
```
+exec Square/setup
```
---
**方式2**  
创建一个名为 `autoexec.cfg` 的文件，在 `Counter-Strike Global Offensive\game\csgo\cfg` 里面，内容应该为：
```
exec Square/setup
```

## 🗑️ 卸载CFG
```
bind mouse_x yaw;bind mouse_y pitch;unbindall;binddefaults;binddefaults;joy_response_move 1;joy_side_sensitivity 1.000000;joy_forward_sensitivity 1.000000;cl_scoreboard_mouse_enable_binding +attack2;cl_quickinventory_filename radial_quickinventory.txt;host_writeconfig
```
