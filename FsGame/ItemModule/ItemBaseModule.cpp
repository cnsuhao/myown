//--------------------------------------------------------------------
// 文件名:		ItemBaseModule.cpp
// 内  容:		物品基础数据
// 说  明:		
// 创建日期:	2014年10月17日
// 创建人:		 
// 修改人：
//    :	   
//--------------------------------------------------------------------
#include "ItemBaseModule.h"
#include "utils/extend_func.h"
#include "utils/XmlFile.h"
#include "utils/string_util.h"
//#include "FsGame/ItemModule/EquipmentModule.h"
#include "FsGame/CommonModule/LuaExtModule.h "
#include "FsGame/CommonModule/ContainerModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
//#include "FsGame/SkillModule/BianstoneModule.h"
#include "ToolItem/ToolItemModule.h"
#endif
#include "CommonModule/FunctionEventModule.h"
#include "FsGame/Define/GameDefine.h"
#include "EquipmentModule/EquipmentModule.h"


const char* const ITEM_CONFIG_FILE = "ini/item/PathConfig.xml";

ItemBaseModule * ItemBaseModule::m_pItemBaseModule = NULL;
//EquipmentModule * ItemBaseModule::m_pEquipmentModule = NULL;
#ifndef FSROOMLOGIC_EXPORTS
//BianstoneModule* ItemBaseModule::m_pBianstoneModule = NULL;
#endif
struct SortItem 
{
	const char* configID;
	int colorLevel;
	int amount;

	bool operator<(const SortItem& args)
	{
		if (args.colorLevel < colorLevel)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

// 重加载物品基础配置文件
int nx_reload_item_base_config(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	return true == ItemBaseModule::m_pItemBaseModule->LoadItemPathConfig(pKernel) ? 1 : 0;
}

bool ItemBaseModule::Init(IKernel* pKernel)
{
	m_pItemBaseModule = this;
// 	m_pEquipmentModule = (EquipmentModule*)pKernel->GetLogicModule("EquipmentModule");
// 	Assert(m_pEquipmentModule != NULL );

#ifndef FSROOMLOGIC_EXPORTS
// 	m_pBianstoneModule = (BianstoneModule*)pKernel->GetLogicModule("BianstoneModule");
// 	Assert(m_pBianstoneModule != NULL);
#endif

	pKernel->AddClassCallback("item", "OnCreateClass", OnCreateClass);

	pKernel->AddEventCallback("item", "OnCreate", OnCreate);
	pKernel->AddEventCallback("item", "OnLoad", OnLoad);
	pKernel->AddEventCallback("item", "OnRecover", OnRecover);

	DECL_CRITICAL(ItemBaseModule::C_OnAmountChanged);																								   
	DECL_LUA_EXT(nx_reload_item_base_config);

	LoadItemPathConfig(pKernel);

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("ItemBaseModule", ItemBaseModule::ReloadItemBaseConfig);
#endif

	return true;
}

bool ItemBaseModule::Shut(IKernel* pKernel)
{
	return true;
}

int ItemBaseModule::OnCreateClass(IKernel* pKernel, int index)
{
#ifndef FSROOMLOGIC_EXPORTS
	createclass_from_xml(pKernel, index, "struct\\item\\item.xml");
#else
	createclass_from_xml(pKernel, index, "room_struct\\item\\item.xml");
#endif // FSROOMLOGIC_EXPORTS
	return 0;
}

int ItemBaseModule::OnCreate(IKernel * pKernel, const PERSISTID & self, 
						   const PERSISTID & sender, const IVarList & args)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}
	pSelf->SetInt("Type", TYPE_ITEM);

	return 0;
}

int ItemBaseModule::OnLoad(IKernel * pKernel, const PERSISTID & self, 
		   const PERSISTID & sender, const IVarList & args)
{
	//pKernel->SetString(self, "ConfigID", pKernel->GetConfig(self));

	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}
	//生成唯一标识号
	::gen_item_unique_id(pKernel, self);

	//当前数量与最大数量的处理
	int amount = pSelf->QueryInt("Amount");
	int maxamount = pSelf->QueryInt("MaxAmount");

	if (amount == 0)
	{
		amount = 1;
		pSelf->SetInt("Amount", amount);
	}

	if (maxamount == 0)
	{
		maxamount = 1;
		pSelf->SetInt("MaxAmount", maxamount);
	}

	if (amount > maxamount)
	{
		amount = maxamount;
		pSelf->SetInt("Amount", amount);
	}

	pKernel->AddCritical(self, "Amount", "ItemBaseModule::C_OnAmountChanged");
	return 0;
}

int ItemBaseModule::OnRecover(IKernel * pKernel, const PERSISTID & self, 
						   const PERSISTID & sender, const IVarList & args)
{
	//最大数量的处理
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}
	if (pSelf->QueryInt("MaxAmount") == 0)
		pSelf->SetInt("MaxAmount", 1);

	pKernel->AddCritical(self, "Amount", "ItemBaseModule::C_OnAmountChanged");
	return 0;
}

int ItemBaseModule::C_OnAmountChanged(IKernel * pKernel, const PERSISTID & self, const char* property, const IVar & old)
{
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}
	int amount = pSelf->QueryInt("Amount");
	if (amount <= 0)
	{
		//数量到0，删除之
		pKernel->DestroySelf(self);
		return 0;
	}

	// 已经提醒去使用的数量
	int remindAmount = pSelf->QueryInt("RemindAmount");
	if (remindAmount > amount)
	{
		// 当前数量减少 重置当前提醒的数量
		pSelf->SetInt("RemindAmount", amount);
	}

	return 0;
}

bool ItemBaseModule::LoadItemPathConfig( IKernel* pKernel )
{
	std::string xmlPath = pKernel->GetResourcePath();
	xmlPath += ITEM_CONFIG_FILE;

	CXmlFile xml(xmlPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_WARNING, "[ItemBaseModule::LoadItemPathConfig] failed");
		return false;
	}

	const char* ITEM_DIR = "ini/Item/";
	size_t nSecCount = xml.GetSectionCount();
	LoopBeginCheck(a);
	for (size_t i = 0; i < nSecCount; ++i)
	{
		LoopDoCheck(a);

		const char* section = xml.GetSectionByIndex(i);
		const char* path = xml.ReadString(section, "Path", "");
		if (0 == path || 0 == strcmp("", path))
		{
			continue;
		}

		std::string tmp_file_name = ITEM_DIR;
		tmp_file_name += path;
		if (!pKernel->PreloadConfig(tmp_file_name.c_str()))
		{
			extend_warning(LOG_ERROR, "Load ItemResource %s Error!", tmp_file_name.c_str());
		}
	}

	return true;
}

//创建物品
PERSISTID ItemBaseModule::CreateItem(IKernel * pKernel, const PERSISTID & container, const char * config, int count,
	EmFunctionEventId nSrcFromId/* = FUNCTION_EVENT_ID_SYS*/, bool bIsGain /*= false*/, int equip_prop_percent)
{
	PERSISTID item;

	if (!pKernel->Exists(container))
	{
		return item;
	}

	if (StringUtil::ItemIsNull(config))
	{
		return item;
	}

	PERSISTID sceneid = pKernel->GetScene();
	item = pKernel->CreateFromConfig(sceneid, "", config);

	// 创建装备时count为ColorLevel并给属性
	IGameObj* pItem = pKernel->GetGameObj(item);
	if (NULL == pItem)
	{
		 return PERSISTID();
	}

	const char * szScript = pItem->GetScript();
	if (!StringUtil::CharIsNull(szScript) && strcmp(szScript, "Equipment") == 0)
	{
	    EquipmentModule::m_pEquipmentModule->SetRandEquipProp(pKernel, item, item, count, equip_prop_percent);

		pItem->SetInt("Amount", 1);
	}
#ifndef FSROOMLOGIC_EXPORTS
	else if (strcmp(szScript, "Bianstone") == 0) // 砭石增加随机属性
	{
		//m_pBianstoneModule->InitItemProperty(pKernel, item, count);

		pItem->SetInt("Amount", 1);
	}
#endif
	else
	{
		if (count < 1)	count = 1;

		pItem->SetInt("Amount", count);
	}

	//if (container != sceneid && !pKernel->Place(item, container))
	//{
	//	return PERSISTID();
	//}
	
	if (container != sceneid )
	{
		int nBindState = FunctionEventModule::GetItemBindState(nSrcFromId);
#ifndef FSROOMLOGIC_EXPORTS
		if (nBindState == ITEM_BOUND && ToolItemModule::IsBindAble(pKernel, item) )
		{
			pKernel->SetInt(item, FIELD_PROP_BIND_STATUS, nBindState);
		}
#endif
		if (ContainerModule::PlaceItem(pKernel, container, item, nSrcFromId, bIsGain) <= 0)
		{
			pKernel->DestroySelf(item);
			return PERSISTID();
		}
	}

	return item;
}

// 重新加载物品基础配置
void ItemBaseModule::ReloadItemBaseConfig(IKernel* pKernel)
{
	ItemBaseModule::m_pItemBaseModule->LoadItemPathConfig(pKernel);
}

//物品排序
bool ItemBaseModule::ItemSort(IKernel * pKernel, CVarList args, CVarList &returnList)
{
	std::list<SortItem> itemList;
	std::list<SortItem> equipList;
	int size = (int)args.GetCount();
	LoopBeginCheck(b);
	for (int i = 0; i < size; i = i + 2)
	{
		LoopDoCheck(b);
		const char* config = args.StringVal(i);
		int amount = args.IntVal(i + 1);

		SortItem item;

		const char* script = pKernel->GetConfigProperty(config, "Script");

		if (strcmp("Equipment", script) == 0)
		{
			//装备
			item.configID = config;
			item.colorLevel = amount;
			item.amount = amount; // 装备的时候,这个值代表品质

			equipList.push_back(item);
		}
		else
		{
			const char* colorProperty = pKernel->GetConfigProperty(config, "ColorLevel");
			int colorLevel = StringUtil::StringAsInt(colorProperty);

			item.configID = config;
			item.colorLevel = colorLevel;
			item.amount = amount;

			itemList.push_back(item);
		}
	}
	equipList.sort();
	itemList.sort();
	std::list<SortItem>::iterator itemIter = itemList.begin();
	LoopBeginCheck(c);
	for ( ; itemIter != itemList.end(); ++itemIter)
	{
		LoopDoCheck(c);
		SortItem& obj = *itemIter;

		returnList << obj.configID << obj.amount;
	}
	std::list<SortItem>::iterator equipIter = equipList.begin();
	LoopBeginCheck(d);
	for ( ; equipIter != equipList.end(); ++equipIter)
	{
		LoopDoCheck(d);
		SortItem& obj = *equipIter;

		returnList << obj.configID << obj.amount;
	}
	return true;
}