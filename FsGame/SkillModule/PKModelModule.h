//--------------------------------------------------------------------
// 文件名:		PKModelModule.h
// 内  容:		PK模式
// 说  明:		
//				
// 创建日期:	2017年07月24日
// 整理日期:	
// 创建人:		liumf   
//--------------------------------------------------------------------

#ifndef __PK_Model_MODULE_H__
#define __PK_Model_MODULE_H__

#include "Fsgame/Define/header.h"
#include <vector>

class PKModelModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);

	// 关闭
	virtual bool Shut(IKernel* pKernel);

	// 是否可攻击
	bool static IsBeAttacked(IKernel* pKernel, const PERSISTID &self, const PERSISTID &target);

	// 是否为友方
	bool static IsFriendly(IKernel* pKernel, const PERSISTID &self, const PERSISTID &target);
private:
	struct ScenePKModel 
	{
		ScenePKModel() : nSceneId(0),
					nDefaultModel(0)
		{

		}
		int		nSceneId;			// 场景id
		int		nDefaultModel;		// 默认类型
	};

	typedef std::vector<ScenePKModel> ScenePKModelVec;
	/// \brief 上线
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	static int OnPlayerReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应客户端消息
	static int OnCustomPKModel(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 响应PK模式切换
	void OnCustomChangePKModel(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 响应设置默认pk模式
	//void OnCustomSetDefaultModel(IKernel* pKernel, const PERSISTID& self, const IVarList& args);

	// 设置默认的pk模式
	//void ResetDefaultPKModel(IKernel* pKernel, const PERSISTID& self);

	// 重新加载配置
	static void ReloadConfig(IKernel* pKernel);
private:
	static PKModelModule* m_pPKModelModule;
	ScenePKModelVec		m_vecScenePKModel;	
};

#endif //__GAINT_MODULE_H__
