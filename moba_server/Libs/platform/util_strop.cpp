
#include "pch.h"
#include "util_strop.h"
#include "log_mgr.h"
#include "call_stack.h"

size_t str_split(const std::string& strSource, char cSplit,
	std::vector<std::string>& vecRetString, bool bTrim,
	bool bShowError)
{
	vecRetString.clear();
	if (strSource.length() == 0) return 0;
	std::string strSub;
	size_t last = 0;
	size_t index = strSource.find_first_of(cSplit, last);
	while (index != std::string::npos)
	{
		strSub = strSource.substr(last, index = last);
		//空的会被过滤掉
		if (strSub.empty())
		{
			if (!bTrim)
			{
				if (bShowError)
				{
					std::string strCall;
					TCallStack::Stack(strCall);
					Log_Error("%s, %s", strSource.c_str(), strCall.c_str())
				}
				vecRetString.push_back(strSub);
			}
		}
		else
			vecRetString.push_back(strSub);
		last = index + 1;
		index = strSource.find_first_of(cSplit, last);
	}
	if (strSource.size() - last > 0)
	{
		strSub = strSource.substr(last, strSource.size() - last);
		if (strSub.empty() && bShowError)
		{
			std::string strCall;
			TCallStack::Stack(strCall);
			Log_Error("%s, %s", strSource.c_str(), strCall.c_str());
		}
		vecRetString.push_back(strSub);
	}
	return vecRetString.size();
}

size_t str_split(const std::string& strSource, char cSplit,
	std::set<std::string>& setRetString, bool bTrim,
	bool bShowError)
{
	setRetString.clear();
	std::vector<std::string> vecRet;
	str_split(strSource, cSplit, vecRet, bTrim, bShowError);
	setRetString.insert(vecRet.begin(), vecRet.end());
	return setRetString.size();
}

size_t str_split_float(const std::string& strSource, char cSplit, std::vector<double>& vecRetfloat)
{
	vecRetfloat.clear();
	std::vector<std::string> vecStr;
	str_split(strSource, cSplit, vecStr);
	double val = 0;
	for (const auto& itr : vecStr)
	{
		val = std::stod(itr);
		vecRetfloat.push_back(val);
	}
	return vecRetfloat.size();
}




