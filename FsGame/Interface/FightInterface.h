//------------------------------------------------------------------------------
// 文件名:      FightInterface.h
// 内  容:      战斗系统公共接口。包括技能释放，buffer功能，战斗伤害结算等。
// 说  明:
// 创建日期:    2013年2月22日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------

#ifndef __FIGHTINTERFACE_H__
#define __FIGHTINTERFACE_H__

#include "Fsgame/Define/header.h"
#include "utils/singleton.h"
//#include "FsGame/Interface/DeclareProperty.h"
#include "FsGame/Define/FightDefine.h"
#include "FsGame/Define/SkillDefine.h"
//#include "FsGame/Define/BufferDefine.h"

class FightInterface
{
public:
    #pragma region // 技能流程相关接口

    /**
    *@brief  判断对象是否学习了指定的技能
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 待检查的对象
    *@param  [in] szSkillConfigID 技能的ConfigID
    *@return bool 包含有指定的技能返回true，否则返回false
    */
    bool Contains(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);

    /**
    *@brief  查找获取对象是否学习了指定的技能
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 待检查的对象
    *@param  [in] szSkillConfigID 技能的ConfigID
    *@return PERSISTID 如果能查到指定技能，则返回对应的技能对象id
    */
    PERSISTID FindSkill(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);

	/**
	*@brief  学习技能
	*@param  [in] pKernel 引擎内核指针
	*@param  [in] self 学习技能的对象
	*@param  [in] szSkillConfigID 技能的ConfigID
	*@return PERSISTID 添加成功返回skill，否则返回PERSISTID()
	*/
	PERSISTID AddSkill(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);

    /**
    *@brief  使用技能(暂时只提供NPC使用)
    *@param  [IN] pKernel 引擎内核指针
    *@param  [IN] self 技能施放者
    *@param  [IN] szSkillConfigID 技能ID
    *@param  [in] args 参数列表：可以是包含一个技能作用对象，可以是一个技能作用目标点，也可以是空列表
    *@return int 返回技能施放状态，返回USESKILL_RESULT_SUCCEED表示施放成功，否则返回对应错误号
    */
    int UseSkill(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID, const IVarList& args);

    /**
    *@brief  使用技能，向指定点施放一个技能。（要求该施放的技能配置符合向指定点施放的要求）
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 技能施放者
    *@param  [in] skill 施放的技能对象
    *@param  [in] args 参数列表：可以是包含一个技能作用对象，可以是一个技能作用目标点，也可以是空列表
    *@return int 返回技能施放状态，返回USESKILL_RESULT_SUCCEED表示施放成功，否则返回对应错误号
    */
    int UseSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill, const IVarList& args);

	/**
    *@brief  使用技能(暂时只提供NPC使用)
    *@param  [IN] pKernel 引擎内核指针
    *@param  [IN] self 技能施放者
	*@param  [OUT] fSkillMinDis 技能最小距离
	*@param  [OUT] fSkillMaxDis 技能最大距离
	*@param  [IN] strSkillId 优先要使用的技能
    *@return bool 寻找是否成功
    */
    bool ReadyUseRandomSkill(IKernel* pKernel, const PERSISTID& self, float& fSkillMinDis, float& fSkillMaxDis, const char* strSkillId = "");

    /**
    *@brief  打断技能
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 技能施放者
    *@param  [in] breaker 技能打断者
    *@param  [in] bForce 是否强制打断
    *@param  [in] iBreakReason 打断原因
    *@return bool 成功打断返回true，否则返回false
    */
    bool BreakSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& breaker, bool bForce = false, int iBreakReason=SKILL_BREAK_REASON_TYPE_NONE);

	/**
	*@brief  强制一个技能开始冷却
	*@param  [in] pKernel 引擎内核指针
	*@param	  [in] self 技能释放者
	*@param	  [in]	skill 要求强制冷却的技能对象
	*@return bool 成功冷却返回true,否则返回false
	*/
	bool ForceBeginCoolDown(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);

    /**
    *@brief  判断是否在正在使用技能
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 技能施放者
    *@return bool 正在施放技能返回true，否则返回false
    */
    int IsSkillUseBusy(IKernel* pKernel, const PERSISTID& self);

    /**
    *@brief  指定技能是否正在冷却
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 待检查的对象
    *@param  [in] skill 技能对象
    *@return bool 技能正在冷却中返回true，否则返回false
    */
    bool IsCoolDown(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);
    bool IsCoolDown(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);

	/*!
	* @brief	计算本次技能产生的仇恨值
	* @param	[in] pKernel 引擎内核指针
	* @param	[in] skill 技能
	* @param	[in] nDamValue 伤害值
	* @return	int 返回增加的仇恨值
	*/
	int ComputeHateValue(IKernel* pKernel, const PERSISTID& skill, int nDamValue);

	/*!
	 * @brief	通过技能id,判定是否为玩家的普通技能 
	 * @param	[in] pKernel 引擎内核指针
	 * @param	[in] self 施法者
	 * @param	[in] strSkillId 技能id
	 * @return	int 返回值见USOperationState
	 */
	//bool IsPlayerNormalAttackSkill(IKernel* pKernel, const PERSISTID& self, const char* strSkillId);

	/*!
	 * @brief	将所有技能等级设置为最低级 
	 * @param	[in] pKernel 引擎内核指针
	 * @param	[in] self 施法者
	 * @return	void 
	 */
	//void SetSkillLevelMinLevel(IKernel* pKernel, const PERSISTID& self);

	/*!
	 * @brief	将所有技能等级设置为最低级 
	 * @param	[in] pKernel 引擎内核指针
	 * @param	[in] self 施法者
	 * @param	[in] strSkill 技能名字
	 * @return	int 剩余的CD毫秒数 -1 获取失败 
	 */
	//int GetSkillLeftCDSeconds(IKernel* pKernel, const PERSISTID& self, const char* strSkill);

	/*!
	 * @brief	获取技能的总CD 
	 * @param	[in] pKernel 引擎内核指针
	 * @param	[in] self 施法者
	 * @param	[in] strSkill 技能名字
	 * @return	int 总CD毫秒数 -1 获取失败 
	 */
	int QuerySkillCD(IKernel* pKernel, const PERSISTID& self, const char* strSkill);
#ifndef FSROOMLOGIC_EXPORTS
	/*!
	* @brief	是否在轻功中
	* @param	[in] pKernel 引擎内核指针
	* @param	[in] self 施法者
	* @return	bool 
	*/
//	bool IsInFlySkill(IKernel* pKernel, const PERSISTID& self);
#endif
    #pragma endregion // 技能流程相关接口
public:
    #pragma region // buffer相关接口

    /**
    *@brief  获取对象的所有buffer
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 待查找的对象
    *@param  [out] bufferIDList 返回获取的bufferID列表
    *@return bool 返回true，获取到buff
    */
    bool GetAllBuffer(IKernel* pKernel, const PERSISTID& self, IVarList& bufferIDList);
    bool GetAllBufferObject(IKernel* pKernel, const PERSISTID& self, IVarList& bufferObjectList);

    /**
    *@brief  查找buffer对象
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 待检查的对象
    *@param  [in] szBufferConfigID buffer的ConfigID
    *@param  [out] result 查找结果传出参数
    *@return bool 如果查找到指定buffer返回true，否则返回false
    */
    bool FindBuffer(IKernel* pKernel, const PERSISTID& self, const char* szBufferConfigID, IVarList& result);

    /**
    *@brief  添加buffer 调用前,确认无敌的目标是否能够加buff,不能加的要做好保护
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 被添加buffer的对象
    *@param  [in] sender buffer的添加者
    *@param  [in] szBufferConfigID 待添加的buffer config id
    *@return bool 添加成功true，否则返回false
    */
    bool AddBuffer(IKernel* pKernel, const PERSISTID& self,
        const PERSISTID& sender, const char* szBufferConfigID);

    /**
    *@brief  添加buffer 调用前,确认无敌的目标是否能够加buff,不能加的要做好保护
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 被添加buffer的对象
    *@param  [in] sender buffer的添加者
    *@param  [in] szBufferConfigID 待添加的buffer config id
    *@param  [in] BUFF时间
    *@return bool 添加成功true，否则返回false
    */
    bool AddBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender,
                   const char* szBufferConfigID, const int iLifeTime);

	/**
	*@brief  删除buffer
	*@param  [in] pKernel 引擎内核指针
	*@param  [in] self 待检查的对象
	*@param  [in] szBufferID 删除的BUff的配置ID
	*@return bool 删除成功返回true，否则返回false
	*/
	bool RemoveBuffer(IKernel* pKernel, const PERSISTID& self, const char* szBufferID);

    /**
    *@brief  清除角色身上的所有BUFFER
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 待检查的对象
    *@return
    */
    void ClearAllBuffer(IKernel* pKernel, const PERSISTID& self);

	/**
    *@brief  添加控制类buffer
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 被添加buffer的对象
    *@param  [in] sender buffer的添加者
    *@param  [in] bufftypelist 见ControlBuff 例如: bufftypelist << CB_CANT_MOVE << CB_CANT_ATTACK;
	*@param  [in] iLifeTime buff持续时间 CONTROL_BUFF_FOREVER为永久
    *@return bool 添加成功true，否则返回false
    */
    bool AddControlBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& bufftypelist, const int iLifeTime);

	/**
    *@brief  移除控制类buffer
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 被添加buffer的对象
    *@param  [in] bufftypelist 见ControlBuff 例如: bufftypelist << CB_CANT_MOVE << CB_CANT_ATTACK;
    *@return bool 添加成功true，否则返回false
    */
	bool RemoveControlBuffer(IKernel* pKernel, const PERSISTID& self, const IVarList& bufftypelist, int buffTimeType = CONTROL_BUFF_FOREVER);

	/**
    *@brief  根据移除类型移除buff
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self		对象
    *@param  [in] nClearType (对应BufferClear)
    *@return bool 添加成功true，否则返回false
    */
    bool RemoveBuffByClearType(IKernel* pKernel, const PERSISTID& self, int nClearType);

    #pragma endregion // buffer相关接口

public:
    #pragma region // 战斗伤害结算相关接口

    /**
    *@brief  直接伤害目标
    *@param  [in] pKernel 引擎内核指针
    *@param  [in] self 攻击者
    *@param  [in] target 攻击目标
    *@param  [in] object 技能对象或buffer对象，对于无法确定的，可以是空
    *@param  [in] uuid 技能对象或buffer对象相应的uuid，对于无法确定的object，可以是空
    *@param  [in] bCanTriggerEvent 是否能触发事件
    *@param  [in] iDamageValue 造成的伤害值
    *@return int 返回真实伤害值
    */
    int Damage(IKernel* pKernel, const PERSISTID& self, const PERSISTID& target,
               const PERSISTID& object, const int64_t uuid, bool bCanTriggerEvent,
               int iDamageValue);

    #pragma endregion // 战斗伤害结算相关接口
public:
	#pragma region
	/*!
	 * @brief	更新宠物或人物的技能数据及效果
	 * @param	self 对象号(可能是宠物也可能是玩家)
	 * @param	strSkillId 技能id
	 * @param	nSkillLevel 技能等级
	 * @param	nSkillSrc  技能来源(被动技能用到)
	 * @return	bool
	 */
	bool UpdateSkillData(IKernel* pKernel, const PERSISTID &self, const char* strSkillId, int nSkillLevel, int nSkillSrc = 0);

	/*!
	 * @brief	移除宠物或人物的技能
	 * @param	self 对象号(可能是宠物也可能是玩家)
	 * @param	strSkillId 技能id
	 * @return	bool
	 */
	bool RemoveSkillData(IKernel* pKernel, const PERSISTID &self, const char* strSkillId);

	/*!
	 * @brief	通过技能名判定技能类型
	 * @param	strSkillId 技能id
	 * @return	int 技能类型对应SkillType
	 */
	int GetSkillTypeByConfig(IKernel* pKernel, const char* strSkillId);

	/*!
	* @brief	获取击杀玩家助攻列表
	* @param	outAttackerList 玩家助攻列表
	* @param	self 被击杀目标
	* @param	killer 击杀者(为空时,返回的列表中带有击杀者)
	* @return	bool 是否查询成功
	*/
//	bool GetAttackerList(IKernel* pKernel, IVarList& outAttackerList, const PERSISTID& self, const PERSISTID& killer);
	#pragma endregion
protected:
private:
	/**
	*@brief  删除技能
	*@param  [in] pKernel 引擎内核指针
	*@param  [in] self 将删除技能的对象
	*@param  [in] skill 将被删除的技能
	*@return bool 删除成功返回true，否则返回false
	*/
	bool RemoveSkill(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill);

	/**
	*@brief  删除技能
	*@param  [in] pKernel 引擎内核指针
	*@param  [in] self 将删除技能的对象
	*@param  [in] szSkillConfigID 将被删除的技能id
	*@return bool 删除成功返回true，否则返回false
	*/
	bool RemoveSkill(IKernel* pKernel, const PERSISTID& self, const char* szSkillConfigID);

}; // end of class FightInterface

typedef HPSingleton<FightInterface> FightInterfaceSingleton;

#define FightInterfaceInstance FightInterfaceSingleton::Instance()

#endif // __FIGHTINTERFACE_H__


