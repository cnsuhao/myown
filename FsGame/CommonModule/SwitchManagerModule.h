//----------------------------------------------------------
// 文件名:      SwitchManagerModule.h
// 内  容:      功能开关定义
// 说  明:
// 创建人:        
// 创建日期:    2015年6月30日
//    :        
//----------------------------------------------------------
#ifndef __SWITCHMANAGER_MODULE_H_
#define __SWITCHMANAGER_MODULE_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/SwitchManagerDefine.h"

class SwitchManagerModule : public ILogicModule
{
public:
    bool Init(IKernel* pKernel);
    bool Shut(IKernel* pKernel);

public:
    // 检测功能是否开启
	static bool CheckFunctionEnable(IKernel* pKernel, const int function_id, const PERSISTID& self = PERSISTID());
    // 设置状态
    static bool SetFunctionEnable(IKernel* pKernel, const int function_id, const int newState);

	// 加载公共数据到数据库
	bool LoadResource(IKernel* pKernel);

	//消息解析
	static bool GetSwitchFunctionInfo(IKernel* pKernel, const char* content);
private:
    const std::wstring GetDomainName(IKernel* pKernel);
    const CVarList& QueryPubFunctionState(IKernel* pKernel, const int function_id);

	// 延迟加载配置文件 
	static int CB_DomainDB(IKernel* pKernel, const PERSISTID& self, int slice);

	// 场景创建
	static int OnSceneCreate(IKernel* pKernel, const PERSISTID& scene, 
		const PERSISTID& sender, const IVarList& args);

	//保存功能开关配置到数据库
	bool SaveSwitchConfigToDB(IKernel* pKernel, const int function_id,
		const int newState, const std::string& function_name);

	// 给客户端提示信息
	static void SendCustomSysInfo(IKernel* pKernel, const PERSISTID& self, const int functin_id);
private:
    std::wstring m_DomainName;  // 公共数据名称
public:
    static SwitchManagerModule* m_SwitchManagerModule;
};
#endif //__SWITCHMANAGER_MODULE_H_