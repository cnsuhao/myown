//--------------------------------------------------------------------
// 文件名:      AITBoss.h
// 内  容:      BOSS模板
// 说  明:
// 创建日期:    2014年12月31日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#pragma once

#include "AITemplateBase.h"

//AI普通战斗类模版类
class AITBoss : public AITemplateBase
{
public:
	AITBoss(void);

	~AITBoss();

    //基本行为结束的消息处理
    virtual int	OnBasicActionMsg(IKernel* pKernel, const PERSISTID& self, int msg, const IVarList& args = CVarList());


};
