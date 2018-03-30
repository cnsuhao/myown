//------------------------------------------------------------------------------
// 文件名:      FlowCategoryDefine.h
// 内  容:      流程分类
// 说  明:
// 创建日期:    2013年3月18日
// 创建人:       
// 备注:
//    :       
//------------------------------------------------------------------------------


#ifndef __FLOWCATEGORYDEFINE_H__
#define __FLOWCATEGORYDEFINE_H__

// 流程分类
enum E_FLOW_CATEGORY
{
    FLOW_CATEGORY_NONE              = 0,

    FLOW_CATEGORY_SKILL             = 1,  // 技能使用流程

    FLOW_CATEGORY_MAX,
};

enum EFlowType
{
    FLOW_TYPE_DEFAULT = 0,  // 普通流程
    FLOW_TYPE_LEAD    = 1,  // 引导流程
};

#endif // __FLOWCATEGORYDEFINE_H__


