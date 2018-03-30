//--------------------------------------------------------------------
// 文件名:      ApperanceModule.cpp
// 内  容:      外观模块
// 说  明:		
// 创建日期:    2018年03月14日
// 创建人:      tzt      
//--------------------------------------------------------------------
#include "ApperanceModule.h"
#include "FsGame/Define/GameDefine.h"
#include "utils/string_util.h"
#include "CommonModule/CommRuleModule.h"
#include "Define/CommandDefine.h"
#include "EquipStrengthenModule.h"
#include "Define/ContainerDefine.h"
#include "Define/PlayerBaseDefine.h"
#include "utils/XmlFile.h"
#include "utils/extend_func.h"
#include "EquipmentModule.h"
#include "FashionModule.h"

ApperanceModule *ApperanceModule::m_pThis = NULL;

//初始化
bool ApperanceModule::Init(IKernel* pKernel)
{
	m_pThis = this;
	//人物穿脱装备回调
	pKernel->AddEventCallback(CLASS_NAME_EQUIP_BOX, "OnAfterAdd", ApperanceModule::OnEquipBoxAfterAdd, 1);
	pKernel->AddEventCallback(CLASS_NAME_EQUIP_BOX, "OnRemove", ApperanceModule::OnEquipBoxRemove, 1);

	// 获得物品
	pKernel->AddEventCallback(CLASS_NAME_ITEM_BOX, "OnAfterAdd", ApperanceModule::OnItemBoxAfterAdd, 1);

	// 上线
	pKernel->AddEventCallback("player", "OnRecover", ApperanceModule::OnPlayerRecover, 9999);

	// 强化等级变化回调
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_EQUIP_STRENGTHEN_CHANGE_LEVEL, ApperanceModule::OnStrLvlChanged);

	if (!LoadApperancePriRes(pKernel)
		|| !LoadApperancePkgRes(pKernel))
	{
		Assert(false);
		return false;
	}

    return true;
}

//释放
bool ApperanceModule::Shut(IKernel* pKernel)
{
	m_ApperancePriVec.clear();
	m_ApperancePkgVec.clear();
    return true;
}

// 加载外观优先级配置
bool ApperanceModule::LoadApperancePriRes(IKernel* pKernel)
{
	m_ApperancePriVec.clear();
	std::string path = pKernel->GetResourcePath();
	path += APPERANCE_PRI_FILE;

	CXmlFile xml(path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	LoopBeginCheck(a);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(a);
		const char *sec_name = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(sec_name))
		{
			continue;
		}

		const int src = StringUtil::StringAsInt(sec_name);

		ApperancePri cfg(src);
		cfg.pri = xml.ReadInteger(sec_name, "Pri", 0);
		

		m_ApperancePriVec.push_back(cfg);
	}

	return true;
}

// 加载外观穿戴属性包配置
bool ApperanceModule::LoadApperancePkgRes(IKernel* pKernel)
{
	m_ApperancePkgVec.clear();
	std::string path = pKernel->GetResourcePath();
	path += APPERANCE_PKG_FILE;

	CXmlFile xml(path.c_str());
	if (!xml.LoadFromFile())
	{
		std::string err_msg = path;
		err_msg.append(" does not exists.");
		::extend_warning(LOG_ERROR, err_msg.c_str());
		return false;
	}

	LoopBeginCheck(a);
	for (int i = 0; i < (int)xml.GetSectionCount(); ++i)
	{
		LoopDoCheck(a);
		const char *sec_name = xml.GetSectionByIndex(i);
		if (StringUtil::CharIsNull(sec_name))
		{
			continue;
		}

		const int apperance_id = StringUtil::StringAsInt(sec_name);

		ApperancePkg cfg(apperance_id);
		cfg.package = xml.ReadInteger(sec_name, "Package", 0);

		m_ApperancePkgVec.push_back(cfg);
	}

	return true;
}

// 取得外观显示优先级
const int ApperanceModule::GetApperancePri(const int src)
{
	ApperancePri temp(src);
	ApperancePriVec::const_iterator find_it = find(
		m_ApperancePriVec.begin(),
		m_ApperancePriVec.end(),
		temp);
	if (find_it == m_ApperancePriVec.end())
	{
		return 0;
	}

	return find_it->pri;
}

// 取得外观穿戴属性包
const int ApperanceModule::GetApperancePkg(const int apperance_id)
{
	ApperancePkg temp(apperance_id);
	ApperancePkgVec::const_iterator find_it = find(
		m_ApperancePkgVec.begin(),
		m_ApperancePkgVec.end(),
		temp);
	if (find_it == m_ApperancePkgVec.end())
	{
		return 0;
	}

	return find_it->package;
}

// 改变外观
void ApperanceModule::Change(IKernel *pKernel, const PERSISTID &self, 
	const EQUIP_POS equip_pos)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 外观表
	IRecord *pAppRec = pSelf->GetRecord(FIELD_RECORD_APPERANCE_REC);
	if (NULL == pAppRec)
	{
		return;
	}

	if (!CommRuleModule::m_pThis->IsValidEquipPos(equip_pos))
	{
		return;
	}

	const char *apperance_prop = GetApperanceProp(EQUIP_POS(equip_pos));
	if (StringUtil::CharIsNull(apperance_prop))
	{
		return;
	}

	// 改变前外观id，移除影响属性包
	const int old_apperance_id = pSelf->QueryInt(apperance_prop);
	const int old_pkg = GetApperancePkg(old_apperance_id);
	if (old_pkg != 0)
	{
		// 属性包更新
		CVarList s2s_msg;
		s2s_msg << COMMAND_APPERANCE_PKG
				<< EQUIP_STRPROP_OPTION_REMOVE
				<< old_pkg;
		pKernel->Command(self, self, s2s_msg);
	}

	// 选出优先级最高的外观id
	int show_id = 0;
	int init_pri = INT_MIN;

	LoopBeginCheck(a);
	for (int row = pAppRec->GetRows() - 1; row >= 0; --row)
	{
		LoopDoCheck(a);
		if (pAppRec->QueryInt(row, COLUMN_APPERANCE_REC_TYPE) != equip_pos)
		{
			continue;
		}

		const int src = pAppRec->QueryInt(row, COLUMN_APPERANCE_REC_SOURCE);
		const int pri = GetApperancePri(src);
		if (pri >= init_pri)
		{
			show_id = pAppRec->QueryInt(row, COLUMN_APPERANCE_REC_ID);
			init_pri = pri;
		}
	}

	pSelf->SetInt(apperance_prop, show_id);

	// 新外观属性包
	const int new_pkg = GetApperancePkg(show_id);

	// 属性包更新
	CVarList s2s_msg;
	s2s_msg << COMMAND_APPERANCE_PKG
			<< EQUIP_STRPROP_OPTION_ADD
			<< new_pkg;
	pKernel->Command(self, self, s2s_msg);
}

// 增加一个外观
void ApperanceModule::Add(IKernel *pKernel, const PERSISTID &self, 
	const EQUIP_POS equip_pos, const ApperanceSource src, const int apperance_id)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 外观表
	IRecord *pAppRec = pSelf->GetRecord(FIELD_RECORD_APPERANCE_REC);
	if (NULL == pAppRec)
	{
		return;
	}

	if (apperance_id <= 0)
	{
		return;
	}

	if (!CommRuleModule::m_pThis->IsValidEquipPos(equip_pos))
	{
		return;
	}

	if (src <= APPERANCE_SOURCE_NONE 
		|| src >= APPERANCE_SOURCE_MAX)
	{
		return;
	}

	CVarList row_value;
	row_value << apperance_id
			  << src
			  << equip_pos;
	pAppRec->AddRowValue(-1, row_value);

	// 外观变化
	Change(pKernel, self, equip_pos);
}

// 根据装备部位和来源删除一个外观
void ApperanceModule::Remove(IKernel *pKernel, const PERSISTID &self, 
	const EQUIP_POS equip_pos, const ApperanceSource src)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	// 外观表
	IRecord *pAppRec = pSelf->GetRecord(FIELD_RECORD_APPERANCE_REC);
	if (NULL == pAppRec)
	{
		return;
	}

	LoopBeginCheck(a);
	for (int row = pAppRec->GetRows() - 1; row >= 0; --row)
	{
		LoopDoCheck(a);
		if (pAppRec->QueryInt(row, COLUMN_APPERANCE_REC_TYPE) != equip_pos)
		{
			continue;
		}

		if (pAppRec->QueryInt(row, COLUMN_APPERANCE_REC_SOURCE) != src)
		{
			continue;
		}

		pAppRec->RemoveRow(row);
	}

	// 外观变化
	Change(pKernel, self, equip_pos);
}

// 根据性别取得装备外观
const int ApperanceModule::GetEquipApperance(IKernel *pKernel, const PERSISTID &self,
	const PERSISTID &equip)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	IGameObj *pEquip = pKernel->GetGameObj(equip);
	if (NULL == pEquip)
	{
		return 0;
	}

	// 玩家性别
	const int sex = pSelf->QueryInt(FIELD_PROP_SEX);

	if (SEX_MAN == sex)
	{
		return pEquip->QueryInt(FIELD_PROP_APPERANCE_M);
	}

	if (SEX_WOMAN == sex)
	{
		return pEquip->QueryInt(FIELD_PROP_APPERANCE_W);
	}

	return 0;
}

// 根据性别取得装备时装
const int ApperanceModule::GetEquipFashion(IKernel *pKernel, const PERSISTID &self,
	const PERSISTID &equip)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	IGameObj *pEquip = pKernel->GetGameObj(equip);
	if (NULL == pEquip)
	{
		return 0;
	}

	// 玩家性别
	const int sex = pSelf->QueryInt(FIELD_PROP_SEX);

	if (SEX_MAN == sex)
	{
		return pEquip->QueryInt(FIELD_PROP_FASHION_M);
	}

	if (SEX_WOMAN == sex)
	{
		return pEquip->QueryInt(FIELD_PROP_FASHION_W);
	}

	return 0;
}

// 取得指定位置的外观ID
const int ApperanceModule::GetApperanceByEquipPos(IKernel *pKernel, const PERSISTID &self, 
	const EQUIP_POS equip_pos)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	if (!CommRuleModule::m_pThis->IsValidEquipPos(equip_pos))
	{
		return 0;
	}

	const char *apperance_prop = GetApperanceProp(EQUIP_POS(equip_pos));
	if (StringUtil::CharIsNull(apperance_prop))
	{
		return 0;
	}

	return pSelf->QueryInt(apperance_prop);
}

// 取得指定部位外观属性
const char * ApperanceModule::GetApperanceProp(const EQUIP_POS equip_pos)
{
	// 衣服
	if (EQUIP_POS_CLOTH == equip_pos)
	{
		return FIELD_PROP_CLOTH;
	}
	else if (EQUIP_POS_WEAPON == equip_pos)
	{
		return FIELD_PROP_WEAPON;
	}
	else if (EQUIP_POS_HELMET == equip_pos)
	{
		return FIELD_PROP_HAIR;
	}

	return "";
}

// 初始化穿戴属性包
void ApperanceModule::InitPackage(IKernel *pKernel, const PERSISTID &self)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return;
	}

	for (int pos = EQUIP_POS_STR_MIN; pos <= EQUIP_POS_STR_MAX; ++pos)
	{
		const int apperance_id = GetApperanceByEquipPos(pKernel, self, EQUIP_POS(pos));
		if (apperance_id == 0)
		{
			continue;
		}

		const int pkg = GetApperancePkg(apperance_id);
		if (pkg == 0)
		{
			continue;
		}

		// 属性包更新
		CVarList s2s_msg;
		s2s_msg << COMMAND_APPERANCE_PKG
				<< EQUIP_STRPROP_OPTION_ADD
				<< pkg;
		pKernel->Command(self, self, s2s_msg);
	}
}

// 穿装备
int ApperanceModule::OnEquipBoxAfterAdd(IKernel* pKernel, const PERSISTID& equipbox, 
	const PERSISTID& sender, const IVarList& args)
{
	PERSISTID owner = pKernel->Parent(equipbox);
	IGameObj *pSelf = pKernel->GetGameObj(owner);
	if (NULL == pSelf)
	{
		return 0;
	}

	if (pSelf->GetClassType() != TYPE_PLAYER)
	{
		return 0;
	}

	IGameObj *pEquip = pKernel->GetGameObj(sender);
	if (NULL == pEquip)
	{
		return 0;
	}

	const char *equip_type = pEquip->QueryString(FIELD_PROP_EQUIP_TYPE);
	if (StringUtil::CharIsNull(equip_type))
	{
		return  0;
	}

	// 装备部位
	const int equip_pos = EquipmentModule::m_pEquipmentModule->GetEquipIndex(equip_type);

	// 增加装备外观
	const int equip_apper = m_pThis->GetEquipApperance(pKernel, owner, sender);
	if (equip_apper > 0)
	{
		ApperanceModule::m_pThis->Add(pKernel, owner, EQUIP_POS(equip_pos), APPERANCE_SOURCE_EQUIP, equip_apper);
	}

	// 增加强化外观
	const int str_lvl = EquipStrengthenModule::m_pInstance->GetStrLvlByPos(pKernel, owner, equip_pos);
	const int strengthen_apper = EquipStrengthenModule::m_pInstance->GetStrApperance(pEquip->GetConfig(), str_lvl);
	if (strengthen_apper > 0)
	{
		ApperanceModule::m_pThis->Add(pKernel, owner, EQUIP_POS(equip_pos), APPERANCE_SOURCE_STRENGTHEN, strengthen_apper);
	}

	return 0;
}

// 脱装备
int ApperanceModule::OnEquipBoxRemove(IKernel* pKernel, const PERSISTID& equipbox, 
	const PERSISTID& sender, const IVarList& args)
{
	PERSISTID owner = pKernel->Parent(equipbox);
	IGameObj *pSelf = pKernel->GetGameObj(owner);
	if (NULL == pSelf)
	{
		return 0;
	}

	if (pSelf->GetClassType() != TYPE_PLAYER)
	{
		return 0;
	}

	IGameObj *pEquip = pKernel->GetGameObj(sender);
	if (NULL == pEquip)
	{
		return 0;
	}

	const char *equip_type = pEquip->QueryString(FIELD_PROP_EQUIP_TYPE);
	if (StringUtil::CharIsNull(equip_type))
	{
		return  0;
	}

	// 装备部位
	const int equip_pos = EquipmentModule::m_pEquipmentModule->GetEquipIndex(equip_type);

	// 去除装备外观
	ApperanceModule::m_pThis->Remove(pKernel, owner, EQUIP_POS(equip_pos), APPERANCE_SOURCE_EQUIP);

	// 去除强化外观
	ApperanceModule::m_pThis->Remove(pKernel, owner, EQUIP_POS(equip_pos), APPERANCE_SOURCE_STRENGTHEN);

	return 0;
}

// 获得物品
int ApperanceModule::OnItemBoxAfterAdd(IKernel* pKernel, const PERSISTID& item_box, 
	const PERSISTID& sender, const IVarList& args)
{
	PERSISTID owner = pKernel->Parent(item_box);
	IGameObj *pSelf = pKernel->GetGameObj(owner);
	if (NULL == pSelf)
	{
		return 0;
	}

	if (pSelf->GetClassType() != TYPE_PLAYER)
	{
		return 0;
	}

	IGameObj *pItem = pKernel->GetGameObj(sender);
	if (NULL == pItem)
	{
		return 0;
	}

	// 非装备不处理
	if (!StringUtil::CharIsEqual(pItem->GetScript(), CLASS_NAME_EQUIPMENT))
	{
		return 0;
	}

	// 取得装备的时装id
	const int fashion_id = m_pThis->GetEquipFashion(pKernel, owner, sender);
	if (fashion_id == 0)
	{
		return 0;
	}

	// 解锁时装
	FashionModule::m_pFashionModule->GmUnlock(pKernel, owner, fashion_id, 0, LOG_FASHION_UNLOCK_EQUIP);

	return 0;
}

// 强化等级变化
int ApperanceModule::OnStrLvlChanged(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	IGameObj *pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	PERSISTID equipbox = pKernel->GetChild(self, EQUIP_BOX_NAME_WSTR);
	if (!pKernel->Exists(equipbox))
	{
		return 0;
	}

	const int equip_pos = args.IntVal(1);

	// 取出部位的装备
	PERSISTID equip = pKernel->GetItem(equipbox, equip_pos);
	IGameObj *pEquip = pKernel->GetGameObj(equip);
	if (NULL == pEquip)
	{
		return 0;// 无装备
	}

	// 取出新强化等级对应的外观
	const int str_lvl = EquipStrengthenModule::m_pInstance->GetStrLvlByPos(pKernel, self, equip_pos);
	const int strengthen_apper = EquipStrengthenModule::m_pInstance->GetStrApperance(pEquip->GetConfig(), str_lvl);
	if (strengthen_apper > 0)
	{
		// 去除旧强化外观
		ApperanceModule::m_pThis->Remove(pKernel, self, EQUIP_POS(equip_pos), APPERANCE_SOURCE_STRENGTHEN);

		// 增加新强化外观
		ApperanceModule::m_pThis->Add(pKernel, self, EQUIP_POS(equip_pos), APPERANCE_SOURCE_STRENGTHEN, strengthen_apper);
	}

	return 0;
}

// 玩家数据恢复完成
int ApperanceModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	m_pThis->InitPackage(pKernel, self);

	return 0;
}

