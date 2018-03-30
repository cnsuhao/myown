//----------------------------------------------------------
// 文件名:      LookModule.cpp
// 内  容:      查看信息
// 说  明:
// 创建日期:
// 创建人:             
//----------------------------------------------------------
#include "LookModule.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/custom_func.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/ContainerDefine.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "LookDefine.h"



LookModule * LookModule::m_pLookModule = NULL;
ContainerModule* LookModule::m_pContainerModule = NULL;


//初始化
bool LookModule::Init(IKernel* pKernel)
{
	m_pLookModule = this;

	m_pContainerModule = (ContainerModule*)pKernel->GetLogicModule("ContainerModule");

	Assert(m_pContainerModule !=NULL);

	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_LOOK_CHAT_ITEM, OnCustomLookChatItem);

	//pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_LOOK_EQUIPS, OnCustomLookEquips);
    //pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_LOOK_ITEM, OnCustomLookItem);   
	//pKernel->AddIntCommandHook("player", COMMAND_BELOOK_EQUIPS, OnCommandBeLookEquips);
	//pKernel->AddIntCommandHook("player", COMMAND_LOOK_EQUIPS_ECHO, OnCommandLookEquipsEcho);
	//pKernel->AddIntCommandHook("player", COMMAND_BELOOK_ITEM, OnCommandBeLookItem);
	//pKernel->AddIntCommandHook("player", COMMAND_LOOK_ITEM_ECHO, OnCommandLookItemEcho);


	return true;
}

bool LookModule::Shut(IKernel* pKernel)
{
	return true;
}

//查看其他角色的背包中的某件物品
int LookModule::OnCustomLookItem(IKernel * pKernel, const PERSISTID & self, 
								 const PERSISTID & sender, const IVarList & args)
{
	const wchar_t * destname = args.WideStrVal(1);
	const char * uniqueid = args.StringVal(2);
	
	m_pLookModule->LookItem(pKernel, self, destname, uniqueid);

	return 0;
}

//查看其他角色展示的某件物品
int LookModule::OnCustomLookChatItem(IKernel * pKernel, const PERSISTID & self, 
                                        const PERSISTID & sender, const IVarList & args)
{
    // args 的格式: 
    //     [msg_id][target_user][item_unique_id]
    
    const wchar_t *target_user = args.WideStrVal(1);
    const char *item_unique_id = args.StringVal(2);
    
    bool look_ret = LookRoleShowoffItem(pKernel, self, target_user, item_unique_id);
    
    if (!look_ret)
    {
        //查看[{@0:名称}]的物品失败，生成物品信息失败
		CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_19202, CVarList() << target_user);
        
        return 0;
    }
    
    return 0;
}

//被其他角色查看某件物品
int LookModule::OnCommandBeLookItem(IKernel * pKernel, const PERSISTID & self,
									const PERSISTID & sender, const IVarList & args)
{
	const wchar_t * srcname = args.WideStrVal(1);
	const char * identifyid = args.StringVal(2);
	m_pLookModule->BeLookItem(pKernel, self, srcname, identifyid);

	return 0;
}

//查看其他角色的某件物品的回复
int LookModule::OnCommandLookItemEcho(IKernel * pKernel, const PERSISTID & self,
									  const PERSISTID & sender, const IVarList & args)
{
	const wchar_t * destname = args.WideStrVal(1);
	const char * info = args.StringVal(2);
	int type = args.IntVal(3);
	const char* uniqueid = args.StringVal(4);
	m_pLookModule->LookItemEcho(pKernel, self, destname, info, type,uniqueid);

	return 0;
}



//查看其他角色的装备
void LookModule::LookEquips(IKernel * pKernel, const PERSISTID & self, const wchar_t * destname)
{
	if (destname == NULL || wcslen(destname) == 0)
	{
		return;
	}

	const wchar_t * self_name = pKernel->QueryWideStr(self, "Name");
	if (wcscmp(self_name,destname) == 0)
	{
		//不要查看自己的装备
		return;
	}

	//发送命令
	//CVarList msg;
	//msg << COMMAND_BELOOK_EQUIPS << self_name;
	//if (!pKernel->CommandByName(destname, msg))
	//{
	//	//[{@0:名称}]可能已下线，查看装备失败
	//	::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, LOOK_ROLE_OFFLINE, CVarList() << destname);
	//}
}

//被其他角色查看装备
void LookModule::BeLookEquips(IKernel * pKernel, const PERSISTID & self, const wchar_t * srcname)
{
	//CVarList msg;
	//msg << COMMAND_LOOK_EQUIPS_ECHO << pKernel->QueryWideStr(self, "Name");

	//fast_string szInfo = "";

	////将角色的外型数据准备到字符串中
	//if (::player_info_to_string(pKernel, self, szInfo))
	//{
	//	msg << szInfo.c_str();
	//}

	////将角色属性面板属性放到字符串中
	//if (::player_prop_to_string(pKernel, self, szInfo))
	//{
	//	msg << szInfo.c_str();
	//}

	////将装备的信息准备到字符串中
	//PERSISTID equip_box = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	//if (!pKernel->Exists(equip_box))
	//{
	//	return;
	//}

	//IRecord * pStrengthenRec = pKernel->GetRecord(self, EQUIP_STRHENTHEN_REC);
	//if (NULL == pStrengthenRec)
	//{
	//	return;
	//}

	//int nStrLevel = 0;
	//int nStrLevelRow = 0;
	//fast_string strEquipAdd = "";

 //   LoopBeginCheck(a)
	//for (int i = EQUIP_POS_WEAPON; i < EQUIP_MAX_PLACEHOLDER; ++i)
	//{
	//    LoopDoCheck(a)
	//    
	//	nStrLevel = 0;
	//	strEquipAdd = "";

	//	nStrLevelRow = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, i);
	//	if (nStrLevelRow >= 0)
	//	{
	//		//强化等级
	//		nStrLevel = pStrengthenRec->QueryInt(nStrLevelRow, EQUIP_STRHENTHEN_LEVEL);

	//		strEquipAdd += "StrLevel,";
	//		strEquipAdd += util_int64_as_string(nStrLevel);
	//		strEquipAdd += ";";

	//		//宝石
	//		LoopBeginCheck(c)
	//		for (int m = EQUIP_STRHENTHEN_JEWEL1; m <= EQUIP_STRHENTHEN_JEWEL4; ++m)
	//		{
	//		    LoopDoCheck(c)
	//		    
	//			strEquipAdd += "Jewel";
	//			strEquipAdd += util_int64_as_string(m-EQUIP_STRHENTHEN_JEWEL1+1).c_str();
	//			strEquipAdd += ",";
	//			strEquipAdd += pStrengthenRec->QueryString(nStrLevelRow, m);
	//			strEquipAdd += ";";
	//		}

	//		//灵魂
	//		strEquipAdd += "Soul1,";
	//		strEquipAdd += pStrengthenRec->QueryString(nStrLevelRow, EQUIP_STRHENTHEN_SOUL);
	//		strEquipAdd += ";";
	//	}

	//	PERSISTID obj = pKernel->GetItem(equip_box, i);
	//	if (pKernel->Exists(obj) && ::player_equip_to_string(pKernel, obj, szInfo))
	//	{
	//		szInfo += strEquipAdd;
	//		msg << szInfo.c_str();
	//	}
	//	else
	//	{
	//		msg << "";
	//	}
	//}

	////发送命令
	//pKernel->CommandByName(srcname, msg);
}

//查看其他角色的装备回复
void LookModule::LookEquipsEcho(IKernel * pKernel, const PERSISTID & self,
								const wchar_t * destname, const IVarList & infos)
{
	/*CVarList msg;
	msg << SERVER_CUSTOMMSG_LOOK_EQUIPS_ECHO << destname << infos;
	pKernel->Custom(self, msg);*/
} 


//查看其他角色的背包中的某件物品
void LookModule::LookItem(IKernel * pKernel, const PERSISTID & self,
						  const wchar_t * destname, const char * uniqueid)
{
	//if (destname == NULL || wcslen(destname) == 0)
	//{
	//	return;
	//}

	//std::wstring self_name = pKernel->QueryWideStr(self, "Name");

	////发送命令
	//CVarList msg;
	//msg << COMMAND_BELOOK_ITEM << self_name.c_str() << uniqueid;
	//if (!pKernel->CommandByName(destname, msg))
	//{
	//	//[{@0:名称}]可能已下线，查看物品失败
	//	::CustomSysInfo(pKernel, self, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, LOOK_ROLE_OFFLINE, CVarList() << destname);
	//}
}

//被其他角色查看背包中的某件物品
void LookModule::BeLookItem(IKernel * pKernel, const PERSISTID & self,
							const wchar_t * srcname, const char * uniqueid)
{
	//const wchar_t * selfname = pKernel->QueryWideStr(self, "Name");

	////寻找指定UniqueID号的物品

	//// 	先在背包中寻找指定装备

	//PERSISTID toolbox = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	//PERSISTID item = m_pContainerModule->FindItemByUid(pKernel, toolbox, uniqueid);

 //	if (!pKernel->Exists(item))
 //	{
 //		//未找到，到装备栏中查询
 //		PERSISTID equip_box = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	//	item = ((ContainerModule*)m_pContainerModule)->FindItemByUid(pKernel, equip_box, uniqueid);
 //	}

	//if (!pKernel->Exists(item))
	//{
	//	//查看[{@0:名称}]的物品失败，指定的物品已不在对方的背包中
	//	CustomSysInfoByName(pKernel, srcname, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, LOOK_ROLE_NOT_IN_TOOLBOX, CVarList() << selfname);
	//	return;
	//}

	//fast_string szInfo;
	//if (!(::player_item_to_string(pKernel, item, szInfo)))
	//{
	//	//查看[{@0:名称}]的物品失败，生成物品信息失败
	//	CustomSysInfoByName(pKernel, srcname, TIPSTYPE_SYSFUNCTION_PROMPT_MESSAGE, LOOK_ROLE_ITEM_INFO_FAILED, CVarList() << selfname);
	//	return;
	//}

	////发送命令
	//CVarList msg;
	//msg << COMMAND_LOOK_ITEM_ECHO << selfname;
	//msg << szInfo.c_str();
	//msg << 0;
	//msg << uniqueid;


	//pKernel->CommandByName(srcname, msg);
}

//查看其他角色的背包中的某件物品的回复
void LookModule::LookItemEcho(IKernel * pKernel, const PERSISTID & self,
							  const wchar_t * destname, const char * info, int type, const char* uniqueid)
{/*
	CVarList msg;
	if (type == 0)
	{
		msg << SERVER_CUSTOMMSG_LOOK_ITEM_ECHO << destname << info;
    }

	pKernel->Custom(self, msg);*/
}

//查看其他角色的装备
int LookModule::OnCustomLookEquips(IKernel * pKernel, const PERSISTID & self, 
								   const PERSISTID & sender, const IVarList & args)
{
	/*const wchar_t * destname = args.WideStrVal(1);
	m_pLookModule->LookEquips(pKernel, self, destname);*/

	return 0;
}

//命令回调
//被其他角色查看装备
int LookModule::OnCommandBeLookEquips(IKernel * pKernel, const PERSISTID & self,
									  const PERSISTID & sender, const IVarList & args)
{
	/*const wchar_t * srcname = args.WideStrVal(1);
	m_pLookModule->BeLookEquips(pKernel, self, srcname);*/

	return 0;
}
//查看其他角色的装备回复
int LookModule::OnCommandLookEquipsEcho(IKernel * pKernel, const PERSISTID & self,
										const PERSISTID & sender, const IVarList & args)
{
	/*const wchar_t * destname = args.WideStrVal(1);

	CVarList infos;
	infos.Append(args, 2, args.GetCount() - 2);

	m_pLookModule->LookEquipsEcho(pKernel, self, destname, infos);*/

	return 0;
}


// 查看其它玩家展示的物品
bool LookModule::LookRoleShowoffItem(IKernel * pKernel, const PERSISTID & self, const wchar_t * destname,
    const char * item_unique_id)
{
    IRecord *showoff_rec = GetShowoffItemRecord(pKernel, self);   
    if (showoff_rec == NULL)
    {
        return false;
    }

    // 根据物品UID查找记录
    int row_no = showoff_rec->FindString(SHOWOFF_ITEM_PUB_RECORD_UID_COL, item_unique_id);
    if (row_no == -1)
    {
        return false;
    }
    
    const char *item_prop = showoff_rec->QueryString(row_no, SHOWOFF_ITEM_PUB_RECORD_PROP_COL);

    CVarList msg;
    msg << SERVER_CUSTOMMSG_SHOWOFF_ITEM << destname << item_prop;

    pKernel->Custom(self, msg);

    return true;
}

// 获取物品展示记录表
IRecord* LookModule::GetShowoffItemRecord(IKernel * pKernel, const PERSISTID & self)
{
    // 获取公共区间的数据
    if (!pKernel->FindPubSpace(PUBSPACE_DOMAIN))
    {
        return NULL;
    }

    IPubSpace * pub_space = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
    if (pub_space == NULL)
    {
        return NULL;
    }

    wchar_t domain_name[256] = {0};
    SWPRINTF_S(domain_name, DOMAIN_LOOK, pKernel->GetServerId());

    IPubData * pub_data = pub_space->GetPubData(domain_name);
    if (pub_data == NULL)
    {
        return NULL;
    }

    // 展示记录表
    IRecord * item_rec = pub_data->GetRecord(SHOWOFF_ITEM_PUB_RECORD_NAME);

    return item_rec;
}