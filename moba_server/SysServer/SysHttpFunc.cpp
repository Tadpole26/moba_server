#include "SysHttpFunc.h"
#include "CSysLogic.h"
#include "json.h"
#include "CSession.h"
#include "msg_interface.h"
#include "msg_module_http.pb.h"
#include "parse_pb.h"

CHttpMgr g_HttpMgr;
CMsgQueue CSysHttpFunc::_queue;

CSysHttpFunc::CSysHttpFunc()
{
}

CSysHttpFunc::~CSysHttpFunc()
{
}

void CSysHttpFunc::Run(CHttpParam& param)
{
	g_HttpMgr.PushPost(param);
}

bool CSysHttpFunc::Init(std::string szHost, event_base* pEvBase, int iThreadNum)
{
	bool ret = g_HttpMgr.Init(iThreadNum);
	if (!ret)
	{
		Log_Error("http mgr init error!");
		return false;
	}

	CHttpRequest::Init(szHost);
	g_HttpMgr.RegFunc(&CSysHttpFunc::HttpRetRun);
	ret = _queue.init(64 * 1024, pEvBase, std::bind(&CSysHttpFunc::OnQueue, this, std::placeholders::_1), EMQ_MUL);
	if (!ret)
	{
		Log_Error("queue init error!");
		return false;
	}
	return true;
}

//非线程安全的
//当需要操作其它成员或者全局时,请不要在此处理
void CSysHttpFunc::HttpRetRun(CHttpParam& req)
{
	switch (req.m_cType) 
	{
	case eGetUniqRolId:
		HttpRetGetUniqRolId(req);
		break;
	case eLoginUser:
		HttpRetUserLogin(req);
		break;
	case eModifyName:
		HttpRetModifyName(req);
		break;
	default:
		break;
	}
}

void CSysHttpFunc::HttpRetGetUniqRolId(CHttpParam& req)
{
	std::string strRet = req.m_strRet;
	ProtoMsg::HttpGetUniqRolId ret;
	int64_t llPlayerId = 0;
	uint32_t state = 0;
	std::string strRoleName;
	ret.set_llplayerid(req.m_llKey);
	ret.set_uistate(state);
	ret.set_uiserverid((uint32_t)req.GetParamInt("svrid"));
	ret.set_strplayername("");

	Json::Reader read;
	Json::Value oRoot, rol, ste, name;

	if (!read.parse(strRet, oRoot))
	{
		Log_Error("role:%lld ret:%s", req.m_llKey, strRet.c_str());
		goto Exit0;
	}
	ste = oRoot["state"];
	if (!ste.isNumeric())
	{
		Log_Error("role:%lld ret:%s", req.m_llKey, strRet.c_str());
		goto Exit0;
	}
	state = ste.asInt();
	if (state == 1)			//成功
	{
		rol = oRoot["rol_id"];
		name = oRoot["rol_name"];

		if (!rol.isNumeric() || !name.isString())
		{
			Log_Error("player: %lld ret:%s", req.m_llKey, strRet.c_str());
			goto Exit0;
		}
		llPlayerId = rol.asInt64();
		strRoleName = name.asString();
		ret.set_llplayerid(llPlayerId);
		ret.set_uistate(state);
		ret.set_strplayername(strRoleName);
	}
Exit0:
	SendMainLogic(req.m_cType, ret.SerializeAsString());
}

void CSysHttpFunc::HttpRetUserLogin(CHttpParam& req)
{
	std::string strRet = req.m_strRet;
	Json::Reader read;
	Json::Value oRoot, jRes, jPro, jProName, jGm, jDev, jProArea;
	ProtoMsg::HttpUserLoginRet loginret;
	loginret.set_llplayerid(req.m_llKey);
	loginret.set_uigateid((uint32_t)req.GetParamInt("id"));
	loginret.set_strsessionid(req.GetParam("p"));
	loginret.set_uigmlevel(0);
	loginret.set_iprovince(0);
	loginret.set_strprovincename("");
	loginret.set_eresult(ProtoMsg::Code_Common_Failure);
	loginret.set_strdevid("");
	loginret.set_uiproarea(0);

	int res = 0;
	if (!read.parse(strRet, oRoot))
	{
		Log_Error("role:%lld, ret:%s", req.m_llKey, strRet.c_str());
		goto Exit0;
	}

	jRes = oRoot["state"];
	jPro = oRoot["addr_id"];
	jProName = oRoot["addr"];
	jGm = oRoot["gm"];
	jDev = oRoot["client_dev"];
	jProArea = oRoot["battle_area"];

	if (jRes.isNull() || !jRes.isNumeric())
	{
		Log_Error("role:%lld, ret:%s", req.m_llKey, strRet.c_str());
		goto Exit0;
	}

	res = jRes.asInt();
	if (res != 1)
	{
		Log_Error("role:%lld, ret:%s", req.m_llKey, strRet.c_str());
		goto Exit0;
	}

	if (jPro.isNumeric())
		loginret.set_iprovince(jPro.asInt());
	if (jProName.isString())
		loginret.set_strprovincename(jProName.asString());
	if (jGm.isNumeric())
		loginret.set_uigmlevel(jGm.asInt());
	if (jDev.isString())
		loginret.set_strdevid(jDev.asString());
	if (jProArea.isNumeric())
		loginret.set_uiproarea(jProArea.asInt());

	loginret.set_eresult(ProtoMsg::Code_Common_Success);
Exit0:
	SendMainLogic(req.m_cType, loginret.SerializeAsString());
}

void CSysHttpFunc::HttpRetModifyName(CHttpParam& req)
{
	std::string strRet = req.m_strRet;
	ProtoMsg::HttpModifyNameRet loginret;
	loginret.set_llplayerid(req.m_llKey);
	loginret.set_strplayername(req.GetParam("n"));
	loginret.set_eresult(ProtoMsg::Code_Common_Failure);
	loginret.set_uiserverid((uint32_t)req.GetParamInt("svrid"));

	uint32_t uiState = 0;
	Json::Reader read;
	Json::Value oRoot, ste;
	if (!read.parse(strRet, oRoot))
	{
		Log_Error("role:%lld, ret:%s", req.m_llKey, strRet.c_str());
		goto Exit0;
	}
	ste = oRoot["state"];
	if (!ste.isNumeric())
	{
		Log_Error("role:%lld, ret:%s", req.m_llKey, strRet.c_str());
		goto Exit0;
	}
	uiState = ste.asInt();
	if (uiState == 1)
	{
		loginret.set_eresult(ProtoMsg::Code_Common_Success);
		goto Exit0;
	}
	uiState = ste.asInt();
	if (uiState == 1)			//成功
	{
		loginret.set_eresult(ProtoMsg::Code_Common_Success);
	}
Exit0:
	SendMainLogic(req.m_cType, loginret.SerializeAsString());
}

void CSysHttpFunc::SendMainLogic(uint32_t uiMsgId, const std::string& strMsg)
{
	ProtoMsg::HttpRetMsg ret;
	ret.set_strmsg(strMsg);
	ret.set_uimsgid(uiMsgId);
	std::string strSend = ret.SerializeAsString();

	if (!_queue.push(strSend.c_str(), strSend.size()))
		Log_Error("recv %d push error!", strSend.size());
}

void CSysHttpFunc::OnQueue(const event_msg_t msg)
{
	tagBufItem* pNewMsg = (tagBufItem*)msg;
	if (pNewMsg == nullptr) return;
	ProtoMsg::HttpRetMsg ret;
	PARSE_PTL(ret, pNewMsg->_body, pNewMsg->_size);

	switch (ret.uimsgid())
	{
	case eGetUniqRolId:
		OnRetGetRolId(ret.strmsg());
		break;
	case eLoginUser:
		OnRetUserLogin(ret.strmsg());
		break;
	case eModifyName:
		OnRetModifyName(ret.strmsg());
		break;
	default:
		break;
	}
}

void CSysHttpFunc::OnRetGetRolId(const std::string& strMsg)
{
	ProtoMsg::HttpGetUniqRolId getrole;
	PARSE_PTL_STR(getrole, strMsg);

	CSession::SendCreateRoleRet(getrole.llplayerid(),
		getrole.uiserverid(), getrole.strplayername());
}

void CSysHttpFunc::OnRetUserLogin(const std::string& strMsg)
{
	ProtoMsg::HttpUserLoginRet loginret;
	PARSE_PTL_STR(loginret, strMsg);
	CSession::SendPlayerLoginRet(loginret.llplayerid()
		, loginret.eresult(), loginret.uigateid()
		, loginret.iprovince(), loginret.strprovincename()
		, loginret.strsessionid(), loginret.uigmlevel()
		, loginret.strdevid(), loginret.uiproarea());
}

void CSysHttpFunc::OnRetModifyName(const std::string& strMsg)
{
	ProtoMsg::HttpModifyNameRet ret;
	PARSE_PTL_STR(ret, strMsg);

	CSession::SendModifyRoleNameRet(ret.llplayerid()
		, ret.uiserverid(), ret.strplayername(), ret.eresult());
}