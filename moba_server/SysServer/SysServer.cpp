#include  "CSysLogic.h"
#include "log_mgr.h"

int main(int argc, char* argv[])
{
	Log_Custom("start", "start sys server!!!");
	if (SYS_LOGIC_INS)
	{
		if (!SYS_LOGIC_INS->Arg(argc, argv))
			return -1;
		if (!SYS_LOGIC_INS->Launch())
			return -1;
	}
	return 0;
}
