#include "CZkOperation.h"
#include <cassert>
#include "global_define.h"
#include "log_mgr.h"

#include "CHeroConfig.h"
#include "CUserConfig.h"
#include "CItemConfig.h"
#include "CGameConstConfig.h"

const char* ZkOperation::Type2String(int iEventType)
{
    if (iEventType == ZOO_CREATED_EVENT) return ("CREATED_EVENT");
    else if (iEventType == ZOO_DELETED_EVENT) return ("DELETED_EVENT");
    else if (iEventType == ZOO_CHANGED_EVENT) return ("CHANGED_EVENT");
    else if (iEventType == ZOO_CHILD_EVENT) return ("CHILD_EVENT");
    else if (iEventType == ZOO_SESSION_EVENT) return ("SESSION_EVENT");
    else if (iEventType == ZOO_NOTWATCHING_EVENT) return ("NOTWATCHING_EVENT");

    return ("UNKNOWN_EVENT_TYPE");
}

const char* ZkOperation::State2String(int iState)
{
    if (iState == 0) return ("CLOSED_STATE");
    else if (iState == ZOO_CONNECTING_STATE) return ("CONNECTING_STATE");
    else if (iState == ZOO_ASSOCIATING_STATE) return ("ASSOCIATING_STATE");
    else if (iState == ZOO_CONNECTED_STATE) return ("CONNECTED_STATE");
    else if (iState == ZOO_EXPIRED_SESSION_STATE) return ("EXPIRED_SESSION_STATE");
    else if (iState == ZOO_AUTH_FAILED_STATE) return ("AUTH_FAILED_STATE");

    return ("INVALID_STATE");
}

const char* ZkOperation::Errno2String(int iRet)
{
    return (zerror(iRet));
}

//////////////////////////////////////////////////////////////////////
void ZkOperation::InitAddWatchFile()
{
    AddConfigFile("user_config", USER_CFG_INS, this);
    AddConfigFile("hero_config", HERO_CFG_INS, this);
    AddConfigFile("item_config", ITEM_CFG_INS, this);
    AddConfigFile("game_const_config", GAME_CONST_CFG_INS, this);
}

void ZkOperation::AddConfigFile(const std::string& strFileName, CUtilLoadBase* pLoadBase, ZkOperation* pZookeeperConfig)
{
    std::string strFullName;
    strFullName = m_strNormalConfigPath + "/" + strFileName;
 

    auto stIter = m_stConfigLoadMap.find(strFullName);
    if (stIter == m_stConfigLoadMap.end())
    {
        m_stConfigLoadMap[strFullName] = ZkOperation::config_map_type_t::mapped_type(pLoadBase, pZookeeperConfig);
    }
}

bool ZkOperation::IsLoadComplete()
{
    return (m_uiHasCompletedNum == m_stConfigLoadMap.size());
}

bool ZkOperation::UpdateConfig(const std::string& strFileName, const std::string& strContent)
{
    auto stIter = m_stConfigLoadMap.find(strFileName);
    if (stIter == m_stConfigLoadMap.end())
        return false;
    bool bLoad = stIter->second.first->LoadConfig(strContent);
    if (bLoad) m_uiHasCompletedNum++;
    return bLoad;
}

void ZkOperation::DeleteConfigFile(const std::string& strFileName)
{
    auto stIter = m_stConfigLoadMap.find(strFileName);
    if (stIter == m_stConfigLoadMap.end())
        return;

    m_stConfigLoadMap.erase(strFileName);
}

bool ZkOperation::InitZookeeper(std::string& strHost, std::string& strNormalPath)
{
    m_strHost = strHost;
    m_strNormalConfigPath = strNormalPath;
    //��ʼ����������load����
    InitAddWatchFile();
    // enable deterministic order
    //zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
    //zoo_deterministic_conn_order(1);
 
    m_pZooHandle = zookeeper_init(m_strHost.c_str(), ZooHandleEventWatcher, 60000, NULL, (void*)this, 0);
    assert(m_pZooHandle != nullptr && "zookeeper init fault!!!");

    int iRet = CreateServerNode();
    if (ZOK != iRet)
    {
        Log_Error("create server node error|path:%s|errcode:%d|errmsg:%s",
            m_strNormalConfigPath.c_str(),
            iRet,
            Errno2String(iRet));
        goto fail;
    }

    //��һ��������ȡ���������ļ�
    if (!WaitAllConfigReady(-1))
    {
        Log_Error("select load config error");
    }

    return (true);

fail:
    DestoryZookeeper();
    return (false);
}

void ZkOperation::DestoryZookeeper()
{
    zookeeper_close(m_pZooHandle);
    m_pZooHandle = nullptr;
}

// �����ڵ�(/normal_config)
int ZkOperation::CreateServerNode()
{
    return (zoo_acreate(
        m_pZooHandle,							                //zk��� 
        m_strNormalConfigPath.c_str(), 	                        //�ڵ�·��
        "", 									                //�ýڵ㱣�������
        0,										                //�ýڵ㱣�����ݴ�С 
        &ZOO_OPEN_ACL_UNSAFE,
        0,
        CreateServerNodeComplete,				                //���������Ļص����� 
        (void*)this));							                //�Զ�������
}

// �����ӽڵ���ɾ(/normal_config),���ܶ����ӽڵ����ݱ仯
int ZkOperation::WatchAndGetMapConfigList()
{
    return (zoo_awget_children(
        m_pZooHandle,                                           //zk��� 
        m_strNormalConfigPath.c_str(),                          //�ڵ�·��
        MapConfigListEventWatcher,                              //������ɾwatch�ص�
        this,                                                   //������ɾwatch�ص�����
        MapConfigListEventComplete,                             //���Ľ����ص�
        this));                                                 //���Ľ����ص�����
}
// �����ӽڵ����ݱ仯(/normal_config)
int ZkOperation::WatchAndGetConfigContent()
{
    int iRet = ZOK;
    for (auto& stConfigKV : m_stConfigLoadMap)
    {
        iRet = zoo_awget(
            m_pZooHandle,                                       //zk��� 
            stConfigKV.first.c_str(),                           //�ڵ����·��
            ConfigContentEventWatcher,                          //���ݷ����ı�watch�ص�
            &stConfigKV,                                        //���ݷ����ı�watch�ص�����
            ConfigContentEventComplete,                         //���Ľڵ�������ɻص�
            &stConfigKV);                                       //���Ľڵ����ݻص�����
        if (iRet != ZOK)
        {
            Log_Error("watch config error|config:%s|errcode:%d|errmsg:%s",
                stConfigKV.first.c_str(),
                iRet,
                Errno2String(iRet));
            break;
        }
    }

    return (iRet);
}

void ZkOperation::SetErrorInfo(const std::string& strErrorInfo) const
{
    zoo_aset(m_pZooHandle, m_strNormalConfigPath.c_str(), strErrorInfo.c_str(), int(strErrorInfo.length()), -1, SetDataSilentComplete, nullptr);
}


//����������loadȫ������
bool ZkOperation::WaitAllConfigReady(int iTimeout)
{
    if (m_pZooHandle == nullptr)
    {
        Log_Error("zookeeper handle is nullptr");
        return false;
    }

    fd_set rfds, wfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

#ifdef WIN32
    SOCKET iZookeeperFd;
#else
    int iZookeeperFd = -1;
#endif

    struct timeval stTv;
    struct timeval* pTimeout = NULL;
    if (iTimeout >= 0)
    {
        stTv.tv_sec = decltype(stTv.tv_sec)(iTimeout / 1000);
        stTv.tv_usec = decltype(stTv.tv_usec)((iTimeout % 1000) * 1000);
        pTimeout = &stTv;
    }

    while (true)
    {
        int iCurrentZookeeperIOEvent = 0;
        struct timeval stTmp;
        if (m_pZooHandle == nullptr)
        {
            Log_Error("zookeeper handle is nullptr");
            return (false);
        }

        int iRet = zookeeper_interest(m_pZooHandle, &iZookeeperFd, &iCurrentZookeeperIOEvent, &stTmp);
        if (iRet != ZOK)
        {
            Log_Error("zk interest error|errcode:%d|errmsg:%s",
                iRet,
                Errno2String(iRet));
            DestoryZookeeper();
            return (false);
        }

        if (iCurrentZookeeperIOEvent & ZOOKEEPER_READ) FD_SET(iZookeeperFd, &rfds);
        else FD_CLR(iZookeeperFd, &rfds);

        if (iCurrentZookeeperIOEvent & ZOOKEEPER_WRITE) FD_SET(iZookeeperFd, &wfds);
        else FD_CLR(iZookeeperFd, &wfds);

        iCurrentZookeeperIOEvent = 0;
        iRet = select(int(iZookeeperFd + 1), &rfds, &wfds, &efds, pTimeout);
        if (iRet > 0)
        {
            if (FD_ISSET(iZookeeperFd, &rfds)) iCurrentZookeeperIOEvent |= ZOOKEEPER_READ;
            if (FD_ISSET(iZookeeperFd, &wfds)) iCurrentZookeeperIOEvent |= ZOOKEEPER_WRITE;
        }
        else
        {
            if (iRet == 0)
            {
                Log_Error("wait config complete timeout");
            }
            else
            {
                Log_Error("wait config complete error|errcode:%d|errmsg:%s",
                    errno,
                    Errno2String(errno));
                if (errno != EINTR) return (false);
            }
        }

        zookeeper_process(m_pZooHandle, iCurrentZookeeperIOEvent);
        if (IsLoadComplete())
        {
            return (true);
        }
    }
}



///////////////////////////////////watch&complete�ص�/////////////////////////////////////

void ZkOperation::ZooHandleEventWatcher(
    zhandle_t* pZooHandle,
    int iType,
    int iState,
    const char* szPath,
    void* pContext)
{
    ZkOperation* pSelf = (ZkOperation*)pContext;
    Log_Custom("zk_watch", "watcher event|type:%s|state:%s",
        Type2String(iType),
        State2String(iState));

    if (iType == ZOO_SESSION_EVENT)
    {
        if (iState == ZOO_CONNECTED_STATE)
        {
            const clientid_t* pClientID = zoo_client_id(pZooHandle);
            Log_Custom("zk_watch", "get a new session id|%d", pClientID->client_id);
        }
        else if (iState == ZOO_AUTH_FAILED_STATE)
        {
            Log_Error("zookeeper authentication failure");
            pSelf->DestoryZookeeper();
        }
        else if (iState == ZOO_EXPIRED_SESSION_STATE)
        {
            Log_Error("session expired");
            pSelf->DestoryZookeeper();
        }
    }
}

//��������ڵ�ص�
void ZkOperation::CreateServerNodeComplete(
    int iRet,
    const char* pName,
    const void* pContext)
{
    ZkOperation* pSelf = (ZkOperation*)pContext;
    //�ڵ㴴����ɻ�ڵ��Ѵ���
    if (ZOK == iRet || ZNODEEXISTS == iRet)
    {
        int iMapRet = pSelf->WatchAndGetMapConfigList();
        if (ZOK != iMapRet)
        {
            Log_Error("watch map config list error|path:%s|errcode:%d|errmsg:%s",
                pSelf->m_strNormalConfigPath.c_str(),
                iMapRet,
                Errno2String(iMapRet));
            pSelf->DestoryZookeeper();
        }
    }
    else
    {
        std::string strPath;
        if (pName) strPath = pName;
        Log_Error("create server node error|path:%s|errcode:%d|errmsg:%s",
            strPath.c_str(),
            iRet,
            Errno2String(iRet));
        pSelf->DestoryZookeeper();
    }
}

//��ͨ�����ļ��б�״̬�Ļص�(������ɾ��������,��Ϊ��Ӧ���ô���Ҳ��Ķ�)
void ZkOperation::MapConfigListEventWatcher(
    zhandle_t* pZooHandle,
    int iType, int iState,
    const char* szPath,
    void* pContext)
{
    if (ZOO_SESSION_EVENT == iType)
    {
        Log_Custom("zk_watch", "status notify|type_id:%d|type_name|%s|state_id:%d|state_name:%s",
            iType, 
            Type2String(iType),
            iState,
            State2String(iState));
        return;
    }

    ZkOperation* pSelf = (ZkOperation*)pContext;
    if (pSelf->m_strNormalConfigPath.compare(szPath) != 0)
    {
        Log_Custom("maybe an old path watcher:%s|normal config path|%s",
            szPath,
            pSelf->m_strNormalConfigPath.c_str());
        return;
    }

    Log_Custom("zk_watch", "normal config notify|path:%s|type_id:%d|type:%s|state_id|%d|state:%s", 
        szPath,
        iType,
        Type2String(iType),
        iState,
        State2String(iState));

    //ֻ�ᴥ��һ��,���Իص��м�����ע
    int iRet = pSelf->WatchAndGetMapConfigList();
    if (ZOK != iRet)
    {
        Log_Error("watch normal config list error|errcode:%d|errmsg:%s",
            iRet,
            Errno2String(iRet));
        pSelf->DestoryZookeeper();
    }
}

//��ͨ�����ļ��б�״̬��ע���
void ZkOperation::MapConfigListEventComplete(
    int iRet,
    const struct String_vector* pStrings,
    const void* pContext)
{
    ZkOperation* pSelf = (ZkOperation*)pContext;
    if (ZOK == iRet)
    {
        for (int i = 0; i < pStrings->count; i++)
        {
            //�����µ�����,һ������д��ȡ����,Ҫ��������
            std::string strFullNormalPath = pSelf->m_strNormalConfigPath + "/" + pStrings->data[i];
            //pSelf->AddConfigFile(strFullNormalPath, pSelf);
            //Log_Custom("zk_complete", "normal config list complete:%s", strFullNormalPath.c_str());
        }

        iRet = pSelf->WatchAndGetConfigContent();
        if (ZOK != iRet)
        {
            Log_Error("watch normal config list error|errcode:%d|errmsg:%s",
                iRet,
                Errno2String(iRet));
            pSelf->DestoryZookeeper();
        }
    }
    else
    {
        Log_Error("map config list event complete error|errcode:%d|errmsg:%s",
            iRet,
            Errno2String(iRet));
        pSelf->DestoryZookeeper();
    }
}

//�����ļ�����״̬�Ļص�
void
ZkOperation::ConfigContentEventWatcher(
    zhandle_t* pZooHandle,
    int iType,
    int iState,
    const char* szPath,
    void* pContext)
{
    if (ZOO_SESSION_EVENT == iType)
    {
        Log_Custom("zk_watch", "status notify|type_id:%d|type_name|%s|state_id:%d|state_name:%s",
            iType,
            Type2String(iType),
            iState,
            State2String(iState));
        return;
    }

    Log_Custom("zk_watch", "config content notify|path:%s|type_id:%d|type:%s|state_id:%d|state:%s",
        szPath,
        iType,
        Type2String(iType),
        iState,
        State2String(iState));

    ZkOperation::config_map_type_t::value_type* pValue = (ZkOperation::config_map_type_t::value_type*)pContext;
    ZkOperation* pSelf = pValue->second.second;
    if (ZOO_DELETED_EVENT == iType)
    {
        pSelf->DeleteConfigFile(std::string(szPath));
        Log_Custom("zk_watch", "delete config|path:%s", szPath);
    }
    else
    {
        //���ýڵ����ݷ����ı�,�������ˢ������,��������watch�ص�
        int iRet = zoo_awget(pSelf->m_pZooHandle, szPath, ConfigContentEventWatcher, pValue, ConfigContentEventComplete, pValue);
        if (ZOK != iRet)
        {
            Log_Error("watch config error|path:%s|errcode:%d|errmsg:%s",
                szPath,
                iRet,
                Errno2String(iRet));
            pSelf->DestoryZookeeper();
        }
    }
}
//�����ļ����ݻ�����
void ZkOperation::ConfigContentEventComplete(
    int iRet,
    const char* iValue,
    int iLength,
    const struct Stat* stStat,
    const void* pContext)
{
    (void)stStat;
    ZkOperation::config_map_type_t::value_type* pValue = (ZkOperation::config_map_type_t::value_type*)pContext;
    ZkOperation* pSelf = pValue->second.second;
    if (ZOK == iRet)
    {
        pSelf->UpdateConfig(pValue->first, std::string(iValue, iLength));
        Log_Custom("zk_complete", "config content complete|path:%s", pValue->first.c_str());
    }
    else
    {
        Log_Error("config content complete error|path:%s|errcode:%d|errmsg:%s",
            pValue->first.c_str(),
            iRet,
            Errno2String(iRet));
        pSelf->DestoryZookeeper();
    }
}
