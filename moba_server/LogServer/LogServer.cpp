#include  "CLogLogic.h"
#include "log_mgr.h"

int main(int argc, char* argv[])
{
	Log_Custom("start", "start log server!!!");
	if (LOG_LOGIC_INS)
	{
		if (!LOG_LOGIC_INS->Arg(argc, argv))
			return -1;
		if (!LOG_LOGIC_INS->Launch())
			return -1;
	}
	return 0;
}
