
#ifndef _CAU_H__
#define _CAU_H__

#include <string>
#ifdef FSROOMLOGIC_EXPORTS
#include "server/IRoomKernel.h"
#else
#include "server/IKernel.h"
#endif

namespace Cau
{
//参数类型定义
enum PARAM_TYPE
{
PARAM_TYPE_SELF = 0,            //释放者
PARAM_TYPE_SKILL = 1,           //释放技能
PARAM_TYPE_TARGET = 2,          //目标对象
PARAM_TYPE_TARGET_SKILL = 3,    //目标当前技能对象
PARAM_TYPE_RANDOM = 4,          //随机数
PARAM_TYPE_INPUT = 5,           //使用传入参数
PARAM_TYPE_INPUT_RANDOM = 6,    //传入随机数
PARAM_TYPE_EQUIP = 7,           //装备属性
};


//公式的最大参数数量
const int MAX_INPUT_PARAM = 8;
//函数声明
//计算不带括号的简单公式
float GetSimpleFormulaValue(const std::string& szFormula, const float(&param)[MAX_INPUT_PARAM]);

//计算带括号的复杂公式
float GetFormulaValue(const std::string& szFormula, const float(&param)[MAX_INPUT_PARAM]);

//取得对象的属性值
float QueryPropValue(IKernel* pKernel, const PERSISTID& obj, const char* szPropName);
// 只获取指定属性的值
float QueryValueByPropName(IKernel* pKernel, const PERSISTID& obj, const char* szPropName);

// 只获取指定属性的值
float QueryValueByPropName(IGameObj* pGameObj, const char* szPropName);

//取得传入参数的值
const char* GetInputParamValue(int (&src)[MAX_INPUT_PARAM], size_t nParamIndex, const IVarList& InputParam);

//取得字符串表示的随机数的值
float GetRandomValue(const char* param);

// 按比获取修正值
double ModifyRateValue(IKernel* pKernel, const PERSISTID& obj, double dRate, const char* szPropName);

} // end of namespace Cau


#endif // _CAU_H__
