//--------------------------------------------------------------------
// 文件名:      GroupNpcRefresh.h
// 内  容:      分组场景副本怪物刷新规则
// 说  明:
// 创建日期:    2015年8月4日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#ifndef _GROUP_NPC_REFRESH_H_
#define _GROUP_NPC_REFRESH_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/GroupSceneDefine.h"
#include <vector>
#include <map>
#include "Fsgame/Define/FightDefine.h"

typedef bool (__cdecl* GROUP_SCENE_REFRESH_FUNC)(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

class NpcCreatorModule;
class SecretSceneModule;
class TeamSceneModule;
class GroupNpcRefresh : public ILogicModule
{
public:
    //初始化
    virtual bool Init(IKernel* pKernel);
    //关闭
    virtual bool Shut(IKernel* pKernel);
public:
    //记录野怪数量
    static int RecordNpc(IKernel* pKernel,const PERSISTID& npc, const PERSISTID& player, const IVarList& args);
    //刷新BOSS
    bool RefreshBoss(IKernel * pKernel ,const PERSISTID& self, int sceneID, int killComm, int killElite);
    //生成BOSS
    void CreateBoss(IKernel* pKernel, const PERSISTID& self);
    //BOSS被攻击
    static int AttackBoss(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
    //BOSS被击杀
    static int KillBoss(IKernel* pKernel, const PERSISTID& player, const PERSISTID& boss, int sceneID);
    //刷BOSS机制
    int RefreshBossType(int sceneID);
    //杀怪数标准
    int GetKillNpcAmount(int sceneID);
    //加载配置
    bool LoadResource(IKernel* pKernel);
	//停止玩家组所有人obj行为
	void StopGroupObj(IKernel *pKernel, int groupID, int waitTime = CONTROL_BUFF_FOREVER);
	//开始玩家组所有obj行为
	void StartGroupObj(IKernel *pKernel, int groupID, int buffTimeType = CONTROL_BUFF_TIMES);
	//(延迟3秒防止有npc消息没有处理完)清除所有npc
	void DelayClearGroupNpc(IKernel*pKernel,int groupID);

private:
    //注册刷新规则
    static bool RegisterRefreshRule(GROUP_SCENE_TYPE sceneType, GROUP_SCENE_REFRESH_FUNC func);

    //关卡秘境规则
    static bool CommonSecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    //试剑亭规则
    static bool TreasureSecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    //组队活动副本规则
    static bool TeamActivityRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    //宠物秘境规则
    static bool PetSecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    //天将铜钱规则
    static bool MoneySecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
    //夺命剑州规则
    static bool ItemSecretRule(IKernel* pKernel, const PERSISTID& self, const IVarList& args);
	//心跳检测秘境时间
	static int HB_ClearGroupNpc(IKernel* pKernel, const PERSISTID& self, int silce);

private:
    static std::vector<GROUP_SCENE_REFRESH_FUNC> v_funcList;
private:
    struct GroupNpcInfo 
    {
        GroupNpcInfo():sceneID(0), refreshBossType(0), killAmount(0), 
            bossConfigID(""), bossPosi(""){};
        int sceneID;
        int refreshBossType;
        int killAmount;
        std::string bossConfigID;
        std::string bossPosi;
    };
    std::map<int, GroupNpcInfo> m_mapGroupNpcConfig;
public:
    static GroupNpcRefresh*     m_pGroupNpcRefresh;
private:
    static NpcCreatorModule*    m_pNpcCreatorModule;

#ifndef FSROOMLOGIC_EXPORTS
    static SecretSceneModule*   m_pSecretSceneModule;
    static TeamSceneModule*     m_pTeamSceneModule;
#endif // FSROOMLOGIC_EXPORTS
};

#endif