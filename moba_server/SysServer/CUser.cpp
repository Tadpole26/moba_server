#include "CUser.h"

CUser::CUser()
	: m_llUid(0u)
	, m_eUserStatus(ePlayer_Offline)
	, m_uiGameId(0)
	, m_uiNewGateId(0)
{
}

CUser::CUser(int64_t llUid)
	: m_llUid(llUid)
	, m_eUserStatus(ePlayer_Offline)
	, m_uiGameId(0)
	, m_uiNewGateId(0)
{
}

void CUser::Init()
{
	m_llUid = 0;
	m_eUserStatus = ePlayer_Offline;
	m_uiGateId = 0;
	m_uiGameId = 0;
	m_uiNewGateId = 0;
	m_uiNewGameId = 0;
	m_uiSequence = 0;
}

void CUser::Release()
{
	m_llUid = 0;
	m_eUserStatus = ePlayer_Offline;
	m_uiGateId = 0;
	m_uiGameId = 0;
	m_uiNewGateId = 0;
	m_uiNewGameId = 0;
	m_uiSequence = 0;
}