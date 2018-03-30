//--------------------------------------------------------------------
// 文件名:      SnsFriend.h
// 内  容:      好友消息存储
// 说  明:
// 创建日期:    2014年4月12日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef Sns_Friend_h__
#define Sns_Friend_h__


#include "server/ISnsKernel.h"
//#include "server/ISnsCallee.h"
//#include "server/IRecord.h"
#include "../ISnsLogic.h"

#define SNS_FRIEND_MAXROW	100
#define SNS_FRIEND_GIFT_MAXROW	1024
#define SNS_FRIEND_DEL_SWORN_MAXROW 20 
#define SNS_FRIEND_ENEMY_MAXROW 50
#define SNS_FRIEND_APPLY_RECORD         "FriendApplyRecord"
#define SNS_FRIEND_DELETE_RECORD        "FriendDeleteRecord"
#define SNS_FRIEND_RESULT_RECORD        "FriendApplyResultRecord"
#define SNS_FRIEND_GIFT_RECORD			"FriendGiftRecord"
#define SNS_FRIEND_ADD_INITMACY			"FriendAddInitmacy"
#define SNS_FRIEND_DEL_SWORN            "FriendDelSworn"
#define SNS_FRIEND_ADD_ENEMY			"FriendEnemy"

enum FriendApplyRecCols
{
    SNS_FRIEND_APPLY_COL_SENDER_UID = 0,
    SNS_FRIEND_APPLY_COL_SENDER_NAME,
    SNS_FRIEND_APPLY_COL_RECEIVER_UID,
    SNS_FRIEND_APPLY_COL_RECEIVER_NAME,
    SNS_FRIEND_APPLY_COL_LEVEL,
	SNS_FRIEND_APPLY_COL_JOB,
	SNS_FRIEND_APPLY_COL_SEX,
    SNS_FRIEND_APPLY_COL_FIGHT_CAPACITY,
	SNS_FRIEND_APPLY_COL_GUILD_NAME,
    SNS_FRIEND_APPLY_COL_TYPE,
	SNS_FRIEND_APPLY_COL_VIP_LEVEL,
};

enum FriendDeleteRecCols
{
	SNS_FRIEND_DELETE_COL_SENDER_UID = 0,
	SNS_FRIEND_DELETE_COL_SENDER_NAME,
	SNS_FRIEND_DELETE_COL_RECEIVER_UID,
	SNS_FRIEND_DELETE_COL_RECEIVER_NAME,
};

enum FriendResultRecCols
{
    SNS_FRIEND_RESULT_COL_SENDER_UID = 0,
    SNS_FRIEND_RESULT_COL_SENDER_NAME,
    SNS_FRIEND_RESULT_COL_RECEIVER_UID,
    SNS_FRIEND_RESULT_COL_RECEIVER_NAME,
    SNS_FRIEND_RESULT_COL_LEVEL,
	SNS_FRIEND_RESULT_COL_JOB,
	SNS_FRIEND_RESULT_COL_SEX,
    SNS_FRIEND_RESULT_COL_FIGHT_CAPACITY,
	SNS_FRIEND_RESULT_COL_GUILD_NAME,
    SNS_FRIEND_RESULT_COL_RESULT,
};

enum FriendGiftRecCols
{
	SNS_FRIEND_GIFT_COL_SENDER_UID = 0,
	SNS_FRIEND_GIFT_COL_GIFT_COUNT,
};

enum FriendAddInitmacyRecCols
{
	SNS_FRIEND_ADD_INTIMACY_COL_SENDER_UID = 0,
	SNS_FRIEND_ADD_INTIMACY_COL_VALUE = 1,
};

enum FriendDelSwornCols
{
	SNS_FRIEND_DEL_SWORN_COL_NAME = 0,
};

enum FriendEnemyCols
{
	SNS_FRIEND_ENEMY_COL_UID = 0,
	SNS_FRIEND_ENEMY_COL_NAME,
	SNS_FRIEND_ENEMY_COL_LEVEL,
	SNS_FRIEND_ENEMY_COL_JOB,
	SNS_FRIEND_ENEMY_COL_SEX,
	SNS_FRIEND_ENEMY_COL_ABILITY,
	SNS_FRIEND_ENEMY_COL_GUILDNAME,
	SNS_FRIEND_ENEMY_COL_ACCOUNT,
	SNS_FRIEND_ENEMY_COL_HATER, //  仇恨值
	SNS_FRIEND_ENEMY_COL_TIMES, //  时间
	SNS_FRIEND_ENEMY_COL_VIP_LV, //  vip lv

	SNS_FRIEND_ENEMY_COL_MAX,


};

class SnsFriend : public ISnsLogic
{
public:

    SnsFriend();
    ~SnsFriend();

	// 逻辑对象被创建
	int OnModuleCreate(ISnsKernel* pKernel, const IVarList& args);

	// SNS服务器所有的SNS数据已经加载完成准备就绪(在若干次OnSnsLoad之后)
	int OnReady(ISnsKernel* pKernel, const IVarList& args);

	// SNS数据在内存中创建
	int OnCreate(ISnsKernel* pKernel, const char* uid, const IVarList& args);

	// 加载SNS数据
	int OnLoad(ISnsKernel* pKernel, const char* uid, const IVarList& args);

	//收到来自member的消息
	int OnMessage(ISnsKernel* pKernel, int distribute_id, int server_id,
		int memeber_id, int scene_id, const char* uid, const IVarList& args);

private:

    // 申请离线加好友
    void FriendApply(ISnsKernel *pKernel, const IVarList &msg);

	// 申请解除好友关系
	void FriendDelete(ISnsKernel *pKernel, const IVarList &msg);

	// 获取离线申请
	void QueryApply(ISnsKernel *pKernel,const char *uid, 
							int server_id, int memeber_id, int scene_id, const IVarList &msg);

	// 获取解除关系申请
	void QueryDelete(ISnsKernel *pKernel, const char *uid, 
							int server_id, int memeber_id, int scene_id, const IVarList &msg);

    // 离线申请结果
    void ApplyResult(ISnsKernel *pKernel, const IVarList &msg);

    // 获取申请结果
    void QueryResult(ISnsKernel *pKernel, const char *uid, 
							int server_id, int memeber_id, int scene_id, const IVarList &msg);

	// 离线送花
	void FriendGiftGive(ISnsKernel *pKernel, const char *uid,
		int server_id, int memeber_id, int scene_id, const IVarList &msg);
	// 获取离线送花
	void FriendGiftGet(ISnsKernel *pKernel, const char *uid,
		int server_id, int memeber_id, int scene_id, const IVarList &msg);

	// 离线增加亲密度
	void AddIntimacy(ISnsKernel *pKernel, const char *uid, const IVarList &msg);
	//获取增加亲密度
	void QueryAddIntimacy(ISnsKernel *pKernel, const char *uid,
		int server_id, int memeber_id, int scene_id, const IVarList &msg);
	void AddDelSworn(ISnsKernel *pKernel, const char *uid,
		int server_id, int memeber_id, int scene_id, const IVarList &msg);
	//获取删除结义列表
	void QueryDelSworn(ISnsKernel *pKernel, const char *uid,
		int server_id, int memeber_id, int scene_id, const IVarList &msg);
	//增加敌人
	void AddEnemy(ISnsKernel *pKernel, const char *uid,
		int server_id, int memeber_id, int scene_id, const IVarList &msg);
	void QueryEnemy(ISnsKernel *pKernel, const char *uid,
		int server_id, int memeber_id, int scene_id, const IVarList &msg);


private:

    // 创建多个好友表
    void CreateFriendRecords(ISnsKernel* pPubKernel, const char *uid);

    // 创建一个SNS表
    void _create_record(ISnsKernel *pKernel, ISnsData *pSnsData, const char *recName, const IVarList &coltype, int rows);
};
#endif // Sns_Friend_h__
