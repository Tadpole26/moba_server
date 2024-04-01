#pragma once
#include <string>
#include <memory>
#include "result_code.pb.h"
using namespace ProtoMsg;

class CUser;
class CProcessorBase
{
public:
    CProcessorBase(uint32_t uiModuleId);

	virtual ~CProcessorBase();

    void SetUserBaseInfo(CUser* pUser, uint32_t uiCmd);

	bool DoProcess(const std::string& strMsg);

    void SendToClient(uint32_t uiRspModule, uint32_t uiRspCmd, google::protobuf::Message* pRspMsg, 
        ResultCode eCode = Code_Common_Success, bool bNotify = false);

protected:
	virtual bool DoUserRun(const std::string& strMsg) = 0;

protected:
	uint32_t m_uiModuleId;
    uint32_t m_uiCmd;
    CUser* m_pUser;
};


/////////////////////////构造智能指针和对应得constructor&deletor//////////////////////////////
template <class T>
class DefaultProcessorCreator
{
public:
    static T* Create(int32_t uiModuleId)
    {
        return (new T(uiModuleId));
    }

    static void Destory(CProcessorBase* pProcessor)
    {
        delete pProcessor;
    }
};

using processor_base_ptr_type = std::shared_ptr<CProcessorBase>;

template <class T, class CreatorT = DefaultProcessorCreator<T> >
class CProcessorFactory
{
public:
    static processor_base_ptr_type Create(int32_t uiModuleId)
    {
        return (processor_base_ptr_type(CreatorT::Create(uiModuleId), CreatorT::Destory));
    };
};