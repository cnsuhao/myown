//---------------------------------------------------------
//文件名:       MapModule.h
//内  容:       地图相关操作模块
//说  明:       
//
//创建日期:      2015年4月21日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#ifndef _FSGAME_INTER_ACTIVE_MODULE_MAP_MODULE_H_
#define _FSGAME_INTER_ACTIVE_MODULE_MAP_MODULE_H_

#include "Fsgame/Define/header.h"
#include <vector>
#include <map>

class CapitalModule;
class AsynCtrlModule;
class MapModule: public ILogicModule
{
public:
	// 初始化
	virtual bool Init(IKernel* pKernel);
	// 销毁
	virtual bool Shut(IKernel* pKernel);
private:
	// 加载配置文件
	bool LoadResource(IKernel* pKernel);

private:
	//进入场景
	static int OnPlayerEntry(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);

	// 接收客户端信息
	static int OnCustomCallBack(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);

	// 请求切换场景
	static int OnCustomSwitchScene(IKernel * pKernel, const PERSISTID & self,
		const PERSISTID & sender, const IVarList & args);
	
	// 判断场景是否相通
	// [out]int money 如果相通，则返回价格
	bool CheckSceneThrough(int curSceneId, int toSceneId, int& money);

	// 场景能否通过地图传送
	bool CanThroughMap(int sceneId);

	static void ReLoadConfig(IKernel * pKernel);

private: // 静态数据
	static MapModule* m_pMapModule;
	static AsynCtrlModule* m_pAsynCtrlModule;

private:

#ifdef _DEBUG
private: // 测试
	static int nx_map_switch_scene(void *state);
#endif
};

#endif // FSGAME_INTER_ACTIVE_MODULE_MAP_MODULE_H