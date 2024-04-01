# server
cross windows and linux card server

Bin目录		:		所有生成的可执行文件
BuildBin目录:		生成最终可执行文件的中间产物
Libs目录	:		各种自定义以及非自定义库源代码
Config目录	:		原始.proto文件、数据库配置文件等各种配置文件

network 		(自定义网络库)
platform 		(自定义工具库)
libevent 		(网路库)
protobuf		(序列化库)
protocol		(自定义序列化数据库)


vs安装protobuf
http://www.javashuo.com/article/p-fzyqzjvf-kv.html

vs安装libevent
http://libevent.org/ 下载路径


vs安装mongo 
https://blog.csdn.net/yinxu2008/article/details/119378093

vs安装boost
https://blog.csdn.net/m0_68696793/article/details/124537996

mysql++
https://www.likecs.com/show-307929289.html#sc=100


登录流程:
client:Msg_Login_VerifyAccount_CS->gate
gate:Msg_SysLog_UserLogin->sys
sys:Msg_SysLog_UserLoginRet->gate
gate:Msg_GG_Login_CS->game
