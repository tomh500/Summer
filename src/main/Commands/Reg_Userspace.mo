//标记为 / 的是用户空间命令 标记为. 的是内部命令
//对于部分代码内的行为，可能不存在任何标记

//定义默认帧率到540，后续可能出现需要锁帧的情况


//定义需要LOW帧的情况
alias /fps_low fps_max 64

//定义重置用户空间
alias /reload_userspace "exec MoClient/Userspace/UserKeyBinds.cfg;exec MoClient/Userspace/UserSetting.cfg"

alias /reload_command "exec MoClient/src/main/Commands/_init_.mo"

//定义默认准心颜色
alias /crosshair_color_recovery "rec_crosshair_r;rec_crosshair_g;rec_crosshair_b"

//定义跟随准心颜色
alias /crosshair_color_recoil "recoil_crosshair_r;recoil_crosshair_g;recoil_crosshair_b"

alias /s_1 "Superlow_r;Superlow_s;rec_sensitivity"
alias /s_2 "low_r;low_s;rec_sensitivity"
alias /s_3 "mid_r;mid_s;rec_sensitivity"
alias /s_4 "high_r;high_s;rec_sensitivity"
alias /s_5 "SuperHigh_r;Superhigh_s;rec_sensitivity"

alias /RapidFire ".RapidFire_Enable"
alias .RapidFire_Enable "soundtips_1;mout_rapidfire_1;RapidFire_Enable;alias /RapidFire .RapidFire_Disable"
alias .RapidFire_Disable "soundtips_0;mout_rapidfire_0;RapidFire_Disable;alias /RapidFire .RapidFire_Enable"

alias .CrosshairRecoil_On "mout_recoil_1;Soundtips_1;CrosshairRecoil_On"
alias .CrosshairRecoil_Off "mout_recoil_0;Soundtips_0;CrosshairRecoil_Off"