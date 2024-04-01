#include  "CCrossLogic.h"
#include "log_mgr.h"

int main(int argc, char* argv[])
{
	Log_Custom("start", "start cross server!!!");
	if (CROSS_LOGIC_INS)
	{
		if (!CROSS_LOGIC_INS->Arg(argc, argv))
			return -1;
		if (!CROSS_LOGIC_INS->Launch())
			return -1;
	}
	return 0;
}