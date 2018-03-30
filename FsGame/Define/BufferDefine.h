#ifndef _BufferDefine_H_
#define _BufferDefine_H_

#define BUFFER_HEART_INTERVAL 500

// Buffer阶段定义
enum BUFFER_STAGE
{
    BUFFER_STAGE_NONE = 0,
    BUFFER_STAGE_BEGIN,         // 开始使用技能：使用者，目标类型
    BUFFER_STAGE_EFFECT,        // 造成效果：类型
    BUFFER_STAGE_BREAK,         // 打断：原因
    BUFFER_STAGE_FINISH,        // 结束：原因
};

// Buffer清除原因
enum BUFFER_REMOVE
{
    BUFFER_REMOVE_NONE = 0,
    BUFFER_REMOVE_BREAK,         // 打断
    BUFFER_REMOVE_REPLACE,       // 替换
    BUFFER_REMOVE_TIMEEND,       // 结束
    BUFFER_REMOVE_CLEAR,         //清除
    BUFFER_REMOVE_OFFLINE,       //下线清除
    BUFFER_REMOVE_DEAD,          //死亡清除
    BUFFER_REMOVE_TIMEOVER,      //次数达到
};

//BUFFER的效果类型
enum BUFFER_EFFECT_TYPE
{	
    BUFFER_EFFECTTYPE_0, //0:瞬时BUFFER, 瞬时buff不再使用
    BUFFER_EFFECTTYPE_1, //1:在X秒内有效
    BUFFER_EFFECTTYPE_2, //2:在X秒内Y次事件有效
	BUFFER_EFFECTTYPE_3, //3:在X秒内有效,每Y秒发生一次
    BUFFER_EFFECTTYPE_4, //4:在有限时间内有效,所扣属性扣到0时结束，每Y秒触发一次
	BUFFER_EFFECTTYPE_5, //5:永久类buff,内部逻辑调用增加或删除buff 例如:军衔

	BUFFER_EFFECTTYPE_MAX, //Buffer作用类型的最大值
};

//BUFFER的计算类型
enum
{
    BUFFER_TIMETYPE_ONLINE, //只计算在线时间
    BUFFER_TIMETYPE_ALLTIME,    //计算真实时间（包括下线时间）
	BUFFER_TIMETYPE_MAX,		//最大计时类型值
};

//Buffer替换的方式(替换级别和替换分类号有关)
enum EBufferReplaceType
{
	BUFFER_REPLACE_NO,//空
	BUFFER_REPLACE_DIRECT, //直接按分类号替换
	BUFFER_REPLACE_REFRESH_TIME,//刷新时间
	BUFFER_REPLACE_NOT_REPLACE//不替换
};

//Buffer事件的目标类型
enum EBufferTargetType
{
    BUFFERTARGE_INTO, //传入对象
    BUFFERTARGE_SELECT, //选中对象
    BUFFERTARGE_SELF,  //自己
    BUFFERTARGE_SPRINGER, //事件的触发者
    BUFFERTARGE_SENDER, //Buffer的施加者
    BUFFERTARGE_TARGET //攻击的目标（攻击事件可用）
};

//buffer事件类型
enum EBufferEventType
{
    BUFFEVENT_UNKNOW = 0,
    BUFFEVENT_ACTIVE = 1,               // 1. 启动buff
    BUFFEVENT_DEACTIVE = 2,             // 2. 关闭buff
    BUFFEVENT_TIMER = 3,                // 3. 记时心跳timer
    BUFFEVENT_BEDAMAGE = 4,             // 4. 被攻击
    BUFFEVENT_BEFORE_BEDAMAGE = 5,      // 5. 目标受到伤害之前的事件
	BUFFEVENT_BEFORE_HIT = 6,			// 6. 技能命中目标前
	BUFFEVENT_GAINT_DEAD = 7,			// 7. 免疫死亡
	BUFFEVENT_BE_VA = 8,				// 8. 被暴击
	BUFFEVENT_DAMAGE = 9,				// 9. 伤害目标

    BUFFEVENT_MAX
};

//buffer事件对自身影响
enum EBufferAffectType
{
	BUFFAFFECT_BENEFICIAL = 0,			// 0 增益BUFF
	BUFFAFFECT_DAMGAE	= 1,			// 1 伤害BUFF
	BUFFAFFECT_HARM		= 2,			// 2 减益BUFF
	BUFFAFFECT_CONTROL	= 3,			// 3 控制BUFF

	BUFFAFFECT_MAX
};

// buff清除类型
enum BufferClear
{
	BUFFER_SWITCH_CLEAR,	    		// 切场景清除
	BUFFER_OFFLINE_CLEAR,       		// 下线清除
	BUFFER_DEAD_CLEAR,          		// 死亡清除
};

// buff来源类型
enum BufferSrc
{
	BUFFER_NORMAL_SRC,					// 正常的
	BUFFER_OFFICIAL_SRC					// 军阶buff
};

// debuff类型
enum DeBuffType
{
	NONE_DEBUFF,						// 不是debuff
	DEBUFF_NORMAL,						// 一般的debuff,不需要处理衰减
	DEBUFF_SILENT,						// 沉默类的debuff
	DEBUFF_VERTIGO,						// 眩晕类的debuff
	DEBUFF_CANT_MOVE,					// 禁足类的debuff
	DEBUFF_DEC_SPEED,					// 减速类的debuff
	DEBUFF_DEC_DEFEND,					// 减防类的debuff

	MAX_DEBUFF_TYPE_NUM
};

#define BUFF_ADD_STATICS_REC "buff_add_rec"

// buff统计表
enum BuffAddRecCols
{
	BASR_BUFF_ADD_TIME_COL,
	BASR_BUFF_TYPE_COL
};

#endif

