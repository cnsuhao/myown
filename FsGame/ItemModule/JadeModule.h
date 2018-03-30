//--------------------------------------------------------------------
// 文件名:      JadeModule.h
// 内  容:      玉珏模块
// 说  明:
// 创建日期:    2018年3月23日
// 创建人:      刘明飞 
//    :       
//--------------------------------------------------------------------
#ifndef _JADE_MODULE_H_
#define _JADE_MODULE_H_

#include "Fsgame/Define/header.h"
#include <vector>
#include <map>
#include "CommonModule/CommRuleDefine.h"

class ContainerModule;
class ItemBaseModule;

class JadeModule : public ILogicModule
{
public:
	//初始化
	virtual bool Init(IKernel* pKernel);

	//释放
	virtual bool Shut(IKernel* pKernel){ return true; }

	// 客户端相关消息
	static int  OnCustomJadeMsg(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
private:
	// 阶级数据
	struct StepData
	{
		int				nStepLv;			// 阶级
		Consume_Vec		vecConsumeItem;		// 消耗物品
		int				nPieceNum;			// 消耗碎片数量
		int				nJadeLvLimit;		// 玉珏等级限制
		int				nSkillLv;			// 玉珏技能等级
	};

	typedef std::vector<StepData> StepDataVec;

	// 等级数据
	struct LevelData
	{
		int				nLevel;				// 等级
		Consume_Vec		vecConsumeItem;		// 消耗物品
		int				nPackageId;			// 属性包id
		int				nPlayerLvLimit;		// 玩家等级限制
	};

	typedef std::vector<LevelData> LevelDataVec;

	// 玉珏数据
	struct JadeMainData
	{
		std::string		strJadeId;			// 玉珏id
		std::string		strPiece;			// 玉珏碎片
		int				nPieceNum;			// 碎片数量
		std::string		strSkillid;			// 技能id

		StepDataVec		vecStepData;		// 升阶数据
		LevelDataVec    vecLevelData;		// 升级数据
	};

	typedef std::map<std::string, JadeMainData*> JadeMainDataMap;
	typedef std::map<std::string, JadeMainData*>::iterator JadeMainDataMapIter;

private:
	//回调函数 玩家加载数据完成
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应玉珏升级
	void OnCustomLevelUp(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应玉珏升阶
	void OnCustomStepUp(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应碎片合成玉珏
	void OnCustomCompose(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应穿戴玉珏
	void OnCustomWear(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 读取配置
	bool LoadResource(IKernel* pKernel);

	// 读取主配置
	bool LoadMainResource(IKernel* pKernel);

	// 读取升阶配置
	bool LoadStepResource(IKernel* pKernel);

	// 读取升级配置
	bool LoadLevelResource(IKernel* pKernel);

	// 更新玩家玉珏属性
	void UpdateJadeModifyProp(IKernel* pKernel, const PERSISTID& self);

	// 查询升阶数据
	const StepData* QueryStepData(std::string strJadeId, int nStep);

	// 查询升级数据
	const LevelData* QueryLevelData(std::string strJadeId, int nLevel);

	// 查询玉珏数据
	const JadeMainData*	QueryJadeMainData(std::string strJadeId);

	// 清除配置数据
	void ClearMainData();

	static void ReloadConfig(IKernel* pKernel);
public:
	static JadeModule* m_pJadeModule;

private:         
	JadeMainDataMap	m_mapJadeMainData;	// 玉珏数据

	static ContainerModule* m_pContainerModule;
	static ItemBaseModule* m_pItemBaseModule;
};

#endif // _JADE_MODULE_H_
