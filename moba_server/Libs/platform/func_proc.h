#pragma once

#include <sys/stat.h>
#include <string>
#include "dtype.h"

//获取当前机器cpu数量
extern uint16_t get_cpu_num();
//根据端口号得到打开改端口号的进程ID(支持vista, Win7,不支持 XP, Server 2003)
extern DWORD GetProcessIdByPort(DWORD dwPort, bool bTcp = true);

extern int get_pid();

extern std::string run_cmd_res(const std::string& strCmd);
extern bool is_listen_port(DWORD dwPort, bool bTcp = true);

extern std::string get_local_ip();

extern std::string get_net_ip();
