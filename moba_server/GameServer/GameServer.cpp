#include  "CGameLogic.h"
#include "log_mgr.h"

int main(int argc, char* argv[])
{
	Log_Custom("start", "start game server!!!");
	if (gGameLogic)
	{
		if (!gGameLogic->Arg(argc, argv))
			return -1;
		if (!gGameLogic->Launch())
			return -1;
	}
	return 0;
}
