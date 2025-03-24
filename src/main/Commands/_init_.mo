//标记为 / 的是用户空间命令 标记为. 的是内部命令
//对于部分代码内的行为，可能不存在任何标记

//定义默认帧率到540，后续可能出现需要锁帧的情况
alias /fps_recovery fps_max 540

//定义需要LOW帧的情况
alias /fps_low fps_max 64

//定义重置用户空间
alias /reload_userspace "exec MoClient/Userspace/UserKeyBinds.cfg;exec MoClient/Userspace/UserSetting.cfg"

alias /reload_command "exec Moclient/src/main/Commands/_init_.mo"

//定义默认准心颜色
alias /crosshair_color_recovery "rec_crosshair_r;rec_crosshair_g;rec_crosshair_b"

//定义跟随准心颜色
alias /crosshair_color_recoil "recoil_crosshair_r;recoil_crosshair_g;recoil_crosshair_b"

alias /s_1 "Superlow_s;rec_sensitivity"
alias /s_2 "low_s;rec_sensitivity"
alias /s_3 "mid_s;rec_sensitivity"
alias /s_4 "high_s;rec_sensitivity"
alias /s_5 "Superhigh_s;rec_sensitivity"

//注册内部代码，对标Sqaure的根目录_init_
exec MoClient/src/main/Commands/Reg_Code.mo


