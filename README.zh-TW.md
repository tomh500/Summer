<div align="right">

**繁體中文** | [簡體中文](./README.md) | [English](./github/README.en_US.md)

</div>

<div align="center">
<img src="https://github.com/Yunkong-ouo/CS2Konc_CFG/blob/main/github/image/CS2.jpg" alt="image" width="800">
</div>

<p align="center">
<a href="https://github.com/Yunkong-ouo/Cs2CfgHighlighterExtension">
<img alt="CS2 Visual Studio Code 高亮擴展" src="https://img.shields.io/badge/CS2%20VSCode%20%E9%AB%98%E4%BA%AE%E6%93%B4%E5%B1%95-blue?style=for-the-badge&logo=github">
</a>
<a href="https://github.com/tomh500/SquareConfig_Nextgen">
<img alt="Square下一代" src="https://img.shields.io/badge/Square%20Nextgen%20Config-green?style=for-the-badge&logo=github">
</a>
</p>

## 💼 Square 介紹
**Square_Nextgen** 全新的SquareConfig，本次的革新帶來了超多勁爆的功能，保留了功能模組化的同時，讓玩家可以使用 **更強大的** 的功能，且正在持續更新，超過20+的功能讓你獲取遊戲的自豪感。
**Square_Lite** 本質上是Square_Legacy，是舊時代的遺留產物，通過一次更新，讓它搖身一變，能夠繼續提供服務，玩家可以輕松通過生成器制作一個單檔案的cfg，且所有功能均合法，輕量化，性能++，完全旁路屏幕共享。
在大多數情況喜愛，使用 **Square** 不會導致 **VAC** 封禁。這僅僅是一個CFG項目。官方提供的功能👍
F
## ✨ 特性
- 🔥 配置迅速
- 💤 界面簡潔
- 📦 開箱即用
- 🧹 優雅設計
- 🚀 慢如烏龜

## 📝 功能介紹
| 名字 | 介紹 |
|------------------------- |----------------------------------------|
| 移動方案 | 提供超過3種的移動方案，隨心所欲輔助你急停
| 跳躍等行爲動作 | 提供跳投等行爲綁定，道具扔得更準了
| 空格大跳 | 按下空白鍵自動大跳
| 開關提示音 | 開關提示音，讓你知道你是否開啓
| 準星功能 | 準星跟隨等使用快速綁定
| 滾輪跳 | 提供了多種滾輪跳模式，讓你kz快人一步
| 自動連跳 | 你無需擔心，按住空格鍵可以自動連跳了
| 連跳增強 | 讓你連跳快人一步
| 自動手槍 | 拿起你的手槍，化身馬可波羅吧
## 🔥 特別說明
**！此 CFG 會覆蓋原來的設置，最好備份一下！**

## 🚀 安裝說明
到 Releases 或者 QQ 羣獲取最新版，解壓後在CFG的相對路徑：
```
*\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\cfg

將項目資料夾復制到裏面，並更名爲 Square
確保項目資料夾爲根目錄，然後運行GetCpuInfo.exe並運行OpenConfigSettingFolder.bat 配置 setting.cfg 和 keybinds.cfg
```

## 安裝方式
**方式1**
在 Steam 啓動項中添加：
```
+exec Square/setup
```
---
**方式2**
創建一個名爲 `autoexec.cfg` 的檔案，在 `Counter-Strike Global Offensive\game\csgo\cfg` 裏面，內容應該爲：
```
exec Square/setup
```

## 🗑️ 解除安装CFG
```
bind mouse_x yaw;bind mouse_y pitch;unbindall;binddefaults;binddefaults;joy_response_move 1;joy_side_sensitivity 1.000000;joy_forward_sensitivity 1.000000;cl_scoreboard_mouse_enable_binding +attack2;cl_quickinventory_filename radial_quickinventory.txt;host_writeconfig
```
