//AntiFlash

alias recforward_pack_define "-back;+forward"
alias recforward_pack_action "recforward_pack_define;alias sq_8"
alias recforward_pack "alias sq_8 recforward_pack_action"

alias recback_pack_define "-forward;+back"
alias recback_pack_action "recback_pack_define;alias sq_8"
alias recback_pack "alias sq_8 recback_pack_action"

alias recright_pack_define "-left;+right"
alias recright_pack_action "recright_pack_define;alias sq_9"
alias recright_pack "alias sq_9 recright_pack_action"

alias recleft_pack_define "-right;+left"
alias recleft_pack_action "recleft_pack_define;alias sq_9"
alias recleft_pack "alias sq_9 recleft_pack_action"

alias AntiFlash_NeedMove "Antiflash_needback;Antiflash_needforward;Antiflash_needleft;Antiflash_needright"
alias AntiFlash_NeedRec "AntiFlash_RecForward;AntiFlash_RecBack;AntiFlash_RecRight;AntiFlash_RecLeft"
alias +AntiFlash_define "spinbot;nomove;AntiFlash_NeedMove"
alias -AntiFlash_define "spinbot;AntiFlash_NeedRec"

alias +antiflash_action "+antiflash_define;alias sq_7"
alias -antiflash_action "-antiflash_define;alias sq_7"
alias +antiflash "alias sq_7 +antiflash_action"
alias -antiflash "alias sq_7 -antiflash_action"

alias !Antiflash "!antiflash1"
alias !antiflash1 "+antiflash;alias !antiflash !antiflash2"
alias !antiflash2 "-antiflash;alias !antiflash !antiflash1"
alias !start_spinbot "!start_spin_A"
alias !start_spin_A "alias sq_6ms_4 !antiflash;alias !start_spinbot !start_spin_B"
alias !start_spin_B "alias sq_6ms_4;!antiflash2;alias !start_spinbot !start_spin_A"
alias start_spinbot !start_spinbot