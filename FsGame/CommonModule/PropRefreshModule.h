//------------------------------------------------------------------------------
// 文件名:      PropRefreshModule.h
// 内  容:
// 说  明:
// 创建日期:    2012年12月28日
// 创建人:       
// 修改人:		  
// 备注:
//    :       
//------------------------------------------------------------------------------


#ifndef __PROPREFRESHMODULE_H__
#define __PROPREFRESHMODULE_H__

#include <vector>
#include <string>
#include <map>

#include "Fsgame/Define/header.h"
#include "FsGame/Define/ModifyPackDefine.h"
#include "Define/PlayerBaseDefine.h"
enum {MAX_PARAM_NUM = 8};
// 公式类型
enum FormulaType
{
	FORMULA_TYPE_PARAM = 1, // 参数格式的公式
	FORMULA_TYPE_FLAG = 2,  // 符号前缀的公式
};
// 属性公式结构
struct PropFormula
{
	int iFormulaType;								 // 公式类型
	std::string strRelationFormula[PLAYER_JOB_MAX];  // 属性数值公式

	int srcs[MAX_PARAM_NUM + 1];
	std::string props[MAX_PARAM_NUM + 1];
    PropFormula():iFormulaType(0)
    {
        memset(srcs, 0, sizeof(int) * (MAX_PARAM_NUM + 1));
    }
};
class PropRefreshModule : public ILogicModule
{
public:

    virtual bool Init(IKernel* pKernel);
    virtual bool Shut(IKernel* pKernel);

	
public:
	// 回调函数 玩家加载数据完成
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& sender, const IVarList& args);

	bool LoadResource(IKernel* pKernel);

	// 更新玩家属性  nClassType 属性包类型  strFormId 属性包在表中的id nNewPackageId 更新的属性包id 
	static bool UpdateModifyProp(IKernel* pKernel, const PERSISTID& self, int nClassType, const char* strFormId, int nNewPackageId);

    // 使用刷新数据刷新对象
    bool RefreshData(IKernel* pKernel, const PERSISTID& obj, const MapPropValue& mapPropValue, ERefreshDataType eRefreshType);

	// 玩家刷新数据，不过滤职业不相关属性
	bool PlayerRefreshData(IKernel* pKernel, const PERSISTID& obj, const MapPropValue& mapPropValue, ERefreshDataType eRefreshType);

    // 统计刷新数据
    bool CountRefreshData(IKernel* pKernel, const PERSISTID& obj,
        const char* szPropName, double fValue, int iComputeType, MapPropValue& mapPropValue);

	// 获得不重复的属性
	bool GetFinalProp(IKernel* pKernel, const PERSISTID& self, const PERSISTID& equip,
		const IVarList& vPropNameList, const IVarList& vPropValueList, MapPropValue& mapFinalPropValue);

    // 获取最终由装备影响的属性信息
    bool GetFinalEquipModifyObjProp(IKernel* pKernel, const IVarList& vPropNameList,
        const IVarList& vPropValueList, MapPropValue& mapFinalPropValue) const;

    // 添加/删除附加属性
	bool CalFormulaForProp(IKernel* pKernel,const PERSISTID& self);
	void PropRefreshModule::BindPlayerRefreshProp(IKernel* pKernel, const PERSISTID& self);
private:
	// 初始化玩家等级成长属性
    bool InitPlayerLevelProp(IKernel* pKernel, const PERSISTID& self);
    // 初始化玩家基础战斗力
  //  bool InitPlayerBaseBA(IKernel* pKernel, const PERSISTID& self);
	// 解析公式，计算数值
	void AnalyzeFormula(IKernel* pKernel,const PERSISTID& self,const char* szPropName,const PropFormula& propFormula);
	// 获得对象当前适用的属性关系公式
	const char* GetRelationFormula(IKernel* pKernel, const PERSISTID& self, const PropFormula& propFormula) const;

    // 从公式中解析各个属性名，获取影响公式结果的属性
    bool GetRelationPropList(IKernel* pKernel, const char * propFormula, IVarList& res) const;

    // 加载属性关系数据
    bool LoadPropRelationData(IKernel* pKernel);

    std::string ModifyFormulaFormat(IKernel* pKernel, const PERSISTID& self,
        const MapPropValue& mapSelfData, const std::string& strFormula) const;

    // 检测控制类属性
    bool CheckControlProp(IKernel* pKernel, const char* szPropName);
	static int PropRefreshModule::C_OnBindPropChanged(IKernel* pKernel, const PERSISTID& self,const char* property, const IVar& old);

	// 等级变化回调
	static int OnDCLevelChange(IKernel* pKernel, const PERSISTID &self, const PERSISTID & sender, const IVarList & args);

	// 获取玩家等级属性包id nLevel为0 取玩家当前等级
	int GetLevelPropId(IKernel* pKernel, IGameObj* pSelfObj, int nLevel = 0);

	// 响应人物的属性增加处理
	bool OnAddPlayerProperty(IKernel* pKernel, const PERSISTID& self, const char* strFormId, const IVarList& proNameList, const IVarList& proValueList);

	// 响应人物属性移除
	bool OnRemovePlayerProperty(IKernel* pKernel, const PERSISTID& self, const char* strFormId);

	// 获取属性包
	void GetPropPackage(int nClassType, int nPackageid, IVarList& outFinalNameList, IVarList& outFinalValueList);
public:
	static void ReloadConfig(IKernel* pKernel);
public:
    // 本模块指针
    static PropRefreshModule*   m_pInstance;

private:
	
	
    // 属性描述结构
    struct PropDescStruct
    {
        short iDataType;                                // 属性数据类型
        PropFormula sPropFormual;
        std::vector<std::string> vecEffectProp;     // 会影响那些属性
        PropDescStruct():iDataType(0)
        {
			vecEffectProp.clear();
        }
    };

    std::map<std::string, PropDescStruct> m_PropRelationMap;  // 属性关系表

	std::vector<std::string> m_PropRelationIndex;

	//std::map<std::string, float>	m_mapBattleAbility;	// 战斗力计算
}; // end of class PropRefreshModule


#endif // __PROPREFRESHMODULE_H__

