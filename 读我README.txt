DearMoments-Delta 1.6

在群里提问之前，请确保你已经完整阅读本教程、UserKeyBinds、UserSetting、UserValue、UserKZMode、killing_sound.json、道具轮盘站位图.exe七个文件，并声明你已完整阅读，在提问的时候附带上有用的截图或者视频，且确保为CFG开启了Debug模式，否则你的提问将被忽略

[本次更新日志]
[+]监听器常规更新
[+]调整了自定义道具的文件位置
[+]添加了两个身法的lua文件
[+]添加单键急停13档
[+]添加功能：替换切刀声音
[+]添加功能：自动检视
[+]优化自定义道具读取脚本，不再大小写敏感
[+]优化用户空间配置，可读性更高了


[GSI-击杀音效替换使用方式]
请阅读Userspace->gsi->killing_sound->killing_sound.json

[安装方式]
将项目放入CS2的CFG文件夹，确保项目文件夹名字为DearMoments
请确保完整复制【整个项目文件夹】而不是里面的文件，否则会报错！
然后运行CFG监听器主程序.bat ，点击左上角->文件->更新并安装CFG
然后点击打开用户空间，配置CFG，并保存
注意，游玩的时候不要关闭监听器（CFG启动入口.bat）！

启动项：
+exec DearMoments/setup
也可以在安装器里面自动添加到autoexec，需要注意的是，CFG只能加载一次，如果在autoexec加载了CFG，就不能添加启动项

[更改偏好设置]
在项目文件夹的Userspace文件夹内，修改UserKeyBinds、UserSetting、UserValue、UserKZMode并保存即可

[用户空间命令列表]
/fps_recovery                        解锁FPS上限
/fps_low                             锁定FPS到64帧
/reload_userspace                    重置用户空间   
/reload_command                      重新注册命令
/crosshair_color_recovery            准星颜色调整为默认
/crosshair_color_recoil              准星颜色调整为跟随
/s_1                                 快捷灵敏度—最低
/s_2                                 快捷灵敏度—低   
/s_3                                 快捷灵敏度—中   
/s_4                                 快捷灵敏度—高
/s_5                                 快捷灵敏度—最高
/RapidFire			自动手枪开关
/sma				自动身法菜单
/music				音乐菜单
/musicmode			音游模式
/autostop_all			开关自动急停
/KZmode			进入KZ模式


[如何卸载cfg]
找到游戏的cfg文件夹，打开autoexec.cfg，将里面的exec MoClient/setup删掉
找到你的游戏启动项，删掉+exec DearMoments/setup
打开你的游戏，在控制台输入binddefaults

[为什么进游戏卡死、自动退出]
请确保你没有删除正版验证文件"免费发布.lty"
且运行了“CFG监听器主程序.bat”

[视角无法转动]
请根据提示运行"CFG监听器主程序.bat"，并打开一次购买界面或者重启游戏即可

[为什么锁帧]
请在用户空间的UserValue.cfg设置相关条目，但是请确保帧率不大于540！

[能和Horizon一起使用吗]
不能

[为什么玩了一段时间之后无法操作了]
首先，请确保你在UserValue里面的锁帧设置不超过540，当游戏进行数个小时之后，需要重启游戏

[本CFG的使用范围和授权范围]
仅供个人使用，非官方发布的本项目及其衍生产品，必须免费、开源，且开发团队不对衍生产品提供任何支持

[关于倒卖]
随意，只要你能保留我的著作权，至于别人骂不骂你，就不关我的事了，你大可以尝试绕过我的正版验证，只要你有这个能力

[关于赞助]
本CFG完全免费，无需付费即可使用全部功能，但是开设了赞助渠道，如果觉得好用，可以赞助

[关于定制]
定制不同于赞助，如果需要定制，请私信B站Sangatsu_P或者加入discord频道私信说明来意，定制的内容将包含一般商业条款

[如何自定义CFG]
首先，请打开你复制到CFG目录的DearMoments文件夹，打开里面的Userspace文件夹，你会看见UserKeyBinds、UserKZMode、UserSetting、UserValue四个文件，他们分别是用于设置按键绑定、设置KZ模式按键绑定、设置CFG功能、设置游戏数值的文件，现在，请先打开UserKeyBinds，以

bind w +DM_bind_forward   //你的前进按键
bind s +DM_bind_back      //你的后退按键
bind a +DM_bind_left      //你的左移按键
bind d +DM_bind_right     //你的右移按键

为例，这里的bind是绑定命令 后面带//的是注释 表示行用于绑定的内容，其中bind 后的 w、s、a、d就是你要将此功能绑定到的按键，你可能会看见如同

bind key +DM_bind_radialradio

的内容，其中，这里的key没有任何实际意义，也就是没有提供绑定，你可以把key 改成你键盘上的字符，以绑定此功能

接下来，打开UserSetting，以如下条目为例

//DearMoments_TextOutput_CW        输出到无线电
//DearMoments_TextOutput_Team      输出到队伍
//DearMoments_TextOutput_All       输出到全体
//DearMoments_TextOutput_Echo      输出到控制台
//DearMoments_TextOutput_Null      不输出
DearMoments_TextOutput_CW

其中，带//的是注释，不会发挥任何作用，你可能会看见几行注释和一行不是注释的内容，那么这一个区块表示一个功能，上面的示范是将输出的内容打印为无线电，如果我想修改它，只需要将没有注释的一行，替换成上面其中一条注释的内容即可（不要加//）

接着打开UserValue，例如

//设置你的初始灵敏度和旋转数值，旋转数值可以用项目内的计算器进行计算
//这个旋转数值是180/(m_yaw*你的灵敏度) 一般情况下 m_yaw为0.022 你可以在控制台输入m_yaw获取这个数值
alias my_sensitivity  sensitivity   1     //这个数值是你的视角灵敏度
alias fire_sensitivity sensitivity 0.834     //设置你的开火灵敏度，你可以让开火时的灵敏度与你的视角灵敏度不一样
alias my_yaw m_yaw 0.022                     //一般情况请保持0.022，除非你已经更改了它，如果你不知道，就不要更改


这里是设置你的灵敏度相关的内容，只需要跟着注释一个个填写就可以了，你只需要更改数字，而无需更改其他内容


最后修订By Sangatsu_P at Jun 30th 2025