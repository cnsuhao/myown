//------------------------------------------------------------------------------
// 文件名:       PropRefreshModule.cpp
// 内  容:
// 说  明:
// 创建日期:    2012年12月28日
// 创建人:        
// 修改人:		  
// 备注:
//    :       
//------------------------------------------------------------------------------

#include "PropRefreshModule.h"
#include <algorithm>
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/exptree.h"
#include "utils/string_util.h"
#include "utils/cau.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/Define/PlayerBaseDefine.h"
#include "FsGame/Define/GameDefine.h"
//#include "FsGame/Define/EquipDefine.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
//#include "FsGame/ItemModule/FashionModule.h"
//#include "FsGame/ItemModule/EquipmentModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame/CommonModule/ReLoadConfigModule.h"
#endif
#include "Define/CommandDefine.h"
// 获取变量类型的枚举定义值
int GetVarType(const char* szVarType)
{
	std::string str = szVarType;
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	if (::strcmp(str.c_str(), "int") == 0)
	{
		return VTYPE_INT;
    }
    if (::strcmp(str.c_str(), "int64") == 0)
    {
        return VTYPE_INT64;
    }
	if (::strcmp(str.c_str(), "float") == 0)
	{
		return VTYPE_FLOAT;
	}
	if (::strcmp(str.c_str(), "string") == 0)
	{
		return VTYPE_STRING;
	}

	return VTYPE_UNKNOWN;
}

const char* const CONST_PROP_RELATION_FORMULA_XML = "ini/ModifyPackage/PropDefined.xml";

PropRefreshModule* PropRefreshModule::m_pInstance = NULL;

inline int nx_reload_proprefresh_config(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 如果指针存在，表明模块已创建
	if (NULL != PropRefreshModule::m_pInstance)
	{
		PropRefreshModule::m_pInstance->LoadResource(pKernel);
	}

	return 0;
}

bool PropRefreshModule::Init(IKernel* pKernel)
{
	m_pInstance = this;

	// 比CreateRole的OnRecover晚一些调用
	pKernel->AddEventCallback("player", "OnRecover", PropRefreshModule::OnPlayerRecover, INT_MIN + 1);
	// 玩家等级变化
	pKernel->AddIntCommandHook("player", COMMAND_LEVELUP_CHANGE, OnDCLevelChange);

	LoadResource(pKernel);
	//声明属性改变回调
	DECL_CRITICAL(PropRefreshModule::C_OnBindPropChanged);
	DECL_LUA_EXT(nx_reload_proprefresh_config);
#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("PropRefreshConfig", PropRefreshModule::ReloadConfig);
#endif
	return true;
}

bool PropRefreshModule::Shut(IKernel* pKernel)
{
	return true;
}

//加载关系数据
bool PropRefreshModule::LoadResource(IKernel* pKernel)
{
	LoadPropRelationData(pKernel);
	return true;
}

// 更新玩家属性  nClassType 属性包类型  strFormId 属性包在表中的id nNewPackageId 更新的属性包id 
bool PropRefreshModule::UpdateModifyProp(IKernel* pKernel, const PERSISTID& self, int nClassType, const char* strFormId, int nNewPackageId)
{
	if (!pKernel->Exists(self) || nNewPackageId <= 0 || NULL == m_pInstance)
	{
		return false;
	}

	m_pInstance->OnRemovePlayerProperty(pKernel, self, strFormId);

	// 属性数据
	CVarList PropNameList;
	CVarList PropValueList;

	m_pInstance->GetPropPackage(nClassType, nNewPackageId, PropNameList, PropValueList);
	m_pInstance->OnAddPlayerProperty(pKernel, self, strFormId, PropNameList, PropValueList);
	return true;
}

// 回调函数 玩家加载数据完成
int PropRefreshModule::OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
									   const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}
#ifndef FSROOMLOGIC_EXPORTS
	// 初始化等级成长属性
	m_pInstance->InitPlayerLevelProp(pKernel, self);
	
	//m_pInstance->InitPlayerBaseBA(pKernel, self);

	// 计算属性
	m_pInstance->CalFormulaForProp(pKernel,self);
#endif // FSROOMLOGIC_EXPORTS

	// 绑定属性回调刷新
	m_pInstance->BindPlayerRefreshProp(pKernel, self);

	return 0;
}

// 加载属性关系数据
bool PropRefreshModule::LoadPropRelationData(IKernel* pKernel)
{
	m_PropRelationMap.clear();
	m_PropRelationIndex.clear();

	std::string strFileName = pKernel->GetResourcePath();
	strFileName += CONST_PROP_RELATION_FORMULA_XML;
	CXmlFile xml(strFileName.c_str());
	if (!xml.LoadFromFile())
	{
		::extend_warning(LOG_WARNING,
			"[PropRefreshModule::LoadPropRelationData] not found file:%s.",
			strFileName.c_str());
		return false;
	}

	CVarList vSectionList;
	xml.GetSectionList(vSectionList);
	int nSectionCount = (int)vSectionList.GetCount();
    LoopBeginCheck(a);
	for (int t = 0; t < nSectionCount; t++)
	{
        LoopDoCheck(a);
		std::string strSection = vSectionList.StringVal(t);
		if (strSection.empty() || strSection.compare("-1") == 0 || strSection.compare("0") == 0)
		{
			continue;
		}

		PropDescStruct propdata;
		const char* szDataType = xml.ReadString(strSection.c_str(), "DataType", "");
		propdata.iDataType = (short)::GetVarType(szDataType);

		// 解析公式，设置影响属性列表
		const char* szRelationFormulaDef = xml.ReadString(strSection.c_str(), "RelationFormula0", "");
		CVarList vPropList;
		for (int i = 1; i < PLAYER_JOB_MAX; ++i)
		{
			char strKey[32] = { 0 };
			SPRINTF_S(strKey, "RelationFormula%d", i);
			const char* szRelationFormula = xml.ReadString(strSection.c_str(), strKey, "");
			propdata.sPropFormual.strRelationFormula[i] = StringUtil::CharIsNull(szRelationFormula) ? szRelationFormulaDef : szRelationFormula;

			GetRelationPropList(pKernel, propdata.sPropFormual.strRelationFormula[i].c_str(), vPropList);
		}

		m_PropRelationMap.insert(std::make_pair(strSection, propdata));
		m_PropRelationIndex.push_back(strSection);

        LoopBeginCheck(b);
		for (int i = 0; i < (int)vPropList.GetCount(); ++i)
		{
            LoopDoCheck(b);
			std::string strEffectProp = vPropList.StringVal(i);
			std::map<std::string, PropDescStruct>::iterator it = m_PropRelationMap.find(strEffectProp);
			if (it == m_PropRelationMap.end())
			{
				PropDescStruct p;
				p.vecEffectProp.push_back(strSection);
				m_PropRelationMap.insert(std::make_pair(strEffectProp, p));
				continue;
			}

			it->second.vecEffectProp.push_back(strSection);
		}
	}

	return true;
}

// 从公式中解析各个属性名，获取影响公式结果的属性
bool PropRefreshModule::GetRelationPropList(IKernel* pKernel, const char* propFormula, IVarList& res) const
{								  
	std::string strFormula = propFormula;
	std::vector<std::pair<char, std::string> > vecPropList;
	::GetPropNameFromFormula(strFormula.c_str(), vecPropList);
    LoopBeginCheck(e);
	for (int t = 0; t < (int)vecPropList.size(); ++t)
	{
        LoopDoCheck(e);
		res << vecPropList[t].second.c_str();
	}

	return true;
}

// 解析公式，计算数值
void PropRefreshModule::AnalyzeFormula(IKernel* pKernel, const PERSISTID& self, const char* szPropName, const PropFormula& propFormula)
{
	if (!pKernel->Exists(self) || '\0' == *szPropName)
	{
		return;
	}

	IGameObj * player = pKernel->GetGameObj(self);
	if (NULL == player)
	{
		return;
	}

	// 是否存在属性
	if (!player->FindAttr(szPropName))
	{
		return;
	}

	const char* strFormula = GetRelationFormula(pKernel, self, propFormula);

	ExpTree exp;
	float fvalue = 0.0f;
	if (FORMULA_TYPE_PARAM == propFormula.iFormulaType)
	{
		float params[MAX_PARAM_NUM];
		std::string strSrcProp;
        LoopBeginCheck(f);
		for (int i = 0; i < MAX_PARAM_NUM; ++i)
		{
            LoopDoCheck(f);
			strSrcProp = propFormula.props[i + 1];
			if (!player->FindAttr(strSrcProp.c_str()))
			{
				continue;
			}
			switch (player->GetAttrType(strSrcProp.c_str()))
			{
			case VTYPE_INT:
				params[i] = (float)player->QueryInt(strSrcProp.c_str());
                break;
                
            case VTYPE_INT64:
                params[i] = (float)player->QueryInt64(strSrcProp.c_str());
                break;

			case VTYPE_FLOAT:
				params[i] = player->QueryFloat(strSrcProp.c_str());
				break;

			case VTYPE_DOUBLE:
				params[i] = (float)player->QueryDouble(strSrcProp.c_str());
				break;

			default:
				break;
			}
		}
		fvalue = exp.CalculateParam(strFormula, params);
	}
	else
	{
		fvalue = exp.CalculateModify(pKernel, self, strFormula);
	}

	switch (player->GetAttrType(szPropName))
	{
	case VTYPE_INT:
		player->SetInt(szPropName, (int)fvalue);
		break;

    case VTYPE_INT64:
        player->SetInt64(szPropName, (int)fvalue);
        break;
        
	case VTYPE_FLOAT:
		player->SetFloat(szPropName, fvalue);
		break;

	case VTYPE_DOUBLE:
		player->SetDouble(szPropName, (double)fvalue);
		break;

	default:
		return;
		break;
	}

}
// 获得对象当前适用的属性关系公式
const char* PropRefreshModule::GetRelationFormula(IKernel* pKernel, const PERSISTID& self, const PropFormula& propFormula) const
{
    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(pSelf == NULL)
    {
        return "";
    }

	int nJob = pSelf->QueryInt(FIELD_PROP_JOB);
	if (nJob <= PLAYER_JOB_MIN || nJob >= PLAYER_JOB_MAX)
	{
		return "";
	}

	return propFormula.strRelationFormula[nJob].c_str();
}

// 初始化玩家等级成长属性
bool PropRefreshModule::InitPlayerLevelProp(IKernel* pKernel, const PERSISTID& self)
{
	if (!pKernel->Exists(self) || TYPE_PLAYER != pKernel->Type(self))
	{
	    pKernel->Trace("InitPlayerLevelProp: INVALID PLAYER");
		return false;
	}

    IGameObj *pSelf = pKernel->GetGameObj(self);
    if(NULL == pSelf)
    {
        return false;
    }

	int nSelfPropId = GetLevelPropId(pKernel, pSelf);
	if (-1 == nSelfPropId)
	{
		return false;
	}
	// 初始化基本数据数据
    int iBasePropCount = StaticDataQueryModule::m_pInstance->GetDataCount(STATIC_DATA_LEVELUP_ADDPROP);
    LoopBeginCheck(h);
	for (int i = 0; i < iBasePropCount; ++i)
	{
        LoopDoCheck(h);

		const char* szPropName = StaticDataQueryModule::m_pInstance->GetColName(STATIC_DATA_LEVELUP_ADDPROP, i);
		int value = StaticDataQueryModule::m_pInstance->QueryInt(STATIC_DATA_LEVELUP_ADDPROP, nSelfPropId, i);
		int oldvalue = pSelf->QueryInt(szPropName);
		pSelf->SetInt(szPropName, value + oldvalue);
	}

	return true;
}

// 绑定属性
void PropRefreshModule::BindPlayerRefreshProp(IKernel* pKernel, const PERSISTID& self)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	IGameObj * pPlayer = pKernel->GetGameObj(self);
	if (NULL == pPlayer)
	{
		return;
	}

	std::vector<std::string>::iterator it;
	std::string strPropName;
    LoopBeginCheck(j);
	for (it = m_PropRelationIndex.begin(); it != m_PropRelationIndex.end(); ++it)
	{
        LoopDoCheck(j);
		strPropName = *it;
		if (!pPlayer->FindAttr(strPropName.c_str()))
		{
			continue;
		}
		// 过滤关系的属性回调
		std::map<std::string, PropDescStruct>::iterator itProp = m_PropRelationMap.find(strPropName);
		if (itProp == m_PropRelationMap.end())
		{
			continue;
		}

		if (itProp->second.vecEffectProp.size() <= 0)
		{
			continue;
		}

		ADD_CRITICAL(pKernel, self, strPropName.c_str(), "PropRefreshModule::C_OnBindPropChanged");

		std::string strAddPropName = strPropName + std::string("Add");
		if (pPlayer->FindAttr(strAddPropName.c_str()))
		{
			ADD_CRITICAL(pKernel, self, strAddPropName.c_str(), "PropRefreshModule::C_OnBindPropChanged");
		}
	}
}

bool PropRefreshModule::CalFormulaForProp(IKernel* pKernel,const PERSISTID& self)
{
	std::vector<std::string>::iterator it;
	std::string strPropName;
	IGameObj * obj = pKernel->GetGameObj(self);
	if (NULL == obj)
	{
		return false;
	}

	//计算血量百分比
 	int64_t maxhp = obj->QueryInt64("MaxHP");
	int nJob = obj->QueryInt(FIELD_PROP_JOB);
	if (!IsValidJob(nJob))
	{
		return false;
	}

    LoopBeginCheck(l);
	for (it=m_PropRelationIndex.begin();it!=m_PropRelationIndex.end();++it)
	{
        LoopDoCheck(l);
		strPropName = * it;
		if (!obj->FindAttr(strPropName.c_str()))
		{
			continue;
		}
		
		std::map<std::string, PropDescStruct>::iterator it2 = m_PropRelationMap.find(strPropName);
		if (it2 == m_PropRelationMap.end())
		{
			continue;
		}
		std::string str = it2->second.sPropFormual.strRelationFormula[nJob];
		if (str.size()<=0)
		{
			continue;
		}
		
		const PropFormula& propFormula = it2->second.sPropFormual;

		AnalyzeFormula(pKernel, self, strPropName.c_str(), propFormula);
	}

	//设置血量百分比
	int64_t newmaxhp = obj->QueryInt64("MaxHP");
	int64_t nNewHP = __min(obj->QueryInt64("HP"), newmaxhp);
	if (newmaxhp > maxhp)
	{
		nNewHP = newmaxhp - maxhp + obj->QueryInt64("HP");
		
	}
	else
	{
		nNewHP = __min(obj->QueryInt64("HP"), newmaxhp);
	}

	obj->SetInt64("HP", nNewHP);
	return true;
}

// 获取最终由装备影响的属性信息
bool PropRefreshModule::GetFinalEquipModifyObjProp(IKernel* pKernel, const IVarList& vPropNameList,
                                                   const IVarList& vPropValueList, MapPropValue& mapFinalPropValue) const
{
	// 1.过滤重复属性，累加
	// 2.获得影响属性
	// 3.分析第二步获得的影响属性的公式，将装备传入的值带入公式计算出最终的结果
	// 4.将第3步中，重复的属性，累加。

	//MapPropValue mapEquipData;
	int count = (int)vPropNameList.GetCount();
    LoopBeginCheck(m);
	for (int i=0; i<count; ++i)
	{
        LoopDoCheck(m);
		const char*strPropName = vPropNameList.StringVal(i);
        float value = vPropValueList.FloatVal(i);

		if (FloatEqual(value, 0.0f))
		{
			// 过滤无用值
			continue;
		}

		MapPropValue::iterator it = mapFinalPropValue.find(strPropName);
		if (it == mapFinalPropValue.end())
		{
			mapFinalPropValue.insert(std::make_pair(strPropName, (double)value));
		}
		else
		{
			double fNewValue = it->second + (double)value;
			it->second = fNewValue;
		}
	}

	return true;
}

//符号替换成数值
std::string PropRefreshModule::ModifyFormulaFormat(IKernel* pKernel, const PERSISTID& self,
												   const MapPropValue& mapSelfData, const std::string& strFormula) const
{
	if (!pKernel->Exists(self) || strFormula.empty())
	{
		return "";
	}

	std::string strFinalFormula = strFormula;
	MapPropValue::const_iterator it = mapSelfData.begin();
    LoopBeginCheck(n);
	while (it != mapSelfData.end())
	{
        LoopDoCheck(n);
		std::string strPropName = it->first;
		float value = (float)(it->second);
		strPropName = std::string("@") + strPropName;
		std::string strValue = StringUtil::FloatAsString(value);
		// 公式中的查询数据替换
		strFinalFormula = ::util_replace_string(strFinalFormula, strPropName, strValue);
		++it;
	}

	return strFinalFormula;
}

// 检测控制类属性
bool PropRefreshModule::CheckControlProp(IKernel* pKernel, const char* szPropName)
{
	if (StringUtil::CharIsNull(szPropName))
	{
		return false;
	}

	static const char* szControlPropList[] = 
	{
		"CantMove", "CantAttack", "CantBeAttack", "CantUseSkill"
	};
	int num = sizeof(szControlPropList) / sizeof(const char*);
    LoopBeginCheck(o);
	for (int i=0; i<num; ++i)
	{
        LoopDoCheck(o);
		if (strcmp(szControlPropList[i], szPropName) == 0)
		{
			return true;
		}
	}
	return false;
}

// 绑定属性回调
int PropRefreshModule::C_OnBindPropChanged(IKernel* pKernel, const PERSISTID& self,
										   const char* property, const IVar& old)
{
	m_pInstance->CalFormulaForProp(pKernel,self);

	return 0;
}

int PropRefreshModule::OnDCLevelChange(IKernel* pKernel, const PERSISTID &self, const PERSISTID & sender, const IVarList & args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	int oldlevel = args.IntVal(1);

	// 新旧属性对比刷新差值
	CVarList vOldPropNameList;
	CVarList vOldPropValueList;

	CVarList vNewPropNameList;
	CVarList vNewPropValueList;

	char buff[32] = { 0 };
	int nPropId = m_pInstance->GetLevelPropId(pKernel, pSelfObj, oldlevel);
	itoa(nPropId, buff, 10);
	std::string strOldLevel = buff;

	nPropId = m_pInstance->GetLevelPropId(pKernel, pSelfObj);
	itoa(nPropId, buff, 10);
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
	//pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);

	return 0;
}

// 获取玩家等级属性包id
int PropRefreshModule::GetLevelPropId(IKernel* pKernel, IGameObj* pSelfObj, int nLevel)
{
	if (NULL == pSelfObj)
	{
		return -1;
	}
	int nJob = pSelfObj->QueryInt(FIELD_PROP_JOB);
	int nTmpLevel = nLevel;
	if (0 == nTmpLevel)
	{
		nTmpLevel = pSelfObj->QueryInt(FIELD_PROP_LEVEL);
	}

	int nPropId = nJob * 1000 + nTmpLevel;
	return nPropId;
}


// 响应人物的属性增加处理
bool PropRefreshModule::OnAddPlayerProperty(IKernel* pKernel, const PERSISTID& self, const char* strFormId, const IVarList& proNameList, const IVarList& proValueList)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || StringUtil::CharIsNull(strFormId))
	{
		return false;
	}

	// 获取装备属性修正表
	IRecord* pRecord = pSelfObj->GetRecord(CONST_PROP_MODIFY_BYEQUIP_REC);
	if (NULL == pRecord)
	{
		return false;
	}

	// 综合处理装换接口
	MapPropValue mapPropValue;
	MapPropValue mapFinalPropValue;
	PropRefreshModule::m_pInstance->GetFinalEquipModifyObjProp(pKernel, proNameList,
		proValueList, mapFinalPropValue);

	MapPropValue::const_iterator it = mapFinalPropValue.begin();
	LoopBeginCheck(d);
	for (; it != mapFinalPropValue.end(); ++it)
	{
		LoopDoCheck(d);
		std::string strPropName = it->first;
		float fPropValue = (float)(it->second);

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self,
			strPropName.c_str(), fPropValue, EModify_ADD_VALUE, mapPropValue);

		pRecord->AddRowValue(-1, CVarList() << strPropName.c_str() << fPropValue << strFormId);
	}

	// 玩家属性刷新处理
	PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_ADD);
	return true;
}

// 响应人物属性移除
bool PropRefreshModule::OnRemovePlayerProperty(IKernel* pKernel, const PERSISTID& self, const char* strFormId)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || StringUtil::CharIsNull(strFormId))
	{
		return false;
	}

	MapPropValue mapPropValue;
	// 获取对装备属性修正表
	IRecord* pRecord = pSelfObj->GetRecord(CONST_PROP_MODIFY_BYEQUIP_REC);
	if (NULL == pRecord)
	{
		return false;
	}

	int row = 0;
	LoopBeginCheck(e);
	while (true)
	{
		LoopDoCheck(e);
		row = pRecord->FindString(EPME_FROM, strFormId);
		if (-1 == row)
		{
			break;
		}
		std::string strPropName = pRecord->QueryString(row, EPME_PROP_NAME);
		float fPropValue = pRecord->QueryFloat(row, EPME_PROP_VALUE);

		PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, strPropName.c_str(), fPropValue, EModify_ADD_VALUE, mapPropValue);

		pRecord->RemoveRow(row);
	}

	PropRefreshModule::m_pInstance->RefreshData(pKernel, self, mapPropValue, EREFRESH_DATA_TYPE_REMOVE);
	return true;
}

// 获取属性包
void PropRefreshModule::GetPropPackage(int nClassType, int nPackageid, IVarList& outFinalNameList, IVarList& outFinalValueList)
{
	if (nPackageid <= 0)
	{
		return;
	}
	std::string strPackageId = StringUtil::IntAsString(nPackageid);
	CVarList vTempPropNameList;
	CVarList vTempPropValueList;
	StaticDataQueryModule::m_pInstance->GetOneRowData(nClassType, strPackageId, vTempPropNameList, vTempPropValueList);

	outFinalNameList << vTempPropNameList;
	outFinalValueList << vTempPropValueList;
}

void PropRefreshModule::ReloadConfig(IKernel* pKernel)
{
	m_pInstance->LoadResource(pKernel);
}