
//--------------------------------------------------------------------
// 文件名:      formula.cpp
// 内  容:      
// 说  明:      
// 创建日期:    2015年4月15日
// 创建人:      longch
//    :       
//--------------------------------------------------------------------

#include "FsGame/SkillModule/Impl/Formula.h"
#include "FsGame/SkillModule/Impl/FormulaStack.h"
#include "FsGame/SkillModule/Impl/FormulaPropIndexDefine.h"
#include "FsGame/SkillModule/Impl/FormulaPropIndex.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"
#include <cassert>
#include "utils/cau.h"
#include "utils/util_func.h"
#include <math.h>

Formula::Formula()
{
    Clear();
}

Formula::~Formula()
{

}


//对中缀表达式进行预处理，分离出每个token
bool Formula::PreParse(const char *str, std::vector<std::string>& tokens)
{
    int len = (int)strlen(str);
    char *p = (char *)malloc((len+1)*sizeof(char));
    int j=0;
	LoopBeginCheck(a)
    for (int i=0; i<len; i++)          //去除表达式中的空格
    {
		LoopDoCheck(a)
        if (str[i]==' ')
        {
            continue;
        }
        p[j++] = str[i];
    }
    p[j]='\0';


    len = (int)strlen(p);
	LoopBeginCheck(b)
    for (int i=0; i<len; i++)
    {
		LoopDoCheck(b)
        char temp[2];
        std::string token;
        switch (p[i])
        {
        case '+':
        case '*':
        case '/':
        case '(':
        case ')':
		case '-':
		case '|':
		case '>':
		case '&':
            {
                temp[0] = p[i];
                temp[1] = '\0';
                token = temp;
                tokens.push_back(token);
                break;
            }
		case '%':
			{
				if (p[i-1]==')' || IsDigit(p[i-1]) || IsVariant(p[i-1])) // float取整
				{
					temp[0] = p[i];
					temp[1] = '\0';
					token = temp;
					tokens.push_back(token);
				}
				break;
			}
		default:     //是数字
            {
                int j = i;
				LoopBeginCheck(h)
                while ((IsDigit(p[j]) || IsVariant(p[j])) && j<len)
                {
					LoopDoCheck(h)
                    j++;
                }
                char *opd = (char *)malloc(j-i+1);
                strncpy(opd, p+i, j-i);
                opd[j-i]='\0';
                token=opd;
                tokens.push_back(token);
                i=j-1;
                free(opd);
                break;
            }  
        } // end of switch ...
    }
    free(p);
    return true;
}

int Formula::GetPriority(const std::string& opt)
{
    int priority = -1;
    if (opt=="%")
        priority = 4;  
	else if (opt==">" || opt=="<" || opt=="|")
		priority = 3;
    else if (opt=="*" || opt=="/")
        priority = 2;
    else if (opt=="+" || opt=="-")
        priority = 1;
    else if (opt=="(")
        priority = 0;
    return priority;
}

bool Formula::IsOperator(const std::string& token)
{
    if (token=="+" || token=="-" || token=="*" || token=="/"
		|| token == "%" || token == ">" || token == "|" || token == "<")
    {
        return true;
    }
    return false;
}

bool Formula::IsOperator(char token)
{
    if (token=='+' || token=='-' || token=='*' || token=='/'
		|| token == '%' || token == '>' || token == '|' || token == '&')
    {
        return true;
    }
    return false;
}

bool Formula::IsVariant(const std::string& token)
{
    if (token == "(" || token == ")" || IsOperator(token) || IsDigit(token))
    {
        return false;
    }
    return true;
}

bool Formula::IsVariant(char token)
{
    if (token == '(' || token == ')' || IsOperator(token) || IsDigit(token))
    {
        return false;
    }
    return true;
}

bool Formula::IsDigit(const std::string& token)
{
    int n = (int)token.length();
	LoopBeginCheck(c)
    for (int i=0; i<n; i++)
    {
		LoopDoCheck(c)
        char ch = token[i];
        if (ch > '9' || ch < '0')
        {
            // 小数点在数字中间
            if (ch == '.' && i != 0 && i != n-1)
            {
                continue;
            }
            return false;
        }
    }
    return true;
}

bool Formula::IsDigit(char ch)
{
    if (ch > '9' || ch < '0')
    {
        return false;
    }

    return true;
}

// 转变为后缀形式
bool Formula::ToPostfix(const char* str, std::vector<std::string>& suffix)
{
    std::vector<std::string> tokens;
    if (!PreParse(str, tokens))
    {
        return false;
    }

    //std::vector<std::string> suffix;     //存储后缀表达式
    FormulaStack<std::string> optStack;   //存储操作符

    int n = (int)tokens.size();
	LoopBeginCheck(d)
    for (int i=0; i<n; i++)
    {
		LoopDoCheck(d)
        std::string token = tokens[i];
        if (IsOperator(token))
        {
            if (!optStack.Empty())
            {
                int tokenPriority = GetPriority(token);
                std::string topOpt = "";
                optStack.Top(topOpt);
                int topOptPriority = GetPriority(topOpt);
				LoopBeginCheck(i)
                while (tokenPriority <= topOptPriority)
                {
					LoopDoCheck(i)
                    optStack.Pop();
                    suffix.push_back(topOpt);
                    if (!optStack.Empty())
                    {
                        optStack.Top(topOpt);
                        topOptPriority = GetPriority(topOpt);
                    }
                    else
                    {
                        break;
                    }
                } // end of while (...
            }
            optStack.Push(token);

        } // end of if (IsOperator(token))
        else if (token == "(")
        {
            optStack.Push(token);
        }
        else if (token == ")")
        {
            std::string topOpt = "";
			LoopBeginCheck(j)
            while (optStack.Top(topOpt) && topOpt != "(")
            {
				LoopDoCheck(j)
                suffix.push_back(topOpt);
                optStack.Pop();
                if (optStack.Empty())
                {
                    // 括号不匹配
                    ::extend_warning(LOG_ERROR,
                        "[Formula::ToPostfix] formula:%s less '('.", str);
                    return false;
                }
            }
            optStack.Pop();
        }
        else   //如果是操作数，直接入操作数栈
        {
            suffix.push_back(token);
        }
    } // end of for (int i=0; i<size; i++)

	LoopBeginCheck(k)
    while (!optStack.Empty())
    {
		LoopDoCheck(k)
        std::string topOpt = "";
        optStack.Top(topOpt);
        suffix.push_back(topOpt);
        optStack.Pop();
    }
    return true;
}


bool Formula::Parse(const char* str, const char* szMinValue, const char* szMaxValue)
{
    if (StringUtil::CharIsNull(str))
    {
        return false;
    }

    std::vector<std::string> suffix;
    if (!ToPostfix(str, suffix))
    {
        return false;
    }

    int size = (int)suffix.size();
	LoopBeginCheck(e)
    for (int i=0; i<size; i++, m_len++)
    {
		LoopDoCheck(e)
        if (m_len >= MAX_VECTOR_LEN)
        {
            m_len = 0;
            return false;
        }

        std::string token = suffix[i];
        if (token.empty())
        {
            continue;
        }
        
        if (IsOperator(token))
        {
            EOperatorType op_type = ToOperatorIndex(token);
            m_postfix[m_len].node_type = NODE_TYPE_OP;
            m_postfix[m_len].op_type = op_type;
        }
        else if (IsDigit(token))
        {
            m_postfix[m_len].node_type = NODE_TYPE_DIGIT;
			m_postfix[m_len].digit_value = StringUtil::StringAsFloat(token);
        }
        else
        {
            short obj_type = -1;
            if (token[0] == '@')
            {
                obj_type = OBJECT_TYPE_SELF;
            }
            else if (token[0] == '$')
            {
                obj_type = OBJECT_TYPE_TARGET;
            }
			else if (token[0] == '#')
			{
				obj_type = OBJECT_TYPE_SELF_SKILL;
			}
            std::string prop = token.substr(1, token.length()-1);
            short prop_index = FormulaPropIndex::ToPropIndex(prop.c_str());
            if (-1 == obj_type || -1 == prop_index)
            {
                // 公式错误
                ::extend_warning(LOG_ERROR,
                    "[Formula::Parse] formula:%s token:%s error.", str, token.c_str());
                return false;
            }

            m_postfix[m_len].node_type = NODE_TYPE_VARIANT;
            m_postfix[m_len].obj_type = obj_type;
            m_postfix[m_len].prop_index = prop_index;
        }
    }

    if (IsDigit(szMinValue))
    {
		m_min_value = StringUtil::StringAsFloat(szMinValue);
    }
    if (IsDigit(szMaxValue))
    {
        m_max_value = StringUtil::StringAsFloat(szMaxValue);
    }

    m_formula = str;

    return true;
}

void Formula::Clear()
{
    m_len = 0;
    m_min_value = -1;
    m_max_value = -1;
    m_formula = "";
}

bool Formula::GetValue(IGameObj* pSelfObj, IGameObj* pTargetObj, IGameObj* pSelfSkill, float& value, RecursionFormulaFunc callback)
{
	// 有缓存值取缓存,没有再计算
	if (m_bCache)
	{
		value = m_fCacheVal;
		return true;
	}

    FormulaStack<float> stack;  // 操作数栈

	LoopBeginCheck(g)
    for (int i=0; i<m_len; i++)
    {
		LoopDoCheck(g)
        if (m_postfix[i].node_type == NODE_TYPE_VARIANT)
        {
            float prop_value = 0; 
            if (!GetVariantValue(pSelfObj, pTargetObj,pSelfSkill,
                m_postfix[i].obj_type, m_postfix[i].prop_index, 
                prop_value, callback))
            {
                return false;
            }            
            stack.Push(prop_value);
        }
        else if (m_postfix[i].node_type == NODE_TYPE_DIGIT)
        {
            stack.Push(m_postfix[i].digit_value);
        }
        else if (m_postfix[i].node_type == NODE_TYPE_OP)
        {
            if (!Calculate(stack, m_postfix[i].op_type))
            {
                return false;
            }
        }
    } // end of for (int i=0; i<m_len; i++)

    float prop_value = 0;
    if (!stack.Top(prop_value))
    {
        return false;
    }
    value = AdjustFightData(prop_value);

	// 没缓存结果的,将计算结果存下来
	if (!m_bCache)
	{
		m_fCacheVal = prop_value;
		m_bCache = true;
	}
    return true;
}

// 清除缓存值
void Formula::ClearCacheVal()
{
	m_bCache = false;
	m_fCacheVal = 0.0f;
}

Formula::EOperatorType Formula::ToOperatorIndex(const std::string& token)
{
    if (token=="+") return OPERATOR_TYPE_ADD;
    if (token=="-") return OPERATOR_TYPE_SUB;
    if (token=="*") return OPERATOR_TYPE_MUL;
    if (token=="/") return OPERATOR_TYPE_DIV;
	if (token=="%") return OPERATOR_TYPE_INT;
	if (token==">") return OPERATOR_TYPE_MAX;
	if (token=="<") return OPERATOR_TYPE_MIN;
	if (token=="|") return OPERATOR_TYPE_RANDOM;

    return OPERATOR_TYPE_UNKNOWN;
}

bool Formula::Calculate(FormulaStack<float>& stack, EOperatorType op_type)
{
    switch (op_type)
    {
    case OPERATOR_TYPE_ADD:
        {
            if (stack.Size() < 2)
            {
                // 操作数不够
                return false;
            }

            float rOpd = 0;
            stack.Top(rOpd);
            stack.Pop();
            float lOpd = 0;
            stack.Top(lOpd);
            stack.Pop();
            float result = lOpd + rOpd;
            stack.Push(result); 

            break;
        }
    case OPERATOR_TYPE_SUB:
        {
            if (stack.Size() < 2)
            {
                // 操作数不够
                return false;
            }

            float rOpd = 0;
            stack.Top(rOpd);
            stack.Pop();
            float lOpd = 0;
            stack.Top(lOpd);
            stack.Pop();
            float result = lOpd - rOpd;
            stack.Push(result); 

            break;
        }
    case OPERATOR_TYPE_MUL:
        {
            if (stack.Size() < 2)
            {
                // 操作数不够
                return false;
            }

            float rOpd = 0;
            stack.Top(rOpd);
            stack.Pop();
            float lOpd = 0;
            stack.Top(lOpd);
            stack.Pop();
            float result = lOpd * rOpd;
            stack.Push(result); 

            break;
        }
    case OPERATOR_TYPE_DIV:
        {
            if (stack.Size() < 2)
            {
                // 操作数不够
                return false;
            }

            float rOpd = 0;
            stack.Top(rOpd);
            stack.Pop();
            float lOpd = 0;
            stack.Top(lOpd);
            stack.Pop();
            if (rOpd == 0)
            { // 被除数不能是0
                return false;
            }
            float result = lOpd / rOpd;
            stack.Push(result); 

            break;
        }
    case OPERATOR_TYPE_RANDOM:
        {
            if (stack.Size() < 2)
            {
                // 操作数不够
                return false;
            }

            float rOpd = 0;
            stack.Top(rOpd);
            stack.Pop();
            float lOpd = 0;
            stack.Top(lOpd);
            stack.Pop();
			 // 随机范围异常,保证战斗公式正常,设置最大值与最小值相同
            if (rOpd < lOpd)
            { 
                rOpd = lOpd;
            }
            float result = util_random_float(rOpd-lOpd) + lOpd;
            stack.Push(result); 
            break;
		}
	case OPERATOR_TYPE_MAX:
		{
			if (stack.Size() < 2)
			{
				// 操作数不够
				return false;
			}

			float rOpd = 0;
			stack.Top(rOpd);
			stack.Pop();
			float lOpd = 0;
			stack.Top(lOpd);
			stack.Pop();

			float result = __max(lOpd, rOpd);
			stack.Push(result); 
			break;
		}
	case OPERATOR_TYPE_MIN:
		{
			if (stack.Size() < 2)
			{
				// 操作数不够
				return false;
			}

			float rOpd = 0;
			stack.Top(rOpd);
			stack.Pop();
			float lOpd = 0;
			stack.Top(lOpd);
			stack.Pop();

			float result = __min(lOpd, rOpd);
			stack.Push(result);
			break;
		}
	case OPERATOR_TYPE_INT:
		{
			if (stack.Size() < 1)
			{
				// 操作数不够
				return false;
			}

			float opd = 0;
			stack.Top(opd);
			stack.Pop();
			float result = floor(opd);
			stack.Push(result); 

			break;
       }
    default:
        return false;
    } // end of switch (m_postfix[i].op_type)

    return true;
}

bool Formula::GetVariantValue(IGameObj* pSelfObj,
                              IGameObj* pTargetObj,
							  IGameObj* pSelfSkill,
                              int obj_type,
                              int prop_index,
                              float& prop_value,
                              RecursionFormulaFunc callback)
{
    if (prop_index < 0 || prop_index >= FORMULA_PROP_MAX)
    {
        return false;
    }
    const char* prop = FormulaPropIndex::ToPropName(prop_index);
    if (StringUtil::CharIsNull(prop))
    {
        ::extend_warning(LOG_ERROR,
            "[Formula::GetVariantValue] formula:%s prop no register prop_index:%d error.",
            m_formula.c_str(), prop_index);
        return false;
    }

    if (obj_type == OBJECT_TYPE_SELF)
    {
        // 查看当前变量是否需要递归处理
        Formula* fm = NULL;
        if (NULL != callback)
        {
            fm = callback(prop_index);
        }

        if (NULL != fm)
        {
			if (!fm->GetValue(pSelfObj, pTargetObj, pSelfSkill, prop_value, callback))
			{
				return false;
			}
		}
        else
        {
            prop_value = Cau::QueryValueByPropName(pSelfObj, prop);
        }
    }
    else if (obj_type == OBJECT_TYPE_TARGET)
    {
         prop_value = Cau::QueryValueByPropName(pTargetObj, prop);
    }
	else if (obj_type == OBJECT_TYPE_SELF_SKILL)
	{
		prop_value = Cau::QueryValueByPropName(pSelfSkill, prop);
	}
    else
    {
        ::extend_warning(LOG_ERROR,
            "[Formula::GetVariantValue] formula:%s obj type error.", m_formula.c_str());
        return false;
    }

    return true;
}

float Formula::AdjustFightData(float value)
{
    // 约定-1是一个特殊值，用于表示是否考虑上限或者下限

    // 不能小于最小值
    if (-1 != m_min_value&& value < m_min_value)
    {
        return m_min_value;
    }
    // 在最大值合法的情况下，判断当前值不能超过最大上限
    if (-1 != m_max_value && m_min_value < m_max_value && value > m_max_value)
    {
        return m_max_value;
    }

    return value;
}
