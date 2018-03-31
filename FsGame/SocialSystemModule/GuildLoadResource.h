#ifndef __GuildLoadResource_H__
#define __GuildLoadResource_H__

#include "../Define/GuildDefine.h"

class IKernel;

// 加载公会升级配置信息
bool LoadGuildUpLevelConfig(const char *resourcePath, GuildUpLevelConfigVector &configVec);

// 加载公会捐献配置信息
bool LoadGuildDonateConfig(const char *resourcePath, GuildDonateConfigVector &configVec);

// 加载公会职务配置信息
bool LoadGuildPositionConfig(const char *resourcePath, GuildPositionConfigVector &configVec);

// 加载公会商店配置信息
bool LoadGuildShopConfig(const char *resourcePath, GuildShopConfigVector &configVec);

// 加载公会商店重置定时表
bool LoadGuildTimer(const char *resourcePath, GuildTimerCallbackVector &configVec);

//加载公会技能
bool LoadGuildSkill(const char*resourcePath, GuildSkillConfigMap&configVec);
// 加载技能消耗配置
bool LoadGuildSkillLevelUpConst(const char*resourcePath, GuildSkillLevelUpSpendMap&configVec);
//技能升级经验
bool LoadGuildSkillLvUpExp(const char*resourcePath, GuildSkillLvExp&configVec);

#endif // __GuildLoadResource_H__