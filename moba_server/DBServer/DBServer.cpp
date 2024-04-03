#include  "CDBLogic.h"
#include "log_mgr.h"

int main(int argc, char* argv[])
{
	Log_Custom("start", "start db server!!!");
	if (gDBLogic)
	{
		if (!gDBLogic->Arg(argc, argv))
			return -1;
		if (!gDBLogic->Launch())
			return -1;
	}
	return 0;
}
