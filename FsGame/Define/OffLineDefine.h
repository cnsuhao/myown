//----------------------------------------------------------
// 文件名:      OffLineDefine.h
// 内  容:      离线挂机相关宏定义
// 说  明:
// 创建日期:    2014年10月17日
// 创建人:       
// 修改人:		 
//    :       
//----------------------------------------------------------

#ifndef __OffLineDefine_H__ 
#define __OffLineDefine_H__

//挂机背包
#define   OFFLINE_BOX              L"OffLineBox"

// #define PLAYER_OFFLINE_STATE_REC	"offline_state_rec"
// enum
// {
// 	OFFLINE_PLAYER_NAME	= 0,	//场景中离线玩家姓名
// 	OFFLINE_PLAYER_STATE,		//离线玩家状态 OffLineState
// 	OFFLINE_PLAYER_ENTER_SCENE, //OffLineState为2时,玩家进入场景的id,0正常逻辑进入场景
// 	OFFLINE_PLAYER_ACCOUNT,		//玩家的账号
// 
// 	OFFLINE_STATE_REC_MAX,
// 
// 	MAX_OFFLINE_PLAYER_ROWS = 500
// };
// 
// // 离线挂机技能表
// #define PLAYER_OFFLINE_SKILL_REC	"OffLine_skill_rec"
// enum
// {
// 	OFFLINE_SKILL_ID = 0,			//技能id
// 
// 	OFFLINE_SKILL_REC_MAX,
// };


// 玩家登陆状态
enum LoginState
{
	ONLINE_LOGIN,					//在线登陆
	OFFLINE_LOGIN,					//离线登陆
	NOT_LOGIN,						//未登陆
};

// 场景离线玩家分布表
#define SCENE_OFFLINE_REC "OffLineDistrbuteRec"
enum SceneOfflineRec
{
	SCENE_COL_OFFLINE_POS_INDEX = 0,		// 怪点索引
	SCENE_COL_OFFLINE_POS_NORMAL_NUM,		// 怪点普通玩家人数
	SCENE_COL_OFFLINE_POS_VIP_NUM,			// 怪点VIP人数
};

// 创建离线玩家镜像的相关定义
// 数据类型
enum MirDataType
{	
	DATA_PROPERTY_TYPE,				// 属性数据
	DATA_CONTAINER_TYPE,			// 容器数据
	DATA_RECORD_TYPE				// 表数据
};

// 拷贝离线玩家表
enum OfflineCopyRec
{
	OCR_COL_PLAYER_UID,				// 离线玩家uid
	OCR_COL_NPC_OBJECT_ID,			// npc对象号

	MAX_OCR_COLS
};

// Sns玩家技能表
enum SnsSkillRec
{
	SSR_COL_SKILL_ID,				// 技能id
	SSR_COL_SKILL_LEVEL,			// 技能等级

	MAX_SSR_SKILL_COLS
};

#define NOT_VALID_VALUE -1 			// 无效值

#endif