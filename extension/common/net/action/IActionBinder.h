//--------------------------------------------------------------------
// 文件名:		IActionBinder.h
// 内  容:		动作绑定接口
// 说  明:		
// 创建日期:		2016年5月30日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __I_ACTION_BINDER_H__
#define __I_ACTION_BINDER_H__

#include "IActionHandler.h"

class IActionBinder
{
public:
	virtual ~IActionBinder() = 0;

	virtual bool BindAction( int nActionId, ACTION_EVENT action, int nPriority ) = 0;
	virtual bool BindAction( const char* nActionKey, ACTION_EVENT action, int nPriority ) = 0;
};
inline IActionBinder::~IActionBinder() {};



#endif // END __I_ACTION_BINDER_H__