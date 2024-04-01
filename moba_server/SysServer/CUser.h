#pragma once
#include "msg_common.pb.h"
using namespace ProtoMsg;

class CUser
{
public:
	CUser();
	CUser(int64_t llUid);

	void Init();
	void Release();

public:
	int64_t m_llUid = 0;
	EPlayerStatus m_eUserStatus = ePlayer_Offline;
	uint32_t m_uiGateId = 0;
	uint32_t m_uiGameId = 0;
	uint32_t m_uiNewGateId = 0;
	uint32_t m_uiNewGameId = 0;
	uint32_t m_uiSequence = 0;

};