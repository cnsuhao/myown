//--------------------------------------------------------------------
// 文件名:      Server\FsGame\SkillModule\CoolDownModule.h
// 内  容:      冷却系统定义
// 说  明:
// 创建日期:    2008/01/28
// 创建人:      冯红文
//    :       
//--------------------------------------------------------------------

#ifndef __CoolDownModule_h__
#define __CoolDownModule_h__

#include "Fsgame/Define/header.h"
#include <vector>

//冷却表的列定义
enum CoolDownRecColIndex
{
    COOLDOWN_REC_COL_ID,        //冷却分类ID
    COOLDOWN_REC_COL_BEGINTIME, //冷却开始时间
    COOLDOWN_REC_COL_ENDTIME,   //冷却结束时间

    COOLDOWN_REC_COL_COUNT
};

class CoolDownModule : public ILogicModule
{
public:
	//初始化
	virtual bool Init(IKernel* pKernel);

	//释放
	virtual bool Shut(IKernel* pKernel);

	static CoolDownModule* Instance();
public:

	/**
	@brief        开始一个冷却分类的冷却
	@param        参数
	[IN]cooldowncategory:冷却分类编号
	[IN]cooldowntime:冷却时间，单位ms
	@remarks
	@return       如果成功开始冷却，返回true，否则，返回false
	*/
	bool BeginCoolDown(IKernel* pKernel, const PERSISTID& self,
		int cooldowncategory, int cooldowntime );

	/**
	@brief        结束一个冷却分类的冷却
	@param        参数
	[IN]cooldowncategory:冷却分类编号
	@remarks
	@return       如果成功开始冷却，返回true，否则，返回false
	*/
	bool EndCoolDown(IKernel* pKernel, const PERSISTID& self, int cooldowncategory);

	/**
	@brief        指定分类是否正在冷却中
	@param        参数
	[IN]cooldowncategory:冷却分类编号
	@remarks
	@return       如果某分类正在冷却中，返回true，否则返回false
	*/
	bool IsCoolDown(IKernel* pKernel, const PERSISTID& self, int cooldowncategory);

    // 取得冷却的剩余毫秒数
    int64_t GetMillisecRemain(IKernel* pKernel, const PERSISTID& self, int cooldowncategory);

	//清除全部CD
	void ClearAllCD(IKernel* pKernel, const PERSISTID& self);

	//清除技能CD
	void ClearPlayerNormalSkillCD(IKernel* pKernel, const PERSISTID& self);

	// 读取配置
	bool LoadConfig(IKernel* pKernel);
private:
	// 玩家下线
	static int OnPlayerStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	//添加冷却信息倒冷却表
	bool InnerBeginCoolDown(IRecord* pRecordCool, const int& cooldowncategory,
		const int64_t& begin_time, const int64_t& end_time);

	// 检查是否正在冷却，如果在冷却返回行号，如果不在冷却返回-1
	int InnerIsCoolDown(IRecord* pRecordCool, const int cooldowncategory, const int64_t now);

	//清除玩家超时的冷却表
	bool ClearCoolDownRec(IRecord* pRecordCool);

	// 是否需要保存的CD分类
	bool IsNeedSaveCategory(int nCategory);

	static void ReloadConfig(IKernel* pKernel);
public:
	static CoolDownModule* m_pInstance;
private:
	std::vector<int>	m_vSavedCategoryIds;		// 需要保存的CD编号
};
#endif