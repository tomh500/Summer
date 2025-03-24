//连跳增强指的是对最大帧率锁定到64，因此，在用户空间已经注册命令，但是我们仍需要对空格进行修改
//因为部分用户可能给空格绑定了跳跃，或者说 空格跳本身就需要重写

//activeA表示开启 activeF表示关闭 其中是用于锁帧的命令 当用户空间选项为启用 它才会被定义内容
alias +Momot_space_jump_action "+jump;-jump;betterbhop_activeA;autobhop_activeA;alias sq_6"
alias +Momot_space_jump "alias sq_6 +Momot_space_jump_action"
alias -Momot_space_jump_action "betterbhop_activeF;autobhop_activeF;alias sq_6"
alias -Momot_space_jump "alias sq_6 -Momot_space_jump_action"

