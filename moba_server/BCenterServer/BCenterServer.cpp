// BCenterServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include  "CBCenterLogic.h"
#include "log_mgr.h"

int main(int argc, char* argv[])
{
	Log_Custom("start", "start battle center server!!!");
	if (gBCenterLogic)
	{
		if (!gBCenterLogic->Arg(argc, argv))
			return -1;
		if (!gBCenterLogic->Launch())
			return -1;
	}
	return 0;
}
