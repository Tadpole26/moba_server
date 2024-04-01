#pragma once
#include <functional>
typedef std::function<void(void)> fn_proc_stop;

extern void InstallCoreDumper();
extern void WriteCallList();
extern bool SetConsoleInfo(fn_proc_stop fnStop);
extern fn_proc_stop g_fnStop;
