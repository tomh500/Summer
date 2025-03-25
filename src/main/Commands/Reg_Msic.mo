//这里是注册一些杂项命令和内部命令的 因为cfg不允许引号嵌套 因此可能需要创建一些alias
alias fulljump_action "+jump;-jump"//无需使用+- 一次触发jump需要

alias nomove "forward -1009 0 0;back -1009 0 0;left -1009 0 0;right -1009 0 0" //停止所有移动 

alias fullattack_action "+attack;-attack"

alias autobhop_activeAA "fulljump_action;alias smart_rapidfire_fps_rec /fps_low;/fps_low"

alias autobhop_activeFA "/fps_recovery;alias smart_rapidfire_fps_rec /fps_recovery;alias sq_6ms_1"

//关于单发武器自动化的内部命令定义
//smart_rapidfire_fps_指的是帧率的更改 因为自动手枪、连跳等多个模块涉及到帧率
//所以需要额外定义一层 去判断是否定义命令

//先初始化定义 以免报错 部分代码转到连跳增强部分
alias smart_rapidfire_fps_low /fps_low
alias smart_rapidfire_fps_rec /fps_recovery

alias TakeAttackToRapidFireA "alias sq_6ms_2 fullattack_action;smart_rapidfire_fps_low;alias smart_rapidfire_fps_rec /fps_low"
alias TakeAttackToRapidFire "TakeAttackToRapidFireA;"

alias TakeAttackCancelRapidFire "alias sq_6ms_2;/fps_recovery;alias smart_rapidfire_fps_rec /fps_recovery"