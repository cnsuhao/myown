//--------------------------------------------------------------------
// 文件名:		ILogicInfo.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年12月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_ILOGICINFO_H
#define _PUBLIC_ILOGICINFO_H

#include "Macros.h"
#include "IVarList.h"

// ICallbackInfo
// 逻辑方法信息

class ICallbackInfo
{
public:
	virtual ~ICallbackInfo() = 0;

	virtual const char* GetName() const = 0;
	virtual void* GetMidFunc() const = 0;
	virtual bool GetReturnTable() const = 0;
};

inline ICallbackInfo::~ICallbackInfo() {}

// ILogicInfo
// 逻辑类信息

class ILogicCreator;
class ILogic;

class ILogicInfo
{
public:
	virtual ~ILogicInfo() = 0;

	// 获得创建器
	virtual ILogicCreator* GetCreator() const = 0;
	
	// 返回名字空间
	virtual const char* GetSpaceName() const = 0;

	// 返回类名
	virtual const char* GetLogicName() const = 0;
	
	// 方法数量
	virtual size_t GetCallbackCount() const = 0;
	// 获得方法名字列表
	virtual size_t GetCallbackList(IVarList& result) const = 0;
	// 在本类中获得方法信息
	virtual const ICallbackInfo* GetCallbackInfo(const char* name) const = 0;
};

inline ILogicInfo::~ILogicInfo() {}

#endif // _PUBLIC_ILOGICINFO_H

