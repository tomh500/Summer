//MoMot是MoClient的移动模块定义，这里是MoMot的初始化入口
//MoMot的基础移动使用sq_1ms系列的1~4进行定义，可能存在额外的定义
//其对外暴露的绑定命令是+Momot_bind_forward......
//他们指向另一个命令+Momot_forward......
//对于需要绕过多绑定的，格式一般为+Momot_action_forward.... 以action为中间的词缀注册
//socd_f/b/r/l是用于记录移动状态的，他们将会适用于snaptap功能
//针对CS2的特性，当命令无论多么小 都将视为0，因此可以简易实现

//提前注册-命令，适应snaptap
alias -socd_press_f -forward;alias -socd_press_b -back;alias -socd_press_r -right;alias -socd_press_l -left

//注册前后左右移动
alias +Momot_action_forward "+forward;socd_switch_cancel_back;socd_switch_cancel_back;alias +socd_press_f socd_switch_forward;alias sq_1"
alias -Momot_action_forward "forward -1009 0 0;alias +socd_press_f;+socd_press_b;alias sq_1"
alias +Momot_forward "alias sq_1 +Momot_action_forward"
alias -Momot_forward "alias sq_1 -Momot_action_forward"

alias +Momot_action_back "+back;socd_switch_cancel_forward;socd_switch_cancel_forward;alias +socd_press_b socd_switch_back;alias sq_2"
alias -Momot_action_back "back -1009 0 0;alias +socd_press_b;+socd_press_f;alias sq_2"
alias +Momot_back "alias sq_2 +Momot_action_back"
alias -Momot_back "alias sq_2 -Momot_action_back"

alias +Momot_action_right "+right;socd_switch_cancel_left;socd_switch_cancel_left;alias +socd_press_r socd_switch_right;alias sq_3"
alias -Momot_action_right "right -1009 0 0;alias +socd_press_r;+socd_press_l;alias sq_3"
alias +Momot_right "alias sq_3 +Momot_action_right"
alias -Momot_right "alias sq_3 -Momot_action_right"

alias +Momot_action_left "+left;socd_switch_cancel_right;socd_switch_cancel_right;alias +socd_press_l socd_switch_left;alias sq_4"
alias -Momot_action_left "left -1009 0 0;alias +socd_press_l;+socd_press_r;alias sq_4"
alias +Momot_left "alias sq_4 +Momot_action_left"
alias -Momot_left "alias sq_4 -Momot_action_left"


exec MoClient/src/main/Features/Modules/MoMot/spacejump/_init_.mo