//--------------------------------------------------------------------
// 文件名:      SkillDataQueryModule.h
// 内  容:      技能属性查找接口 
// 说  明:      
// 创建日期:    2014年10月23日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------

#include <string>
#include "SkillDataQueryModule.h"
#include "utils/extend_func.h"
#include "utils/exptree.h"
#include "../../Define/FightPropertyDefine.h"
#include "../../Define/GameDefine.h"
#include "../../Define/SnsDefine.h"
#include "../../CommonModule/LevelModule.h"
//#include "../PassiveSkillSystem.h"
#include <algorithm>
#include "../PassiveSkillSystem.h"

#define SKILL_PATH_CONFIG				"skill/SkillPathConfig.xml"
#define BUFF_PATH_CONFIG				"skill/BuffPathConfig.xml"
#define SKILL_EVENT_FUNID_PARAMS_FILE	"skill/SkillEventFunIdParams.xml"
#define SKILL_GROW_PROPERTY_PATH		"skill/char/SkillPropertyGrow/"
//#define SKILL_TIME_DIFF_PATH			"skill/char/char_skill_time_diff.xml"
#define MULTI_SKILL_PATH				"skill/char/multi_skill.xml"
#define MAX_SECTION_HIT_TIME 400		// 需要分段的命中时间

SkillDataQueryModule*	SkillDataQueryModule::m_pInstance = NULL;
LevelModule*			SkillDataQueryModule::m_pLevelModule = NULL;

SkillDataQueryModule::SkillDataQueryModule()
:m_SkillBase(600)
,m_SkillMainData(700)
,m_SkillHitRangeData(600)
,m_SkillEffectData(600)
,m_BuffBase(500)
,m_BuffEffectData(200)
{

}

bool SkillDataQueryModule::Init(IKernel* pKernel)
{
	m_StringBuffer.Init(300*1024, 1024);
	m_pInstance = this;
	m_strWorkPath = std::string(pKernel->GetResourcePath()) + std::string("ini/");

	m_pLevelModule = (LevelModule*)pKernel->GetLogicModule("LevelModule");
	Assert(NULL != m_pLevelModule && NULL != m_pInstance);

	LoadResource(pKernel);
    return true;
}

bool SkillDataQueryModule::LoadResource(IKernel* pKernel)
{
	ReleaseResource();

	LoadSkillProp();
	LoadBufferProp();
	//LoadSkillFormula();
	LoadSkillGrowPropertyRec();
//	LoadSkillTimeDiffConfig();
//	LoadPassiveSkillData();
//	LoadMultiSkillConfig();
//	LoadNationBattleSkillTips();

	return true;
}

bool SkillDataQueryModule::Shut(IKernel* pKernel)
{
    ReleaseResource();
    return true;
}

bool SkillDataQueryModule::GetSkillConfigPath(const char* type, IVarList& result) const
{
    result.Clear();
    //需要预先加载的配置文件
    DataPathConfigArray::const_iterator it = m_vSkillDataPathConfigArray.find(type);
    if (it != m_vSkillDataPathConfigArray.end())
    {
        const PathConfigVector& paths = (*it).second;
		LoopBeginCheck(a)
        for (PathConfigVector::const_iterator iter = paths.begin(); iter != paths.end(); ++iter)
        {
			LoopDoCheck(a)
            std::string strFile = (*iter);
            result << strFile.c_str();
        }
        return true;
    }
    return false;
}

bool SkillDataQueryModule::GetBuffConfigPath(const char* type, IVarList& result) const
{
    result.Clear();
    //需要预先加载的配置文件
    DataPathConfigArray::const_iterator it = m_vBuffDataPathConfigArray.find(type);
    if (it != m_vBuffDataPathConfigArray.end())
    {
        const PathConfigVector& paths = (*it).second;

		LoopBeginCheck(b)
        for (PathConfigVector::const_iterator iter = paths.begin(); iter != paths.end(); ++iter)
        {
			LoopDoCheck(b)
            std::string strFile = (*iter);
            result << strFile.c_str();
        }
        return true;
    }
    return false;
}

const SkillBaseData* SkillDataQueryModule::GetSkillBase(const char * szSkillID) const
{
    return GetPropPoint(szSkillID, m_SkillBase);
}

const SkillMainData* SkillDataQueryModule::GetSkillMain(IKernel* pKernel, const PERSISTID& skill) const
{
	if (!pKernel->Exists(skill))
	{
		return NULL;
	}

	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	const SkillMainData* pMainData = GetSkillMain(pSkillObj);

	return pMainData;
}

const SkillMainData* SkillDataQueryModule::GetSkillMain(IGameObj* pSkillObj) const
{
	if (NULL == pSkillObj)
	{
		return NULL;
	}

	const char* szMainPackID = pSkillObj->QueryString(SKILL_CONFIGID);
	SkillMainData* pMainData = GetSkillMainConfig(szMainPackID);

	return pMainData;
}

// nGPType对应(SkillGrowProperty)
const int SkillDataQueryModule::QuerySkillGrowProperty(IKernel* pKernel, const PERSISTID& skill, int nGPType)
{
	const SkillMainData* pMainData = GetSkillMain(pKernel, skill);
	if (NULL == pMainData)
	{
		return NO_VALID_GROW_PROPERTY;
	}

	int nQueryVal = NO_VALID_GROW_PROPERTY;
	// 如果有成长属性
	if (pMainData->GetIsGrowProperty())
	{
		nQueryVal = GetGrowProperty(pKernel, skill, nGPType);
	}

	// 没有成长属性或没查到则读取默认配置
	if (NO_VALID_GROW_PROPERTY == nQueryVal)
	{
		nQueryVal = GetPropertyBySkillGPType(pMainData, nGPType);
	}

	return nQueryVal;
}

const SkillHitRangeData* SkillDataQueryModule::GetHitRangeProp(const char* szSkillID) const 
{
    const SkillMainData* pMainData = GetPropPoint(szSkillID, m_SkillMainData);
    if (pMainData)
    {
        const StringArray& hitRangeList = pMainData->GetHitRangeID();
        if (hitRangeList.GetCount() > 0)
        {
            return GetPropPoint(hitRangeList.StringVal(0), m_SkillHitRangeData);
        }
    }

    return NULL;
}

const SkillHitRangeData* SkillDataQueryModule::GetHitRangePropByID(const char* hitrangeID) const
{
    return GetPropPoint(hitrangeID, m_SkillHitRangeData);
}

const SkillEffectData* SkillDataQueryModule::GetSkillEffectProp(const char* effectID) const
{
    return GetPropPoint(effectID, m_SkillEffectData);
}

// 响应技能升级
bool SkillDataQueryModule::OnSkillPropChange(IKernel* pKernel, const PERSISTID& skill, const PERSISTID& self)
{
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (NULL == pSkillObj)
	{
		return false;
	}

	const SkillMainData* pData = GetSkillMain(pSkillObj);
	if (NULL == pData)
	{
		return false;
	}
	int		nAttackAdd = 0;
	float	fSkillDamageRate = 0.0f;
	// 技能配置的属性值
	if (pData->GetIsGrowProperty())
	{
		// 技能附加攻击力
		nAttackAdd = GetGrowProperty(pKernel, skill, SKILL_GP_ATTACK_ADD);
		// 伤害百分比
		int nSkillRate = GetGrowProperty(pKernel, skill, SKILL_GP_DAMAGE_RATE);
		fSkillDamageRate = (float)nSkillRate / 100.0f;
	}
	else
	{
		fSkillDamageRate = pData->GetDamageRate();
	}

	// 统计被动技能影响的属性值
	if (pKernel->Exists(self))
	{
		CVarList args;
		if (QueryPassiveSkillEffect(pKernel, args, self, skill, PS_ADD_SKILL_PROPERTY, "SkillAttackAdd"))
		{
			int nPSAddAttack = args.IntVal(0);
			nAttackAdd += nPSAddAttack;
		}
		args.Clear();
		if (QueryPassiveSkillEffect(pKernel, args, self, skill, PS_ADD_SKILL_PROPERTY, "SkillDamageRate"))
		{
			int nPSDamageRate = args.IntVal(0);
			fSkillDamageRate += (float)nPSDamageRate / 100.0f;
		}
	}

	// 刷新技能属性
	pSkillObj->SetInt("SkillAttackAdd", nAttackAdd);
	pSkillObj->SetFloat("SkillDamageRate", (float)fSkillDamageRate);
	return true;
}

const BuffBaseData* SkillDataQueryModule::GetBuffBaseData(const char* szBuffID) const
{
    return GetPropPoint(szBuffID, m_BuffBase);
}

const BuffEffectData* SkillDataQueryModule::GetBuffEffectProp(const char* effectID) const 
{
	return GetPropPoint(effectID, m_BuffEffectData);
}

// 获取服务器与客户端技能时间差 nType对应SkillTimeDiff
// int	SkillDataQueryModule::GetSkillTimeDiff(IKernel* pKernel, const PERSISTID& self, int nType)
// {
// 	IGameObj* pSelfObj = pKernel->GetGameObj(self);
// 	if (NULL == pSelfObj || nType >= MAX_SKILL_TIME_DIFF_NUM || nType < 0)
// 	{
// 		return 0;
// 	}
// 	
// 	// npc没有差值
// 	if (pSelfObj->GetClassType() == TYPE_NPC)
// 	{ 
// 		return 0;
// 	}
// 	int nTimeDiff = 0;
// 	int nOnline = pSelfObj->QueryInt("Online");
// 	// 只有在线玩家取时间差,离线取正常值
// 	if (ONLINE == nOnline)
// 	{
// 		nTimeDiff = m_vSkillTimeDiff[nType];
// 	}
// 
// 	return nTimeDiff;
// }

// 获取被动技能配置
const PassiveSkill* SkillDataQueryModule::GetPassiveSkillConfig(const int nSkillId) const
{
	PassiveSkill* pSkillData = NULL;
	do 
	{
		PassiveSkillInfoMap::const_iterator iter = m_mapPassiveSkill.find(nSkillId);
		if (iter == m_mapPassiveSkill.end())
		{
			break;
		}
		pSkillData = iter->second;
	} while (0);
	
	return pSkillData;
}

const PassiveSkillLevelInfo* SkillDataQueryModule::GetPassiveSkillLevelInfo(const int nSkillId, const int nSkillLevel) const
{
	const PassiveSkillLevelInfo* pSkillInfo = NULL;
	do 
	{
		const PassiveSkill* pSkillData = GetPassiveSkillConfig(nSkillId);
		if(NULL == pSkillData)
		{
			break;
		}

		if (nSkillLevel <= 0 || nSkillLevel > (int)pSkillData->vUpgradeInfoList.size())
		{
			break;
		}

		pSkillInfo = &pSkillData->vUpgradeInfoList[nSkillLevel - 1];
	} while (0);
	
	return pSkillInfo;
}

// 查询PassiveSkill产生的效果参数
bool SkillDataQueryModule::QueryPassiveSkillEffect(IKernel* pKernel,  IVarList& outData, const PERSISTID& self,
												   const PERSISTID& skill, const int nPSType, const char* strPropName)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	const SkillMainData* pSkillMainData = GetSkillMain(pKernel, skill);
	if (NULL == pSkillMainData || !pKernel->Exists(self))
	{
		return false;
	}

	// 没找到被动技能表
	IRecord* pPassiveSkillRec = pSelfObj->GetRecord(PASSIVE_SKILL_REC_NAME);
	if (NULL == pPassiveSkillRec)
	{
		return false;
	}

	outData.Clear();
	CVarList passiveskills;
	pSkillMainData->GetPassiveSkill().ConvertToVarList(passiveskills);
	int nCount = (int)passiveskills.GetCount();
	if (0 == nCount)
	{
		return false;
	}
	bool bFindProp = false;
	LoopBeginCheck(c)
	for (int i = 0;i < nCount;++i)
	{
		LoopDoCheck(c)
		int nPassiveSkillId = passiveskills.IntVal(i);
		int nPassiveSkillLevel = PassiveSkillSystemSingleton::Instance()->QueryPassiveSkillLevel(pKernel, self, nPassiveSkillId);
		// 说明此技能没学
		if (0 == nPassiveSkillLevel)
		{
			continue;
		}
		// 除了附加技能效果以外,不存在多个被动技能影响一个主动技能的同一个属性
		if(QueryPassiveSkillProp(pKernel, outData, nPassiveSkillId, nPassiveSkillLevel, nPSType, strPropName))
		{
			if (PS_ADD_SKILL_EFFECT != nPSType)
			{
				break;
			}
		}
	}
	if (outData.GetCount() > 0)
	{
		bFindProp = true;
	}
	
	return false;
}

// 根据职业找到所有被动技能id(测试、调试用)
bool SkillDataQueryModule::QueryPassiveSkillIdByJob(IKernel* pKernel,  IVarList& outData, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}
	outData.Clear();
	int nJob = pSelfObj->QueryInt("Job");
	PassiveSkillInfoMap::iterator iter;
	LoopBeginCheck(a);
	for (iter = m_mapPassiveSkill.begin(); iter != m_mapPassiveSkill.end(); ++iter)
	{
		LoopDoCheck(a);
		PassiveSkill* pData = iter->second;
		if (NULL == pData)
		{
			continue;
		}
		if (pData->nCareer == nJob)
		{
			outData << pData->nSkillId;
		}
	}

	return true;
}

// 获取一组连击技能数据
const MultiSkillInfo* SkillDataQueryModule::QueryMultiSkillInfo(int nMultiSkillId)
{
	MultiSkillInfo info;
	info.nMultiSkillId = nMultiSkillId;
	MultiSkillInfoVec::iterator iter = find(m_vecMultiSkillInfo.begin(), m_vecMultiSkillInfo.end(), info);
	if (iter != m_vecMultiSkillInfo.end())
	{
		return &(*iter);
	}

	return NULL;
}

// 获取命中数据
const SkillDataQueryModule::HitTimeDataVec* SkillDataQueryModule::QueryHitTimeList(const char* strSkillId)
{
	HitTimeConfigMap::iterator iter = m_mapHitTimeData.find(strSkillId);
	if (iter == m_mapHitTimeData.end())
	{
		return NULL;
	}

	return &iter->second;
}

// 获取技能最大命中次数
int SkillDataQueryModule::GetMaxHitNum(IKernel* pKernel, IGameObj* pSkillObj)
{
	if (NULL == pSkillObj)
	{
		return 0;
	}

	const char* strSkillId = pSkillObj->GetConfig();
	HitTimeConfigMap::iterator iter = m_mapHitTimeData.find(strSkillId);
	if (iter == m_mapHitTimeData.end())
	{
		return 1; // 没有多次命中数据,最大命中次数为1
	}

	return (int)iter->second.size();
}

void SkillDataQueryModule::ReleaseResource()
{
    ReleaseProp(m_SkillBase);
    ReleaseProp(m_SkillMainData);
    ReleaseProp(m_SkillHitRangeData);
    ReleaseProp(m_SkillEffectData);
    ReleaseProp(m_BuffBase);
	ReleaseProp(m_BuffEffectData);

    m_SkillUseLimitData.clear();

	LoopBeginCheck(e)
	// 技能额外伤害表清除
	for (GrowPropertyRecMap::iterator it = m_mapSkillGrowProperty.begin();it != m_mapSkillGrowProperty.end();++it)
	{
		LoopDoCheck(e)
		SkillGrowPropertyVec& rec = it->second;
		rec.clear();
	}
	m_mapSkillGrowProperty.clear();
	
	// 技能时间差
	//m_vSkillTimeDiff.clear();

	// 被动技能清除
	LoopBeginCheck(f)
	for (PassiveSkillInfoMap::iterator it = m_mapPassiveSkill.begin();it != m_mapPassiveSkill.end();++it)
	{
		LoopDoCheck(f)
		SAFE_DELETE(it->second);
	}
	m_mapPassiveSkill.clear();

	// 连击配置清除
	LoopBeginCheck(s)
	int nSize = (int)m_vecMultiSkillInfo.size();
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(s)
		m_vecMultiSkillInfo[i].vecSkill.clear();
	}
	m_vecMultiSkillInfo.clear();
}

// 获取技能原始配置
SkillMainData* SkillDataQueryModule::GetSkillMainConfig(const char* szSkillID) const
{
	const char* szMainPackID = szSkillID;
	return const_cast<SkillMainData*>(GetPropPoint(szMainPackID, m_SkillMainData));
}


bool SkillDataQueryModule::LoadSkillProp()
{
    if (!LoadSkillPathConfig())
    {
        return false;
    }

    if(!LoadSkillEventFunIdParam())
    {
        return false;
    }

	LoopBeginCheck(g)
    for (DataPathConfigArray::const_iterator iter = m_vSkillDataPathConfigArray.begin();iter != m_vSkillDataPathConfigArray.end(); ++iter)
    {
		LoopDoCheck(g)
        const std::string &strFlag = (*iter).first;
        const PathConfigVector &vPath = (*iter).second;

        if (strFlag.compare("SkillHitRange") == 0)
        {
            LoadProp(vPath, m_SkillHitRangeData);
        }
        else if (strFlag.compare("SkillNew") == 0)
        {
            LoadProp(vPath, m_SkillBase);
        }
        else if (strFlag.compare("SkillMain") == 0)
        {
            LoadProp(vPath, m_SkillMainData);
			ParseHitTimeData();
        }
        else if (strFlag.compare("SkillEffect") == 0)
        {
            LoadProp(vPath, m_eventParamsMap, m_SkillEffectData);
        }
    }

    return true;
}

bool SkillDataQueryModule::LoadSkillPathConfig()
{
    m_vSkillDataPathConfigArray.clear();
    std::string strPath = m_strWorkPath + SKILL_PATH_CONFIG;
    CXmlFile xml(strPath.c_str());
    std::string errMsg;
    if (!xml.LoadFromFile(errMsg))
    {
        // 路径配置文件无法打开
        extend_warning(LOG_ERROR, errMsg.c_str());
        return false;
    }

    CVarList vSectionList;
    xml.GetSectionList(vSectionList);
	LoopBeginCheck(h)
    for (size_t index = 0; index < vSectionList.GetCount(); ++index)
    {
		LoopDoCheck(h)
        const char* section = vSectionList.StringVal(index); 
        if (NULL == section)
        {
            continue;
        }

        const char* type = xml.ReadString(section, "Type", "");
        const char* path = xml.ReadString(section, "Path", "");
        if (NULL == type || '\0' == *type || NULL == path || '\0' == *path)
        {
            continue;
        }
		DataPathConfigArray::iterator iter = m_vSkillDataPathConfigArray.find(type);
		if (iter == m_vSkillDataPathConfigArray.end())
		{
			PathConfigVector pathVec;
			pathVec.push_back(path);
			m_vSkillDataPathConfigArray.insert(make_pair(type, pathVec));
		}
		else
		{
			iter->second.push_back(path);
		}
    }

    return true;
}

bool SkillDataQueryModule::LoadSkillEventFunIdParam()
{
    m_eventParamsMap.clear();

    //获得路径
    std::string path = m_strWorkPath + SKILL_EVENT_FUNID_PARAMS_FILE;
    CXmlFile xml(path.c_str());
    if (!xml.LoadFromFile())
    {
        extend_warning(LOG_ERROR,"[SkillEventManager::LoadResource] no found file:%s.",path.c_str());
        return false;
    }
    //读取静态数据表文件
    CVarList section_list;
    xml.GetSectionList(section_list);
    //先把ini 的段数量设为表的行数量
    size_t section_count = section_list.GetCount();
    //循环处理每个段
	LoopBeginCheck(i)
    for (size_t i = 0; i < section_count; ++i)
    {
		LoopDoCheck(i)
        //ID
        const char* str_func_id = section_list.StringVal(i);
        int func_id = StringUtil::StringAsInt(str_func_id, 0);

        //参数
        CVarList item_list;
        xml.GetItemValueList(str_func_id, "Params", item_list);
        if (item_list.GetCount() != 1)
        {
            extend_warning(LOG_ERROR, "[SkillDataQueryModule::LoadEventParamsResource] Params count != 1 : Skill_EventFunIdParams.xml");
            return false;
        }
        std::vector<std::string> eventparams_list;
        eventparams_list.clear();
        const char* itemvalue = item_list.StringVal(0);
        if (itemvalue == NULL)
        {
            return false;
        }
        CVarList params;
        StringUtil::SplitString(params, itemvalue, ",");
		LoopBeginCheck(j)
        for (size_t iparam = 0; iparam < params.GetCount(); iparam++)
        {
			LoopDoCheck(j)
            eventparams_list.push_back(params.StringVal(iparam));
        }
        m_eventParamsMap.insert(std::make_pair(func_id, eventparams_list));
    }
    return true;
}

// 加载被动技能数据
bool SkillDataQueryModule::LoadPassiveSkillData()
{
	// 获取配置路径
	DataPathConfigArray::iterator iter = m_vSkillDataPathConfigArray.find("PassiveSkill");
	if (iter == m_vSkillDataPathConfigArray.end())
	{
		return false;
	}
	PathConfigVector& pathVec = iter->second;
	// 有两个被动技能配置文件路径
	if (pathVec.size() != 2)
	{
		return false;
	}
	std::string strPath = m_strWorkPath + pathVec[0];

	// 读取被动技能注册配置
	CXmlFile xml(strPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_ERROR,"[SkillDataQueryModule::LoadPassiveSkillData] no found file:%s.",strPath.c_str());
		return false;
	}

	CVarList vSectionList;
	xml.GetSectionList(vSectionList);
	LoopBeginCheck(l)
	for (size_t index = 0; index < vSectionList.GetCount(); ++index)
	{
		LoopDoCheck(l)
		const char* section = vSectionList.StringVal(index);
		if (NULL == section)
		{
			continue;
		}
		// 获取技能id
		PassiveSkill* pData = NEW PassiveSkill;
 		pData->nSkillId		= StringUtil::StringAsInt(section);
		pData->nCareer		= xml.ReadInteger(section, "CareerID", 1);
		pData->nType		= xml.ReadInteger(section, "Type", 0);
		pData->nAddEffectType = xml.ReadInteger(section, "AddEffectType", 0);

		m_mapPassiveSkill.insert(std::make_pair(pData->nSkillId, pData));
	}

	// 获取配置路径
	strPath = m_strWorkPath + pathVec[1];

	// 读取被动技能的详细配置
	xml.SetFileName(strPath.c_str());
	if (!xml.LoadFromFile())
	{
		extend_warning(LOG_ERROR,"[SkillDataQueryModule::LoadPassiveSkillData] no found file:%s.",strPath.c_str());
		return false;
	}

	vSectionList.Clear();
	xml.GetSectionList(vSectionList);
	LoopBeginCheck(a);
	for (size_t index = 0; index < vSectionList.GetCount(); ++index)
	{
		LoopDoCheck(a);
		const char* section = vSectionList.StringVal(index);
		if (NULL == section)
		{
			continue;
		}
		// key是由技能id+技能等级拼接而成.前5位为技能id后三位为技能等级.例如:10001001
		int nSection = StringUtil::StringAsInt(section);
		int nSkillId	= nSection / 1000;
		int nSkillLevel = nSection % 1000; // 技能等级从1计数
		if(nSkillLevel <= 0)
		{	
			// 技能等级为0 客户端会用到
			if (nSkillLevel != 0)
			{
				std::string strInfo = strPath + std::string(section) + "level too large";
				extend_warning(LOG_WARNING, strInfo.c_str());
			}
			continue;
		}

		// 找到对应的数据存储位置
		PassiveSkillInfoMap::iterator it = m_mapPassiveSkill.find(nSkillId);
		if (it == m_mapPassiveSkill.end())
		{
			std::string strInfo = strPath + std::string(section) + "skillid is wrong";
			extend_warning(LOG_WARNING, strInfo.c_str());
			continue;
		}
		PassiveSkill* pData = it->second;
		if (NULL == pData)
		{
			continue;
		}

		// 开始读取每个等级的配置
		PassiveSkillLevelInfo kSkillUpgrade;

		// 读取必要的人物等级条件
		kSkillUpgrade.nMeridianLevel	= xml.ReadInteger(section, "MeridianLevel", 0);
		// 读取必要的前置技能等级条件
		std::string strTemp			= xml.ReadString(section, "SkillNeedTerm", "");
		CVarList args;
		// 升级不需要技能前置条件
		if (strTemp.empty())
		{
			kSkillUpgrade.nPreSkillId		= 0;
			kSkillUpgrade.nPreSkillLevel	= 0;
		}
		else
		{
			StringUtil::SplitString(args, strTemp, ",");
			if (args.GetCount() != 2)
			{
				std::string strInfo = strPath + std::string(section) + "SkillNeedTerm is wrong";
				extend_warning(LOG_WARNING, strInfo.c_str());
				continue;
			}
			kSkillUpgrade.nPreSkillId		= args.IntVal(0);
			kSkillUpgrade.nPreSkillLevel	= args.IntVal(1);
		}
		// 消耗的技能点
		kSkillUpgrade.nSkillPoint	= xml.ReadInteger(section, "CostSkillPoint", 1);
		// 消耗的货币类型
		kSkillUpgrade.nMoneyType	= xml.ReadInteger(section, "MoneyType", 0);
		// 消耗的货币数
		kSkillUpgrade.nMoneyCost	= xml.ReadInteger(section, "CostMoney", 1000);

		// 读取消耗的材料
		strTemp			= xml.ReadString(section, "CostItem", "");
		args.Clear();
		StringUtil::SplitString(args, strTemp, ",");
		// 升级不需要材料
		if (strTemp.empty())
		{
			kSkillUpgrade.strItemId		= "";
			kSkillUpgrade.nItemCost		= 0;
		}
		else
		{
			if (args.GetCount() != 2)
			{
				std::string strInfo = strPath + std::string(section) + "CostItem is wrong";
				extend_warning(LOG_WARNING, strInfo.c_str());
				continue;
			}
			kSkillUpgrade.strItemId		= args.StringVal(0);
			kSkillUpgrade.nItemCost		= args.IntVal(1);
		}
		
		// 读取被动技能属性
		strTemp			= xml.ReadString(section, "PropList", "");
		LoadPassiveSkillProperty(kSkillUpgrade, strTemp);

		pData->vUpgradeInfoList.push_back(kSkillUpgrade);
	}	
	return true;
}

// 读取被动技能属性数据
bool SkillDataQueryModule::LoadPassiveSkillProperty(PassiveSkillLevelInfo& outSkillPro, const std::string& strInfo)
{
	CVarList args;
	StringUtil::SplitString(args, strInfo, ";");

	int nCount = (int)args.GetCount();
	if (0 == nCount)
	{
		return false;
	}
	outSkillPro.vPropInfoList.resize(nCount);
	LoopBeginCheck(m)
	for (int i = 0;i < nCount;++i)
	{
		LoopDoCheck(m)
		PassiveSkillPropInfo& skillprop = outSkillPro.vPropInfoList[i];
		const char* strTemp = args.StringVal(i);
		CVarList temppro;
		StringUtil::SplitString(temppro, strTemp, ",");
		
		const char* strFlag = temppro.StringVal(0);
		if (strcmp(strFlag, ADD_PLAYER_PROPERTY_FLAG) == 0)
		{
			skillprop.nOptType		= PS_ADD_PLAYER_PROPERTY;
			skillprop.strPropName	= temppro.StringVal(1);
			skillprop.uPropAdd.fPropAdd	= temppro.FloatVal(2);
		}
		else if (strcmp(strFlag, ADD_SKILL_PROPERTY_FLAG) == 0)
		{
			skillprop.nOptType		= PS_ADD_SKILL_PROPERTY;
			skillprop.strSkillID	= temppro.StringVal(1);
			skillprop.strPropName	= temppro.StringVal(2);
			skillprop.uPropAdd.nPropAdd	= temppro.IntVal(3);
		}
		else if (strcmp(strFlag, CHANGE_SKILL_CONFIG_FLAG) == 0)
		{
			skillprop.nOptType		= PS_CHANGE_SKILL_CONFIG;
			skillprop.strSkillID	= temppro.StringVal(1);
			skillprop.strPropName	= temppro.StringVal(2);
			skillprop.uPropAdd.nPropAdd	= temppro.IntVal(3);
		}
		else if (strcmp(strFlag, ADD_SKILL_EFFECT_FLAG) == 0)
		{
			skillprop.nOptType		= PS_ADD_SKILL_EFFECT;
			skillprop.strSkillID	= temppro.StringVal(1);
			skillprop.strPropName	= temppro.StringVal(2);
		}
		else if (strcmp(strFlag, ADD_NEW_BUFF_FLAG) == 0)
		{
			skillprop.nOptType		= PS_ADD_NEW_BUFF;
			skillprop.strSkillID	= temppro.StringVal(1);
		}
	}
	return true;
}

bool SkillDataQueryModule::LoadBuffPathConfig()
{
    m_vBuffDataPathConfigArray.clear();
    std::string strPath = m_strWorkPath + BUFF_PATH_CONFIG;
    CXmlFile xml(strPath.c_str());
    std::string errMsg;
    if (!xml.LoadFromFile(errMsg))
    {
        // 路径配置文件无法打开
        extend_warning(LOG_WARNING, errMsg.c_str());
        return false;
    }

    CVarList vSectionList;
    xml.GetSectionList(vSectionList);
	LoopBeginCheck(n)
    for (size_t index = 0; index < vSectionList.GetCount(); ++index)
    {
		LoopDoCheck(n)
        const char* section = vSectionList.StringVal(index);
        if (NULL == section)
        {
            continue;
        }

        const char* type = xml.ReadString(section, "Type", "");
        const char* path = xml.ReadString(section, "Path", "");
        if (NULL == type || '\0' == *type || NULL == path || '\0' == *path)
        {
            continue;
        }
		DataPathConfigArray::iterator iter = m_vBuffDataPathConfigArray.find(type);
		if (iter == m_vBuffDataPathConfigArray.end())
		{
			PathConfigVector pathVec;
			pathVec.push_back(path);
			m_vBuffDataPathConfigArray.insert(make_pair(type, pathVec));
		}
		else
		{
			iter->second.push_back(path);
		}
    }

    return true;
}

// 加载技能额外伤害表
bool SkillDataQueryModule::LoadSkillGrowPropertyRec()
{
	// 技能公式配置
	LoopBeginCheck(o)
	TStringPod<char, SkillMainData*>::iterator it = m_SkillMainData.Begin();
	for(; it != m_SkillMainData.End(); ++it)
	{
		LoopDoCheck(o)
		SkillMainData* pData = it.GetData();
		if (NULL == pData)
		{
			continue;
		}
		// 存在技能成长
		if (pData->GetIsGrowProperty())
		{
			const char* strSkillId = pData->GetID();
			SkillGrowPropertyVec vecGrowProperty;
			if(LoadOneSkillGrowProperty(vecGrowProperty, strSkillId))
			{
				m_mapSkillGrowProperty.insert(std::make_pair(strSkillId, vecGrowProperty));
			}
		}
	}
	return true;
}

// 加载某个技能的额外伤害
bool SkillDataQueryModule::LoadOneSkillGrowProperty(SkillGrowPropertyVec& vecExtraHurt, const std::string& strSkillId)
{
	std::string strPath = m_strWorkPath + SKILL_GROW_PROPERTY_PATH + strSkillId + ".xml";
	CXmlFile xml(strPath.c_str());
	std::string errMsg;
	if (!xml.LoadFromFile(errMsg))
	{
		// 路径配置文件无法打开
		extend_warning(LOG_WARNING, errMsg.c_str());
		return false;
	}

	vecExtraHurt.clear();
	CVarList vSectionList;
	xml.GetSectionList(vSectionList);
	int nSize = (int)vSectionList.GetCount();
	vecExtraHurt.resize(nSize);
	LoopBeginCheck(p)
	for (int index = 0; index < nSize; ++index)
	{
		LoopDoCheck(p)
		const char* section = vSectionList.StringVal(index);
		if (NULL == section)
		{
			continue;
		}
		// ID为对应的等级
		int nLevel  = StringUtil::StringAsInt(section);
		if (nLevel > nSize || nLevel < 1)
		{
			continue;
		}
		// 等级从1计数
		SkillGrowProperty& data = vecExtraHurt[nLevel - 1];
		data.nArrGrowProperty[SKILL_GP_ATTACK_ADD]		= xml.ReadInteger(section, "AttackAdd", 0);
		data.nArrGrowProperty[SKILL_GP_COOL_DOWN_TIME]	= xml.ReadInteger(section, "CoolDownTime", NO_VALID_GROW_PROPERTY);
		data.nArrGrowProperty[SKILL_GP_RESTORE_HP_RATE]	= xml.ReadInteger(section, "RestroeHPRate", NO_VALID_GROW_PROPERTY);
		data.nArrGrowProperty[SKILL_GP_DAMAGE_RATE]		= xml.ReadInteger(section, "SkillDamageRate", 0);
	}

	return true;
}

// 加载技能服务器与客户端的技能时间差
// bool SkillDataQueryModule::LoadSkillTimeDiffConfig()
// {
// 	std::string strPath = m_strWorkPath + SKILL_TIME_DIFF_PATH;
// 	CXmlFile xml(strPath.c_str());
// 	std::string errMsg;
// 	if (!xml.LoadFromFile(errMsg))
// 	{
// 		// 路径配置文件无法打开
// 		extend_warning(LOG_WARNING, errMsg.c_str());
// 		return false;
// 	}
// 
// 	CVarList vSectionList;
// 	xml.GetSectionList(vSectionList);
// 	int nSize = (int)vSectionList.GetCount();
// 	m_vSkillTimeDiff.resize(nSize);
// 	LoopBeginCheck(q)
// 	for (int index = 0; index < nSize; ++index)
// 	{
// 		LoopDoCheck(q)
// 		const char* section = vSectionList.StringVal(index);
// 		if (NULL == section)
// 		{
// 			continue;
// 		}
// 		// ID为对应的索引
// 		int nIndex  = StringUtil::StringAsInt(section);
// 		if (nIndex >= nSize || nIndex < 0)
// 		{
// 			continue;
// 		}
// 		m_vSkillTimeDiff[nIndex] = xml.ReadInteger(section, "TimeDiff", 0);
// 	}
// 	return true;
// }

// 加载连击技能
bool SkillDataQueryModule::LoadMultiSkillConfig()
{
	std::string strPath = m_strWorkPath + MULTI_SKILL_PATH;
	CXmlFile xml(strPath.c_str());
	std::string errMsg;
	if (!xml.LoadFromFile(errMsg))
	{
		// 路径配置文件无法打开
		extend_warning(LOG_WARNING, errMsg.c_str());
		return false;
	}

	CVarList vSectionList;
	xml.GetSectionList(vSectionList);
	int nSize = (int)vSectionList.GetCount();
	m_vecMultiSkillInfo.reserve(nSize);
	LoopBeginCheck(r)
	for (int index = 0; index < nSize; ++index)
	{
		LoopDoCheck(r)
		const char* section = vSectionList.StringVal(index);
		if (NULL == section)
		{
			continue;
		}
		MultiSkillInfo info;
		info.nMultiSkillId = StringUtil::StringAsInt(section);
		const char* strSkillArray = xml.ReadString(section, "SkillArray", "");
		if (StringUtil::CharIsNull(strSkillArray))
		{
			continue;
		}

		// 读取一组连击技能id
		CVarList params;
		StringUtil::SplitString(params, strSkillArray, ",");
		int nSkillCount = (int)params.GetCount();
		info.vecSkill.resize(nSkillCount);
		LoopBeginCheck(s)
		for (int iparam = 0; iparam < nSkillCount; ++iparam)
		{
			LoopDoCheck(s)
			info.vecSkill[iparam] = params.StringVal(iparam);
		}

		m_vecMultiSkillInfo.push_back(info);
	}
	return true;
}	

// 根据技能成长类型找到属性值
int SkillDataQueryModule::GetPropertyBySkillGPType(const SkillMainData* pMainData, int nGPType)
{
	if (NULL == pMainData)
	{
		return NO_VALID_GROW_PROPERTY;
	}
	int nVal = NO_VALID_GROW_PROPERTY;
	switch(nGPType)
	{
	case SKILL_GP_COOL_DOWN_TIME:
		nVal = pMainData->GetCoolDownTime();
		break;
	}

	return nVal;
}

// 查询成长属性的值
int SkillDataQueryModule::GetGrowProperty(IKernel* pKernel, const PERSISTID& skill, int nGPType)
{
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);
	if (pSkillObj == NULL)
	{
		return 0;
	}

	int nQueryVal = NO_VALID_GROW_PROPERTY;
	do 
	{
		if (nGPType < 0 || nGPType >= MAX_SKILL_GP_NUM)
		{
			break;
		}
		// 获取原配置.
		const char* strConfig = pKernel->GetConfig(skill);
		if (StringUtil::CharIsNull(strConfig))
		{
			break;
		}
		GrowPropertyRecMap::const_iterator iter = m_mapSkillGrowProperty.find(strConfig);
		if (iter != m_mapSkillGrowProperty.end())
		{
			int nSkillLevel = pSkillObj->QueryInt("Level");
			const SkillGrowPropertyVec& rec = iter->second;
			if (nSkillLevel > (int)rec.size() || nSkillLevel < 1)
			{
				break;
			}
			
			nQueryVal = rec[nSkillLevel - 1].nArrGrowProperty[nGPType];
		}
	} while (0);
	return nQueryVal;
}


// 查询PassiveSkill的属性
bool SkillDataQueryModule::QueryPassiveSkillProp(IKernel* pKernel, IVarList& outData, const int nPassiveSkillId, 
												 const int nPassiveSkillLV, const int nPSType, const char* strPropName)
{
	// 找到被动技能对应的配置
	const PassiveSkillLevelInfo* pSkillInfo = GetPassiveSkillLevelInfo(nPassiveSkillId, nPassiveSkillLV);
	if (NULL == pSkillInfo)
	{
		return false;
	}
	const PSPropInfoVec& vecProp = pSkillInfo->vPropInfoList;
	int nSize = (int)vecProp.size();
	LoopBeginCheck(r)
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(r)
		const PassiveSkillPropInfo& kInfo = vecProp[i];
		// nOptType为PS_ADD_SKILL_EFFECT时,strPropName为附加效果名
		if (PS_ADD_SKILL_EFFECT == nPSType)
		{
			if (kInfo.nOptType == nPSType)
			{
				outData << kInfo.strPropName;
			}
		}
		else
		{
			// 人物对象属性值类型为float
			if (kInfo.nOptType == nPSType && strcmp(strPropName ,kInfo.strPropName.c_str()) == 0)
			{
				PS_ADD_PLAYER_PROPERTY == nPSType ? outData << kInfo.uPropAdd.fPropAdd : outData << kInfo.uPropAdd.nPropAdd;
			}
		}
	}

	bool bFindPro = false;
	if (outData.GetCount() > 0)
	{
		bFindPro = true;
	}

	return bFindPro;
}

bool SkillDataQueryModule::LoadBufferProp()
{
    if (!LoadBuffPathConfig())
    {
        return false;
    }

	LoopBeginCheck(s)
    for (DataPathConfigArray::const_iterator iter = m_vBuffDataPathConfigArray.begin();iter != m_vBuffDataPathConfigArray.end(); ++iter)
    {
		LoopDoCheck(s)
        const std::string &strFlag = (*iter).first;
        const PathConfigVector &vPath = (*iter).second;

		if (strFlag.compare("BuffNew") == 0)
        {
			LoadProp(vPath, m_BuffBase);
        }
		else if (strFlag.compare("BuffEffect") == 0)
		{
			LoadProp(vPath, m_eventParamsMap, m_BuffEffectData);
		}
    }

    return true;
}

template<class T>
bool SkillDataQueryModule::LoadProp(const PathConfigVector& vPath, TStringPod<char, T*>& map)
{
	fast_string strPath;
	std::string errMsg;
	LoopBeginCheck(a);
	for (PathConfigVector::const_iterator vit = vPath.begin(); vit != vPath.end(); ++vit)
	{
		LoopDoCheck(a);
		strPath = m_strWorkPath.c_str();
		strPath += (*vit).c_str();

		CXmlFile xml(strPath.c_str());
		if (!xml.LoadFromFile(errMsg))
		{
			extend_warning(LOG_WARNING, errMsg.c_str());
			return false;
		}

		size_t uSectionCount = xml.GetSectionCount();
		LoopBeginCheck(b);
		for (size_t i = 0; i < uSectionCount; ++i)
		{
			LoopDoCheck(b);
			const char* szSectionName = xml.GetSectionByIndex(i);
			T* pData = new (T); 
			if (pData)
			{
				pData->SetPropData(xml, szSectionName, m_StringBuffer);
				map.Add(szSectionName, pData);
			}
		}
	}

	return true;
}

template<class T>
bool SkillDataQueryModule::LoadProp(const PathConfigVector& vPath, const EventParamsMap& eventParamsMap, TStringPod<char, T*>& map)
{
	fast_string strPath;
	std::string errMsg;
	LoopBeginCheck(t)
	for (PathConfigVector::const_iterator vit = vPath.begin(); vit != vPath.end(); ++vit)
	{
		LoopDoCheck(t)
		strPath = m_strWorkPath.c_str();
		strPath += (*vit).c_str();

		CXmlFile xml(strPath.c_str());
		if (!xml.LoadFromFile(errMsg))
		{
			extend_warning(LOG_WARNING, errMsg.c_str());
			return false;
		}

		size_t uSectionCount = xml.GetSectionCount();
		LoopBeginCheck(a);
		for (size_t i = 0; i < uSectionCount; ++i)
		{
			LoopDoCheck(a);
			const char* szSectionName = xml.GetSectionByIndex(i);
			T* pData = new (T);
			if (pData)
			{
				pData->SetPropData(xml, szSectionName, eventParamsMap, m_StringBuffer);
				map.Add(szSectionName, pData);
			}
		}
	}

	return true;
}

template<class T>
void SkillDataQueryModule::ReleaseProp(TStringPod<char, T*>& map)
{
    typename TStringPod<char, T*>::iterator it = map.Begin();
	LoopBeginCheck(a);
	for (; it != map.End(); ++it)
    {
		LoopDoCheck(a);
        T* pData = it.GetData();
        if (pData)
        {
            delete(pData);
        }
    }
    map.Clear();
}

template<class T>
const T*  SkillDataQueryModule::GetPropPoint(const char* strKeyID, const TStringPod<char, T*>& map) const
{
	if(NULL == strKeyID)
	{
		return NULL;
	}
    typename TStringPod<char, T*>::const_iterator it = map.Find(strKeyID);
    if (it != map.End())
    {
        return it.GetData();
    }

    return NULL;
}

void SkillDataQueryModule::ParseHitTimeData()
{
	// 技能公式配置
	LoopBeginCheck(o);
	TStringPod<char, SkillMainData*>::iterator it = m_SkillMainData.Begin();
	for (; it != m_SkillMainData.End(); ++it)
	{
		LoopDoCheck(o);
		SkillMainData* pData = it.GetData();
		if (NULL == pData)
		{
			continue;
		}
		const char* strSkillId = pData->GetID();
		if (strcmp(strSkillId, "M4300017_S1") == 0)
		{
			int a = 0;
		}
		const StringArray& sarHitTimeList = pData->GetExtraHitTimes();
		int nDataCount = (int)sarHitTimeList.GetCount();
		if (nDataCount <= 1)
		{
			continue;
		}
		int nCurHitTime = 0;
		int nCurHitCount = 0;
		int nNextHitTime = 0;
		HitTimeDataVec vHitTimeData;
		LoopBeginCheck(u);
		for (int i = 0; i < nDataCount;++i)
		{
			LoopDoCheck(u);
			if (i == 0)
			{
				nCurHitTime = atoi(sarHitTimeList.StringVal(i));
			}
			else
			{
				int nHitTime = atoi(sarHitTimeList.StringVal(i));
				nNextHitTime += nHitTime;
				++nCurHitCount;
				if (nNextHitTime >= MAX_SECTION_HIT_TIME)
				{
					HitTimeData data;
					data.nHitTime = nCurHitTime;
					data.nHitCount = nCurHitCount;

					vHitTimeData.push_back(data);
					nCurHitTime = nNextHitTime;
					nCurHitCount = 0;
					nNextHitTime = 0;
				}
				if (i == nDataCount - 1 && nNextHitTime < MAX_SECTION_HIT_TIME)
				{
					++nCurHitCount;
					HitTimeData data;
					data.nHitTime = nCurHitTime;
					data.nHitCount = nCurHitCount;

					vHitTimeData.push_back(data);
				}
			}
		}

		if (m_mapHitTimeData.find(strSkillId) == m_mapHitTimeData.end())
		{
			m_mapHitTimeData.insert(std::pair<std::string, HitTimeDataVec>(strSkillId, vHitTimeData));
		}
	}
}