//--------------------------------------------------------------------
// 文件名:		AIOnCreateArgs.cpp
// 内  容:		AI模块对带参数创建NPC的集中处理
// 说  明:		
// 创建日期:	2015年03月16日
// 创建人:		  
//    :	   
//--------------------------------------------------------------------
#include "AISystem.h"
#include "FsGame/Define/SkillEventFuncIdDefine.h"

// 带参数创建怪物
int AISystem::OnCreateArgs(IKernel* pKernel, const PERSISTID& self,
						   const PERSISTID& sender, const IVarList& args)
{
	// 获得对象指针
	IGameObj* pSelf = pKernel->GetGameObj(self);

	if (NULL == pSelf)
	{
		return 0;
	}

	// 得到类型
	int type = args.IntVal(0);
	switch(type)
	{
	case CREATE_TYPE_BATTLELEAD_NPC:
		{
			//如果追击范围少于触发范围，追击范围就等于触发范围 + 1.
			if(pSelf->FindAttr("ChaseRange"))
			{
				if(pSelf->QueryFloat("ChaseRange") <= pSelf->QueryFloat("SpringRange"))
				{
					pSelf->SetFloat("ChaseRange", (pSelf->QueryFloat("SpringRange") + 10));
				}
			}

			int iMoveType = args.IntVal(1);
			PERSISTID opt_obj = args.ObjectVal(2);
			int iIndex = args.IntVal(3);
			int iCamp = args.IntVal(4);
			int nLifeTime = args.IntVal(5);
		
			//设置策略移动值
			if (!pSelf->FindData("MoveType"))
			{
				pSelf->AddDataInt("MoveType", 0);
			}
			pSelf->SetDataInt("MoveType", iMoveType);

			pSelf->SetInt("LifeTime", nLifeTime);

			//设置其玩家对象
			if (!pSelf->FindData("BattleOptObj"))
			{
				pSelf->AddDataObject("BattleOptObj", PERSISTID());
			}
			pSelf->SetDataObject("BattleOptObj", opt_obj);

			//设置隐身不可被攻击和不可攻击
			pSelf->SetInt("Invisible",1);
			pSelf->SetInt("CantBeAttack",1);
			pSelf->SetInt("CantAttack",1);

			const char* BattleTeamRec = ("BattleTeamCampRec" + util_int_as_string(iCamp)).c_str();
			IGameObj* pOptObj = pKernel->GetGameObj(opt_obj);
			if (pOptObj != NULL)
			{
				IRecord* pRecord = pOptObj->GetRecord(BattleTeamRec);
				if (pRecord != NULL)
				{
					//玩家信息表增加
					CVarList arg_list;
					arg_list << iIndex + 1 << self << AI_BATTLETEAM_STATUS_BORN << iMoveType << 0.0f << 0.0f << 0.0f << 0.0f;
					pRecord->SetRowValue(iIndex, arg_list);
				}
			}
		}
		break;
	case CREATE_TYPE_BATTLE_NPC:
		{
			//如果追击范围少于触发范围，追击范围就等于触发范围 + 1.
			if(pSelf->FindAttr("ChaseRange"))
			{
				if(pSelf->QueryFloat("ChaseRange") <= pSelf->QueryFloat("SpringRange"))
				{
					pSelf->SetFloat("ChaseRange", (pSelf->QueryFloat("SpringRange") + 10));
				}
			}

			int iMoveType = args.IntVal(1);
			PERSISTID LeadNpc = args.ObjectVal(2);
			int iIndex = args.IntVal(3);
			int iGroupId = args.IntVal(4);
			//PERSISTID Player = args.ObjectVal(5);

            //演武国战队员创建默认隐藏，拷贝完玩家数据后会自动显示
            pSelf->SetInt("Invisible",1);

			pSelf->SetInt("GroupID", iGroupId);


			//设置策略移动值
			if (!pSelf->FindData("MoveType"))
			{
				pSelf->AddDataInt("MoveType", 0);
			}
			pSelf->SetDataInt("MoveType", iMoveType);

			//设置其队长
			if (!pSelf->FindData("LeadNpc"))
			{
				pSelf->AddDataObject("LeadNpc", PERSISTID());
			}
			pSelf->SetDataObject("LeadNpc", LeadNpc);

			//设置序列号
			if (!pSelf->FindData("BattleSerial"))
			{
				pSelf->AddDataInt("BattleSerial", 0);
			}
			pSelf->SetDataInt("BattleSerial", iIndex);

			//队长增加其他玩家成员信息
			IGameObj* pLeadNpcObj = pKernel->GetGameObj(LeadNpc);
			if (pLeadNpcObj != NULL)
			{
				IRecord* pRecord = pLeadNpcObj->GetRecord("BattleChildNpcRec");
				if (pRecord != NULL)
				{
					CVarList arg_list;
					arg_list << self << AI_BATTLEPLAYER_STATUS_BORN;
					pRecord->AddRowValue(-1, arg_list);
				}
			}
		}
		break;

	case CREATE_TYPE_PROPERTY_VALUE://带属性值创建对象 Key-Value值对方式（只支持替换）
		{
			size_t iCount = args.GetCount();
            LoopBeginCheck(a);
			for (size_t i = 1; i < iCount; i+=2)
			{
                LoopDoCheck(a);
				const char* pPropName = args.StringVal(i);
				// 查询是否存在属性
				if (!pSelf->FindAttr(pPropName))
				{
					continue;
				}

				int PorpType = pSelf->GetAttrType(pPropName);
				if (VTYPE_INT == PorpType)
				{
					int iNewValue = args.IntVal(i + 1);
					pSelf->SetInt(pPropName, iNewValue);
				}
				else if (VTYPE_FLOAT == PorpType)
				{
					float fNewValue = args.FloatVal(i + 1);
					pSelf->SetFloat(pPropName, fNewValue);
				}
				else if (VTYPE_DOUBLE == PorpType)
				{
					double dNewValue = args.DoubleVal(i + 1);
					pSelf->SetDouble(pPropName, dNewValue);
				}
				else if (VTYPE_INT64 == PorpType)
				{
					int64_t iNewValue = args.Int64Val(i + 1);
					pSelf->SetInt64(pPropName, iNewValue);
				}
				else if (VTYPE_STRING == PorpType)
				{
					const char* pNewValue = args.StringVal(i + 1);
					pSelf->SetString(pPropName, pNewValue);
				}
				else if (VTYPE_WIDESTR == PorpType)
				{
					const wchar_t* pNewValue = args.WideStrVal(i + 1);
					pSelf->SetWideStr(pPropName, pNewValue);
				}
				else if (VTYPE_OBJECT == PorpType)
				{
					PERSISTID obj = args.ObjectVal(i + 1);
					pSelf->SetObject(pPropName, obj);
				}
				else
				{
					continue;
				}
			}
		}

		break;
	case CREATE_TYPE_SKILL_NPC://技能NPC
		{
			// 设置
			pSelf->SetInt("Type", TYPE_NPC);

			PERSISTID owner = args.ObjectVal(1);
			if (!pKernel->Exists(owner))
			{
				return 0;
			}

			// 设置施法者
			pSelf->SetObject("Master", owner);
			int iGroupID = args.IntVal(2);
			pSelf->SetInt("GroupID", iGroupID);

		}
		break;
	default:
		break;
	}

	return 0;
}
