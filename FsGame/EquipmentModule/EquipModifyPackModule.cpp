//--------------------------------------------------------------------
// 文件名:		ModifyPackModule.h
// 内  容:		管理各种修正包
// 说  明:
// 创建日期:
// 创建人:  
//--------------------------------------------------------------------
#include "EquipModifyPackModule.h"

#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/Define/ContainerDefine.h"
#include "FsGame/CommonModule/PropRefreshModule.h"
#include "FsGame/SkillModule/Impl/SkillDataQueryModule.h"
#include "FsGame/Interface/FightInterface.h"
#include "utils/exptree.h"
#include "EquipDefine.h"
#include "SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/Define/GameDefine.h"
#include "utils/json.h"
#include "EquipBaptiseModule.h"
#include "Define/BattleAbilityDefine.h"

EquipModifyPackModule* EquipModifyPackModule::m_pInstance = NULL;

// 初始化模块
bool EquipModifyPackModule::Init(IKernel* pKernel)
{
	//初始化模块指针
	m_pInstance = this;

	pKernel->AddEventCallback("player", "OnRecover", EquipModifyPackModule::OnPlayerRecover, 100);
    
	//添加装备
	pKernel->AddEventCallback("EquipBox", "OnAfterAdd", EquipModifyPackModule::OnEquipAfterAdd);

	//卸除装备
	pKernel->AddEventCallback("EquipBox", "OnRemove", EquipModifyPackModule::OnEquipRemove);
	
	//升级强化等级
	pKernel->AddIntCommandHook("player", COMMAND_EQUIP_STRENGTHEN_PKG, EquipModifyPackModule::OnEquipStrLevelUp);

	// 强化套装
	pKernel->AddIntCommandHook("player", COMMAND_EQUIP_STR_SUIT_PKG, EquipModifyPackModule::OnEquipStrSuit);
	
	//洗炼属性发生变化
	pKernel->AddIntCommandHook("player", COMMAND_EQUIP_BAPTISE_PKG, EquipModifyPackModule::OnEquipBaptisePropChange);

	//附加属性修正变化
	pKernel->AddIntCommandHook("player", COMMAND_EQUIPMENT_ADDMODIFY, EquipModifyPackModule::OnEquipAddModifyChange);
	//镶嵌宝石
	pKernel->AddIntCommandHook("player", COMMAND_EQUIPMENT_JEWEL_PKG, EquipModifyPackModule::OnEquipJewelChange);
	//镶嵌灵魂
	//pKernel->AddIntCommandHook("player", COMMAND_EQUIPMENT_SOUL, EquipModifyPackModule::OnEquipSoulChange);
 
    // 外观穿戴属性包
	pKernel->AddIntCommandHook("player", COMMAND_APPERANCE_PKG, EquipModifyPackModule::OnFashionChanged);

	//战斗力刷新
	pKernel->AddIntCommandHook("Equipment", COMMAND_EQUIP_ABILITY, EquipModifyPackModule::RefreshEquipAbility);

	// 玩家等级变化
	pKernel->AddIntCommandHook("player", COMMAND_LEVELUP_CHANGE, OnDCLevelChange);
	
    // 玩家时装变化
    //pKernel->AddIntCommandHook("player", COMMAND_FASHION, OnFashionChanged);

	return true;
}

// 清理
bool EquipModifyPackModule::Shut(IKernel* pKernel)
{
	return true;
}

//更新装备包数据
bool EquipModifyPackModule::UpdateEquipModifyObjProp( IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& equip, MapPropValue& mapPropValue)
{
	if (!pKernel->Exists(self) || !pKernel->Exists(equip))
	{
		return false;
	}

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

    IGameObj *pEquip = pKernel->GetGameObj(equip);
    if(pEquip == NULL)
    {
        return false;
    }

	const PERSISTID& container = pKernel->Parent(equip);
	if (!pKernel->Exists(container))
	{
		return false;
	}

    IRecord* pRecord = pSelfObj->GetRecord(CONST_PROP_MODIFY_BYEQUIP_REC);
    if (NULL == pRecord)
    {
        return false;
    }

	int colorLevel = pEquip->QueryInt("ColorLevel");
	const char* szFromID = pEquip->QueryString("UniqueID");
	const char* szModifyAdd = pEquip->QueryString("ModifyAddPackage");
	int basePackage = pEquip->QueryInt("BasePackage");				//基础属性
	int addPackage  = pEquip->QueryInt("AddPackage");				//附加属性
	int strPackage  = pEquip->QueryInt("StrPackage");				//强化属性
	int nEquipIndex = pEquip->GetIndex();

	basePackage += ITEM_BASE_PACKAGE_COEFFICIENT * colorLevel;
	strPackage	+= ITEM_BASE_PACKAGE_COEFFICIENT * colorLevel;

    const PERSISTID& player = pKernel->Parent(container);
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return false;
	}

	IRecord * pStrengthenRec = pPlayerObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec)
	{
		return false;
	}

	int strengthenLevel = 0;
	int strLevelRow = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, nEquipIndex);
	if (strLevelRow == -1)
	{
		return false;
	}

	strengthenLevel = pStrengthenRec->QueryInt(strLevelRow, EQUIP_STRHENTHEN_LEVEL);

	char buff[32] = {0};
	itoa(basePackage, buff, 10);
	std::string strPackageID = buff;
	itoa(strPackage, buff, 10);
	std::string strStrPackageID = buff;
	itoa(addPackage, buff, 10);
	std::string strAddPackageID = buff;

	CVarList vTempPropNameList;
    CVarList vTempPropValueList;
    
    CVarList vFinalePropNameList;
    CVarList vFinalPropValueList;

	// 1.获取装备基础属性信息
	StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_EQUIP_BASE_PACK, 
		strPackageID, vTempPropNameList, vTempPropValueList);
		
    vFinalePropNameList << vTempPropNameList;
    vFinalPropValueList << vTempPropValueList;
    

	// 2. 装备的强化包
	
    vTempPropNameList.Clear();
    vTempPropValueList.Clear();
    
	if (strengthenLevel > 0)
	{
		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_STRENGTHEN_PACK,
			strStrPackageID, vTempPropNameList, vTempPropValueList);

		// 乘以强化等级
		CVarList vTempValList;
		int prop_cnt = (int) vTempPropValueList.GetCount();
        LoopBeginCheck(a);
		for (int i=0; i < prop_cnt; ++i)
		{
            LoopDoCheck(a);
			vTempValList << vTempPropValueList.IntVal(i)*strengthenLevel;
		}
		
        vFinalePropNameList << vTempPropNameList;
        vFinalPropValueList << vTempValList;
	}


	// 3.附加属性包
    
    vTempPropNameList.Clear();
    vTempPropValueList.Clear();
    
	StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_EQUIP_ADD_PACK, 
		strAddPackageID, vTempPropNameList, vTempPropValueList);

    vFinalePropNameList << vTempPropNameList;
    vFinalPropValueList << vTempPropValueList;


	// 4.附加属性随机修正包
    
    vTempPropNameList.Clear();
    vTempPropValueList.Clear();
    
	if (!StringUtil::CharIsNull(szModifyAdd))
	{
		CVarList vModifyAddList;

		util_split_string(vModifyAddList, szModifyAdd, ",");

        LoopBeginCheck(b);
		for (size_t i = 0; i < vModifyAddList.GetCount(); ++i)
		{
            LoopDoCheck(b);
            vTempPropNameList.Clear();
            vTempPropValueList.Clear();
            
			StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_EQUIP_MODIFYADD_PACK, 
				vModifyAddList.StringVal(i), vTempPropNameList, vTempPropValueList);
				
            vFinalePropNameList << vTempPropNameList;
            vFinalPropValueList << vTempPropValueList;
		}
	}


	// 5.宝石属性包
	
    vTempPropNameList.Clear();
    vTempPropValueList.Clear();
	
	const char * jewel_config = NULL;
	const char * jewel_prop = NULL;

    LoopBeginCheck(c);
	for (int i = EQUIP_STRHENTHEN_JEWEL1; i <= EQUIP_STRHENTHEN_JEWEL4; ++i)
	{
        LoopDoCheck(c);
        vTempPropNameList.Clear();
        vTempPropValueList.Clear();
        
		jewel_config = pStrengthenRec->QueryString(strLevelRow, i);
		if (StringUtil::ItemIsNull(jewel_config))
		{
			continue;
		}

		jewel_prop = pKernel->GetConfigProperty(jewel_config, "BasePackage");
		if (StringUtil::CharIsNull(jewel_prop))
		{
			continue;
		}

		//获取宝石属性
		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_JEWEL_BASE_PACK,
			jewel_prop, vTempPropNameList, vTempPropValueList);
			
        vFinalePropNameList << vTempPropNameList;
        vFinalPropValueList << vTempPropValueList;
	}

	//7.获取洗炼属性

	vTempPropNameList.Clear();
	vTempPropValueList.Clear();

	//取出洗炼表
	IRecord *pBaptiseRec = pSelfObj->GetRecord(FIELD_RECORD_BAPTISE_REC);
	if (NULL != pBaptiseRec)
	{
		int iBaptiseRow = pBaptiseRec->FindInt(COLUMN_BAPTISE_REC_POS, nEquipIndex);
		if (iBaptiseRow >= 0)
		{
			const char *value_info = pBaptiseRec->QueryString(iBaptiseRow, COLUMN_BAPTISE_REC_VALUE_INFO);
			if (!StringUtil::CharIsNull(value_info))
			{
				// 转换为json数据
				Json::Reader reader;
				Json::Value root;

				if (reader.parse(value_info, root))
				{
					// 取得需更新的属性
					String_Vec prop_name_list;
					EquipBaptiseModule::m_pEquipBaptiseModule->GetPropNameList(pKernel, self, nEquipIndex, prop_name_list);

					MapPropValue mapPropValue;
					LoopBeginCheck(f);
					for (int i = 0; i < (int)prop_name_list.size(); ++i)
					{
						LoopDoCheck(f);
						const char *prop_name = prop_name_list[i].c_str();
						if (!root[prop_name].isInt())
						{
							continue;
						}

						const int value = root[prop_name].isInt();
						vTempPropNameList << prop_name;
						vTempPropValueList << value;
					}

					vFinalePropNameList << vTempPropNameList;
					vFinalPropValueList << vTempPropValueList;
				}
			}
		}
	}

	// 综合处理装换接口
	MapPropValue mapFinalPropValue;
	PropRefreshModule::m_pInstance->GetFinalEquipModifyObjProp(pKernel, vFinalePropNameList,
	    vFinalPropValueList, mapFinalPropValue);

    MapPropValue::const_iterator it = mapFinalPropValue.begin();
    LoopBeginCheck(e);
	while (it != mapFinalPropValue.end())
	{
        LoopDoCheck(e);
		std::string strPropName = it->first;
		float fPropValue = (float)(it->second);

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, 
			strPropName.c_str(), fPropValue, EModify_ADD_VALUE, mapPropValue);

		pRecord->AddRowValue(-1, CVarList() << strPropName.c_str() << fPropValue << szFromID);
        ++it;
	}

	return true;
}


bool EquipModifyPackModule::RemoveEquipModifyObjProp(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& equip, MapPropValue& mapPropValue)
{
	if (!pKernel->Exists(self) || !pKernel->Exists(equip))
	{
		return false;
	}

	IGameObj* pEquipObj = pKernel->GetGameObj(equip);
	if (pEquipObj == NULL)
	{
		return 0;
	}

	const char* szFromID = pEquipObj->QueryString("UniqueID");

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	IRecord* pRecord = pSelfObj->GetRecord(CONST_PROP_MODIFY_BYEQUIP_REC);
	if (NULL == pRecord)
	{
		return false;
	}

	int num = (int)pRecord->GetRows();
    LoopBeginCheck(e);
	for (int t = num-1; t > -1; --t)
	{
        LoopDoCheck(e);
		int row = pRecord->FindString(EPME_SPECIAL_FROM, szFromID);
		if (-1 == row)
		{
			break;
		}

		std::string strPropName = pRecord->QueryString(row, EPME_SPECIAL_PROP_NAME);
		float fPropValue = pRecord->QueryFloat(row, EPME_SPECIAL_PROP_VALUE);

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, strPropName.c_str(), fPropValue, EModify_ADD_VALUE, mapPropValue);

		pRecord->RemoveRow(row);
	}

	return true;
}

bool EquipModifyPackModule::UpdateEquipStrPack(IKernel* pKernel, const PERSISTID& self, 
										  const PERSISTID& equip,int oldProp, int newProp)
{
	if (oldProp == newProp)
	{
		return true;
	}

	// 新旧属性对比刷新差值
	CVarList vDiffPropNameList;
	CVarList vDiffPropValueList;
    IGameObj *pEquip = pKernel->GetGameObj(equip);
    if(pEquip == NULL)
    {
        return false;
    }

	int strPackage  = pEquip->QueryInt("StrPackage");
	int colorLevel = pEquip->QueryInt("ColorLevel");

	strPackage	+= ITEM_BASE_PACKAGE_COEFFICIENT * colorLevel;

	// 差异等级(可以为负)
	int deffProp = newProp - oldProp;

	char buff[32] = {0};
	itoa(strPackage, buff, 10);
	std::string strPackageID = buff;

	StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_STRENGTHEN_PACK,
		strPackageID, vDiffPropNameList, vDiffPropValueList);

	if (vDiffPropNameList.GetCount() != vDiffPropValueList.GetCount())
	{
		return false;
	}

	// 基础强化属性再乘以强化等级
	int temp = 0;
	CVarList vPropValList;
    LoopBeginCheck(f);
	for (size_t i = 0; i < vDiffPropNameList.GetCount(); ++i)
	{
        LoopDoCheck(f);
		temp = vDiffPropValueList.IntVal(i)*deffProp;
		vPropValList << temp;
	}

	int tempValue = 0;
	MapPropValue mapPropValue;

    LoopBeginCheck(g);
	for (size_t i = 0; i < vDiffPropNameList.GetCount(); ++i)
	{
        LoopDoCheck(g);
		// 过滤无用值
		tempValue = vPropValList.IntVal(i);
		if (tempValue == 0)
		{
			continue;
		}

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, 
			vDiffPropNameList.StringVal(i), tempValue, 
			EModify_ADD_VALUE, mapPropValue);

		// 对表中的数据做一次过滤处理
		/*m_pInstance->HandleSaveRec(pKernel, self, equip,
			vDiffPropNameList.StringVal(i), (float)tempValue);*/
	}

	bool result = PropRefreshModule::m_pInstance->RefreshData(pKernel, 
		self, mapPropValue, EREFRESH_DATA_TYPE_ADD);

	return result;
}

bool EquipModifyPackModule::UpdateEquipBaptisePack(IKernel* pKernel, const PERSISTID& self, 
	IVarList& args, const PERSISTID& equip)
{
	int equip_index = args.IntVal(4); 

	IGameObj *pObj = pKernel->GetGameObj(self);
	if(pObj == NULL)
	{
		return false;
	}

	int diff = 0;
	MapPropValue mapPropValue;

	LoopBeginCheck(a);
	for(int i=0; i < 4; ++i)
	{
		LoopDoCheck(a);
		
		std::string prop_name = ""/*EquipBaptiseModule::GetBaptisePropName(pKernel, self, equip_index, i)*/;

		if(prop_name == "")
		{
			return false;
		}

		diff = args.IntVal(i);
		if (diff == 0)
		{
			continue;
		}

		mapPropValue.insert(std::make_pair(prop_name, (double)diff));

		// 对表中的数据做一次过滤处理
		/*m_pInstance->HandleSaveRec(pKernel, self, equip, 
			prop_name, (float)diff);*/
	}

	bool result = PropRefreshModule::m_pInstance->RefreshData(pKernel, 
		self, mapPropValue, EREFRESH_DATA_TYPE_ADD);
	
	return true;
}

bool EquipModifyPackModule::UpdateEquipAddModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& equip, 
											  const char * oldProp, const char * newProp)
{
	// 新旧属性对比刷新差值
	CVarList vOldPropNameList;
	CVarList vOldPropValueList;

	CVarList vNewPropNameList;
	CVarList vNewPropValueList;

	CVarList vPropList;
	CVarList vValueList;

	vPropList.Clear();
	util_split_string(vPropList, oldProp, ",");

    LoopBeginCheck(h);
	for (size_t i = 0; i < vPropList.GetCount(); ++i)
	{
        LoopDoCheck(h);
		vValueList.Clear();
		util_split_string(vValueList, vPropList.StringVal(i), ":");

		if (vValueList.GetCount() >= 2)
		{
			StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_EQUIP_MODIFYADD_PACK, 
				vValueList.StringVal(1), vOldPropNameList, vOldPropValueList);
		}
	}

	vPropList.Clear();
	util_split_string(vPropList, newProp, ",");

    LoopBeginCheck(i);
	for (size_t i = 0; i < vPropList.GetCount(); ++i)
	{
        LoopDoCheck(i);
		vValueList.Clear();
		util_split_string(vValueList, vPropList.StringVal(i), ":");

		if (vValueList.GetCount() >= 2)
		{
			StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_EQUIP_MODIFYADD_PACK, 
				vValueList.StringVal(1), vNewPropNameList, vNewPropValueList);
		}
	}


	if (vOldPropNameList.GetCount() != vOldPropValueList.GetCount() || 
		vNewPropNameList.GetCount() != vNewPropValueList.GetCount() || 
		vOldPropNameList.GetCount() != vNewPropNameList.GetCount())
	{
		//assert(0);
		return 0;
	}

	MapPropValue mapPropValue;

    LoopBeginCheck(j);
	for (size_t i = 0; i < vOldPropNameList.GetCount(); ++i)
	{
        LoopDoCheck(j);
		float fPropOldValue = vOldPropValueList.FloatVal(i);
		float fProNewValue = vNewPropValueList.FloatVal(i);
		float fDecValue = fProNewValue - fPropOldValue;

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, 
			vOldPropNameList.StringVal(i), fDecValue, EModify_ADD_VALUE, mapPropValue);

		// 对表中的数据做一次过滤处理
		m_pInstance->HandleSaveRec(pKernel, self, equip, 
			vOldPropNameList.StringVal(i), fProNewValue);
	}

	bool result = PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_ADD);

	return result;
}

/// \brief 更新宝石属性
void EquipModifyPackModule::UpdateEquipJewelPack(IKernel* pKernel, const PERSISTID& self, 
	const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}

	PERSISTID equbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equbox))
	{
		return;
	}

	int nArgSize = (int)args.GetCount();
	if (nArgSize < 4)
	{
		return;
	}

	int equipindex = args.IntVal(1);
	int optiontype = args.IntVal(2);

	//有装备
	PERSISTID equip = pKernel->GetItem(equbox, equipindex);
	if (!pKernel->Exists(equbox))
	{
		return;
	}

	IRecord* pRecord = pSelfObj->GetRecord(CONST_PROP_MODIFY_BYEQUIP_REC);
	IRecord * pStrengthenRec = pSelfObj->GetRecord(EQUIP_STRHENTHEN_REC);
	if (NULL == pStrengthenRec || NULL == pRecord)
	{
		return;
	}

	int equiprow = pStrengthenRec->FindInt(EQUIP_STRHENTHEN_INDEX, equipindex);
	if (equiprow == -1)
	{
		return;
	}

	CVarList vPropList;
	CVarList vValueList;

	MapPropValue mapPropValue;

	if (optiontype == EQUIP_STRPROP_OPTION_ADD)
	{
		const char * jewel_config = NULL;
		const char * jewel_prop = NULL;

        LoopBeginCheck(k);
		for (int i = 3; i < nArgSize; ++i)
		{
            LoopDoCheck(k);
			jewel_config = args.StringVal(i);
			if (StringUtil::ItemIsNull(jewel_config))
			{
				continue;
			}

			jewel_prop = pKernel->GetConfigProperty(jewel_config, "BasePackage");
			if (StringUtil::CharIsNull(jewel_prop))
			{
				continue;
			}

			StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_JEWEL_BASE_PACK,
				jewel_prop, vPropList, vValueList);
		}

		//过滤重复和0属性
		PropRefreshModule::m_pInstance->GetFinalEquipModifyObjProp(pKernel, vPropList, vValueList, mapPropValue);
		
		//将属性增加到装备属性表中
  //      LoopBeginCheck(l);
		//for (MapPropValue::iterator it = mapPropValue.begin(); it != mapPropValue.end(); ++it)
		//{
  //          LoopDoCheck(l);
		//	// 对表中的数据做一次过滤处理
		//	m_pInstance->HandleSaveRec(pKernel, self, equip, 
		//		it->first, (float)it->second);
		//}

		bool result = PropRefreshModule::m_pInstance->RefreshData(pKernel, 
			self, mapPropValue, EREFRESH_DATA_TYPE_ADD);
	}
	else if (optiontype == EQUIP_STRPROP_OPTION_REMOVE)
	{
		const char * jewel_config = args.StringVal(3);
		const char * jewel_prop = NULL;

		if (StringUtil::ItemIsNull(jewel_config))
		{
			return;
		}

		jewel_prop = pKernel->GetConfigProperty(jewel_config, "BasePackage");
		if (StringUtil::CharIsNull(jewel_prop))
		{
			return;
		}
	
		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_JEWEL_BASE_PACK,
			jewel_prop, vPropList, vValueList);

		//过滤重复和0属性
		PropRefreshModule::m_pInstance->GetFinalEquipModifyObjProp(pKernel, vPropList, vValueList, mapPropValue);

		//将属性增加到装备属性表中
  //      LoopBeginCheck(m);
		//for (MapPropValue::iterator it = mapPropValue.begin(); it != mapPropValue.end(); ++it)
		//{
  //          LoopDoCheck(m);
		//	// 对表中的数据做一次过滤处理
		//	m_pInstance->HandleSaveRec(pKernel, self, equip, 
		//		it->first, (float)-it->second);
		//}

		//减属性
		PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_REMOVE);
	}
	else if (optiontype == EQUIP_STRPROP_OPTION_UPDATE)
	{
		const char * old_jewel = args.StringVal(3);
		const char * new_jewel = args.StringVal(4);

		if (StringUtil::ItemIsNull(old_jewel) || 
			StringUtil::ItemIsNull(new_jewel))
		{
			return;
		}

		const char * old_jewel_prop = NULL;
		const char * new_jewel_prop = NULL;

		old_jewel_prop = pKernel->GetConfigProperty(old_jewel, "BasePackage");
		new_jewel_prop = pKernel->GetConfigProperty(new_jewel, "BasePackage");
		if (StringUtil::CharIsNull(old_jewel_prop) || 
			StringUtil::CharIsNull(new_jewel_prop))
		{
			return;
		}

		CVarList oldValueList;
		CVarList oldPropList;

		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_JEWEL_BASE_PACK,
			old_jewel_prop, oldPropList, oldValueList);
		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_JEWEL_BASE_PACK,
			new_jewel_prop, vPropList, vValueList);

		if (oldPropList.GetCount() != oldValueList.GetCount() &&
			oldPropList.GetCount() != vPropList.GetCount() &&
			vPropList.GetCount() != vValueList.GetCount())
		{
			return;
		}

        CVarList tempValueList;
		float tempValue = 0.0f;
        LoopBeginCheck(n);
		for (int i = 0; i < (int)vValueList.GetCount(); ++i)
		{
            LoopDoCheck(n);
			tempValue = vValueList.FloatVal(i) - oldValueList.FloatVal(i);
			tempValueList << tempValue;
		}

		//过滤重复和0属性
		PropRefreshModule::m_pInstance->GetFinalEquipModifyObjProp(pKernel, vPropList, tempValueList, mapPropValue);

		//将属性增加到装备属性表中
  //      LoopBeginCheck(o);
		//for (MapPropValue::iterator it = mapPropValue.begin(); it != mapPropValue.end(); ++it)
		//{
  //          LoopDoCheck(o);
		//	// 对表中的数据做一次过滤处理
		//	m_pInstance->HandleSaveRec(pKernel, self, equip, 
		//		it->first, (float)it->second);
		//}

		//加属性
		PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_ADD);
	}
	else
	{
		return;
	}
}

/// \brief 更新时装带来的属性变化
void EquipModifyPackModule::UpdateFashionPack(IKernel* pKernel, const PERSISTID& self, 
	const IVarList& args)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

    int op_type = args.IntVal(1);

	const int base_package = args.IntVal(2);

	std::string package_name = StringUtil::IntAsString(base_package);
	if (package_name.empty())
	{
		return;
	}

    if (op_type == EQUIP_STRPROP_OPTION_ADD)
    {
        CVarList vPropList;
        CVarList vValueList;

        MapPropValue mapPropValue;

		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_APPERANCE_PACK,
			package_name, vPropList, vValueList);

        // 合并属性
        PropRefreshModule::m_pInstance->GetFinalEquipModifyObjProp(pKernel, vPropList, vValueList, mapPropValue);

        if (!mapPropValue.empty())
        {
            //最终加成
            PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_ADD);
        }
    }
    else if (op_type == EQUIP_STRPROP_OPTION_REMOVE)
    {
        CVarList vPropList;
        CVarList vValueList;

        MapPropValue mapPropValue;

		StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_APPERANCE_PACK,
			package_name, vPropList, vValueList);

        //合并属性
        PropRefreshModule::m_pInstance->GetFinalEquipModifyObjProp(pKernel, vPropList, vValueList, mapPropValue);

        if (!mapPropValue.empty())
        {
            //移除加成
            PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_REMOVE);
        }        
    }
}

int EquipModifyPackModule::OnDCLevelChange(IKernel* pKernel, const PERSISTID &self, 
	const PERSISTID & sender, const IVarList & args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}
	
	int oldlevel = args.IntVal(1);
	int nowlevel = pSelfObj->QueryInt("Level");

	// 新旧属性对比刷新差值
	CVarList vOldPropNameList;
	CVarList vOldPropValueList;

	CVarList vNewPropNameList;
	CVarList vNewPropValueList;

	char buff[32] = {0};
	itoa(oldlevel, buff, 10);
	std::string strOldLevel = buff;
	itoa(nowlevel, buff, 10);
	std::string strNowLevel = buff;

	StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_LEVELUP_ADDPROP,
		strOldLevel, vOldPropNameList, vOldPropValueList);

	if (vOldPropNameList.GetCount() != vOldPropValueList.GetCount())
	{
		return 0;
	}

	StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_LEVELUP_ADDPROP,
		strNowLevel, vNewPropNameList, vNewPropValueList);

	if (vNewPropNameList.GetCount() != vNewPropValueList.GetCount() ||
		vNewPropNameList.GetCount() != vOldPropNameList.GetCount())
	{
		return 0;
	}

	MapPropValue mapPropValue;

    LoopBeginCheck(x);
	for (size_t i = 0; i < vOldPropNameList.GetCount(); ++i)
	{
        LoopDoCheck(x);
		int ivalue = vNewPropValueList.IntVal(i) - vOldPropValueList.IntVal(i);
		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, 
			vOldPropNameList.StringVal(i), ivalue, EModify_ADD_VALUE, mapPropValue);
	}
 
	bool result = PropRefreshModule::m_pInstance->PlayerRefreshData(pKernel, 
		self, mapPropValue, EREFRESH_DATA_TYPE_ADD);

	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);

	return 0;
}