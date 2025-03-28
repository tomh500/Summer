//reg modulesregister
alias Register_Momot Momot_Need_Load
alias Momot_Need_Load exec Moclient/src/main/Features/Modules/Momot/_init_.mo
alias MoClient_Autostop_pass "alias Momot_Need_Load"
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
//alias MoClient_Rapidfire_Enable "RapidFire_Enable"
//alias MoClient_Rapidfire_Disable "RapidFire_Disable"

alias +MoClient_attack2 "+attack2"
alias -MoClient_attack2 "-attack2;spec_prev"

alias MoClient_Autostop_pass ""
alias MoClient_Autostop_sideload "exec MoClient/src/jiting/setup"


//注册连跳增强 其中带/直接调用用户空间命令
alias MoClient_BetterBhop_Enable "alias betterbhop_activeA /fps_low;alias betterbhop_activeF /fps_recovery"
alias MoClient_BetterBhop_Disable "alias betterbhop_activeA;alias betterbhop_activeF"

//注册自动连跳
alias MoClient_AutoBhop_Enable "alias autobhop_activeA alias sq_6ms_1 autobhop_activeAA;alias autobhop_activeF autobhop_activeFA"
alias MoClient_AutoBhop_Disable "alias autobhop_activeA ;alias autobhop_activeF"


//注册是否开启声音提示
alias Moclient_SoundTips_Enable "alias soundtips_1 .soundtips_1;alias soundtips_0 .soundtips_0;"
alias Moclient_SoundTips_Disable "alias soundtips_1;alias soundtips_0;"

alias +smartattack "+attack;"
alias -smartattack "-attack;spec_next"

//注册输出系统
alias MoClient_TextOutput_Team "exec MoClient/src/main/Features/OutputSystem/TextTips/Team.mo
alias MoClient_TextOutput_All "exec MoClient/src/main/Features/OutputSystem/TextTips/All.mo
alias MoClient_TextOutput_CW "exec MoClient/src/main/Features/OutputSystem/TextTips/CW.mo
alias MoClient_TextOutput_Echo "exec MoClient/src/main/Features/OutputSystem/TextTips/Echo.mo
alias MoClient_TextOutput_Null "exec MoClient/src/main/Features/OutputSystem/TextTips/Null.mo

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

alias +Moclient_define_attack "+smartattack;alias sq_6ms_4"
alias -Moclient_define_attack "-smartattack;needrec_spinbot;spec_next"
alias +MoClient_action_attack "+Moclient_define_attack;alias sq_7"
alias -MoClient_action_attack "-Moclient_define_attack;alias sq_7"
alias +MoClient_attack "alias sq_7 +MoClient_action_attack"
alias -MoClient_attack "alias sq_7 -MoClient_action_attack"
alias +MoClient_bind_attack +MoClient_attack
alias -MoClient_bind_attack -MoClient_attack
//开火
//smartattack指的是智能开火 通过判断rapidfire的开关状态 判断按下开火执行的是什么命令
//因为就算是普通开火 也是直接把smartattack定义为+attack 所以无需更改结构

alias +MoClient_bind_slot1     +MoClient_slot1 
alias +MoClient_bind_slot2     +MoClient_slot2 
alias +MoClient_bind_slot3     +MoClient_slot3
alias +MoClient_bind_slot4     +MoClient_slot4
alias +MoClient_bind_slot5     +MoClient_slot5
alias +MoClient_bind_drop    +MoClient_drop
alias +MoClient_bind_lastslot    +MoClient_lastslot

alias -MoClient_bind_slot1     -MoClient_slot1 
alias -MoClient_bind_slot2     -MoClient_slot2
alias -MoClient_bind_slot3     -MoClient_slot3
alias -MoClient_bind_slot4     -MoClient_slot4 
alias -MoClient_bind_slot5     -MoClient_slot5
alias -MoClient_bind_drop     -MoClient_drop
alias -MoClient_bind_lastslot    -MoClient_lastslot

alias esutools  //用于外置急停，此处先进行初始化，避免unknown command
alias slot1tools;alias slot2tools;alias slot3tools;alias slot4tools;alias slot5tools
alias +MoClient_define_slot1 "slot1;slot1tools;"
alias +MoClient_define_slot2 "slot2;slot2tools"
alias +MoClient_define_slot3 "slot3;slot3tools"
alias +MoClient_define_slot4 "slot4;slot4tools"
alias +MoClient_define_slot5 "slot5;slot5tools"
alias +MoClient_define_drop "drop"
alias +MoClient_define_lastslot "lastinv"

alias -MoClient_define_slot1 ""
alias -MoClient_define_slot2 ""
alias -MoClient_define_slot3 ""
alias -MoClient_define_slot4 ""
alias -MoClient_define_slot5 ""
alias -MoClient_define_drop ""
alias -MoClient_define_lastslot ""

alias +MoClient_action_slot1 "+MoClient_define_slot1;alias sq_9"
alias +MoClient_action_slot2 "+MoClient_define_slot2;alias sq_9"
alias +MoClient_action_slot3 "+MoClient_define_slot3;alias sq_9"
alias +MoClient_action_slot4 "+MoClient_define_slot4;alias sq_9"
alias +MoClient_action_slot5 "+MoClient_define_slot5;alias sq_9"
alias +MoClient_action_drop "+MoClient_define_drop;alias sq_9"
alias +MoClient_action_lastslot "+MoClient_define_lastslot;alias sq_9"

alias -MoClient_action_slot1 "-MoClient_define_slot1;alias sq_9"
alias -MoClient_action_slot2 "-MoClient_define_slot2;alias sq_9"
alias -MoClient_action_slot3 "-MoClient_define_slot3;alias sq_9"
alias -MoClient_action_slot4 "-MoClient_define_slot4;alias sq_9"
alias -MoClient_action_slot5 "-MoClient_define_slot5;alias sq_9"
alias -MoClient_action_drop "-MoClient_define_drop;alias sq_9"
alias -MoClient_action_lastslot "-MoClient_define_lastslot;alias sq_9"

alias +MoClient_slot1 "alias sq_9 +MoClient_action_slot1"
alias +MoClient_slot2  "alias sq_9 +MoClient_action_slot2"
alias +MoClient_slot3 "alias sq_9 +MoClient_action_slot3"
alias +MoClient_slot4 "alias sq_9 +MoClient_action_slot4"
alias +MoClient_slot5 "alias sq_9 +MoClient_action_slot5"
alias +MoClient_drop "alias sq_9 +MoClient_action_drop"
alias +MoClient_lastslot "alias sq_9 +MoClient_action_lastslot"

alias -MoClient_slot1  "alias sq_9 -MoClient_action_slot1"
alias -MoClient_slot2  "alias sq_9 -MoClient_action_slot2"
alias -MoClient_slot3 "alias sq_9 -MoClient_action_slot3"
alias -MoClient_slot4 "alias sq_9 -MoClient_action_slot4"
alias -MoClient_slot5 "alias sq_9 -MoClient_action_slot5"
alias -MoClient_drop "alias sq_9 -MoClient_action_drop"
alias -MoClient_lastslot "alias sq_9 -MoClient_action_lastslot"





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