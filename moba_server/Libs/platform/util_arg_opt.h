#pragma once
#ifndef WIN32
#include <getopt.h>
#endif
#include "log_mgr.h"

class ArgOpt
{
public:
	ArgOpt() : _index(0) {}
	~ArgOpt() { }

	void Man();
	bool Argv(int argc, char* argv[]);

	uint32 GetIndex() const { return _index; }
	const std::string& GetConfig() const { return _config; }
	const std::string& GetType() const { return _typeName; }
	uint32 GetAreanNo() const { return _areano; }
	const std::string& GetLevel() const { return _level; }
	const std::string& GetComment() const { return _comment; }

protected:
	uint32								_index = 0;
	uint32								_areano = 0;
	std::string							_config;
	std::string							_typeName;
	std::string							_level;
	std::string							_comment;
	std::map<int32_t, std::string>		_mapValue;
};
