//--------------------------------------------------------------------
// 文件名:      Formula.h
// 内  容:      公式表达式类
// 说  明:      将策划配置的公式解析成一个Formula。Formula中用一个后缀表达式
//              表示一个公式串，公式中的属性以属性索引的方式保存。在获取公式值
//              的时候，通过所以在属性数值中以0（1）的速度找到属性名并获取属性值，
//              避免每次计算重复解析字符串，后缀表达式可以快速地访问数值节点依次
//              计算进而提升效率。
// 创建日期:    2015年7月28日
// 创建人:       
//    :       
//--------------------------------------------------------------------


#ifndef __FORMULA_H__
#define __FORMULA_H__

#include <string>
#include <vector>
#include "FsGame/SkillModule/Impl/FormulaStack.h"
#include "server/IGameObj.h"
#include "public/Macros.h"

class Formula;
typedef  Formula* (*RecursionFormulaFunc)(int formula_index);

class Formula
{
    enum ENodeType
    {
        NODE_TYPE_VARIANT,
        NODE_TYPE_DIGIT,
        NODE_TYPE_OP,
		NODE_TYPE_SPECIAL
    };
    enum EOperatorType
    {
        OPERATOR_TYPE_UNKNOWN,
        OPERATOR_TYPE_ADD,				// +
        OPERATOR_TYPE_SUB,				// -
        OPERATOR_TYPE_MUL,				// *
        OPERATOR_TYPE_DIV,				// /
		OPERATOR_TYPE_INT,				// % float取整
		OPERATOR_TYPE_MAX,				// > 取最大值
		OPERATOR_TYPE_MIN,				// < 取最小值
		OPERATOR_TYPE_RANDOM,			// | 取随机值
    };
    enum EObjectType
    {
        OBJECT_TYPE_SELF,			// 施法者或者是一个公式
        OBJECT_TYPE_TARGET,			// 目标
		OBJECT_TYPE_SELF_SKILL		// 施法者技能
    };

public:
    Formula();
    ~Formula();

    bool Parse(const char* str, const char* szMinValue, const char* szMaxValue);
    void Clear();
    bool GetValue(IGameObj* pSelfObj, IGameObj* pTargetObj, IGameObj* pSelfSkill, float& value, RecursionFormulaFunc fun);

	// 清除缓存值
	void ClearCacheVal();
private:
    int GetPriority(const std::string& opt);
    bool IsOperator(const std::string& opt);
    bool IsOperator(char opt);
    bool IsVariant(const std::string& token);
    bool IsVariant(char token);
    bool IsDigit(const std::string& token);
    bool IsDigit(char token);

    // 对中缀表达式进行预处理，分离出每个token
    bool PreParse(const char *str, std::vector<std::string>& out_token);

     // 转变为后缀形式
    bool ToPostfix(const char* str, std::vector<std::string>& suffix);

    EOperatorType ToOperatorIndex(const std::string& token);
    bool Calculate(FormulaStack<float>& statck, EOperatorType op_type);
    bool GetVariantValue(IGameObj* pSelfObj, 
						IGameObj* pTargetObj, 
						IGameObj* pSelfSkill,
                        int obj_type, int prop_index, 
                        float& value, RecursionFormulaFunc fun);

    // 把value设置到取值范围内
    // 约定-1是一个特殊值，用于表示是否考虑上限或者下限
    float AdjustFightData(float value);

private:
// 	typedef struct _ExpNode
// 	{
// 		int type;
// 		union
// 		{
// 			char op;
// 			float fValue;
// 			char chValue[MAX_NODE_EXP];
// 		};
// 		char from;
// 		int sign;
// 		struct _ExpNode* left;
// 		struct _ExpNode* right;
// 	} ExpNode;

    struct TokenNode
    {
        ENodeType node_type;
        union
        {
            // 操作符节点
            EOperatorType op_type;

            // 属性节点
            struct
            {
                short obj_type;         // 属性对象类型
                short prop_index;       // 属性索引
            };

            // 数值节点
            float digit_value;
        };

        TokenNode()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    enum {MAX_VECTOR_LEN = 100};
    TokenNode m_postfix[MAX_VECTOR_LEN];
    int m_len;

    // 公式的取值范围
    float m_min_value;
    float m_max_value;

    std::string m_formula;	// 保存公式配置文本，便于分析
	float		m_fCacheVal;		// 缓存值
	bool		m_bCache;			// 是否缓存
};

#endif // __FORMULA_H__
