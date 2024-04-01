#include "CModuleProFactory.h"
#include "CErrorProcessor.h"
#include "msg_module.pb.h"
#include "log_mgr.h"

CModuleProFactory* CModuleProFactory::m_pModuleCmdMap = nullptr;

CModuleProFactory::CModuleProFactory()
{
	m_pErrorProcessor = std::static_pointer_cast<CProcessorBase>(std::make_shared<CErrorProcessor>());

	//预设协议号最大值,协议号在内存中规则(moduleId*1000 + cmdId)
	const size_t uiArraySize = ProtoMsg::MsgModule::End * 100;
    m_mapModuleProFactory.resize(uiArraySize);
}

CModuleProFactory& CModuleProFactory::GetInstance()
{
	if (nullptr == m_pModuleCmdMap)
		m_pModuleCmdMap = new CModuleProFactory();

	return (*m_pModuleCmdMap);
}


processor_base_ptr_type CModuleProFactory::GetProcessor(uint32_t uiModuleId)
{
    if (nullptr == m_pModuleCmdMap)
    {
        Log_Error("module pro factory is nullptr!!!");
        return (m_pErrorProcessor);
    }

    std::function<processor_base_ptr_type()> pCreateProFunc = m_mapModuleProFactory[uiModuleId];
    if (nullptr == pCreateProFunc)
    {
        Log_Error("module cmd is not register, moduleId:%d", uiModuleId);
        return (m_pErrorProcessor);
    }

    processor_base_ptr_type pProcessorObj = pCreateProFunc();

    return (pProcessorObj);
}

void CModuleProFactory::RegisterProcessorFactory(uint32_t uiModuleId, processor_function_type pFactory)
{
    if ((uiModuleId > 0) && (nullptr != pFactory))
    {
        m_mapModuleProFactory[uiModuleId] = std::bind(pFactory, uiModuleId);
    }
}
