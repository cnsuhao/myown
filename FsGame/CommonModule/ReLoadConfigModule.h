//--------------------------------------------------------------------
// 文件名:		ReLoadConfigModule.h
// 内  容:		重新加载配置
// 说  明:		
// 创建日期:	2016年09月13日
// 整理日期:	2016年09月13日
// 创建人:		刘明飞   
//--------------------------------------------------------------------

#ifndef __ReLoadConfigModule_H__
#define __ReLoadConfigModule_H__

#include "Fsgame/Define/header.h"
#include <string>
#include <map>

// 重新读取配置回调
typedef void(__cdecl* RELOAD_CONFIG_CALLBACK)(IKernel *pKernel);

class ReLoadConfigModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init( IKernel* pKernel );

	// 释放
	virtual bool Shut( IKernel* pKernel );

	// 注册重载配置回调
	void RegisterReloadConfig(const char* strKey, RELOAD_CONFIG_CALLBACK pReloadCall);

	// 发送重载配置请求
	bool SendReloadConfigCommand(IKernel* pKernel, const char* strKey);
private:
	// 重载配置文件
	static int OnCommandRereadLoadResource(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	typedef std::map< std::string, RELOAD_CONFIG_CALLBACK > ReloadConfigCallMap;
public:
	static ReLoadConfigModule* m_pReLoadConfigModule;
	
	ReloadConfigCallMap m_mapReloadConfig;			// 配置读取回调
};

#define RELOAD_CONFIG_REG(key, call) ReLoadConfigModule::m_pReLoadConfigModule->RegisterReloadConfig(key, call);

#endif	// __GMModule_H__