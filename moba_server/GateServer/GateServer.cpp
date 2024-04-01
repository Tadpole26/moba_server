#include "CGateLogic.h"
#include "log_mgr.h"

int main(int argc, char* argv[])
{
	Log_Custom("start", "start gate server!!!");
	if (gGateLogic)
	{
		if (!gGateLogic->Arg(argc, argv))
			return -1;
		if (!gGateLogic->Launch())
			return -1;
	}
	return 0;
}


