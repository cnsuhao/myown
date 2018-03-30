
//--------------------------------------------------------------------
// 文件名:      FormulaManagerModule.h
// 内  容:      
// 说  明:      
// 创建日期:    2015年7月29日
// 创建人:       
//    :       
//--------------------------------------------------------------------


#ifndef __FORMULA_MANAGER_H__
#define __FORMULA_MANAGER_H__

#include "Fsgame/Define/header.h"
#include "utils/xmlfile.h"
#include "utils/StringPod.h"
#include "FormulaPropIndex.h"
#include "FormulaPropIndexDefine.h"
#include <vector>

class Formula;

class FormulaManagerModule : public ILogicModule
{
public:

    bool Init(IKernel* pKernel);
    bool Shut(IKernel* pKernel);

    // 获取属性关系表中配置的静态公式关系的值
    bool GetValue(IGameObj* pSelfObj, IGameObj* pTargetObj, IGameObj* pSelfSkill, int formula_index, float& value);

    // 获取即时解析的公式值
    bool GetImmediateFormualValue(IGameObj* pSelfObj, IGameObj* pTargetObj, const char* str, float& value);

	// 读取配置
	bool LoadResource(IKernel* pKernel);
private:
	// 传给Formula做递归计算用
    static Formula* GetRecursionFormula(int formula_index); 

	// 获取公式
    Formula* GetFormula(int formula_index);

	// 公式配置解析
    bool ParseFormula(const CXmlFile& xml);

	// 重置计算过程中的中间值缓存
	void ResetCacheVal();

	// 公式数据删除
	bool Release();
private:
    static const char* const CONST_PROP_RELATION_FORMULA_XML;
    static const char* const CONST_PROP_DAMAGER_FORMULA_XML;

	typedef std::vector<Formula*> FormulaVec;

	FormulaVec m_vecFormulas;

    // 保存即时解析的公式，没当解析一个公式时，先查找该容器是否已经解析过
   // TStringPod<char, Formula*>  m_ImmediateFormulaMap;

public:
    static FormulaManagerModule* m_instance;
};

#endif // __FORMULA_MANAGER_H__
