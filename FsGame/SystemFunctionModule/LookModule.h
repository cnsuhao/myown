//----------------------------------------------------------
// 文件名:      LookModule.h
// 内  容:      查看信息
// 说  明:
// 创建日期:
// 创建人:             
//----------------------------------------------------------
#ifndef __LookModule_h__
#define __LookModule_h__

#include "Fsgame/Define/header.h"

class ContainerModule;
class LookModule : public ILogicModule
{
public:
	//初始化
	virtual bool Init(IKernel* pKernel);
	
	//释放
	virtual bool Shut(IKernel* pKernel);
	
public:	
	//消息回调
	//查看其他角色的装备
	static int OnCustomLookEquips(IKernel * pKernel, const PERSISTID & self, 
		const PERSISTID & sender, const IVarList & args);

	//查看其他角色的背包中的某件物品
	static int OnCustomLookItem(IKernel * pKernel, const PERSISTID & self, 
        const PERSISTID & sender, const IVarList & args);
        
    //查看其他角色展示的某件物品
    static int OnCustomLookChatItem(IKernel * pKernel, const PERSISTID & self, 
        const PERSISTID & sender, const IVarList & args);

	//命令回调
	//被其他角色查看装备
	static int OnCommandBeLookEquips(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);

	//查看其他角色的装备回复
	static int OnCommandLookEquipsEcho(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);

	//被其他角色查看背包中的某件物品
	static int OnCommandBeLookItem(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);

	//查看其他角色的背包中的某件物品的回复
	static int OnCommandLookItemEcho(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);


public:

	//查看其他角色的装备
	void LookEquips(IKernel * pKernel, const PERSISTID & self, const wchar_t * destname);

	//被其他角色查看装备
	void BeLookEquips(IKernel * pKernel, const PERSISTID & self, const wchar_t * srcname);

	//查看其他角色的装备回复
	void LookEquipsEcho(IKernel * pKernel, const PERSISTID & self, const wchar_t * destname, 
		const IVarList & infos);

	//查看其他角色的背包中的某件物品
	void LookItem(IKernel * pKernel, const PERSISTID & self, const wchar_t * destname, const char * uniqueid);

	//被其他角色查看背包中的某件物品
	void BeLookItem(IKernel * pKernel, const PERSISTID & self, const wchar_t * srcname, const char * uniqueid);

	//查看其他角色的背包中的某件物品的回复
	void LookItemEcho(IKernel * pKernel, const PERSISTID & self, const wchar_t * destname, const char * info, int type, const char* uniqueid);

private:

    // 查看其它玩家展示的物品
    static bool LookRoleShowoffItem(IKernel * pKernel, const PERSISTID & self, const wchar_t * destname,
        const char * item_unique_id);

    // 获取物品展示记录表
    static IRecord* GetShowoffItemRecord(IKernel * pKernel, const PERSISTID & self);
    
public:
	static LookModule * m_pLookModule;
	static ContainerModule* m_pContainerModule;
};
#endif