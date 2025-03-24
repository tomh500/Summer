//reg userspace
alias MoMot_fb_SOCD "exec Moclient/src/main/Features/Modules/Momot/mode/fb/socd.mo"
alias MoMot_fb_NullBind "exec Moclient/src/main/Features/Modules/Momot/mode/fb/nullbind.mo"
alias MoMot_fb_Normal "exec Moclient/src/main/Features/Modules/Momot/mode/fb/normal.mo"
alias MoMot_rl_SOCD "exec Moclient/src/main/Features/Modules/Momot/mode/rl/socd.mo"
alias MoMot_rl_NullBind "exec Moclient/src/main/Features/Modules/Momot/mode/rl/nullbind.mo"
alias MoMot_rl_Normal "exec Moclient/src/main/Features/Modules/Momot/mode/rl/normal.mo"
alias MoMot_MW_A "exec Moclient/src/main/Features/Modules/Momot/mwheeljump/MW_A.mo"
alias MoMot_MW_B "exec Moclient/src/main/Features/Modules/Momot/mwheeljump/MW_B.mo"
alias MoMot_MW_C "exec Moclient/src/main/Features/Modules/Momot/mwheeljump/MW_C.mo"
alias MoMot_MW_D "exec Moclient/src/main/Features/Modules/Momot/mwheeljump/MW_D.mo"
alias MoClient_CrosshairRecoil_Disable CrosshairRecoil_Off
alias MoClient_CrosshairRecoil_Enable CrosshairRecoil_On

//注册连跳增强 其中带/直接调用用户空间命令
alias MoClient_BetterBhop_Enable "alias betterbhop_activeA /fps_low;alias betterbhop_activeF /fps_recovery"
alias MoClient_BetterBhop_Disable "alias betterbhop_activeA;alias betterbhop_activeF"

//注册自动连跳
alias MoClient_AutoBhop_Enable "alias autobhop_activeA alias sq_6ms_1 fulljump_action;alias autobhop_activeF alias sq_6ms_1"
alias MoClient_AutoBhop_Disable "alias autobhop_activeA ;alias autobhop_activeF"


//reg keybinds
alias +MoClient_bind_forward +Momot_forward
alias +MoClient_bind_back +Momot_back
alias +MoClient_bind_left +Momot_left
alias +MoClient_bind_right +Momot_right
alias -MoClient_bind_forward -Momot_forward
alias -MoClient_bind_back -Momot_back
alias -MoClient_bind_left -Momot_left
alias -MoClient_bind_right -Momot_right
alias +Moclient_bind_spacejump +Momot_space_jump
alias -Moclient_bind_spacejump -Momot_space_jump
//嵌套一层命令 是为了正确处理desubtick等内容

alias fulljump_action "+jump;-jump"//无需使用+- 一次触发jump需要

//滚轮处理
alias MoClient_Mwheelup_Jump "alias +MoClient_bind_mwheelup +MoMot_jump;alias -MoClient_bind_mwheelup -MoMot_jump"
alias MoClient_Mwheeldown_Jump "alias +MoClient_bind_mwheeldown +MoMot_jump;alias -MoClient_bind_mwheeldown -MoMot_jump;"
alias MoClient_Mwhellup_slot "alias +MoClient_bind_mwheelup invprev"
alias MoClient_Mwhelldown_slot "alias +MoClient_bind_mwheeldown  invnext"

alias nomove "forward -1009 0 0;back -1009 0 0;left -1009 0 0;right -1009 0 0" //停止所有移动 

alias MoClient_Reload_define "nomove;/reload_command;/reload_userspace;say_team 已重置MoClient"
alias MoClient_Reload_action "MoClient_Reload_define;alias sq_20"
alias MoClient_Reload "alias sq_20 MoClient_Reload_action"