//--------------------------------------------------------------------
// 文件名:      ActivateFunctionModule.h
// 内  容:      功能开启
// 说  明:
// 创建日期:    2018年3月21日
// 创建人:      刘明飞 
//    :       
//--------------------------------------------------------------------

#ifndef _ACTIVATE_FUNCTION_MODULE_H
#define _ACTIVATE_FUNCTION_MODULE_H

#include "Fsgame/Define/header.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include<map>
#include<set>
#include<vector>

//功能开启方式
enum ACTIVATETYPE
{
	ACTIVATE_FUNCTION_BY_LEVEL = 0,			// 根据等级开启
    ACTIVATE_FUNCTION_BY_TASK,				// 根据任务开启
    ACTIVATE_FUNCTION_MAX,
};

// 功能类型
enum AFMFunction
{
	NONE_FUNCTION_TYPE = 0,
	AFM_WING_FUNCTION,		// 翅膀功能
	AFM_BAPTISE,			// 洗练功能
	AFM_FORGING,			// 锻造功能
	AFM_SMELT,				// 熔炼功能
	AFM_DAILY_TASK,			// 日常任务
	AFM_JADE_FUNCTION,		// 玉珏功能
	AFM_RIDE,				// 坐骑功能
};

class ActivateFunctionModule:public ILogicModule
{
public:

  virtual bool Init(IKernel *pKernel);
  virtual bool Shut(IKernel* pKernel) ;

  //查看功能是否激活
  static bool CheckActivateFunction(IKernel *pKernel, const PERSISTID &self, const int& type);
private:
	// 功能开启数据
	struct ActivateData
	{
		int nActivateType;		// 开启类型(ACTIVATETYPE)
		int nValue;				// 开启参数
		int	nFunctionType;		// 功能类型
	};

	typedef std::vector<ActivateData> ActivateDataVec;
	typedef std::vector<int> FuncVec;
	typedef std::vector<int>::iterator FuncVecIter;

	// 配置读取
	bool LoadRes(IKernel *pKernel);
	static void ReloadRes(IKernel*pKernel);
	//任务激活功能
	static int ActivateFunctionByTask(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	//等级激活功能
	static int ActivateFunctionByLevel(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 激活功能
	void ActiveFunction(IKernel*pKernel, const PERSISTID&self, int nFuncType);

	//激活所有功能
	bool ActivateAllFunction(IKernel* pKernel, const PERSISTID& self);

	//清除所有功能
	bool CloseAllFunction(IKernel* pKernel, const PERSISTID& self);

	// 查询功能类型
	bool QueryFunctionType(std::vector<int>& outAcFunc, int nType, int nValue);

public:
	static ActivateFunctionModule* m_pInstance;

private:
	ActivateDataVec m_vecActivateData;		// 功能开启数据
 
};









#endif
