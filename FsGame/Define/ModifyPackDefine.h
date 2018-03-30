//----------------------------------------------------------
// 文件名:      ModifyPackDefine.h
// 内  容:      修正包管理模块公共头文件
// 说  明:
// 创建人:      hehui
// 创建日期:    2012年4月5日
//    :       
//----------------------------------------------------------
#ifndef _FSGAME_MODIFY_PACK_DEFINE_H_
#define _FSGAME_MODIFY_PACK_DEFINE_H_

#include <string>
#include <map>

// 对自身属性修正表
const char* const CONST_PROP_MODIFY_REC     = "PropModifyRec";			 // buff或临时属性修正表(不需要计算战力)

// 修正角色属性数据表结构
enum PropModifyRec
{
	PMPS_PROP_NAME = 0,
	PMPS_PROP_VALUE,
	PMPS_PROP_SOURCE,
};

// 对装备属性修正表
const char* const CONST_PROP_MODIFY_BYEQUIP_REC = "PropModifyByEquip";	 // 属性修正表(被动技能、经脉、装备等)

// 新装备属性修正表结构
enum EnumPropModifyByEquipRec
{
    EPME_PROP_NAME,
    EPME_PROP_VALUE,
    EPME_FROM,
};

// 对被动技能自身属性修正表
// const char* const PSKILL_PROP_MODIFY_REC     = "PropModifyByPSkill";
// 
// // 被动技能修正角色属性数据表结构
// enum PropModifyByPSkillRec
// {
// 	PMPS_PROP_NAME = 0,
// 	PMPS_PROP_VALUE,
// 	PMPS_PROP_SOURCE,
// };

// 新装备特殊属性修正表
enum EnumSpecialPropModifyByEquipRec
{
	EPME_SPECIAL_PROP_NAME,
	EPME_SPECIAL_PROP_VALUE,
	EPME_SPECIAL_FROM,
};

enum EModifyPackComputeType//nComputeType的类型定义
{
    EModify_ADD_VALUE	= 0,	//增加值
    EModify_ADD_RATE	= 1,	//增加百分比
};

enum ERefreshDataType
{
    EREFRESH_DATA_TYPE_ADD,
    EREFRESH_DATA_TYPE_REMOVE,
};

typedef std::map<std::string, double> MapPropValue;

//有颜色的属性包ID等于基础属性包ID加颜色等级乘以这个数
const int ITEM_BASE_PACKAGE_COEFFICIENT = 1000; 

#endif // _FSGAME_MODIFY_PACK_DEFINE_H_
