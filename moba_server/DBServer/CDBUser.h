#pragma once
#include "util_time.h"

class CDBUser
{
public:
	CDBUser();
	~CDBUser();

	void Init();
	void Release();

	bool GetOnline() { return m_bOnline; }
	void SetOnline(bool bValue) { m_bOnline = bValue; }

	int64_t GetUserId() { return 0; }

public:
	time_t m_llFix = GetCurrTime();
	bool m_bOnline = false;
};
