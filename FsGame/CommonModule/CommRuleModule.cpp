// -------------------------------------------
// 文件名称：	 CommRuleModule.cpp
// 文件说明： 常用规则处理（定义一些通用并且简单的规则）
// 创建日期： 2018/03/02
// 创 建 人：  tzt
// -------------------------------------------

#include "CommRuleModule.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include <vector>
#include "utils/util_func.h"
#include "utils/string_util.h"
#include "ContainerModule.h"
#include "FunctionEventModule.h"
#include "SystemFunctionModule/CapitalModule.h"
#include "EquipmentModule/EquipDefine.h"
#include "Define/PlayerBaseDefine.h"
#include "Define/ItemTypeDefine.h"

CommRuleModule *CommRuleModule::m_pThis = NULL;


// 初始化
bool CommRuleModule::Init(IKernel *pKernel)
{
	m_pThis = this;
	// npc被击杀
	pKernel->AddIntCommandHook(CLASS_NAME_NPC, COMMAND_BEKILL, CommRuleModule::OnNpcBeKilled);
	// 玩家被击杀
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_BEKILL, CommRuleModule::OnPlayerBeKilled);
	// 玩家被伤害
	pKernel->AddIntCommandHook(CLASS_NAME_PLAYER, COMMAND_BEDAMAGE, CommRuleModule::OnPlayerBeDamaged);

	// CG播放结束消息
	pKernel->AddIntCustomHook(CLASS_NAME_PLAYER, CLIENT_CUSTOMMSG_PLAY_CG_DONE, CommRuleModule::OnCustomCGDone);

	return true;
}

// 关闭
bool CommRuleModule::Shut(IKernel *pKernel)
{
	return true;
}

// 应用杀怪规则
void CommRuleModule::ApplyKillNpcRule(IKernel *pKernel, const PERSISTID &npc)
{
	if (NULL == m_pThis)
	{
		return;
	}
	// 杀怪队伍共享规则
	m_pThis->KillNpc_TeamShare(pKernel, npc);
}

// 应用杀人规则
void CommRuleModule::ApplyKillPlayerRule(IKernel *pKernel, const PERSISTID &deader)
{
	if (NULL == m_pThis)
	{
		return;
	}

	// 有效击杀规则
	m_pThis->KillPlayer_Validate(pKernel, deader);
}

// 记录攻击玩家
void CommRuleModule::RecordAttackPlayer(IKernel *pKernel, const PERSISTID &sufferer, 
	const PERSISTID &damager)
{
	// 受害者
	IGameObj *pSufferer = pKernel->GetGameObj(sufferer);
	if (NULL == pSufferer)
	{
		return;
	}

	// 攻击者，宠物的话，找到它的主人
	PERSISTID true_damager = get_pet_master(pKernel, damager);
	IGameObj* pDamager = pKernel->GetGameObj(true_damager);
	if (NULL == pDamager)
	{
		return;
	}

	// 不记录非玩家
	if (pDamager->GetClassType() != TYPE_PLAYER)
	{
		return;
	}

	// 有效伤害表
	IRecord *pDamageRec = pSufferer->GetRecord(FIELD_RECORD_VALID_DAMAGE_REC);
	if (NULL == pDamageRec)
	{
		return;
	}

	// 清理
	const int64_t now_time = util_get_time_64();
	LoopBeginCheck(a);
	for (int row = pDamageRec->GetRows() - 1; row >= 0; --row)
	{
		LoopDoCheck(a);
		const int64_t time = pDamageRec->QueryInt64(row, COLUMN_VALID_DAMAGE_REC_TIME);
		const int64_t diff_time = now_time - time;
		// 超过有效时间
		if (diff_time > VALID_DAMAGE_REC_TIME 
			|| diff_time < 0)
		{
			pDamageRec->RemoveRow(row);
		}
	}

	// 记录伤害
	const char *uid = pDamager->QueryString(FIELD_PROP_UID);
	const int exist_row = pDamageRec->FindString(COLUMN_VALID_DAMAGE_REC_UID, uid);
	if (exist_row < 0)
	{
		CVarList row_value;
		row_value << uid
				  << now_time;
		pDamageRec->AddRowValue(-1, row_value);
	}
	else
	{
		pDamageRec->SetInt64(exist_row, COLUMN_VALID_DAMAGE_REC_TIME, now_time);
	}
}

// 玩家改变分组
bool CommRuleModule::ChangeGroup(IKernel* pKernel, const PERSISTID& self, 
	const int change_type, const int new_group/* = 0*/)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj 
		|| TYPE_PLAYER != pSelfObj->GetClassType())
	{
		return false;
	}

	// 进入分组
	if (change_type == GROUP_CHANGE_TYPE_ENTRY)
	{
		// 非法的分组号
		if (new_group <= 0)
		{
			return false;
		}

		pSelfObj->SetInt(FIELD_PROP_BEFORE_GROUP, pSelfObj->QueryInt(FIELD_PROP_GROUP_ID));
		pSelfObj->SetInt(FIELD_PROP_GROUP_ID, new_group);

		// 宠物进入分组
		// TODO
	}
	else// 离开分组
	{		
		// 进入前分组
		int before_group = pSelfObj->QueryInt(FIELD_PROP_BEFORE_GROUP);

		// 回到指定分组
		if (new_group != 0)
		{
			before_group = new_group;
		}
		
		before_group = before_group == 0 ? -1 : before_group;
		pSelfObj->SetInt(FIELD_PROP_GROUP_ID, before_group);
		pSelfObj->SetInt(FIELD_PROP_BEFORE_GROUP, 0);

		// 宠物离开分组
		// TODO
	}

	// 通知客户端切分组表现
	pKernel->Custom(self, CVarList() << SERVER_CUSTOMMSG_SWITCH_GROUP_EFFECT << change_type);
	pKernel->Command(self, self, CVarList() << COMMAND_GROUP_CHANGE << change_type);

	return true;
}

// 播放CG
void CommRuleModule::PlayCG(IKernel* pKernel, const PERSISTID& self, 
	const char *cg_id, const IVarList &args/* = CVarList()*/)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	CVarList s2c_msg;
	s2c_msg << SERVER_CUSTOMMSG_PLYA_CG
			<< cg_id;
	s2c_msg.Concat(args);
	pKernel->Custom(self, s2c_msg);
}

// 解析为消耗列表
void CommRuleModule::ParseConsumeVec(const char *src_str, Consume_Vec& consume_vec, 
	const char *del_first /*= ","*/, const char *del_second /*= ":"*/)
{
	if (StringUtil::CharIsNull(src_str))
	{
		return;
	}

	consume_vec.clear();
	CVarList result_first;
	util_split_string(result_first, src_str, del_first);

	LoopBeginCheck(a);
	for (int i = 0; i < (int)result_first.GetCount(); ++i)
	{
		LoopDoCheck(a);
		CVarList result_second;
		util_split_string(result_second, result_first.StringVal(i), del_second);
		if (result_second.GetCount() < 2)
		{
			continue;;
		}

		Consume cfg;
		cfg.id = result_second.StringVal(0);
		cfg.num = result_second.IntVal(1);

		consume_vec.push_back(cfg);
	}
}

// 可否消耗物品
bool CommRuleModule::CanDecItems(IKernel *pKernel, const PERSISTID &self, 
	const Consume_Vec &items)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 取得背包
	PERSISTID item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(item_box))
	{
		return false;
	}

	// 所需物品是否满足
	bool can_dec = true;
	LoopBeginCheck(a);
	for (int i = 0; i < (int)items.size(); ++i)
	{
		LoopDoCheck(a);
		const char *item_id = items[i].id.c_str();
		const int count = items[i].num;
		if (!StringUtil::CharIsNull(item_id)
			&& count > 0
			&& !ContainerModule::m_pContainerModule->TryRemoveItems(pKernel, item_box, item_id, count))
		{
			can_dec = false;
			break;
		}
	}

	return can_dec;
}

// 扣除物品
bool CommRuleModule::ConsumeItems(IKernel *pKernel, const PERSISTID &self, 
	const Consume_Vec &items, const int function_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 取得背包
	PERSISTID item_box = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
	if (!pKernel->Exists(item_box))
	{
		return false;
	}

	// 扣除物品
	bool remove_done = true;
	LoopBeginCheck(a);
	for (int i = 0; i < (int)items.size(); ++i)
	{
		LoopDoCheck(a);
		const char *item_id = items[i].id.c_str();
		const int count = items[i].num;
		if (StringUtil::CharIsNull(item_id) || count == 0)
		{
			continue;
		}

		// 扣除
		int remove_count = 0;
		if (FunctionEventModule::GetItemBindPriorUse(function_id))
		{
			remove_count = ContainerModule::RemoveItemsBindPrior(pKernel, item_box,
				item_id, count, EmFunctionEventId(function_id), true);
		}
		else
		{
			remove_count = ContainerModule::m_pContainerModule->RemoveItems(pKernel, item_box,
				item_id, count, EmFunctionEventId(function_id), true);
		}

		if (remove_count != count)
		{
			remove_done = false;
			break;
		}
	}

	return remove_done;
}

// 可否消耗资金
bool CommRuleModule::CanDecCapitals(IKernel *pKernel, const PERSISTID &self,
	const Consume_Vec &capitals)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 所需资金是否满足
	bool can_dec = true;
	LoopBeginCheck(a);
	for (int i = 0; i < (int)capitals.size(); ++i)
	{
		LoopDoCheck(a);
		const int capital_type = StringUtil::StringAsInt(capitals[i].id);
		const int count = capitals[i].num;

		if (!CapitalModule::m_pCapitalModule->CanDecCapital(pKernel, self, capital_type, count))
		{
			can_dec = false;
			break;
		}
	}

	return can_dec;
}

// 扣除资金
bool CommRuleModule::ConsumeCapitals(IKernel *pKernel, const PERSISTID &self, 
	const Consume_Vec &capitals, const int function_id)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 所需资金是否满足
	bool dec_done = true;
	LoopBeginCheck(a);
	for (int i = 0; i < (int)capitals.size(); ++i)
	{
		LoopDoCheck(a);
		const int capital_type = StringUtil::StringAsInt(capitals[i].id);
		const int count = capitals[i].num;

		if (CapitalModule::m_pCapitalModule->DecCapital(pKernel, self, capital_type, count, EmFunctionEventId(function_id)) != DC_SUCCESS)
		{
			dec_done = false;
			break;
		}
	}

	return dec_done;
}

// 解析坐标数据
bool CommRuleModule::ParsePosInfo(PosInfo& outPos, const char* strPos, const char *delims)
{
	CVarList pos;
	StringUtil::SplitString(pos, strPos, delims);
	if (pos.GetCount() != 3)
	{
		return false;
	}
	outPos.fPosX = pos.FloatVal(0);
	outPos.fPosZ = pos.FloatVal(1);
	outPos.fOrient = pos.FloatVal(2);

	return true;
}

// 是否合法装备部位
bool CommRuleModule::IsValidEquipPos(const int equip_pos)
{
	return equip_pos >= EQUIP_POS_STR_MIN 
		&& equip_pos <= EQUIP_POS_STR_MAX;
}

// 是否合法职业
bool CommRuleModule::IsValidJob(const int job)
{
	return job > PLAYER_JOB_MIN 
		&& job < PLAYER_JOB_MAX;
}

// 是否合法品质
bool CommRuleModule::IsValidColor(const int color)
{
	return color > COLOR_MIN 
		&& color < COLOR_MAX;
}

// NPC被杀时回调
int CommRuleModule::OnNpcBeKilled(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	CommRuleModule::m_pThis->ApplyKillNpcRule(pKernel, self);

	return 0;
}

// 玩家被击杀回调
int CommRuleModule::OnPlayerBeKilled(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	CommRuleModule::m_pThis->ApplyKillPlayerRule(pKernel, self);

	return 0;
}

// 玩家被伤害回调
int CommRuleModule::OnPlayerBeDamaged(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& sender, const IVarList& args)
{
	CommRuleModule::m_pThis->RecordAttackPlayer(pKernel, self, sender);

	return 0;
}

// CG播放结束
int CommRuleModule::OnCustomCGDone(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID &sender, const IVarList &args)
{
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	// 通知cg播放结束
	pKernel->Command(self, self, CVarList() << COMMAND_PLAY_CG_DONE << args.StringVal(1));

	return 0;
}


// 队伍共享击杀
void CommRuleModule::KillNpc_TeamShare(IKernel *pKernel, const PERSISTID &npc)
{
	IGameObj *pNpc = pKernel->GetGameObj(npc);
	if (NULL == pNpc)
	{
		return;
	}

	// 攻击列表
	IRecord *pAttackRec = pNpc->GetRecord(FIELD_RECORD_ATTACKER_LIST);
	if (pAttackRec == NULL)
	{
		return;
	}

	// 队伍列表
	std::vector<int> team_list;

	// 享受击杀玩家列表
	CVarList player_list;

	// npc的分组号
	const int npc_group_id = pNpc->QueryInt(FIELD_PROP_GROUP_ID);

	// 攻击列表中的玩家可共享
	LoopBeginCheck(a);
	for (int row = 0; row < pAttackRec->GetRows(); ++row)
	{
		LoopDoCheck(a);
		if (pAttackRec->QueryInt(row, COLUMN_ATTACKER_LIST_0001) == 0)
		{
			continue;
		}

		PERSISTID obj = pAttackRec->QueryObject(row, COLUMN_ATTACKER_LIST_0000);
		IGameObj* pObj = pKernel->GetGameObj(obj);
		if (pObj == NULL)
		{
			continue;
		}

		if (pObj->GetClassType() != TYPE_PLAYER)
		{
			continue;
		}

		if (pObj->QueryInt(FIELD_PROP_GROUP_ID) != npc_group_id)
		{
			continue;
		}

		// 有队伍的只记录该队伍的一个成员
		const int team_id = 0/*pObj->QueryInt(FIELD_PROP_TEAM_ID)*/; // TODO
		if (team_id > 0)
		{
			std::vector<int>::const_iterator find_it = find(
				team_list.begin(),
				team_list.end(),
				team_id);

			// 第一次记录
			if (find_it == team_list.end())
			{
				team_list.push_back(team_id);
				player_list << obj;
			}
		}
		else // 无队伍直接记录
		{
			player_list << obj;
		}
	}

	// 队伍共享击杀
	CVarList s2s_msg;
	s2s_msg << COMMAND_KILL_NPC_WITH_RULE;
	LoopBeginCheck(b);
	for (int i = 0; i < (int)player_list.GetCount(); ++i)
	{
		LoopDoCheck(b);
		PERSISTID player = player_list.ObjectVal(i);
		IGameObj* pPlayer = pKernel->GetGameObj(player);
		if (pPlayer == NULL)
		{
			continue;
		}

		// 队伍成员
		CVarList member_list;
		bool have_team = false/*TeamModule::m_pTeamModule->GetTeamMemberList(pKernel, player, member_list)*/; // TODO
		if (have_team)
		{
			LoopBeginCheck(c);
			for (int j = 0; j < (int)member_list.GetCount(); ++j)
			{
				LoopDoCheck(c);
				// 队伍成员名
				const wchar_t *member_name = member_list.WideStrVal(j);

				// 本场景查找队伍成员
				const PERSISTID member = pKernel->FindPlayer(member_name);
				IGameObj *pMember = pKernel->GetGameObj(member);
				if (NULL == pMember)
				{
					continue;
				}

				// 分组号一致
				if (pMember->QueryInt(FIELD_PROP_GROUP_ID) != npc_group_id)
				{
					continue;
				}

				// 队伍成员与当前玩家的距离大于上限
				if (pKernel->Distance2D(npc, member) > TEAM_MEMBER_VERIFY_DISTANCE)
				{
					continue;
				}

				// 发送更新任务命令
				pKernel->Command(npc, member, s2s_msg);
			}
		}
		else
		{
			// 分组号不一致
			if (pPlayer->QueryInt(FIELD_PROP_GROUP_ID) != npc_group_id)
			{
				continue;
			}

			// 与死亡npc的距离大于上限
			if (pKernel->Distance2D(npc, player) > TEAM_MEMBER_VERIFY_DISTANCE)
			{
				continue;
			}

			// 发送更新任务命令
			pKernel->Command(npc, player, s2s_msg);
		}
	}

	team_list.clear();
	player_list.Clear();
}

// 有效击杀玩家验证
void CommRuleModule::KillPlayer_Validate(IKernel *pKernel, const PERSISTID &deader)
{
	// 死亡者
	IGameObj *pDeader = pKernel->GetGameObj(deader);
	if (NULL == pDeader)
	{
		return;
	}

	// 有效伤害记录表
	IRecord *pDamageRec = pDeader->GetRecord(FIELD_RECORD_VALID_DAMAGE_REC);
	if (NULL == pDamageRec)
	{
		return;
	}

	// 死者uid
	const char *deader_uid = pDeader->QueryString(FIELD_PROP_UID);

	// 击杀消息
	CVarList s2s_msg;
	s2s_msg << COMMAND_KILL_PLAYER_WITH_RULE;

	// 共享击杀
	LoopBeginCheck(a);
	for (int row = 0; row < pDamageRec->GetRows(); ++row)
	{
		LoopDoCheck(a);
		const char *uid = pDamageRec->QueryString(row, COLUMN_VALID_DAMAGE_REC_UID);
		const int64_t time = pDamageRec->QueryInt64(row, COLUMN_VALID_DAMAGE_REC_TIME);
		const int64_t diff_time = util_get_time_64() - time;
		// 超过有效时间
		if (diff_time > VALID_DAMAGE_REC_TIME
			|| diff_time < 0)
		{
			continue;
		}

		// 只通知当前场景存在玩家
		const wchar_t *name = pKernel->SeekRoleName(uid);
		PERSISTID player = pKernel->FindPlayer(name);
		IGameObj *pPlayer = pKernel->GetGameObj(player);
		if (NULL == pPlayer)
		{
			continue;
		}

		// 需要检查击杀是否有效
		if (RecordValidKill(pKernel, player, deader_uid))
		{
			pKernel->Command(player, player, s2s_msg);
		}
	}

	pDamageRec->ClearRow();
}

// 记录有效击杀
bool CommRuleModule::RecordValidKill(IKernel* pKernel, const PERSISTID& self,
	const char *deader_uid)
{
	IGameObj *pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 无uid，表示非玩家的Npc参与计数，不做验证
	if (StringUtil::CharIsNull(deader_uid))
	{
		return true;
	}

	// 有效击杀验证表
	IRecord *pKillRec = pSelfObj->GetRecord(FIELD_RECORD_VALID_KILL_REC);
	if (NULL == pKillRec)
	{
		return false;
	}

	// 清理已超过有效限制时间的记录
	const int64_t now_time = util_get_time_64();
	LoopBeginCheck(a);
	for (int row = pKillRec->GetRows() - 1; row >= 0; --row)
	{
		LoopDoCheck(a);
		const int64_t diff_time = now_time - pKillRec->QueryInt64(row, COLUMN_VALID_KILL_REC_TIME);
		if (diff_time < 0 || diff_time > LIMIT_KILL_REC_TIME)
		{
			pKillRec->RemoveRow(row);
		}
	}

	// 查找
	const int exist_row = pKillRec->FindString(COLUMN_VALID_KILL_REC_UID, deader_uid);
	// 有记录表示肯定不满足
	if (exist_row >= 0)
	{
		return false;
	}

	// 无记录，有效击杀并增加记录
	CVarList row_value;
	row_value << deader_uid
		<< now_time;
	pKillRec->AddRowValue(-1, row_value);

	return true;
}
