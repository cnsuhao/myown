//--------------------------------------------------------------------
// 文件名:		CreateRoleModule.h
//--------------------------------------------------------------------

#ifndef __CreateRoleModule_H__
#define __CreateRoleModule_H__

#include "Fsgame/Define/header.h"
#include <string>
#include <vector>
#include <map>

class MotionModule;
class ContainerModule;
class LevelModule;
class StaticDataQueryModule;
class EquipmentModule;
class ItemBaseModule;
class CapitalModule;



//设置role属性的command子消息编号
enum
{
	SUB_EAI_SET_OS_TYPE = 0, //级效设置ostype
	SUB_EDIT_PLAYER_OS_TYPE = 1,//离线编辑玩家属性回调的子消息编号
};


// 创建角色
class CreateRoleModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);

public:
	// 创建角色
	static int OnCreateRole(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
	// 首次进入角色
	static int OnFirstRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 角色离线
	static int OnStore(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 保存玩家登陆时的外观信息
	static int OnSaveRoleInfo(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	// 加载保存属性信息
	bool LoadSaveRoleInfo(IKernel* pKernel);
	
	// 竞技场的机器人配置
	bool LoadArenaRobotInfo(IKernel* pKernel);

	// 保存玩家登陆时的外观信息
	bool SaveRoleInfo(IKernel* pKernel, const PERSISTID& self);
	
	//初始化角色信息
	int RoleInit(IKernel* pKernel,const PERSISTID& self);
private:
	// 出生配置
	struct BornConfig
	{
		int				nNationId;			// 阵营Id
		int				nSceneId;			// 场景id
		std::string 	strBornPoint;		// 出生点
	};
	typedef std::vector<BornConfig> BornConfigVec;

	//初始化容器
	void _initRoleInfoContainers(IKernel* pKernel, const PERSISTID& self);
	//初始化表
	void _initRoleStrRecord(IKernel* pKernel, const PERSISTID& self);
	// 初始化某个容器，needReset参数如果为true，则在初始过程中，如果容器存在，则将容器清空
	bool _initRoleInfoContainer(IKernel* pKernel, const PERSISTID& self, const wchar_t* name,
		const char* script, int capacity, bool needReset = true);
	// 初始化角色宝箱信息
	void _initRoleInfoTreasure(IKernel* pKernel, const PERSISTID& self);
	// 初始化角色登录、等级奖励的首次登录时间
//	void _initRoleInfoFestival(IKernel* pKernel, const PERSISTID& self);
	// 重加载配置文件
	static int nx_reload_create_role_config(void* state);
    //脱离卡死
    static int EscapeFromLock(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const IVarList& args);	
public:
	static CreateRoleModule *	m_pCreateRoleModule;
private:
    static MotionModule *		m_pMotionModule;
    static ContainerModule *	m_pContainerModule;
	static LevelModule *		m_pLevelModule;
	static StaticDataQueryModule *	m_pStaticDataQueryModule;
	static EquipmentModule *	m_pEquipmentModule;
	static ItemBaseModule *		m_pItemBaseModule;
	static CapitalModule *		m_pCapitalModule;

	std::vector<std::string> m_vSaveRoleProps;
	
	// 竞技场机器人的初始装备
	struct ArenaRobotEquipment
	{
		ArenaRobotEquipment(): role_job(0), role_level(0), equip(""), equip_strlevel(""), equip_jewel(""), equip_soul(""){}
	    int role_job;
	    int role_level;
        std::string equip;
        std::string equip_strlevel;
        std::string equip_jewel;
        std::string equip_soul;
	};
	
	// key = job_lelvel
	typedef std::map<std::string, std::vector<ArenaRobotEquipment>> ArenaEquipMap;
    ArenaEquipMap m_ArenaRobotEquipment;
};

#endif // __CreateRoleModule_H__
