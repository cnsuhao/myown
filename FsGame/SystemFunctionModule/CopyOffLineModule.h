//----------------------------------------------------------
// 文件名:      CopyOfflineModule.h
// 内  容:      为NPC拷贝离线玩家数据
// 说  明:		
// 创建日期:    2015年3月16日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#ifndef _CopyOfflineModule_H_
#define _CopyOfflineModule_H_

#include "Fsgame/Define/header.h"
#include "../Define/OffLineDefine.h"
#include <vector>

class CopyOffLineModule : public ILogicModule
{
public:
	// 初始化
	bool Init(IKernel* pKernel);
	// 关闭
	bool Shut(IKernel* pKernel);

	/*!
	 * @brief	拷贝离线玩家数据
	 * @param	self	NPC对象
	 * @return	bool
	 */
	bool CopyOffLineData(IKernel* pKernel, const PERSISTID& self);

	/*!
	 * @brief	清除拷贝记录表
	 * @param	int nGroupId
	 * @return	void
	 */
	void ClearOfflinePubRec(IKernel* pKernel, int nGroupId);

	/*!
	 * @brief	清除某一条记录表的数据
	 * @param	int nGroupId
	 * @param	PERSISTID self
	 * @return	void
	 */
	void ClearOfflinePubRecOneRow(IKernel* pKernel, int nGroupId, const PERSISTID& self);

	// 加载配置
	bool LoadResource(IKernel* pKernel);

	/*!
	 * @brief	拷贝离线玩家数据通过玩家uid
	 * @param	self	NPC对象
	 * @param	strUid	玩家uid
	 * @return	bool
	 */
	bool CopyPlayerDataByUid(IKernel* pKernel, const PERSISTID& self, const char* strUid);
#ifdef _DEBUG
	// 测试接口
	bool TestCopyOffLineData(IKernel* pKernel, const PERSISTID& self, const PERSISTID& npc);
#endif

	// 获取离线玩家等级差buff
	const char* GetLevelBuffer(const int level);

	// 随机取出不同一组的名字
	bool FindNameList(int nNameNum, std::vector<std::wstring>& vecOutNameList);
private:
	enum
	{
		DEFAULT_PROPERTY_LEVEL_INTERVAL = 5,// 默认属性 每隔5级一套数值
		MAX_GREATER_THAN_LEVEL = 10			// 最大等级差
	};
	// 数据信息
	struct MirDataInfo
	{
		MirDataInfo():  nType(0), bIsModify(false), bIsLoadConfig(false){}
		std::string		strName;			// 名字
		int				nType;				// 数据类型对应(DataType)
		std::string		strSnsTable;		// 容器对应的sns表
		bool			bIsModify;			// 是否需要修改
		bool			bIsLoadConfig;		// 是否需要重新读取的配置
	};

	typedef std::vector<MirDataInfo> MirDataInfoVec;

	// 系数类型
	enum RatioType
	{
		GREATER_THAN_RATIO,					// 大于等级差的系数
		LESS_THAN_RATIO,					// 小于等级差的系数
		LESS_THAN_ZERO_RATIO,				// 等级差小于0的系数

		MAX_RATIO_NUM
	};

	// 系数计算数据
	struct RatioInfo 
	{
		RatioInfo() : nLevelDiff(0), fMinDCRatio(0.0f), fMaxDCRatio(0.0f)
		{
			memset(fRatio, 0, MAX_RATIO_NUM * sizeof(float));
		}
		int		nLevelDiff;					// 等级差(镜像NPC的Level - 玩家的Level的值)
		float	fRatio[MAX_RATIO_NUM];		// 系数的值
		float	fMinDCRatio;				// 默认属性最小变化率
		float	fMaxDCRatio;				// 默认属性最大变化率
		float   fArenaRatio;				// 竞技场npc实力系数
	};

	// 重加载配置文件
	static int nx_reload_copy_playertonpc_config(void* state);

	// 读取数据信息配置
	bool LoadDataInfoConfig(IKernel* pKernel);

	// 读取默认的名字配置
	bool LoadDefaultName(IKernel* pKernel);

    // 加载等级buff配置
	bool LoadLevelBuffCfg(IKernel* pKernel);

	// 加载npcai不需要学的技能
	bool LoadNotAIUseSkill(IKernel* pKernel);

	// 处理公共数据服务器下传的消息
	static int OnPublicMessage(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 处理SNS服务器下传的消息
	static int OnSNSMessage(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 拷贝属性数据
	bool CopyPropertyData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex);

	// 拷贝容器数据
	bool CopyContainerData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex);
 
	// 拷贝技能容器数据
	bool CopySkillContainerData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex);

	// 拷贝表数据
	bool CopyRecordData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex);

	// 设置宠物技能数据
// 	void SetPetSkillData(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& petitem, const char* strSkillData);
// 
// 	// 设置宠物天赋数据
// 	void SetPetTalentData(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& petitem, const char* strTalentData);

	// 获取domain的名字
	static const std::wstring& GetDomainName(IKernel * pKernel);

	// 从默认配置数据赋值给NPC
	bool CopyDataFromDefaultConfig(IKernel* pKernel, const PERSISTID& self);

	// 为NPC拷贝一个默认的属性数据
	//void CopyDefaultProperty(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, int nSecIndex, float fDCRatio = 1.0f);

	// 为NPC拷贝一个默认的技能数据
//	void CopyDefaultSkillContainer(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, int nSecIndex);

	// 拷贝数据后的操作
	void OnAferCopyData(IKernel* pKernel, IGameObj* pSelfObj, int nPlayerLevel);
	
	// 获取属性变化系数
	const float GetPropertyRatio(IKernel* pKernel, IGameObj* pSelfObj, int nPlayerLevel);

	// 计算默认属性变化率
	const float ComputeDCRatio();
	
	// 等级buff补偿
	struct LevelBuffer
	{
		LevelBuffer(): m_MinLevel(0), m_MaxLevel(0), m_BuffID(""){} 
	    int		        m_MinLevel;	  // 等级段(小)
	    int				m_MaxLevel;	  // 等级段(大)
	    std::string		m_BuffID;	  // 对应BuffID
	};
	typedef std::vector<LevelBuffer> levelBufferVec;

private:
	MirDataInfoVec				m_vDataInfo;			// 数据信息
	static std::wstring			m_wsDomainName;			// 公共区名
	std::vector<std::wstring>   m_vDefaultName;			// 默认属性的名字库
	RatioInfo					m_kRatioInfo;			// 系数配置数据
public:
	static CopyOffLineModule*	m_pCopyOffLineModule;	
};

#endif