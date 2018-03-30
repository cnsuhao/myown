//------------------------------------------------------------------------------
// 文件名:      SkillLuaExt.cpp
// 内  容:
// 说  明:
// 创建日期:    2014年10月27日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#include "FsGame/SkillModule/SkillModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/SkillModule/SkillToSprite.h"
#include "FsGame/SkillModule/Impl/SkillDataQueryModule.h"
#include "../../Interface/FightInterface.h"
#include "../../SystemFunctionModule/StaticDataQueryModule.h"
#include "../../Define/StaticDataDefine.h"
// #include "../inner/SkillEventFuncSkill.h"
// #include "../BufferDecayLogic.h"
#include "CommonModule/EnvirValueModule.h"

// 原型：int nx_add_skill(object self, object skill)
// 功能：添加技能
int nx_add_skill(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_add_skill, 2);
    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_add_skill, 1);
    CHECK_ARG_OBJECT(state, nx_add_skill, 2);

    // 获取参数
    PERSISTID self = pKernel->LuaToObject(state, 1);
    PERSISTID skill = pKernel->LuaToObject(state, 2);

    pKernel->LuaPushBool(state, SKillToSpriteSingleton::Instance()->AddSkill(pKernel, self, skill));
    return 1;
}

// 原型：int nx_remove_skill(object self, object skill)
// 功能：添加技能
int nx_remove_skill(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_remove_skill, 2);
    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_remove_skill, 1);
    CHECK_ARG_OBJECT(state, nx_remove_skill, 2);

    // 获取参数
    PERSISTID self = pKernel->LuaToObject(state, 1);
    PERSISTID skill = pKernel->LuaToObject(state, 2);

    pKernel->LuaPushBool(state, SKillToSpriteSingleton::Instance()->RemoveSkill(pKernel, self, skill));
    return 1;
}


// 原型：int nx_clear_all_skill(object self)
// 功能：清空所学技能
int nx_clear_all_skill(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_clear_all_skill, 1);
    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_clear_all_skill, 1);

    // 获取参数
    PERSISTID self = pKernel->LuaToObject(state, 1);

    pKernel->LuaPushBool(state, SKillToSpriteSingleton::Instance()->ClearAllSkill(pKernel, self));
    return 1;
}


// 原型：object nx_find_skill(object self, int skillid)
// 功能：寻找技能
int nx_find_skill(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_find_skill, 2);
    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_find_skill, 1);
    CHECK_ARG_STRING(state, nx_find_skill, 2);

    // 获取参数
    PERSISTID self = pKernel->LuaToObject(state, 1);
    const char* skillid = pKernel->LuaToString(state, 2);

    pKernel->LuaPushObject(state, SKillToSpriteSingleton::Instance()->FindSkill(pKernel, self, skillid));
    return 1;
}

// 原型：object nx_find_skill(object self, int skillid)
// 功能：复活
int nx_relive(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	if (pKernel == NULL)
	{
		return 1;
	}

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_relive, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_relive, 1);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);

	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}

	//float fRunSpeed = EnvirValueModule::EnvirQueryFloat(ENV_VALUE_DEFAULT_RUN_SPEED);
	pSelf->SetInt64("HP", pSelf->QueryInt64("MaxHP"));
	pSelf->SetInt("Dead", 0);
	//pSelf->SetFloat("RunSpeed",fRunSpeed);
	pSelf->SetInt("CantUseSkill", 0);

	pKernel->LuaPushBool(state, true);
	return 1;
}

// 测试无锁定目标的技能
int nx_use_skill(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_use_skill, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_use_skill, 1);
	CHECK_ARG_OBJECT(state, nx_use_skill, 2);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);
	PERSISTID skill = pKernel->LuaToObject(state, 2);

	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return 0;
	}
	
	FightInterfaceInstance->UseSkill(pKernel, self, skill, CVarList());

	pKernel->LuaPushBool(state, true);
	return 1;
}

int nx_load_skill_resource(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);


    if (SkillModule::m_pInstance != NULL)
    {
        // 检查参数数量
        CHECK_ARG_NUM(state, nx_load_skill_resource, 1);
        // 检查参数类型
        CHECK_ARG_OBJECT(state, nx_load_skill_resource, 1);

        // 获取参数
        PERSISTID self = pKernel->LuaToObject(state, 1);
        // 加载技能
        if (!SkillModule::m_pInstance->LoadAllResource(pKernel))
        {
            pKernel->LuaPushBool(state, false);
        }

		// buff衰减的配置
//		BufferDecayLogicSingleton::Instance()->LoadConfig(pKernel);

        bool berefresh = SkillModule::m_pInstance->RefreshAllSkillConfigProperty(pKernel, self);

        pKernel->LuaPushBool(state, berefresh);
    }
    else
    {
        pKernel->LuaPushBool(state, false);
    }

    return 1;
}

// 测试随机释放技能
int nx_use_random_skill(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_use_skill, 2);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_use_skill, 1);
	CHECK_ARG_OBJECT(state, nx_use_skill, 2);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);
	PERSISTID target = pKernel->LuaToObject(state, 2);

	//FightInterfaceInstance->ReadyUseRandomSkill(pKernel, self, target);

	pKernel->LuaPushBool(state, true);
	return 1;
}

void declare_skill_lua_ext(IKernel* pKernel)
{
    DECL_LUA_EXT(nx_add_skill);
    DECL_LUA_EXT(nx_remove_skill);
    DECL_LUA_EXT(nx_clear_all_skill);
    DECL_LUA_EXT(nx_find_skill);
	DECL_LUA_EXT(nx_use_skill);
	DECL_LUA_EXT(nx_use_random_skill);

    DECL_LUA_EXT(nx_load_skill_resource);
	DECL_LUA_EXT(nx_relive);
}
