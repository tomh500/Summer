

exec MoClient/src/main/Features/Modules/MoMot/autostop/lock.mo


//判断是否开启了急停 和临时禁用
//alias /autostop_rl ".autostop_rl_enable"
//alias .autostop_rl_enable "alias autostop_left_cmd_active autostop_left_cmd;alias autostop_right_cmd_active autostop_right_cmd;alias /autostop_rl .autostop_rl_disable"
//alias .autostop_rl_disable "alias autostop_left_cmd_active;alias autostop_right_cmd_active;alias /autostop_rl .autostop_rl_enable"

alias /autostop_r ".autostop_r_enable"
alias .autostop_r_enable "autostop_r_enable_pack"
alias .autostop_r_disable "autostop_r_disable_pack"
alias /autostop_l ".autostop_l_enable"
alias .autostop_l_enable "autostop_l_enable_pack "
alias .autostop_l_disable "autostop_l_disable_pack "

//alias /autostop_fb ".autostop_fb_enable"
//alias .autostop_fb_enable "alias autostop_forward_cmd_active autostop_forward_cmd;alias autostop_back_cmd_active autostop_back_cmd;alias /autostop_fb .autostop_fb_disable"
//alias .autostop_fb_disable "alias autostop_forward_cmd_active;alias autostop_back_cmd_active;alias /autostop_fb .autostop_fb_enable"
//alias /autostop_f ".autostop_f_enable"

alias .autostop_f_enable "autostop_f_enable_pack"
alias .autostop_f_disable "autostop_f_disable_pack"
alias /autostop_b ".autostop_b_enable"
alias .autostop_b_enable "autostop_b_enable_pack"
alias .autostop_b_disable "autostop_b_disable_pack"

alias /autostop_all ".autostop_disable_all"
alias .autostop_disable_all "+autostop_switchpack;soundtips_0;mout_autostop_disable;.autostop_r_disable;.autostop_l_disable;.autostop_f_disable;.autostop_b_disable;alias /autostop_all .autostop_enable_all"
alias .autostop_enable_all "-autostop_switchpack;soundtips_1;mout_autostop_enable;.autostop_r_enable;.autostop_l_enable;.autostop_f_enable;.autostop_b_enable;alias /autostop_all .autostop_disable_all"

//JumpTool
//跳跃的情况下计时 关闭急停 这里引用 sq_66ms_2
alias autostop_jumptool ".autostop_f_disable;.autostop_r_disable;.autostop_b_disable;.autostop_l_disable;alias sq_66ms_2 autostop_jumptool_start"
alias autostop_jumptool_start "autostop_jumptool_1"
alias autostop_jumptool_1 "echo jump;alias autostop_jumptool_start autostop_jumptool_2"
alias autostop_jumptool_2 "alias autostop_jumptool_start autostop_jumptool_3"
alias autostop_jumptool_3 "alias autostop_jumptool_start autostop_jumptool_4"
alias autostop_jumptool_4 "alias autostop_jumptool_start autostop_jumptool_5"
alias autostop_jumptool_5 "alias autostop_jumptool_start autostop_jumptool_6"
alias autostop_jumptool_6 "alias autostop_jumptool_start autostop_jumptool_7"
alias autostop_jumptool_7 "alias autostop_jumptool_start autostop_jumptool_8"
alias autostop_jumptool_8 "alias autostop_jumptool_start autostop_jumptool_9"
alias autostop_jumptool_9 "alias autostop_jumptool_start autostop_jumptool_10"
alias autostop_jumptool_10 "alias autostop_jumptool_start autostop_jumptool_11"
alias autostop_jumptool_11 "alias autostop_jumptool_start autostop_jumptool_12"
alias autostop_jumptool_12 "alias autostop_jumptool_start autostop_jumptool_13"
alias autostop_jumptool_13 "alias autostop_jumptool_start autostop_jumptool_14"
alias autostop_jumptool_14 "alias autostop_jumptool_start autostop_jumptool_15"
alias autostop_jumptool_15 "echo unjump;rec_f_autostop;rec_r_autostop;rec_b_autostop;rec_l_autostop;alias autostop_jumptool_start autostop_jumptool_1;alias sq_66ms_2"

