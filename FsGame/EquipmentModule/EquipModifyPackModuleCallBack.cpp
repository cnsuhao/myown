//--------------------------------------------------------------------
// 文件名:		ModifyPackModuleCallBack.cpp
// 内  容:		管理各种修正包
// 说  明:  
// 创建日期:	2014年11月1日
// 创建人:        
//    :       
//--------------------------------------------------------------------
#include "EquipModifyPackModule.h"

#include "FsGame/CommonModule/PropRefreshModule.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "utils/extend_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/util_func.h"
#include "Define/CommandDefine.h"
#include "SystemFunctionModule/StaticDataQueryModule.h"
#include "EquipDefine.h"
#include "EquipBaptiseModule.h"
#include "utils/json.h"
#include "Define/BattleAbilityDefine.h"

//玩家登陆进入的时候
int EquipModifyPackModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
                                      const PERSISTID& sender, const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	IGameObj* pPlayer = pKernel->GetGameObj(self);
	if(pPlayer == NULL)
	{
		return 0;
	}
	
	m_pInstance->InitPropModifyRec(pKernel, self);

	m_pInstance->OnEquipStrSuit(pKernel, self, self, CVarList());
    return 0;
}

//添加一件装备到角色身上的回调
int EquipModifyPackModule::OnEquipAfterAdd(IKernel* pKernel, const PERSISTID& equipbox,
                                      const PERSISTID& equip, const IVarList& args)
{
    PERSISTID player = pKernel->Parent(equipbox);
	if (!pKernel->Exists(player))
	{
		return 0;
	}

    // 把此装备的战斗力写入战斗力表
    int equip_index = pKernel->GetIndex(equip);
    //PropRefreshModule::m_pInstance->OnEquipPutOn(pKernel, player, equip_index);

    //将这件装备的附加属性包刷到玩家的5个修正表中去
	MapPropValue mapPropValue;

	//更新装备包数据
	m_pInstance->UpdateEquipModifyObjProp(pKernel, player, equip, mapPropValue);

	// 为玩家加属性
	PropRefreshModule::m_pInstance->RefreshData(pKernel, player, mapPropValue, EREFRESH_DATA_TYPE_ADD);

	// 战斗力更新
	pKernel->Command(player, player, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
    return 0;
}

int EquipModifyPackModule::OnEquipRemove(IKernel* pKernel, const PERSISTID& equipbox,
                                    const PERSISTID& equip, const IVarList& args)
{
    PERSISTID player = pKernel->Parent(equipbox);
	if (!pKernel->Exists(player))
	{
		return 0;
	}

	MapPropValue mapPropValue;

	// 移除装备属性包
	m_pInstance->RemoveEquipModifyObjProp(pKernel, player, equip, mapPropValue);

	// 移除玩家属性
	PropRefreshModule::m_pInstance->RefreshData(pKernel, player, mapPropValue, EREFRESH_DATA_TYPE_REMOVE);

	// 战斗力更新
	pKernel->Command(player, player, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
    return 0;
}

int EquipModifyPackModule::OnEquipStrLevelUp(IKernel* pKernel, const PERSISTID& self, 
										const PERSISTID& equip, const IVarList& args)
{
	int old_level = args.IntVal(1);
	int new_level = args.IntVal(2);

	if (old_level < 0 || new_level < 0)
	{
		return 0;
	}

	m_pInstance->UpdateEquipStrPack(pKernel, self, equip, old_level, new_level);

	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
	return 0;
}

//强化套装改变回调
int EquipModifyPackModule::OnEquipStrSuit(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	// 当前属性包
	const int cur_pkg = pSelfObj->QueryInt(FIELD_PROP_STREN_SUIT_PKG);
	if (cur_pkg == 0)
	{
		return 0;
	}

	const int old_pkg = args.IntVal(1);
	char old_buff[32] = { 0 };
	itoa(old_pkg, old_buff, 10);
	std::string old_pck_id = old_buff;

	// 旧属性包
	CVarList old_prop_name_list;
	CVarList old_prop_value_list;
	StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_STRENGTHEN_SUIT_PACK,
		old_pck_id, old_prop_name_list, old_prop_value_list);

	char buff[32] = { 0 };
	itoa(cur_pkg, buff, 10);
	std::string new_pck_id = buff;

	// 新属性包
	CVarList new_prop_name_list;
	CVarList new_prop_value_list;
	StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_STRENGTHEN_SUIT_PACK,
		new_pck_id, new_prop_name_list, new_prop_value_list);

	if (new_prop_name_list.GetCount() == old_prop_name_list.GetCount())
	{
		LoopBeginCheck(ff);
		for (int j = 0; j < (int)old_prop_name_list.GetCount(); ++j)
		{
			LoopDoCheck(ff);
			int new_value = new_prop_value_list.IntVal(j) - old_prop_value_list.IntVal(j);
			new_prop_value_list.SetInt(j, new_value);
		}
	}

	int temp_value = 0;
	MapPropValue mapPropValue;
	LoopBeginCheck(gg);
	for (int i = 0; i < (int)new_prop_name_list.GetCount(); ++i)
	{
		LoopDoCheck(gg);
		// 过滤无用值
		temp_value = new_prop_value_list.IntVal(i);
		if (temp_value == 0)
		{
			continue;
		}

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self,
			new_prop_name_list.StringVal(i), temp_value,
			EModify_ADD_VALUE, mapPropValue);
	}

	PropRefreshModule::m_pInstance->RefreshData(pKernel,
		self, mapPropValue, EREFRESH_DATA_TYPE_ADD);

	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
// 	CVarList param;
// 	param << REFRESH_PROP_STR_SUIT;
// 	PropRefreshModule::m_pInstance->RefreshPartBattleAbility(pKernel, self,
// 		REFRESH_PROP_BA_OP_ADD, param, new_prop_name_list, new_prop_value_list);
// 
// 	PropRefreshModule::m_pInstance->RefreshRoleAbility(pKernel, self);

	return 0;
}

int EquipModifyPackModule::OnEquipBaptisePropChange(IKernel* pKernel, const PERSISTID& self, 
										const PERSISTID& equip, const IVarList& args)
{
	if (!pKernel->Exists(equip))
	{
		return 0;
	}

	IGameObj *pSelf = pKernel->GetGameObj(self);
	if(NULL == pSelf)
	{
		return 0;
	}

	const char *update_str = args.StringVal(1);
	if (StringUtil::CharIsNull(update_str))
	{
		return 0;
	}

	// 转换为json数据
	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(update_str, root))
	{
		return 0;
	}
		
	// 更新部位
	const int equip_pos = pKernel->GetIndex(equip);

	// 取得需更新的属性
	String_Vec prop_name_list;
	EquipBaptiseModule::m_pEquipBaptiseModule->GetPropNameList(pKernel, self, equip_pos, prop_name_list);

	MapPropValue mapPropValue;
	LoopBeginCheck(a);
	for (int i = 0; i < (int)prop_name_list.size(); ++i)
	{
		LoopDoCheck(a);
		const char *prop_name = prop_name_list[i].c_str();
		if (!root[prop_name].isInt())
		{
			continue;
		}

		const int value = root[prop_name].isInt();
		mapPropValue.insert(std::make_pair(prop_name, (double)value));
	}
	
	PropRefreshModule::m_pInstance->RefreshData(pKernel,
		self, mapPropValue, EREFRESH_DATA_TYPE_ADD);

	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
	return 0;
}

//附加属性变化回调
int EquipModifyPackModule::OnEquipAddModifyChange(IKernel* pKernel, const PERSISTID& self, 
											const PERSISTID& equip, const IVarList& args)
{
	const char * szOldProp = args.StringVal(1);
	const char * szNewProp = args.StringVal(2);

	if (StringUtil::CharIsNull(szOldProp) || 
		StringUtil::CharIsNull(szNewProp))
	{
		return 0;
	}

	m_pInstance->UpdateEquipAddModifyPack(pKernel, self, equip, szOldProp, szNewProp);

	// 刷新装备战斗力
	EquipModifyPackModule::RefreshEquipAbility(pKernel, equip, self, args);

	return 0;
}

//宝石镶嵌or升级or拆除
int EquipModifyPackModule::OnEquipJewelChange(IKernel* pKernel, const PERSISTID& self,
										 const PERSISTID& equip, const IVarList& args)
{
    // 宝石变化  
	m_pInstance->UpdateEquipJewelPack(pKernel, self, args);

	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
	return 0;
}

// 时装变化回调
int EquipModifyPackModule::OnFashionChanged(IKernel* pKernel, const PERSISTID &self,
                                       const PERSISTID & sender, const IVarList & args)
{    
    // 先刷新属性包以外的加成
    m_pInstance->UpdateFashionPack(pKernel, self, args);

	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
    return 0;
}

int EquipModifyPackModule::RefreshEquipAbility(IKernel* pKernel, const PERSISTID& equip, 
										  const PERSISTID& self, const IVarList& args)
{
	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);

	return 1;
}

// 对信息存储表的操作
void EquipModifyPackModule::HandleSaveRec(IKernel* pKernel, const PERSISTID& self, const PERSISTID& equip, 
									 const std::string& strPropName, const float fValue)
{
	if (!pKernel->Exists(self) || !pKernel->Exists(equip))
	{
		return;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}

	IRecord* pRecord = pSelfObj->GetRecord(CONST_PROP_MODIFY_BYEQUIP_REC);
	if (NULL == pRecord)
	{
		return;
	}

	IGameObj* pEquipObj = pKernel->GetGameObj(equip);
	if (pEquipObj == NULL)
	{
		return;
	}

	const char* szFromID = pEquipObj->QueryString("UniqueID");
	float fOldValue = 0.0f;
	int nRows = pRecord->GetRows();
	int i = 0;
    LoopBeginCheck(j);
	for ( i = 0; i < nRows; ++i)
	{
        LoopDoCheck(j);
		const char* szOldFormID = pRecord->QueryString(i, EPME_FROM);
		const char* szOldPropName = pRecord->QueryString(i, EPME_PROP_NAME);
		if ( 0 == ::strcmp(szOldFormID, szFromID) &&
			0 == ::strcmp(szOldPropName, strPropName.c_str()))
		{
			fOldValue = pRecord->QueryFloat(i, EPME_PROP_VALUE);
			pRecord->SetFloat(i, EPME_PROP_VALUE, fOldValue+fValue);
			break;
		}
	}

	if ( i == nRows )
	{
		pRecord->AddRowValue(-1, CVarList() << strPropName.c_str() << fValue << szFromID);
	}

	return ;
}


void EquipModifyPackModule::InitPropModifyRec(IKernel* pKernel, const PERSISTID& self)
{
    MapPropValue mapPropValue;

    //装备
    PERSISTID equipbox = pKernel->GetChild(self, L"EquipBox");
    if (pKernel->Exists(equipbox))
    {
        unsigned int i;
        PERSISTID equip = pKernel->GetFirst(equipbox, i);
        LoopBeginCheck(l);
        while (pKernel->Exists(equip))
        {
            LoopDoCheck(l);
            UpdateEquipModifyObjProp(pKernel, self, equip, mapPropValue);
            equip = pKernel->GetNext(equipbox, i);
        }
    }

	PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_ADD);
}