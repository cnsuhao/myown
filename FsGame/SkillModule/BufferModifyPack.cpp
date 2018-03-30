//--------------------------------------------------------------------
// 文件名:      BufferModifyPack.h
// 内  容:      Buffer属性包逻辑
// 说  明:
// 创建日期:    2016年3月3日
// 创建人:		 刘明飞
//    :       
//--------------------------------------------------------------------

#include "BufferModifyPack.h"
#include "..\CommonModule\PropRefreshModule.h"
#include "utils\exptree.h"
#include "utils\extend_func.h"
#include "..\SystemFunctionModule\StaticDataQueryModule.h"
#include "Impl\SkillDefineDataStruct.h"
#include "Impl\SkillDataQueryModule.h"
#include "..\Define\StaticDataDefine.h"

/**
@brief  初始化
@param  pKernel [IN] 引擎核心指针
@return 返回初始化结果
*/
bool BufferModifyPack::Init(IKernel* pKernel)
{
	//移除buff
	pKernel->AddEventCallback("BufferContainer", "OnRemove", BufferModifyPack::OnBuffRemove);

	//添加buff
	pKernel->AddEventCallback("BufferContainer", "OnAfterAdd", BufferModifyPack::OnBuffAfterAdd);

	// 玩家上线
	pKernel->AddEventCallback("player", "OnRecover", BufferModifyPack::OnPlayerRecover);

	return true;
}

//回调函数 玩家加载数据完成
int BufferModifyPack::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	MapPropValue mapPropValue;

	PERSISTID buffbox = pKernel->GetChild(self, L"BufferContainer");
	if (pKernel->Exists(buffbox))
	{
		unsigned int i;
		PERSISTID buff = pKernel->GetFirst(buffbox, i);
		LoopBeginCheck(m);
		while (pKernel->Exists(buff))
		{
			LoopDoCheck(m);
			BufferModifyPackSingleton::Instance()->UpdateBufferModifyPack(pKernel, self, buff, mapPropValue);
			buff = pKernel->GetNext(buffbox, i);
		}
	}

	PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_ADD);

	return 0;
}

// 增加buff属性包
bool BufferModifyPack::UpdateBufferModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer, MapPropValue& mapPropValue)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	IGameObj* pBufferObj = pKernel->GetGameObj(buffer);
	if (NULL == pSelfObj || NULL == pBufferObj)
	{
		return false;
	}
	IRecord* pPropModifyRec = pSelfObj->GetRecord(CONST_PROP_MODIFY_REC);
	if (NULL == pPropModifyRec)
	{
		return false;
	}

	const char* szFromID = pKernel->GetConfig(buffer);

	bool isMilitaryRank = false;

	// 产生buff的技能对象
	PERSISTID skill = pBufferObj->QueryObject("Skill");

	// 产生buff的对象
	PERSISTID sender = pBufferObj->QueryObject("Sender");

	// 下线保存的buff上线时,sender是空的
	if (!pKernel->Exists(sender))
	{
		sender = self;
	}

	const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(szFromID);
	if ( pBuffBaseData == NULL  || !pBuffBaseData->GetIsModifyProp()) 
	{
		return false;
	}

	// 值增加类型
	int nCalType = pBuffBaseData->GetCalculateType();
	//获取新的包
	std::string strBasePackageID = pBuffBaseData->GetBasePackage();
	if ( strBasePackageID.empty() )
	{
		return false;
	}

	CVarList vPropNameList;
	CVarList vPropValueList;

	//查看对应包的数据
	StaticDataQueryModule::m_pInstance->GetOneRowDataStr(STATIC_DATA_BUFF_BASE_PACK, 
		strBasePackageID, vPropNameList, vPropValueList);

	LoopBeginCheck(v);
	for ( size_t i = 0; i < vPropNameList.GetCount(); ++i )
	{
		LoopDoCheck(v);
		const char* strPropName = vPropNameList.StringVal(i);

		// 获取计算公式数据
		fast_string strPropFormula = vPropValueList.StringVal(i);
		if (strPropFormula.empty())
		{
			continue;
		}

		// 计算技能伤害系数
		ExpTree exp;

		float fVal = exp.CalculateEvent(pKernel, sender, skill, PERSISTID(), PERSISTID(), strPropFormula.c_str());

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, 
			vPropNameList.StringVal(i), fVal, nCalType, mapPropValue);

		MapPropValue::iterator iter = mapPropValue.find(strPropName);
		if (iter != mapPropValue.end())
		{
			fVal = (float)iter->second;
		}

		// 保存BUFF影响的属性数据
		CVarList data;
		data << strPropName << fVal << szFromID;
		pPropModifyRec->AddRowValue(-1, data);
	}

	return true;
}

// 移除buff属性包
bool BufferModifyPack::RemoveBufferModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer, MapPropValue& mapPropValue)
{
	if (!pKernel->Exists(buffer))
	{
		return false;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	IRecord* pPropModifyRec = pSelfObj->GetRecord(CONST_PROP_MODIFY_REC);
	if (NULL == pPropModifyRec)
	{
		return false;
	}

	const char* szFromID = pKernel->GetConfig(buffer);
	const BuffBaseData* pBuffBaseData = SkillDataQueryModule::m_pInstance->GetBuffBaseData(szFromID);
	if ( pBuffBaseData == NULL) 
	{
		return false;
	}

	bool isMilitaryRank = false;
	CVarList propNameList;
	CVarList propValueList;

	LoopBeginCheck(w);
	while (true)
	{
		LoopDoCheck(w);
		int nRow = pPropModifyRec->FindString(PMPS_PROP_SOURCE, szFromID);
		if (nRow < 0)
		{
			break;
		}

		std::string strPropName = pPropModifyRec->QueryString(nRow, PMPS_PROP_NAME);
		float fValue = pPropModifyRec->QueryFloat(nRow, PMPS_PROP_VALUE);

		// fValue值在加入record时已经是计算修正过的最终加值，此处只需传入EModify_ADD_VALUE统计即可
		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, strPropName.c_str(), fValue, EModify_ADD_VALUE, mapPropValue); 

		pPropModifyRec->RemoveRow(nRow);
	}

	return true;
}

// 增加buff
int BufferModifyPack::OnBuffAfterAdd(IKernel* pKernel, const PERSISTID& buffcontainer, const PERSISTID& buffer, const IVarList& args)
{
	if (!pKernel->Exists(buffcontainer) || !pKernel->Exists(buffer))
	{
		return 0;
	}

	PERSISTID player = pKernel->Parent(buffcontainer);
	if (!pKernel->Exists(player))
	{
		return 0;
	}

	MapPropValue mapPropValue;

	//先将这件buff的附加属性包刷到修正表中去
	BufferModifyPackSingleton::Instance()->UpdateBufferModifyPack(pKernel, player, buffer, mapPropValue);

	// 属性值改变
	PropRefreshModule::m_pInstance->RefreshData(pKernel, player, mapPropValue, EREFRESH_DATA_TYPE_ADD);

	return 0;
}

// 移除buff
int BufferModifyPack::OnBuffRemove(IKernel* pKernel, const PERSISTID& buffcontainer, const PERSISTID& buffer, const IVarList& args)
{
	if (!pKernel->Exists(buffcontainer) || !pKernel->Exists(buffer))
	{
		return 0;
	}

	PERSISTID player = pKernel->Parent(buffcontainer);
	if (!pKernel->Exists(player))
	{
		return 0;
	}

	MapPropValue mapPropValue;

	//先将这件buff的附加属性包从修正表中去掉
	BufferModifyPackSingleton::Instance()->RemoveBufferModifyPack(pKernel, player, buffer, mapPropValue);

	// 改变的属性值移除
	PropRefreshModule::m_pInstance->RefreshData(pKernel, player, mapPropValue, EREFRESH_DATA_TYPE_REMOVE);

	return 0;
}