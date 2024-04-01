#pragma once
#include "../network/logic_interface.h"
#include "msg_make.h"

struct tagMsgHead;
class CSvrLogicFace : public CLogicInterface, public CMsgMake
{
public:
	CSvrLogicFace() { }
	~CSvrLogicFace() { }
};
