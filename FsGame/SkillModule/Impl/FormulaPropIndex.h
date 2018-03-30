
//--------------------------------------------------------------------
// 文件名:      SkillModule\impl\FormulaPropIndex.h
// 内  容:      
// 说  明:      
// 创建日期:    2015年7月28日
// 创建人:       
//    :       
//--------------------------------------------------------------------

#ifndef __FORMULA_PROP_INDEX_H__
#define __FORMULA_PROP_INDEX_H__

#include "utils/StringPod.h"
#include "utils/arraypod.h"

class FormulaPropIndex 
{
public:
    // 让管理器调用一次即可
    static void RegisterPropIndex();
    static short ToPropIndex(const char* prop);
    static const char* ToPropName(int index);
private:

    static TStringPod<char, int>    s_prop_to_index;
    static TArrayPod<const char*, 1> s_index_to_prop;
}; // end of class FormulaPropIndex

#endif // __FORMULA_PROP_INDEX_H__

