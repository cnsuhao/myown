// -------------------------------------------
// 文件名称：	 CommRuleDefine.h
// 文件说明： 常用规则定义（定义一些通用并且简单的规则）
// 创建日期： 2018/03/09
// 创 建 人：  tzt
// -------------------------------------------

#ifndef __CommRuleDefine_H__
#define __CommRuleDefine_H__

#include "Fsgame/Define/header.h"
#include <vector>

#define TEAM_MEMBER_VERIFY_DISTANCE 50.0f	// 组队任务队伍成员间最大距离
#define VALID_DAMAGE_REC_TIME 60000		// 伤害有效时间
#define LIMIT_KILL_REC_TIME 900000		// 有效击杀限制时间

// 进入/切出 分组
enum GroupChangeType
{
	GROUP_CHANGE_TYPE_ENTRY,
	GROUP_CHANGE_TYPE_QUIT
};

// 消耗数据结构
struct Consume
{
	std::string id;
	int num;
	Consume() : num(0){}
};
typedef std::vector<Consume> Consume_Vec;

// 整型，字符列表
typedef std::vector<int> Integer_Vec;
typedef std::vector<std::string> String_Vec;

#endif

