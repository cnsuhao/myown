//--------------------------------------------------------------------
// 文件名:      BossRefreshModule.h
// 内  容:      Boss刷新模块
// 说  明:		
// 创建日期:    2016年6月28日
// 创建人:      刘明飞
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef __BossRefreshModule_H__
#define __BossRefreshModule_H__

#include "Fsgame/Define/header.h"
#include <vector>
#include <map>
#include "CommonModule/CommRuleDefine.h"

#define SCENE_BOSS_REC "scene_boss_rec"    //场景BOSS信息表
enum
{
	SCENE_BOSS_CONFIG = 0,         //boss编号
	SCENE_BOSS_OBJ = 1,            //boss对象
    SCENE_BOSS_POS_X,
    SCENE_BOSS_POS_Z,
	SCENE_BOSS_POS_ORIENT,
};

enum
{
	/*!
	* @brief	boss状态数据
	* @param	int n Boss个数
	以下循环n次
	* @param	string	bossid
	* @param	int		刷新倒计时(1表示已刷新)
	* @param	int		场景id
	* @param	wstring	击杀玩家
	*/
    SC_SUBMSG_QUERY_BOSS_IN_SCENE = 1,      //查询BOSS信息  

	/*!
	* @brief	boss状态改变
	* @param	string	bossid
	* @param	int		状态 
	*/
	SC_SUBMSG_BOSS_STATE_CHANGE,
};

enum
{
    UPDATE_BOSS_AMOUNT_ADD = 0,
    UPDATE_BOSS_AMOUNT_DEL = 1,
};

using namespace std;
class SceneInfoModule;
class NpcCreatorModule;
class AsynCtrlModule;
class BossRefreshModule: public ILogicModule  
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);

	// 是否为刷boss场景
	bool IsRefreshBossScene(IKernel* pKernel);

	// 查询某个场景的野外boss数据 outData 格式:outData << bossid << lefttime << bossid << lefttime..
	bool QueryBossInfo(IKernel * pKernel, int nSceneId, IVarList& outData);
private:
    //boss配置信息
    struct BossInfo
    {
        PosInfo BossPos;
		std::string sBossConfig;
		int nInterval;	//最小间隔
    };
public:
	//加载配置
	bool LoadResource(IKernel* pKernel);

	//场景创建
	static int OnCreate(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    //NPC进入场景
    static int OnBossEntry(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    //NPC销毁
    static int OnCommandBossBeKilled(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	//场景创建BOSS心跳
	static int HB_BossCreate(IKernel* pKernel, const PERSISTID& self,int silce);

    //处理客户端消息
    static int OnCustomQuerySceneBoss(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 黑夜城退出分组
	static int OnCustomQuitGroup(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    //获取公共服场景BOSS信息表
    IRecord * GetPubBossInSceneRec(IKernel * pKernel, const char* strRecName);

	// 场景boss状态改变
	void SendBossStateChangeMsg(IKernel * pKernel, const char* strBossId, int nState);
private:
	//场景刷怪信息
	struct SceneRefreshInfo
	{
		SceneRefreshInfo() : nOpen(0), beginTime(0),
			endTime(0)
		{
			VecSceneBossInfo.clear();
		};
		int nOpen; //是否开放时间段内不刷新
		int beginTime;	//开启时间
		int endTime;	//结束时间
		//int nInterval;	//最小间隔
		vector<BossInfo> VecSceneBossInfo;
	};

	typedef map<int, SceneRefreshInfo> SceneInfoMap;

	// 查询boss的刷新间隔
	int QueryBossInterval(const SceneRefreshInfo& sceneInfo, const char* strBoss);

    //查询场景和BOSS信息
    int QueryBossInfoInScene(IKernel* pKernel, const PERSISTID& self);

	// 查询boss数据
	const BossInfo* FindBossInfo(IKernel* pKernel, const string& strBossConfig);

	//创建boss
	void CreateBoss(IKernel* pKernel, const char* strBossConfig, IRecord* pBossRecord, int nRowIndex);

	// 读取场景boss数据
	bool LoadSceneBoss(IKernel* pKernel);

    //是否统计此场景内BOSS数量
    bool ContainSceneID(int sceneID);

    //是否在不刷新时间段内
    bool NotRefresh(int beginTime, int endTime);

	// 重新加载技能配置
	static void ReloadConfig(IKernel* pKernel);

	// 获取公共数据名称
	static const std::wstring & GetDomainName(IKernel * pKernel);
public:
	static BossRefreshModule * m_pBossRefreshModule;
private:
    static NpcCreatorModule * m_pNpcCreatorModule;
    static AsynCtrlModule * m_pAsynCtrlModule;
private:
	SceneInfoMap	m_MapSceneBossInfo;
	static std::wstring	m_domainName;
};

#endif // __BossRefreshModule_H__
