
//--------------------------------------------------------------------
// 文件名:      Server\FsGame\Define\skilleventmanagerdefine.h
// 内  容:      
// 说  明:      
// 创建日期:    2014年4月25日
// 创建人:      刘明飞
// 版权所有:    苏州蜗牛电子有限公司
//--------------------------------------------------------------------



#ifndef __SKILLEVENTMANAGERDEFINE_H__
#define __SKILLEVENTMANAGERDEFINE_H__


// 事件类型定义定义
enum ESkillEventType
{
    UNKNOW_TYPE = 0,

    // 技能检测可以释放
    SKILL_BEGIN = 1,
    // 技能开始准备的事件
    SKILL_PREPARE = 2,
    // 技能准备之后的事件
    SKILL_AFTER_PREPARE = 3,
    // 技能命中之前的事件
    SKILL_BEFORE_HIT = 4,
    // 技能命中并已伤害了对象的事件
    SKILL_HIT_DAMAGE = 5,
    // 技能使用结束的事件
    SKILL_FINISH = 6,
    // 技能被打断的事件
    SKILL_BREAK = 7,

    // 技能命中目标后，扣血之前触发的事件
    SKILL_AFTER_HIT = 8,

    MAX_EVENT_TYPE = 30,
};


// 使用对象类型定义
enum ETargetType
{
    // 传入对象
    TARGET_SEND = 0,
    // 选择对象
    TARGET_SELECT_OBJ = 1,
    // 自身对象
    TARGET_SELF_OBJ = 2,
    // 触发对象
    TARGET_SPRING_OBJ = 3,
    //
    TARGET_END = 4,
};

enum EParamsType
{
    PARAM_EVENT_SKILL, //技能事件参数
    PARAM_EVENT_BUFF,  //buff事件参数
};


enum EParamType
{
    EPARAMTYPE_SKILL, // 技能事件的技能对象
    EPARAMTYPE_TARGET, // 目标对象
    EPARAMTYPE_TARGET_XYZ, //目标点
    EPARAMTYPE_TARGET_HURT, // 伤害对象
    EPARAMTYPE_BULLET,  // 子弹对象
};

#endif // __SKILLEVENTMANAGERDEFINE_H__

