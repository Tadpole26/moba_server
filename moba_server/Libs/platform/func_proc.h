#pragma once

#include <sys/stat.h>
#include <string>
#include "dtype.h"

//��ȡ��ǰ����cpu����
extern uint16_t get_cpu_num();
//���ݶ˿ںŵõ��򿪸Ķ˿ںŵĽ���ID(֧��vista, Win7,��֧�� XP, Server 2003)
extern DWORD GetProcessIdByPort(DWORD dwPort, bool bTcp = true);

extern int get_pid();

extern std::string run_cmd_res(const std::string& strCmd);
extern bool is_listen_port(DWORD dwPort, bool bTcp = true);

extern std::string get_local_ip();

extern std::string get_net_ip();
