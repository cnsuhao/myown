//----------------------------------------------------------
// 文件名:      LandPosModule.h
// 内  容:		玩家登陆点管理模块
// 说  明:		
// 创建日期:    2015年4月8日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#include "LandPosModule.h"
#include "public\Inlines.h"
#include "utils\string_util.h"
#include "..\Define\SnsDefine.h"
//#include "..\Define\OffLineDefine.h"
#include "AsynCtrlModule.h"
//#include "..\Define\NationDefine.h"
#include "utils\extend_func.h"
#include "FsGame\CommonModule\ActionMutex.h"

LandPosModule*	LandPosModule::m_pLandPosModule = NULL;
AsynCtrlModule* LandPosModule::m_pAsynCtrlModule = NULL;

// 初始化
bool LandPosModule::Init(IKernel* pKernel)
{
	m_pLandPosModule = this;
	m_pAsynCtrlModule = (AsynCtrlModule*)pKernel->GetLogicModule("AsynCtrlModule");

	Assert(NULL != m_pLandPosModule && NULL != m_pAsynCtrlModule);

	pKernel->AddEventCallback("player","OnStore",LandPosModule::OnPlayerStore);
	return true;
}

// 关闭
bool LandPosModule::Shut(IKernel* pKernel)
{
	return true;
}

/*!
 * @brief	设置玩家登陆点
 * @param	self		玩家对象
 * @param	nSceneId	登陆的场景id
 * @param	fPosX,fPosZ	登陆点
 * @return	bool
 */
bool LandPosModule::SetPlayerLandPosi(IKernel* pKernel, const PERSISTID& self, int nSceneId, float fPosX, float fPosY, float fPosZ)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj || 0 == nSceneId)
	{
		return false;
	}
	// 记录下线时场景和坐标
	char positionInfo[256] = {0};
	SPRINTF_S(positionInfo, "%f,%f,%f,%d", fPosX, fPosY, fPosZ, nSceneId);
	pSelfObj->SetString("BeforeOffLine", positionInfo);

	return true;
}

/*!
 * @brief	玩家从副本出来后,返回上次进入的位置,取消保存的登陆点
 * @param	self		玩家对象
 * @return	bool
 */
bool LandPosModule::PlayerReturnLandPosi(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	//ActionMutexModule::ExitAction(pKernel, self);

	const char* strLastPos = pSelfObj->QueryString("BeforeOffLine");
	// 可能异常关服,此属性为空字符,切回皇城
	if (StringUtil::CharIsNull(strLastPos))
	{
		int nMainSceneId = 2;// GetNationMainSceneId(nNation);
		m_pAsynCtrlModule->SwitchBorn(pKernel, self, nMainSceneId, true);
		return false;
	}

	CVarList pos;
	StringUtil::SplitString(pos, strLastPos, ",");
	if (pos.GetCount() != 4)
	{
		return false;
	}
	float x = pos.FloatVal(0);
	float y = pos.FloatVal(1);
	float z = pos.FloatVal(2);
	int nSceneId = pos.IntVal(3);

	pSelfObj->SetString("BeforeOffLine", "");
	
	m_pAsynCtrlModule->SwitchLocate(pKernel, self, nSceneId, x,  y, z, pSelfObj->GetOrient(), true);

	return true;
}

/// \brief 下线
int LandPosModule::OnPlayerStore(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	int reason = args.IntVal(0);
	if (reason != STORE_EXIT)
	{
		return 0;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	const char* strLandPosi = pSelfObj->QueryString("BeforeOffLine");

	CVarList pos;
	StringUtil::SplitString(pos, strLandPosi, ",");
	if (pos.GetCount() != 4)
	{
		return 0;
	}
	float x = pos.FloatVal(0);
	float y = pos.FloatVal(1);
	float z = pos.FloatVal(2);
	int nSceneId = pos.IntVal(3);

	pKernel->SetLandPosi(self, nSceneId, x, y, z, pSelfObj->GetOrient());
	return 0;
}