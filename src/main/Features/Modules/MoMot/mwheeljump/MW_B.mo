alias MoMot_jump_action "exec Moclient/src/main/Tools/Actions/muti-jump.mo;alias sq_5"
alias +Momot_jump_define "alias sq_5 MoMot_jump_action"

alias wjump "+Momot_jump_define"
setinfo wjump 0
alias +Momot_jump "toggle wjump 0"

alias -MoMot_jump -jump