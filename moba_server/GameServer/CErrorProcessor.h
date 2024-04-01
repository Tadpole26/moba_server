#pragma once

#include "CProcessorBase.h"

class CErrorProcessor : public CProcessorBase
{
public:
	CErrorProcessor() : CProcessorBase(0)
	{
	}

	bool DoUserRun(const std::string& strCmd) override
	{
		return (true);
	}
};




