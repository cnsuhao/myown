//--------------------------------------------------------------------
// 文件名:      DoorNpcModule.h
// 内  容:		传送门NPC模块
// 说  明:		
// 创建日期:    2014年12月6日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------

#include "DoorNpcModule.h"
#include "utils/util_func.h"
#include "FsGame/CommonModule/LuaScriptModule.h"
#include "utils/XmlFile.h"
#include "FsGame/CommonModule/AsynCtrlModule.h"
#include "utils/exptree.h"
#include "../Define/SnsDefine.h"
#include "../Define/CommandDefine.h"
#include "CommonModule/LandPosModule.h"
#include "Define/Fields.h"

AsynCtrlModule * DoorNpcModule::m_pAsynCtrlModule = NULL;
// 初始化
bool DoorNpcModule::Init(IKernel* pKernel)
{
	m_pAsynCtrlModule = (AsynCtrlModule*)(pKernel->GetLogicModule("AsynCtrlModule"));
	Assert( m_pAsynCtrlModule != NULL );

	pKernel->AddEventCallback("DoorNpc","OnSpring", OnSpring);
	pKernel->AddEventCallback("DoorNpc", "OnEndSpring", OnEndSpring);

	return true;
}

// 释放
bool DoorNpcModule::Shut(IKernel* pKernel)
{
	return true;
}

// 载入回调
int DoorNpcModule::OnSpring(IKernel* pKernel, const PERSISTID& self, 
						  const PERSISTID& sender, const IVarList& args)
{
	//查看对应的数据
	if ( !pKernel->Exists(self) || !pKernel->Exists(sender) )
	{
		return 0;
	}

	// 传送门对象
	IGameObj * doorObj = pKernel->GetGameObj(self);
	if ( doorObj == NULL )
	{
		return 0;
	}

	// 玩家对象
	IGameObj *pPlayer = pKernel->GetGameObj(sender);
	if ( pPlayer == NULL )
	{
		return 0;
	}
	if (pPlayer->GetClassType() != TYPE_PLAYER)
	{
		return 0;
	}


	// 离线玩家不传送 [4/8/2015 liumf]
	if (pPlayer->QueryInt("Online") == OFFLINE)
	{
		return 0;
	}

	//如果传送门有国家 就判断传送门传送目标国家跟当前传送门国家是否相同
	//int doorNation = doorObj->QueryInt(FIELD_PROP_NATION);
// 	if (doorNation == NATION_JIANGHU || doorNation == NATION_COURT)
// 	{
// 		int sendNation = pPlayer->QueryInt(FIELD_PROP_NATION);
// 		if (sendNation != doorNation)
// 		{
// 			return 0;
// 		}
// 	}



	// 触发播放任务CG，暂时关闭传送门 [2015.07.30 tongzt]
// 	if (QuestModule::m_pQuestModule->TriggerPlayCG(pKernel, sender))
// 	{
// 		// 记录传送门Npc
// 		if(!pPlayer->FindData("QuestDoorNPC"))
// 		{
// 			pPlayer->AddDataObject("QuestDoorNPC", self);
// 		}
// 		else
// 		{
// 			pPlayer->SetDataObject("QuestDoorNPC", self);
// 		}
// 		return 0;
// 	}

	//玩家自动寻路中
	if (pPlayer->FindData("AutoWalk"))
	{
		int nAutoWalk = pPlayer->QueryDataInt("AutoWalk");
		if (nAutoWalk == 1)
		{
			//自动寻路状态 和 onSpring 是异步触发的 保存临时传送门对象 
			if(!pPlayer->FindData("InDoorNPC"))
			{
				pPlayer->AddDataObject("InDoorNPC", self);
			}
			else
			{
				pPlayer->SetDataObject("InDoorNPC", self);
			}
			return 0;
		}
		else
		{	//移除临时变量
			pPlayer->RemoveData("InDoorNPC");
			pPlayer->RemoveData("AutoWalk");
		}
	}

	//获取对应场景编号和坐标
	const char * iTranslateSceneID = doorObj->QueryString("TranslateSceneID");
	len_t fTranslatePosX = doorObj->QueryFloat("TranslatePosX");
	len_t fTranslatePosY = doorObj->QueryFloat("TranslatePosY");
	len_t fTranslatePosZ = doorObj->QueryFloat("TranslatePosZ");
	len_t fOrient		 = doorObj->QueryFloat("TranslateAy");

	//获取场景
	PERSISTID scene = pKernel->GetScene();
	//转换对应的场景号
	ExpTree exp;
	int nNewScene = static_cast<int>(exp.CalculateEvent(pKernel, sender, scene, PERSISTID(), PERSISTID(), iTranslateSceneID));

	// 能否传送
	if (!AsynCtrlModule::m_pAsynCtrlModule->CanSwitchLocate(pKernel, pPlayer->QueryWideStr("Name"), nNewScene))
	{
		return 0;
	}

	// 宝库场景判断
// 	bool bInTrueasuryScene = false;
// 	if (!FrontierTreasuryModule::TestEntryTreasury(pKernel, pPlayer, nNewScene, bInTrueasuryScene))
// 	{
// 		if (bInTrueasuryScene)
// 		{
// 			return 0;
// 		}
// 	}
	int nSceneId_ = pKernel->GetSceneId();
// 	if (bInTrueasuryScene)
// 	{
// 		// 记录返回点
// 		float x_ = 0, y_ = 0, z_ = 0, o_ = 0;
// 		pKernel->GetSceneBorn(x_, y_, z_, o_);
// 		y_ = pKernel->GetMapHeight(x_, z_);
// 		LandPosModule::m_pLandPosModule->SetPlayerLandPosi(pKernel, sender, nSceneId_, x_, y_, z_);
// 	}

	if ( nNewScene == nSceneId_ )
	{
		pKernel->MoveTo( sender, fTranslatePosX, fTranslatePosY, fTranslatePosZ, fOrient);
	}
	else
	{
		m_pAsynCtrlModule->SwitchLocate( pKernel, sender, nNewScene, fTranslatePosX, 
			fTranslatePosY, fTranslatePosZ, fOrient );
	}
	return 0;
}

int DoorNpcModule::OnEndSpring(IKernel* pKernel, const PERSISTID& self, 
				const PERSISTID& sender, const IVarList& args)
{

	IGameObj *pPlayer = pKernel->GetGameObj(sender);
	if (NULL == pPlayer)
	{
		return 0;
	}

	// 清除玩家身上临时对象
	if(pPlayer->FindData("InDoorNPC"))
	{
		pPlayer->RemoveData("InDoorNPC");
	}

	return 0;
}