#pragma once

#include <functional>
#include <memory>
#include <stdio.h>
#include "msg_module.pb.h"
#include "CProcessorBase.h"
#include "CErrorProcessor.h"
/*
    ����ģʽ
    ������Э��źͶ�Ӧprocessor�����
*/

#define _REGISTER_MODULE_FACTORY_LINE(MODULE, CLASS_NAME, LINE_NUM)                                                                     \
struct _static_##CLASS_NAME##LINE_NUM                                                                                                   \
{                                                                                                                                       \
    _static_##CLASS_NAME##LINE_NUM()                                                                                                    \
    {                                                                                                                                   \
         CModuleProFactory::GetInstance().RegisterProcessorFactory(MODULE, &CProcessorFactory<CLASS_NAME>::Create);                     \
    }                                                                                                                                   \
};                                                                                                                                      \
static _static_##CLASS_NAME##LINE_NUM g_stInstance##CLASS_NAME##LINE_NUM;

//�����һ��,��Ϊֱ����REGISTER_CMD_FACTORY_WITH_LINE_NUM����ִ���ȥ��__LINE__��������
#define REGISTER_MODULE_FACTORY_LINE(MODULE, CLASS_NAME, LINE_NUM) _REGISTER_MODULE_FACTORY_LINE(MODULE, CLASS_NAME, LINE_NUM)
#define REGISTER_MODULE_FACTORY(MODULE, CLASS_NAME) REGISTER_MODULE_FACTORY_LINE(MODULE, CLASS_NAME, __LINE__)


class CModuleProFactory
{
public:
    using processor_function_type = processor_base_ptr_type(*)(int32_t uiModuleId);

public:
    static CModuleProFactory&            GetInstance();

    processor_base_ptr_type			    GetProcessor(uint32_t uiModuleId);

    void                                RegisterProcessorFactory(uint32_t uiModuleId, processor_function_type pFactory);

private:
    CModuleProFactory();

private:
    static CModuleProFactory*                                 m_pModuleCmdMap;

private:
    processor_base_ptr_type                                   m_pErrorProcessor;

    std::vector<std::function<processor_base_ptr_type()>>     m_mapModuleProFactory;
};

#define PROCESSOR_FACTORY_INS CModuleProFactory::GetInstance()