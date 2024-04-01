#include "pch.h"
#include "util_arg_opt.h"
#include "win_get_opt.h"

void ArgOpt::Man()
{
	Log_Error("Usage:./test -i 101");
	Log_Error(" -i: server no.");
	Log_Error(" -f: config file.");
	Log_Error(" -t: typename.");
	Log_Error(" -c: comment.");
	Log_Error(" -a: areano.");
	Log_Error(" -l: areano list.");
	Log_Error(" -h: help.");
}

bool ArgOpt::Argv(int argc, char* argv[])
{
	int32_t opt;
	while ((opt = getopt(argc, argv, "i:f:t:c:a:l:h")) != -1)
	{
		_mapValue.insert(std::make_pair(opt, optarg));

		switch (opt)
		{
		case 'i':
			_index = std::stoi(optarg, nullptr, 10);
			break;
		case 'f':
			_config = optarg;
			break;
		case 't':
			_typeName = optarg;
			break;
		case 'c':
			_comment = optarg;
			break;
		case 'a':
			_areano = std::stoi(optarg, nullptr, 10);
			break;
		case 'l':
			_level = optarg;
			break;
		case 'h':
			Man();
			return false;
			break;
		default:
			Log_Error("argv error, use -h help!");
			return false;
			break;
		}
	}

	if (_index == 0)
	{
		Log_Warning("index not set default is 1");
		_index = 1;
	}
	if (_config.empty())
		Log_Warning("config file not set, use default path");

	return true;
}