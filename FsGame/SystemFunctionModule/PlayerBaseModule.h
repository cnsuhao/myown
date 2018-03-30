//--------------------------------------------------------------------
// 文件名:		PlayerBaseModule.h
// 内  容:		玩家基础数据
// 说  明:		
// 创建日期:	2014年10月17日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------
#ifndef __PlayerBaseModule_H__
#define __PlayerBaseModule_H__

#include "Fsgame/Define/header.h"
#include <vector>
#include "Define/PlayerBaseDefine.h"
#include <set>

// 玩家版本号对应函数定义
enum PlayerVersion
{
	PLAYER_VERSION_MIN          = 0,  // 最小版本号


	PLAYER_VERSION_MAX,               // 最大版本号    
};

//客户端提示文本
//const char * const CROSSBATTLE_MATCHBATTLE_NO_EXIT_ROLE = "CrossBattle_MatchBattle_No_Exit_Role";  //当前在跨服匹配中不可小退

// 玩家升级版本号功能函数定义
typedef bool (__cdecl* PLAYER_VERSION_FUNC)(IKernel *pKernel, const PERSISTID &self, const IVarList& args);

class PlayerBaseModule: public ILogicModule 
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);

public:
	//属性
	static int OnCreateClass(IKernel* pKernel, int index);
	/// \brief 上线
	static int OnRecover(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	/// \brief 断线重连
	static int OnContinue(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	/// \brief 上线
	static int OnEntry(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	/// \brief 进入游戏
	static int OnEntryGame(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	/// \brief 下线
	static int OnStore(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	/// \brief 下线
	static int OnStore2(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	/// \brief 离开
	static int OnLeave(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	static int OnDisconnect(IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args);

	//选择对象
	static int OnPlayerSelect(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	//获取服务器时间点
	static int OnCustomGetServerTime(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 获取玩家属性
	static int OnCustomGetPlayerPropData(IKernel* pKernel, const PERSISTID& self,
							const PERSISTID& sender, const IVarList& args);

	// （小退）返回角色选择
	static int OnCheckedSelectRole(IKernel* pKernel,const PERSISTID& self,
		const PERSISTID& player, const IVarList& args);

	//获取设备号
	static int GetToken(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	//客户端请求设置屏幕可视玩家数量
	static int OnCustomSetVisualPlayers(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 响应被伤害事件
	static int OnCommandBeDamage(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 冻结角色
	static int OnCommandBlockPlayer(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
	// 客户端查询命令
	static int OnCommandClientQueryPropData(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 踢下线
	static int CB_ForceOffline(IKernel* pKernel, const PERSISTID& self, int time);

	// 冻结在线玩家角色 time为终止时间
	static bool BlockPlayer(IKernel* pKernel, const PERSISTID& self, int64_t time);
	// 冻结解封角色 time=0 解封 time为终止时间
	static bool BlockPlayer(IKernel* pKernel, const wchar_t* playerName, int64_t time);

    // 0点更新登录时间
	static int ResetLoginTimeTimer(IKernel* pKernel, const PERSISTID& self, int slice);

	// 客户端消息实力检查
	static int OnCustomCheckStrength(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	// 客户端开始(结束)加速请求
	static int OnCustomChangeSpeed(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

    // 客户端转到后台
    static int OnClientBackgroundTask(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	// 是否是机器人判断
	static bool IsRobot(IKernel *pKernel, const PERSISTID &player);

	// 设置全局标志
// 	static void AddPlayerGlobalFlag(IKernel *pKernel, const PERSISTID &player, EPlayerGlobalFlag flag);
// 	// 移除全局标志
// 	static void RemovePlayerGlobalFlag(IKernel *pKernel, const PERSISTID &player, EPlayerGlobalFlag flag);
// 	// 设置所有标志
// 	static void SetPlayerGlobalFlag(IKernel *pKernel, const PERSISTID &player, int nValue );
// 	// 测试标志状态
// 	static bool TestPlayerGlobalFlag(IKernel *pKernel, const PERSISTID &player, EPlayerGlobalFlag flag);
// 	// 测试标志集合
// 	static bool TestPlayerGlobalFlags(IKernel* pKernel, const PERSISTID &player, int64_t nValue);
// 	// 测试标志状态值
// 	static bool TestPlayerGlobalValue(IKernel *pKernel, const PERSISTID &player, int64_t nValue);

private:
	// 更新渠道uid
	void UpdateAccountUID(IKernel* pKernel, const PERSISTID& self);

	// 注册升级版本号函数
	static bool RegisterVersionFun(PlayerVersion version, PLAYER_VERSION_FUNC fun);

	// 处理查询玩家属性
	void HandlerPlayerPropQuery(IKernel* pKernel, const wchar_t* pszSender, IGameObj& player, const IVarList& query, int nOffset, int nUID);
public:
	// 处理玩家版本号升级逻辑
	static bool HandlePlayerVersion(IKernel *pKernel, const PERSISTID &self, const IVarList& args);

private:

public:
	static PlayerBaseModule * m_pPlayerBaseModule;

	//判断函数地址表
	static std::vector<PLAYER_VERSION_FUNC> m_funList;

	// 客户端可通过属性名直接查询的属性集合
	std::set<std::string>	m_propPubset;
};

#endif // __PlayerBaseModule_H__
