// BattleServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CBattleLogic.h"

int main(int argc, char* argv[])
{
	Log_Custom("start", "start fight server!");
	CBattleLogic* pLogic = gBattleLogic;
	if (pLogic)
	{
		if (!pLogic->Arg(argc, argv))
			return -1;
		if (!pLogic->Launch())
			return -1;
	}
	return 0;
}

