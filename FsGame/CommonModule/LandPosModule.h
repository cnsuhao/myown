//----------------------------------------------------------
// 文件名:      LandPosModule.h
// 内  容:		玩家登陆点管理模块
// 说  明:		
// 创建日期:    2015年4月8日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#ifndef _LandPosModule_H_
#define _LandPosModule_H_

#include "Fsgame/Define/header.h"

class AsynCtrlModule;

class LandPosModule : public ILogicModule
{
public:
	// 初始化
	bool Init(IKernel* pKernel);

	// 关闭
	bool Shut(IKernel* pKernel);

	/*!
	 * @brief	设置玩家登陆点
	 * @param	self		玩家对象
	 * @param	nSceneId	登陆的场景id
	 * @param	fPosX,fPosZ	登陆点
	 * @return	bool
	 */
	bool SetPlayerLandPosi(IKernel* pKernel, const PERSISTID& self, int nSceneId, float fPosX, float fPosY, float fPosZ);

	/*!
	 * @brief	玩家从副本出来后,返回上次进入的位置,取消保存的登陆点
	 * @param	self		玩家对象
	 * @return	bool
	 */
	bool PlayerReturnLandPosi(IKernel* pKernel, const PERSISTID& self);
private:
	/// \brief 上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	/// \brief 下线
	static int OnPlayerStore(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);
public:
	static LandPosModule*	m_pLandPosModule;
	static AsynCtrlModule*  m_pAsynCtrlModule;
};

#endif