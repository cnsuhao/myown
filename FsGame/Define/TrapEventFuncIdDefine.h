//------------------------------------------------------------------------------
// 文件名:      TrapEventFuncIdDefine.h
// 内  容:      陷阱触发事件回调函数ID定义
// 说  明:
// 创建日期:    2015年07月13日
// 创建人:        
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __TRAPEVENTFUNCIDDEFINE_H__
#define __TRAPEVENTFUNCIDDEFINE_H__

enum SpringEvent
{
	SPRINGEVENT_INVALID = 0,

// 	//单人伤害
// 	SPRINGEVENT_SINGLE_DAMAGE = 1,  
// 
// 	//群体伤害
// 	SPRINGEVENT_MUTI_DAMAGE = 2,  

	//单人BUFF
	SPRINGEVENT_SINGLE_BUFF,  

	//群体BUFF
	SPRINGEVENT_MUTI_BUFF,

// 	//删除目标BUFF
// 	SPRINGEVENT_DELETE_TARGET_BUFF,

	//清空表并删除表中目标的buff
//	SPRINGEVENT_CLEAR_REC_TARGET_BUFF = 6,
	//销毁目标
	SPRINGEVENT_DESTORY_OBJ,

	SPRINGEVENT_MAX,
};

//触发过滤结果
enum SPRING_FILTER_RESULT
{
	SPRING_FILTER_RESULT_NULL = 0,            //空值
	SPRING_FILTER_RESULT_SUCCESS,             //成功
	SPRING_FILTER_RESULT_IGNORE,              //忽略，不处理
};

//国家过滤
enum SPRING_FILTER_NATION
{
	SPRING_FILTER_NATION_ALL            =  10,  //所有敌对
    SPRING_FILTER_NATION_UNLESS_OWN     =  99,  //除本国敌对
    SPRING_FILTER_NATION_UNLESS_ALLY    =  98,  //除盟国敌对
};

//触发对象过滤
enum SPRING_FILTER_TYPE
{
	SPRING_FILTER_TYPE_ALL              =  0,  //所有类型
	SPRING_FILTER_TYPE_ONLY_SCENE       =  1,  //场景
	SPRING_FILTER_TYPE_ONLY_PLAYER      =  2,  //玩家
	SPRING_FILTER_TYPE_ONLY_NPC         =  4,  //NPC
	SPRING_FILTER_TYPE_ONLY_GOODS       =  8,  //物品
};

//触发销毁规则
enum SPRING_DESTROY
{
    SPRING_DESTROY_TIME_OUT             = 0,   //销毁时间销毁
	SPRING_DESTROY_IMMEDIATELY          = 1,   //触发就销毁
    SPRING_DESTROY_EFFECT_TIME_OUT      = 2,   //特效时间后销毁
};


#endif // __TRAPEVENTFUNCIDDEFINE_H__
