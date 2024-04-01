#include "CDBUser.h"

CDBUser::CDBUser()
{
}

CDBUser::~CDBUser()
{
}

void CDBUser::Init()
{
	m_llFix = GetCurrTime();
	m_bOnline = false;
}

void CDBUser::Release()
{

}