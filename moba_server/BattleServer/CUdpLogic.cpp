#include "CUdpLogic.h"
#include "CRoomThread.h"
#include "CBattleUser.h"
#include "ikcp.h"
#include "CRoom.h"
#include "CUdpSess.h"
#include "util_malloc.h"
#include "ikcp.h"
#include "msg_module_battle.pb.h"
#include <boost/bind.hpp>

const int KCP_HEAD_SIZE = 4;

CUdpLogic::CUdpLogic(int iPort, CRoomThread* pRoomNet)
	: m_oIoService()
	, m_oUdpSocket(m_oIoService, ip::udp::endpoint(ip::udp::v4(), iPort))
	, m_oKcpTimer(m_oIoService)
{
	m_pRoomNet = pRoomNet;
	m_iPort = iPort;
	memset(m_arrErrBuf, 0, sizeof(m_arrErrBuf));
}

CUdpLogic::~CUdpLogic()
{
	m_bStoped = true;
	if (m_oThread.joinable())
		m_oThread.join();
}

void CUdpLogic::InitKcp()
{
	ikcp_allocator(msg_alloc, msg_free);
}

bool CUdpLogic::Init(int iPort, CRoomThread* pRoomNet)
{
	m_pRoomNet = pRoomNet;
	m_iPort = iPort;

	//设置缓冲区大小
	boost::asio::socket_base::send_buffer_size oSendSize(2 * 1024 * 1024);
	boost::asio::socket_base::receive_buffer_size oRecvSize(2 * 1024 * 1024);
	boost::asio::socket_base::reuse_address oIoReuse(true);

	m_oUdpSocket.set_option(oSendSize);
	m_oUdpSocket.set_option(oRecvSize);
	m_oUdpSocket.set_option(oIoReuse);
	m_oUdpSocket.non_blocking(true);
	//为udp server对应监听udp socket创建回调(收到数据分包处理)
	HookUdpAsyncReceive();
	//设置定时器和定时器回调
	m_oKcpTimer.expires_from_now(boost::posix_time::milliseconds(TICK_MILL_SEC));
	m_oKcpTimer.async_wait(boost::bind(&CUdpLogic::HandleKcpTimer, this));

	m_oThread = std::thread(&CUdpLogic::Run, this);
	return true;
}

void CUdpLogic::Run()
{
	m_oIoService.run();
}

void CUdpLogic::KillUser(CBattleUser* pBtlUser)
{
	CUdpSess* pSess = pBtlUser->GetUdpSess();
	if (pSess != nullptr)
		pSess->on_disconnect();
	else
		pBtlUser->on_disconnect();
}

void CUdpLogic::EnterUser(CBattleUser* pBtlUser)
{
	auto iter = m_mapConvToRoom.find(pBtlUser->GetConvId());
	if (iter != m_mapConvToRoom.end())
		iter->second.m_pBtlUser = pBtlUser;
	else
	{
		tagConvItem stItem;
		stItem.m_llUserId = pBtlUser->GetUserId();
		stItem.m_llRoomId = pBtlUser->GetRoomId();
		stItem.m_pBtlUser = pBtlUser;
		m_mapConvToRoom[pBtlUser->GetConvId()] = stItem;
	}
}

void CUdpLogic::DeleteUser(CBattleUser* pBtlUser)
{
	auto iter = m_mapConvToRoom.find(pBtlUser->GetConvId());
	if (iter != m_mapConvToRoom.end())
		iter->second.m_pBtlUser = nullptr;
}

void CUdpLogic::AddRoom(CRoom* pRoom)
{
	tagConvItem stItem;
	auto& mapConv = pRoom->GetConvList();
	for (auto& iter : mapConv)
	{
		stItem.m_llUserId = iter.first;
		stItem.m_llRoomId = pRoom->GetRoomId();
		stItem.m_pBtlUser = nullptr;
		m_mapConvToRoom[iter.second] = stItem;
	}
}

void CUdpLogic::RemoveRoom(CRoom* pRoom)
{
	auto& mapConv = pRoom->GetConvList();
	for (auto& iter : mapConv)
	{
		m_mapConvToRoom.erase(iter.second);
	}
}

void CUdpLogic::Stop()
{
	m_bStoped = true;
}

void CUdpLogic::Fini()
{
	m_bStoped = true;
}

void CUdpLogic::OnTimer(int iTime, int64_t llTick)
{
	if (m_oTmrTempEnd.On(iTime))
	{
		if (m_mapTempEnd.size() > 0) m_mapTempEnd.clear();
	}

	if (m_pRoomNet != nullptr)
		m_pRoomNet->OnTimer(iTime, llTick);
}

void CUdpLogic::Tick(int64_t llTick)
{
	if (m_mapConvToRoom.size() == 0) return;

	CUdpSess* pSess = nullptr;
	for (auto& iter : m_mapConvToRoom)
	{
		if (iter.second.m_pBtlUser != nullptr)
		{
			pSess = iter.second.m_pBtlUser->GetUdpSess();
			if (pSess != nullptr)
				pSess->Tick(llTick);
		}
	}
}

bool CUdpLogic::SendUdp(const char* pBuf, int iLen, const ip::udp::endpoint& oEndPoint)
{
	try
	{
		m_oUdpSocket.send_to(boost::asio::buffer(pBuf, iLen), oEndPoint);
		return true;
	}
	catch (const std::runtime_error& err)
	{
		Log_Error("error!:%s, endpoint:%lld", err.what(), CUdpSess::EndpointToI(oEndPoint));
	}
	return false;
}

void CUdpLogic::SetOnline(CUdpSess* pUdp)
{
	if (pUdp == nullptr || pUdp->GetUdpPointKey() == 0)
		return;
	
	auto iter = m_mapSessOnline.find(pUdp->GetUdpPointKey());
	if (iter != m_mapSessOnline.end() && iter->second != nullptr &&
		iter->second != pUdp && iter->second->GetUserId() != pUdp->GetUserId())
	{
		Log_Error("kill old fd, userid1=%lld, userid2=%lld", iter->second->GetUserId(), pUdp->GetUserId());
		iter->second->on_disconnect();
	}
	m_mapSessOnline[pUdp->GetUdpPointKey()] = pUdp;
}

void CUdpLogic::RemoveOnline(CUdpSess* pUdp)
{
	if (pUdp != nullptr && pUdp->GetUdpPointKey() != 0)
	{
		auto iter = m_mapSessOnline.find(pUdp->GetUdpPointKey());
		if (iter != m_mapSessOnline.end())
			m_mapSessOnline.erase(iter);
		else
			Log_Error("kill error!!! userid=%lld", pUdp->GetUserId());
	}
}

tagMsgHead* CUdpLogic::Gethead(size_t uiBytesRecvd)
{
	if (uiBytesRecvd > MAX_UDP_BUFF)
		return nullptr;

	if (uiBytesRecvd < (NET_HEAD_SIZE + KCP_HEAD_SIZE))
		return nullptr;

	tagMsgHead* pHead = (tagMsgHead*)(m_arrUdpData + KCP_HEAD_SIZE);
	if (pHead->uiFlag >> 12 != 0x8)
		return nullptr;

	if (uiBytesRecvd < (pHead->uiLen + KCP_HEAD_SIZE))
		return nullptr;

	return pHead;
}


void CUdpLogic::PushLinkResult(ResultCode eCode, int iReqId, int iTimes)
{
	//Msg_Common_Empty oRsp;
}

void CUdpLogic::SendEmptyBuf(const tagMsgHead* pMsg)
{
	memcpy(m_arrErrBuf + KCP_HEAD_SIZE, pMsg, pMsg->uiLen);
	SendUdp(m_arrErrBuf, pMsg->uiLen + KCP_HEAD_SIZE, m_oUdpEndPoint);
}

CBattleUser* CUdpLogic::GetUserByConv(uint32_t uiConvId, int64_t& llRoomId, int64_t& llUserId)
{
	if (uiConvId == 0) return nullptr;

	for (int i = 0; i < 2; ++i)
	{
		auto iter = m_mapConvToRoom.find(uiConvId);
		if (iter != m_mapConvToRoom.end())
		{
			llRoomId = iter->second.m_llRoomId;
			llUserId = iter->second.m_llUserId;
			return iter->second.m_pBtlUser;
		}
		else
		{
			if (!m_pRoomNet->PopAdd())
				return nullptr;
		}
	}
	return nullptr;
}

void CUdpLogic::HandleUdpReceiveFrom(const boost::system::error_code& oError, size_t uiBytesRecvd)
{
	static int iCount = 0;
	if (uiBytesRecvd > 0)
		HandleKcpPacket(uiBytesRecvd);
	else
	{
		if ((++iCount) % 1000 == 0)
		{
			Log_Error("error end! error:%s, byte recvd:%ld, count:%d, errorid:%d",
				oError.message().c_str(), uiBytesRecvd, iCount, CUdpSess::EndpointToI(m_oUdpEndPoint), oError.value());
		}
	}
	HookUdpAsyncReceive();
}

void CUdpLogic::HookUdpAsyncReceive(void)
{
	if (m_bStoped)
		return;

	m_oUdpSocket.async_receive_from(boost::asio::buffer(m_arrUdpData, sizeof(m_arrUdpData)), m_oUdpEndPoint,
		boost::bind(&CUdpLogic::HandleUdpReceiveFrom, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void CUdpLogic::HandleKcpTimer(void)
{
	HookKcpTimer(TICK_MILL_SEC);

	int64_t llCur = GetMillSec();
	if (m_llTickMs == 0) m_llTickMs = llCur;

	OnTimer(TICK_MILL_SEC, llCur);
	if (m_llIndex != 0 && m_llIndex % 5 == 0)
	{
		int64_t llSub = llCur - m_llTickMs;
		m_llTickMs = llCur;
		if (llSub >= 100)
		{
			++m_uiOutSize;
			if (m_uiOutSize == 1 || m_uiOutSize % 5 == 0)
				Log_Error("timer error!!!, subms:%lld, outimes:%d, port:%u", llSub, m_uiOutSize, m_iPort);
		}
	}
	++m_llIndex;
}

void CUdpLogic::HookKcpTimer(int64_t llMicroSec)
{
	if (m_bStoped)
	{
		m_oUdpSocket.cancel();
		m_oIoService.stop();
		return;
	}
	m_oKcpTimer.expires_at(m_oKcpTimer.expires_at() + boost::posix_time::milliseconds(llMicroSec));
	m_oKcpTimer.async_wait(boost::bind(&CUdpLogic::HandleKcpTimer, this));
}

void CUdpLogic::HandleKcpPacket(size_t uiBytesRecvd)
{
	if (m_pRoomNet == nullptr) return;

	m_uiBytesRecvd = uiBytesRecvd;
	IUINT32 uiConv;
	int iRet = ikcp_get_conv(m_arrUdpData, (long)uiBytesRecvd, &uiConv);
	if (iRet == 0 || uiConv == 0)
	{
		tagMsgHead* pHead = Gethead(uiBytesRecvd);
		if (pHead == nullptr || pHead->usModuleId != ProtoMsg::Battle)
		{
			//不回执任何消息,防止被钓鱼,网络带宽猛增!!!
			Log_Error("error head error~!");
			return;
		}

		//测试用的心跳的直接发送出来
		if (pHead->uiCmdId == 1)
		{
			if (AddTempEnd(m_oUdpEndPoint, 10))
				SendEmptyBuf(pHead);				//心跳原数据返回
		}
		else if (pHead->uiCmdId == MsgModule_Battle::Msg_Battle_EnterRoom_Req)
		{
			m_iCurConvId = pHead->uiSeqid;
			OnRecvDataInit(pHead);
		}
		return;
	}
	m_iCurConvId = uiConv;
	OnRecvData();
}

CBattleUser* CUdpLogic::EnterRoomUdp(uint32_t uiConvId, int64_t llRoomId, int64_t llUserId)
{
	if (llRoomId == 0 || llUserId == 0) return nullptr;
	CBattleUser* pBtlUser = nullptr;
	CRoom* pRoom = m_pRoomNet->GetRoom(llRoomId);
	//房间已经结束
	if (pRoom == nullptr)
	{
		Log_Error("room error! roomid:%lld, convid:%u", llRoomId, uiConvId);
		return nullptr;
	}

	//房间已经结束
	if (pRoom->IsEnd() || pRoom->IsAllOver())
	{
		Log_Error("room end error! roomid:%lld, convid:%u", llRoomId, uiConvId);
		return nullptr;
	}

	ProtoMsg::user_battle_info_t* pUserInfo = pRoom->GetUserDataById(llUserId, true);
	if (pUserInfo == nullptr)
	{
		Log_Error("room user error! roomid:%lld, convid:%u, user:%lld", llRoomId, uiConvId, llUserId);
		return nullptr;
	}
	pBtlUser = m_pRoomNet->GetUser(pUserInfo->ouserinfo().lluid());
	if (pBtlUser != nullptr)
	{
		if (pBtlUser->GetRoomId() != 0 && pBtlUser->GetRoomId() != pRoom->GetRoomId())
		{
			bool bOb = pBtlUser->IsOb();
			Log_Error("user has enter other room! roomid:%lld, userid:%lld, ob:%d",
				pRoom->GetRoomId(), llUserId, bOb);
			//退出
			pBtlUser->on_disconnect();
			return nullptr;
		}
	}
	else
	{
		pBtlUser = m_pRoomNet->CreateUser(pUserInfo->ouserinfo().lluid());
		if (pBtlUser == nullptr)
		{
			Log_Error("create user error! roomid:%lld, userid:%lld", pRoom->GetRoomId(), llUserId);
			return nullptr;
		}
		else
		{
			Log_Custom("enter", "create user! roomid:%lld, userid:%lld", pRoom->GetRoomId(), llUserId);
		}
	}
	pBtlUser->SetRoom(pRoom);
	pBtlUser->SetUserBaseData(*pUserInfo);
	pBtlUser->InitUdp();
	pBtlUser->GetUdpSess()->SetUdpLogic(this);
	pRoom->Enter(pBtlUser);

	return pBtlUser;
}

CBattleUser* CUdpLogic::OnRecvDataUser(uint32_t uiConvId, int64_t& llRoomId, int64_t& llUserId)
{
	CBattleUser* pBtlUser = GetUserByConv(uiConvId, llRoomId, llUserId);
	if (llUserId == 0 || llRoomId == 0)
	{
		uint32 uiTemp = AddTempEnd(m_oUdpEndPoint, 10);
		if (uiTemp > 0)
		{
			PushLinkResult(Code_Match_RoomRelease, 0);
			Log_Error("error conv:%u, roomid:%lld, user:%lld, times:%u", uiConvId, llRoomId, llUserId, uiTemp);
		}
		return nullptr;
	}
	return pBtlUser;
}

void  CUdpLogic::OnRecvData()
{
	if (m_pRoomNet != nullptr)
	{
		bool bHave = false;
		int64_t llRoomId = 0, llUserId = 0;
		CBattleUser* pBtlUser = OnRecvDataUser(m_iCurConvId, llRoomId, llUserId);
		if (llRoomId == 0 || llUserId == 0) return;
		CUdpSess* pUdp = nullptr;
		if (pBtlUser != nullptr)
		{
			pUdp = pBtlUser->GetUdpSess();
			if (pUdp != nullptr)
			{
				if (pUdp->GetUdpPoint() != m_oUdpEndPoint)
				{
					//已经在被T队列的不予处理
					if (pBtlUser->IsBeginKill())
					{
						SendEndRoomResult(llRoomId);
						Log_Warning("user has kill conv:%u, roomid:%lld, userid:%lld", m_iCurConvId, llRoomId, llUserId);
						return;
					}

					//最近接受和不是最近建立连接的,瞬间变化网络的,可以直接处理
					//2s内有限的支持10次,防止作弊
					if ((pUdp->GetLastTime() + 1) >= GetCurrTime()
						&& (pUdp->GetCreateMs() + 1000) <= GetTickCount()
						&& pUdp->AddResetTimes() <= 10)
					{
						Log_Custom("enter", "reset udp point success!! userid:%lld, %s->%s, lastrecv:%lld",
							llUserId, pUdp->GetIpStr().c_str(), m_oUdpEndPoint.address().to_string().c_str(),
							GetCurrTime() - pUdp->GetLastTime());
						pUdp->InitEndPoint(m_oUdpEndPoint);
						SetOnline(pUdp);
						pBtlUser->InitIp();
					}
					else
					{
						Log_Custom("enter", "reset udp point error!! userid:%lld, %s->%s, lastrecv:%lld",
							llUserId, pUdp->GetIpStr().c_str(), m_oUdpEndPoint.address().to_string().c_str(),
							GetCurrTime() - pUdp->GetLastTime());
						PushLinkResult(Code_Match_NotEnterRoom, 0);
						pUdp->on_disconnect();
						return;
					}
				}
				pUdp->Input(m_arrUdpData, m_uiBytesRecvd);
			}
		}
		else
		{
			uint32 uiTemps = AddTempEnd(m_oUdpEndPoint, 10);
			if (uiTemps > 0)
			{
				SendEndRoomResult(llRoomId);
				Log_Error("error conv:%d, roomid:%lld, userid:%lld, times:%u", m_iCurConvId, llRoomId, llUserId, uiTemps);
			}
		}
	}
}

void CUdpLogic::OnRecvDataInit(tagMsgHead* pHead)
{
	if (m_pRoomNet != nullptr)
	{
		bool bHave = false;
		int64_t llRoomId = 0, llUserId = 0;
		CBattleUser* pBtlUser = OnRecvDataUser(m_iCurConvId, llRoomId, llUserId);
		if (llRoomId == 0 || llUserId == 0) return;

		CUdpSess* pUdp = nullptr;
		if (pBtlUser != nullptr)
		{
			//已经在被T队列的不予处理
			if (pBtlUser->IsBeginKill())
			{
				SendEndRoomResult(llRoomId);
				Log_Warning("user has kill conv:%d, roomid:%lld, userid:%lld", m_iCurConvId, llRoomId, llUserId);
				return;
			}
			PushLinkResult(Code_Common_Success, pHead->uiSeqid);
			Log_Custom("enter", "reset udp point success!! userid:%lld, %s->$s", 
				llUserId, pUdp->GetIpStr().c_str(), m_oUdpEndPoint.address().to_string().c_str());

			if (pUdp->GetUdpPointKey() != 0 && pUdp->GetUdpPoint() != m_oUdpEndPoint)
				RemoveOnline(pUdp);

			if (pUdp->GetAllRecv() != 0 || pUdp->GetUdpPoint() != m_oUdpEndPoint)
				pUdp = pBtlUser->InitUdp();

			pUdp->InitEndPoint(m_oUdpEndPoint);
			SetOnline(pUdp);

			pBtlUser->InitIp();
		}
		else 
		{
			pBtlUser = EnterRoomUdp(m_iCurConvId, llRoomId, llUserId);
			if (pBtlUser != nullptr)
			{
				PushLinkResult(Code_Common_Success, pHead->uiSeqid);
				pUdp = pBtlUser->GetUdpSess();
				pUdp->InitEndPoint(m_oUdpEndPoint);
				SetOnline(pUdp);
				pBtlUser->InitIp();
			}
			else
			{
				PushLinkResult(Code_Match_RoomRelease, pHead->uiSeqid);
				Log_Error("error enter conv:%d, roomid:%lld, userid:%lld", m_iCurConvId, llRoomId, llUserId);
			}
		}
	}
}

int CUdpLogic::AddTempEnd(ip::udp::endpoint& oEndPoint, int iTimes)
{
	uint64_t ullEnd = CUdpSess::EndpointToI(m_oUdpEndPoint);
	auto iterTemp = m_mapTempEnd.find(ullEnd);
	if (iterTemp == m_mapTempEnd.end())
	{
		m_mapTempEnd.insert(std::make_pair(ullEnd, 1));
		return 1;
	}
	else
	{
		if (iTimes <= iterTemp->second)
			return 0;

		++(iterTemp->second);
		return iterTemp->second;
	}
	return 0;
}

bool CUdpLogic::IsEndRoom(int64_t llRoomId)
{
	CRoom* pRoom = m_pRoomNet->GetRoom(llRoomId);
	//房间已经结束
	if (pRoom == nullptr)
		return true;

	//房间已经结束
	if (pRoom->IsEnd() || pRoom->IsAllOver())
		return true;

	return false;
}

void CUdpLogic::SendEndRoomResult(int64_t llRoomId)
{
	CRoom* pRoom = m_pRoomNet->GetRoom(llRoomId);
	//房间已经结束
	if (pRoom == nullptr || pRoom->IsEnd() || pRoom->IsAllOver())
	{
		//PushLinkResult(Code_Match_RoomRelease, 0);
	}
	else
	{
		PushLinkResult(Code_Match_NotEnterRoom, 0);
	}
}