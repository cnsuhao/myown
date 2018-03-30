#if !defined __RequestDefine_h__
#define __RequestDefine_h__

//请求阶段与应该阶段
typedef enum _RequestState
{
    REQUEST_STATE_REQ,
    REQUEST_STATE_ANS
} RequestState;

#define REQUEST_REC "request_rec"

enum Request_Rec
{
    REQUEST_REC_INDEX,      //请求索引
    REQUEST_REC_NAME,      //被请求者姓名
    REQUEST_REC_TYPE,        //请求类型
    REQUEST_REC_TIME,        //请求时间
	REQUEST_REC_STATE,      //请求状态(请求参数)
    REQUEST_REC_JOB,        //职业
    REQUEST_REC_BATTLE_ABILITY, //战斗力
	REQUEST_REC_LEVEL,		//等级
	REQUEST_REC_SEX,		// 性别
};

typedef enum
{
    //无类型
	 REQUESTTYPE_NONE = 0,

    //申请请加入队伍
    REQUESTTYPE_JOIN_SINGLEPLAYER = 1,
    
    //邀请对方加入队伍
    REQUESTTYPE_INVITE_SINGLEPLAYER = 2,

    // 总数
    REQUESTTYPE_COUNT ,

} REQUESTTYPE;

#endif