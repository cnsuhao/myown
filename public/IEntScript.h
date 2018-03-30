//--------------------------------------------------------------------
// 文件名:		IEntScript.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_IENTSCRIPT_H
#define _PUBLIC_IENTSCRIPT_H

#include "Macros.h"
#include "IVarList.h"

// IEntScript
// 实体脚本

class ILogic;

class IEntScript
{
public:
	virtual ~IEntScript() = 0;

	// 获得脚本名称
	virtual const char* GetName() const = 0;
	// 获得逻辑类
	virtual ILogic* GetLogic() const = 0;
	// 添加回调
	virtual bool AddCallback(const char* event, const char* func) = 0;
	// 删除回调
	virtual bool RemoveCallback(const char* event) = 0;
	// 清空回调
	virtual void ClearCallback() = 0;
	// 获得回调数量
	virtual size_t GetCallbackCount() const = 0;
	// 获得回调事件列表
	virtual size_t GetCallbackList(IVarList& result) const = 0;
	// 获得回调对应的脚本函数
	virtual const char* GetCallbackFunc(const char* event) const = 0;
};

inline IEntScript::~IEntScript() {}

#endif // _PUBLIC_IENTSCRIPT_H

