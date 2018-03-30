//--------------------------------------------------------------------
// 文件名:      SkillDataQueryModule.h
// 内  容:      技能属性查找接口 
// 说  明:      
// 创建日期:    2014年10月23日
// 创建人:      liumf
//    :       
//--------------------------------------------------------------------
#ifndef __SKILL_DATA_QUERY_MANAGER_H__
#define __SKILL_DATA_QUERY_MANAGER_H__

#include "FsGame/Define/header.h"
#include "utils/StringPod.h"
#include "utils/string_util.h"
#include "utils/XmlFile.h"
#include "FsGame/SkillModule/Impl/SkillDefineDataStruct.h"
#include "utils/IniFile.h"

class LevelModule;

// 技能成长属性
struct SkillGrowProperty
{
	int	nArrGrowProperty[MAX_SKILL_GP_NUM]; // 所有成长属性
	SkillGrowProperty()
	{
		memset(nArrGrowProperty, 0, sizeof(int) * MAX_SKILL_GP_NUM);
	}
};

// 属性值
union ValType
{
	float	fPropAdd;
	int		nPropAdd;
};
// 被动技能属性
struct PassiveSkillPropInfo
{
	int						nOptType;			// 被动技能类型(PassiveSkill)
	// 影响主动技能ID(玩家时为"")(nOptType为PS_ADD_NEW_SKILL时,strSkillID为新技能id;nOptType为PS_ADD_NEW_BUFF时,strSkillID为新buffid)
	std::string				strSkillID;			
	ValType					uPropAdd;			// 属性加值
	std::string				strPropName;		// 属性名 (nOptType为PS_ADD_SKILL_EFFECT时,strPropName为附加效果名)
	PassiveSkillPropInfo() : strSkillID("")
		,nOptType(0)
		,strPropName("")
	{

	}
};

typedef std::vector<PassiveSkillPropInfo> PSPropInfoVec;

// 被动技能升级条件、消耗、被动技能效果
struct PassiveSkillLevelInfo
{
	// 升级的必要条件
	int						nMeridianLevel;		// 需要的玩家等级
	int						nPreSkillId;		// 前置技能id
	int						nPreSkillLevel;		// 前置技能等级

	// 升级的消耗
	int						nSkillPoint;		// 消耗的技能点
	int						nMoneyType;			// 消耗的货币类型
	int						nMoneyCost;			// 花费的货币数量
	std::string				strItemId;			// 消耗的物品id
	int						nItemCost;			// 消耗的物品数量

	PSPropInfoVec			vPropInfoList;		// 改变属性数据

	PassiveSkillLevelInfo() : nMeridianLevel(0),
							nPreSkillId(0),
							nPreSkillLevel(0),
							nSkillPoint(0),
							nMoneyType(0),
							nMoneyCost(0),
							nItemCost(0)
	{
		vPropInfoList.clear();
	}

	~PassiveSkillLevelInfo()
	{
		vPropInfoList.clear();
	}
};

typedef std::vector<PassiveSkillLevelInfo> PSLevelInfoVec;

// 被动技能
struct PassiveSkill 
{
	PassiveSkill() : nSkillId(0), nCareer(0), nType(0)
	{
		vUpgradeInfoList.clear();
	}
	int						nSkillId;			// 被动技能id
	int						nCareer;			// 所属职业
	int						nType;				// 被动技能类型 见(PassiveSkillType)
	int						nAddEffectType;	// 增加的特效类型索引
	PSLevelInfoVec			vUpgradeInfoList;	// 升级属性数据(数组下标为技能等级)
	~PassiveSkill()
	{
		vUpgradeInfoList.clear();
	}
};

// 连击技能
struct MultiSkillInfo 
{
	MultiSkillInfo() : nMultiSkillId(0)
	{
		vecSkill.clear();
	}
	int						nMultiSkillId;		// 对应id
	std::vector<std::string>	vecSkill;		// 连击技能表
	
	bool operator==(const MultiSkillInfo& rhs) const
	{
		return nMultiSkillId == rhs.nMultiSkillId;
	}
};

// 技能命中数据
struct HitTimeData 
{
	int				nHitTime;		// 命中时间
	int				nHitCount;		// 命中次数
};

class SkillDataQueryModule : public ILogicModule
{
public:
	typedef std::map<std::string, std::string>          KeyToValueMap;
	typedef std::map<std::string, KeyToValueMap>        SkillSectionMap;

public:
    typedef std::vector<std::string> PathConfigVector;
    typedef std::map<std::string, PathConfigVector> DataPathConfigArray;

    typedef std::vector<std::string> EventParamVector;
    typedef std::map<int, EventParamVector> EventParamsMap;

	typedef std::vector<SkillGrowProperty> SkillGrowPropertyVec;
	typedef std::map<std::string, SkillGrowPropertyVec> GrowPropertyRecMap;
	typedef std::map<int, PassiveSkill*> PassiveSkillInfoMap;
	typedef std::vector<MultiSkillInfo> MultiSkillInfoVec;
	typedef std::vector<HitTimeData>	HitTimeDataVec;
	typedef std::map<std::string, HitTimeDataVec>	HitTimeConfigMap;
public:
    SkillDataQueryModule();
    virtual ~SkillDataQueryModule(){}

    virtual bool Init(IKernel* pKernel);
    virtual bool Shut(IKernel* pKernel);

public:
	bool LoadResource(IKernel* pKernel);
    // 获取某类型的配置文件
    bool GetSkillConfigPath(const char* type, IVarList& result) const;
    bool GetBuffConfigPath(const char* type, IVarList& result) const;

    // 关于技能数据的接口
	const SkillBaseData* GetSkillBase(const char * szSkillID) const;
	const SkillMainData* GetSkillMain(IKernel* pKernel, const PERSISTID& skill) const;
	const SkillMainData* GetSkillMain(IGameObj* pSkillObj) const;

	// nGPType对应(SkillGrowProperty)
	const int QuerySkillGrowProperty(IKernel* pKernel, const PERSISTID& skill, int nGPType);

	// 获取玩家技能原始配置
	SkillMainData* GetSkillMainConfig(const char* szSkillID) const;
    const SkillHitRangeData* GetHitRangeProp(const char* szSkillID) const;
    const SkillHitRangeData* GetHitRangePropByID(const char* hitrangeID) const;
    const SkillEffectData* GetSkillEffectProp(const char* effectID) const;

	// 响应技能升级
	bool OnSkillPropChange(IKernel* pKernel, const PERSISTID& skill, const PERSISTID& self);
	
    // 关于Buff数据的接口
	const BuffBaseData* GetBuffBaseData(const char* szBuffID) const;
	const BuffEffectData* GetBuffEffectProp(const char* effectID) const;

	// 获取服务器与客户端技能时间差 nType对应SkillTimeDiff
	// 服务器端所有的技能时间要比客户端的技能时间稍短一些,解决在线玩家释放技能时,偶尔会有CD失败的情况
	//int	GetSkillTimeDiff(IKernel* pKernel, const PERSISTID& self, int nType);

	// 获取被动技能配置
	const PassiveSkill* GetPassiveSkillConfig(const int nSkillId) const;
	const PassiveSkillLevelInfo* GetPassiveSkillLevelInfo(const int nSkillId, const int nSkillLevel) const;

	// 查询PassiveSkill产生的效果参数 nPSType对应(PassiveSkillType)
	bool QueryPassiveSkillEffect(IKernel* pKernel,  IVarList& outData, const PERSISTID& self, const PERSISTID& skill, const int nPSType, const char* strPropName);

	// 根据职业找到所有被动技能id(测试、调试用)
	bool QueryPassiveSkillIdByJob(IKernel* pKernel,  IVarList& outData, const PERSISTID& self);

	// 获取一组连击技能数据
	const MultiSkillInfo* QueryMultiSkillInfo(int nMultiSkillId);

	// 获取命中数据
	const HitTimeDataVec* QueryHitTimeList(const char* strSkillId);

	// 获取技能最大命中次数
	int GetMaxHitNum(IKernel* pKernel, IGameObj* pSkillObj);
private:
    void ReleaseResource();

    bool LoadSkillProp();
    bool LoadBufferProp();
    bool LoadNpcNormalSkillProp();

    // 加载SKILL属性
    bool LoadSkillPathConfig();
    bool LoadSkillEventFunIdParam();

	// 加载技能公式
	bool LoadSkillFormula();

	// 加载被动技能数据
 	bool LoadPassiveSkillData();
 
 	// 读取被动技能属性数据
 	bool LoadPassiveSkillProperty(PassiveSkillLevelInfo& outSkillPro, const std::string& strInfo);

	// 加载BUFFER属性
    bool LoadBuffPathConfig();

	// 加载技能额外伤害表
	bool LoadSkillGrowPropertyRec();

	// 加载某个技能的额外伤害
	bool LoadOneSkillGrowProperty(SkillGrowPropertyVec& vecExtraHurt, const std::string& strSkillId);

	// 加载技能服务器与客户端的技能时间差
	//bool LoadSkillTimeDiffConfig();

	// 加载连击技能
	bool LoadMultiSkillConfig();

	// 根据技能成长类型找到属性值
	int GetPropertyBySkillGPType(const SkillMainData* pMainData, int nGPType);

	// 查询成长属性的值
	int GetGrowProperty(IKernel* pKernel, const PERSISTID& skill, int nGPType);

	// 查询PassiveSkill的属性
	bool QueryPassiveSkillProp(IKernel* pKernel, IVarList& outData, const int nPassiveSkillId,
		const int nPassiveSkillLV, const int nPSType, const char* strPropName);

    template<class T>
    bool LoadProp(const PathConfigVector& vPath, TStringPod<char, T*>& map);
    template<class T>
    bool LoadProp(const PathConfigVector& vPath, const EventParamsMap& eventParamsMap, TStringPod<char, T*>& map);
    template<class T>
    void ReleaseProp(TStringPod<char, T*>& map);
    template<class T>
    const T*  GetPropPoint(const char* strKeyID, const TStringPod<char, T*>& map) const;
	// 解析技能命中数据
	void ParseHitTimeData();
private:
	// 针对技能配置的数据成员
    TStringPod<char, SkillBaseData*>         m_SkillBase;
    TStringPod<char, SkillMainData*>         m_SkillMainData;
    TStringPod<char, SkillHitRangeData*>     m_SkillHitRangeData;
    TStringPod<char, SkillEffectData*>       m_SkillEffectData;
	TStringPod<char, BuffEffectData*>		 m_BuffEffectData;
	std::vector<int>						 m_vSkillTimeDiff;
	
	// 针对Buffer配置的数据成员
	TStringPod<char, BuffBaseData*>         m_BuffBase;

    SkillSectionMap                         m_SkillUseLimitData;
	GrowPropertyRecMap						m_mapSkillGrowProperty;		// 技能成长属性查询表
	PassiveSkillInfoMap						m_mapPassiveSkill;			// 被动技能数据表
	MultiSkillInfoVec						m_vecMultiSkillInfo;		// 连击技能配置
	HitTimeConfigMap						m_mapHitTimeData;			// 命中数据
private:
    // 配置路径表
    DataPathConfigArray m_vSkillDataPathConfigArray;
    DataPathConfigArray m_vBuffDataPathConfigArray;

	// 技能事件数据保存
	EventParamsMap m_eventParamsMap;

    // 数据文件主目录
    std::string m_strWorkPath;

	StringBuffer m_StringBuffer;

	static LevelModule*			m_pLevelModule;
public:
	// 本模块指针
	static SkillDataQueryModule* m_pInstance;
};

#endif // __SKILL_DATA_QUERY_MANAGER_H__
