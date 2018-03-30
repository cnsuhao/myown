#ifndef _EXPTREE_H_
#define _EXPTREE_H_
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#ifdef FSROOMLOGIC_EXPORTS
#include "server/IRoomKernel.h"
#else
#include "server/IKernel.h"
#endif

#define MAX_EXP 256
#define MAX_NODE_EXP 64
#define MAX_NODE 128
#define MAX_PARAM 8

extern bool IsDigitStr(const char* str , float& val);
extern bool GetPropNameFromFormula(const char* szFormula, std::vector<std::pair<char, std::string> >& vecRes);

class ExpTree
{
    typedef struct _ExpNode
    {
        int type;
        union
        {
            char op;
            float fValue;
            char chValue[MAX_NODE_EXP];
        };
        char from;
        int sign;
        struct _ExpNode* left;
        struct _ExpNode* right;
    } ExpNode;
public:
    ExpTree() : m_root(NULL), m_count(0)
    {}
    ~ExpTree()
    {
    }
public:
    //中缀表达式建树
    float CalculateParam(const char* szFormula, const float(&arg)[MAX_PARAM]);
    float CalculateBuff(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const char* szFormula);
    float CalculateModify(IKernel* pKernel, const PERSISTID& self, const char* szFormula);
    float CalculateEvent(IKernel* pKernel, const PERSISTID& self, const PERSISTID& skill,
                         const PERSISTID& target, const PERSISTID& target_skill, const char* szFormula);
    float CalculateItem(IKernel* pKernel, const PERSISTID& item, const char* szFormula, const float(&arg)[MAX_PARAM]);
    void Clear();

    bool CreateExpTree(const char* str);

    float Value();
private:
    //计算
    float _Cacul(float a, char op, float b);
    //求值函数
    float _ValNode(ExpNode* cur);
    int _CmpSymbolPri(const char& ispop, const char& icpop);
    bool _ScanNode(const char* str);
    bool _ScanNumber(const char* str, int len, float& val, int& bytes);
    bool _ScanSymbol(const char* str, int len, char* buf, int& bytes);
    bool _ScanExpandSymbol(const char* str, int len, char* buf, int& bytes);
    bool _ScanRandom(const char* str, int len, char* buf, int& bytes);
    bool _ScanParam(const char* str, int len, char* buf, int& bytes);
    float _QueryPropValue(IKernel* pKernel, char chr, const char* szPropName);
    float _GetParamValue(const char* szFormula);
private:
    ExpNode m_nodes[MAX_NODE];
    ExpNode* m_root;
    int m_count;
    float m_param[MAX_PARAM];
    PERSISTID m_self;
    PERSISTID m_skill;
    PERSISTID m_target;
    PERSISTID m_target_skill;
    IKernel* m_pKernel;
};
#endif
