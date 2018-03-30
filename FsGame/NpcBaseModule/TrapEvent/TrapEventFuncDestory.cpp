#include "FsGame/NpcBaseModule/TrapEvent/TrapEventManagerModule.h"
#include "FsGame/NpcBaseModule/TrapEvent/TrapEventFuncDestory.h"

#include "FsGame/Define/BufferDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/CommandDefine.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "utils/extend_func.h"

#include "FsGame/Interface/FightInterface.h"

bool TrapEventFuncDestory::Execute(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& target, const IVarList& vEventParams)
{
	if (!pKernel->Exists(target))
	{
		return false;
	}

	//判定玩家是否存在
	IGameObj * pTargetObj = pKernel->GetGameObj(target);
	if (pTargetObj == NULL)
	{
		return false;
	}
	if (pTargetObj->GetClassType() == TYPE_NPC)
	{
		pKernel->DestroySelf(target);
	}



	
	return true;
}
