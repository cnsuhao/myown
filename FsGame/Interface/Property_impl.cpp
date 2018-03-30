
//----------------------------------------------------------
// 文件名:      PropertyInterface.h
// 内  容:      技能、buffer、通用属性查询提供的接口
// 创建人:      袁磊
// 创建日期:    2014年7月
//    :        
//----------------------------------------------------------
#include "PropertyInterface.h"

bool FightPropertyInterface::Query(IGameObj* pGameObj,const char* name)
{
	if (!pGameObj)
	{
		return false;
	}
	return pGameObj->FindAttr(name);
}