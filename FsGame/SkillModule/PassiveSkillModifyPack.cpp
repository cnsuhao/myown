//--------------------------------------------------------------------
// 文件名:      PassiveSkillModifyPack.h
// 内  容:      被动技能属性包逻辑
// 说  明:
// 创建日期:    2016年3月3日
// 创建人:		 刘明飞
//    :       
//--------------------------------------------------------------------

#include "..\CommonModule\PropRefreshModule.h"
#include "PassiveSkillModifyPack.h"
#include "..\Define\CommandDefine.h"
#include "utils\string_util.h"

/**
@brief  初始化
@param  pKernel [IN] 引擎核心指针
@return 返回初始化结果
*/
bool PassiveSkillModifyPack::Init(IKernel* pKernel)
{
	// 被动技能升级消息处理
	pKernel->AddIntCommandHook("player", COMMAND_PASSIVE_SKILL_ADD_PROP, PassiveSkillModifyPack::AddPassiveSkillModifyPack);

	// 被动技能升级消息处理
	pKernel->AddIntCommandHook("PetNpc", COMMAND_PASSIVE_SKILL_ADD_PROP, PassiveSkillModifyPack::AddPassiveSkillModifyPack);

	// 被动技能移除消息处理
	pKernel->AddIntCommandHook("player", COMMAND_PASSIVE_SKILL_REMOVE_PROP, PassiveSkillModifyPack::RemovePassiveSkillModifyPack);

	// 被动技能移除消息处理
	pKernel->AddIntCommandHook("PetNpc", COMMAND_PASSIVE_SKILL_REMOVE_PROP, PassiveSkillModifyPack::RemovePassiveSkillModifyPack);

	return true;
}

// 更新被动技能影响人物的属性
int PassiveSkillModifyPack::AddPassiveSkillModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	IRecord* pPropModifyRec = pSelfObj->GetRecord(CONST_PROP_MODIFY_BYEQUIP_REC);
	if (NULL == pPropModifyRec)
	{
		return 0;
	}

	int nIndex = 1;
	int nSkillId = args.IntVal(nIndex++);
	int nPropCount = args.IntVal(nIndex++);

	CVarList propNameList;
	CVarList propValueList;

	// 统计对象属性值改变
	MapPropValue mapPropValue;
	LoopBeginCheck(b);
	for (int i = 0;i < nPropCount;++i)
	{
		LoopDoCheck(b);
		const char* strPorp = args.StringVal(nIndex++);
		float fValue = args.FloatVal(nIndex++);

		propNameList << strPorp;
		propValueList << fValue;

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, strPorp, fValue, EModify_ADD_VALUE, mapPropValue);

		// 被动技能自身属性修正表
		CVarList data;
		data << strPorp << fValue << StringUtil::IntAsString(nSkillId);
		pPropModifyRec->AddRowValue(-1, data);
	}

	// 增加人物属性
	PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_ADD);
	return 0;
}

// 移除被动技能影响人物的属性
int PassiveSkillModifyPack::RemovePassiveSkillModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	int nPassiveSkillId = args.IntVal(1);
	std::string strPassiveSkillId = StringUtil::IntAsString(nPassiveSkillId);
	IRecord* pPropModifyRec = pSelfObj->GetRecord(CONST_PROP_MODIFY_BYEQUIP_REC);
	if (NULL == pPropModifyRec)
	{
		return 0;
	}

	CVarList propNameList;
	CVarList propValueList;

	// 统计对象属性值改变
	MapPropValue mapPropValue;
	LoopBeginCheck(c);
	while (true)
	{
		LoopDoCheck(c);
		int nRow = pPropModifyRec->FindString(PMPS_PROP_SOURCE, strPassiveSkillId.c_str());
		if (nRow < 0)
		{
			break;
		}

		std::string strPropName = pPropModifyRec->QueryString(nRow, PMPS_PROP_NAME);
		float fValue = pPropModifyRec->QueryFloat(nRow, PMPS_PROP_VALUE);

		propNameList << strPropName.c_str();
		propValueList << fValue;

		// fValue值在加入record时已经是计算修正过的最终加值，此处只需传入EModify_ADD_VALUE统计即可
		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, strPropName.c_str(), fValue, EModify_ADD_VALUE, mapPropValue); 
		pPropModifyRec->RemoveRow(nRow);
	}

	// 属性值清除
	PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_REMOVE);
	return 0;
}