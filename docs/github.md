<div align="right">

**简体中文** 

## 💼 Autumn 介绍
因为Valve禁止了exec_async，导致许多CFG无法使用，**Autumn**继承了**Summer**的部分特性，继续重新开发

## ✨ 特性
- 🔥 配置迅速
- 💤 界面简洁
- 📦 开箱即用
- 🧹 优雅设计
- 🚀 快如闪电

## 🔥 特别说明
**！此 CFG 会覆盖原来的设置，最好备份一下！**

## 🚀 安裝说明
到 Releases 或者 QQ 群获取最新版，解压后在CFG的相对路径：
```
*\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\cfg

将项目文件夹复制到里面，并更名为 Autumn
确保项目文件夹为根目录，然后在Userspace文件夹下更改UserKeyBinds、UserSetting、UserValue
```

## 安装方式
**方式1**  
在 Steam 启动项中添加：
```
+exec Autumn/setup
```
---
**方式2**  
创建一个名为 `autoexec.cfg` 的文件，在 `Counter-Strike Global Offensive\game\csgo\cfg` 里面，内容应该为：
```
exec Autumn/setup
```

## 🗑️ 卸载CFG
```
bind mouse_x yaw;bind mouse_y pitch;unbindall;binddefaults;binddefaults;null_response_move 1;null_side_sensitivity 1.000000;null_forward_sensitivity 1.000000;cl_scoreboard_mouse_enable_binding +attack2;cl_quickinventory_filename radial_quickinventory.txt;host_writeconfig
```

## 常见问题
  Q.为什么打开游戏闪退/卡死<br>
  A.请确保Autumn文件夹下存在```免费发布.lty```

  Q.为什么玩久了无法在游戏内操作<br>
  A.重启游戏就好了，这是所有CFG无法避免的问题
