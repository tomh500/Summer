alias JumpThrow_define "exec MoClient/src/main/Features/Modules/ThrowItems/Jumpthrow.mo"
alias JumpThrow_action "JumpThrow_define;alias sq_10 "
alias JumpThrow "alias sq_10 JumpThrow_action "

alias WJumpThrow_define "exec MoClient/src/main/Features/Modules/ThrowItems/WJumpthrow.mo"
alias WJumpThrow_action "WJumpThrow_define;alias sq_10"
alias +Sqaure_Bind_WJumpThrow "alias sq_10 WJumpThrow_action"
alias -Sqaure_Bind_WJumpThrow "forward -1009 0 0;"