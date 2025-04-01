//reg modulesregister
alias Register_Momot Momot_Need_Load
alias Momot_Need_Load exec MoClient/src/main/Features/Modules/Momot/_init_.mo
alias MoClient_Autostop_pass "alias Momot_Need_Load"
//reg userspace
alias MoMot_fb_SOCD "exec MoClient/src/main/Features/Modules/Momot/mode/fb/socd.mo"
alias MoMot_fb_NullBind "exec MoClient/src/main/Features/Modules/Momot/mode/fb/nullbind.mo"
alias MoMot_fb_Normal "exec MoClient/src/main/Features/Modules/Momot/mode/fb/normal.mo"
alias MoMot_rl_SOCD "exec MoClient/src/main/Features/Modules/Momot/mode/rl/socd.mo"
alias MoMot_rl_NullBind "exec MoClient/src/main/Features/Modules/Momot/mode/rl/nullbind.mo"
alias MoMot_rl_Normal "exec MoClient/src/main/Features/Modules/Momot/mode/rl/normal.mo"
alias MoMot_MW_A "alias default_mwmode exec MoClient/src/main/Features/Modules/Momot/mwheeljump/MW_A.mo;exec MoClient/src/main/Features/Modules/Momot/mwheeljump/MW_A.mo"
alias MoMot_MW_B "alias default_mwmode exec MoClient/src/main/Features/Modules/Momot/mwheeljump/MW_B.mo;exec MoClient/src/main/Features/Modules/Momot/mwheeljump/MW_B.mo"
alias MoMot_MW_C "alias default_mwmode exec MoClient/src/main/Features/Modules/Momot/mwheeljump/MW_C.mo;exec MoClient/src/main/Features/Modules/Momot/mwheeljump/MW_C.mo"
alias MoMot_MW_D "exec MoClient/src/main/Features/Modules/Momot/mwheeljump/MW_D.mo"
alias MoClient_CrosshairRecoil_Disable CrosshairRecoil_Off
alias MoClient_CrosshairRecoil_Enable CrosshairRecoil_On
//alias MoClient_Rapidfire_Enable "RapidFire_Enable"
//alias MoClient_Rapidfire_Disable "RapidFire_Disable"

alias +MoClient_attack2 "+attack2"
alias -MoClient_attack2 "-attack2;spec_prev"

alias MoClient_Autostop_pass ""
alias MoClient_Autostop_sideload "exec MoClient/src/jiting/setup"


//注册连跳增强 其中带/直接调用用户空间命令
alias betterbhop_Adefine "/fps_low;exec MoClient/src/main/Features/Modules/Momot/mwheeljump/MW_C.mo"
alias betterbhop_Fdefine "/fps_recovery;default_mwmode"
alias MoClient_BetterBhop_Enable "alias betterbhop_activeA betterbhop_Adefine;alias betterbhop_activeF betterbhop_Fdefine "
alias MoClient_BetterBhop_Disable "alias betterbhop_activeA;alias betterbhop_activeF"

//注册自动连跳
alias MoClient_AutoBhop_Enable "alias autobhop_activeA alias sq_6ms_1 autobhop_activeAA;alias autobhop_activeF autobhop_activeFA"
alias MoClient_AutoBhop_Disable "alias autobhop_activeA ;alias autobhop_activeF"


//注册是否开启声音提示
alias MoClient_SoundTips_Enable "alias soundtips_1 .soundtips_1;alias soundtips_0 .soundtips_0;"
alias MoClient_SoundTips_Disable "alias soundtips_1;alias soundtips_0;"

alias +smartattack "+attack;"
alias -smartattack "-attack;spec_next"

//注册输出系统
alias MoClient_TextOutput_Team "exec MoClient/src/main/Features/OutputSystem/TextTips/Team.mo
alias MoClient_TextOutput_All "exec MoClient/src/main/Features/OutputSystem/TextTips/All.mo
alias MoClient_TextOutput_CW "exec MoClient/src/main/Features/OutputSystem/TextTips/CW.mo
alias MoClient_TextOutput_Echo "exec MoClient/src/main/Features/OutputSystem/TextTips/Echo.mo
alias MoClient_TextOutput_Null "exec MoClient/src/main/Features/OutputSystem/TextTips/Null.mo



//reg keybinds


//滚轮处理
alias MoClient_Mwheelup_Jump "alias +MoClient_bind_mwheelup +MoMot_jump;alias -MoClient_bind_mwheelup -MoMot_jump"
alias MoClient_Mwheeldown_Jump "alias +MoClient_bind_mwheeldown +MoMot_jump;alias -MoClient_bind_mwheeldown -MoMot_jump;"
alias MoClient_Mwhellup_slot "alias +MoClient_bind_mwheelup invprev"
alias MoClient_Mwhelldown_slot "alias +MoClient_bind_mwheeldown  invnext"


//reload
alias MoClient_Reload_define "exec MoClient/src/main/Tools/Actions/Reload.mo"
alias MoClient_Reload_action "MoClient_Reload_define;alias sq_20"
alias MoClient_Reload "alias sq_20 MoClient_Reload_action"

alias MoClient_User "log_flags Console +DoNotEcho;log_flags IME +DoNotEcho;log_flags InputService +DoNotEcho"
alias MoClient_Debug "log_flags Console -DoNotEcho;log_flags IME -DoNotEcho;log_flags InputService -DoNotEcho"