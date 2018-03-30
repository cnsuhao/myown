//--------------------------------------------------------------------
// 文件名:      DropModule.cpp
// 内  容:      掉落系统模块
// 说  明:      用于处理NPC掉落随机物品
// 创建日期:    2014年11月3日
// 创建人:        
//    :       
//--------------------------------------------------------------------

#include "DropModule.h"

#include "public/VarList.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/extend_func.h"
#include "utils/string_util.h"
#include "utils/XmlFile.h"

#include "FsGame/Define/ItemTypeDefine.h"
#include "FsGame/Define/ContainerDefine.h"
#include "FsGame/Define/CommandDefine.h"
//#include "FsGame/Define/EquipDefine.h"
#include "FsGame/Define/StaticDataDefine.h"
#include "FsGame/Define/ViewDefine.h"
#include "FsGame/Define/ToolBoxSysInfoDefine.h"
//#include "FsGame/Define/ItemDropDefine.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/GameDefine.h"
#include "FsGame/Define/LogDefine.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
//#include "FsGame/Define/SceretSceneDefine.h"

#include "FsGame/CommonModule/LuaExtModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/ContainerModule.h"
#include "FsGame/SystemFunctionModule/StaticDataQueryModule.h"
#include "FsGame/SystemFunctionModule/CapitalModule.h"
#include "FsGame/Define/ModifyPackDefine.h"
#include "FsGame/SceneBaseModule/GroupScene/GroupSceneBase.h"
#include "../Define/SnsDefine.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "FsGame\ItemModule\ToolItem\ToolItemModule.h"
//#include "FsGame/SceneBaseModule/FrontierModule.h"
//#include "FsGame/Define/FrontierSceneDefine.h"
#include "FsGame/CommonModule/ReLoadConfigModule.h"
//#include "DropAllocate.h"
//#include "DropLimitLogic.h"
#else
#include "FsGame/Define/FieldsLogic.h"
#endif
#include "FsGame/CommonModule/EnvirValueModule.h"
#include "FsGame/SystemFunctionModule/RewardModule.h"
#include "FsGame/CommonModule/FunctionEventModule.h"


#define DROP_ID_CONFIG_FILE        "ini/SystemFunction/Drop/DropId.xml"
#define DROP_PACKAGE_CONFIG_FILE   "ini/SystemFunction/Drop/DropPackage.xml"
#define DROP_ITEM_TYPE_CONFIG_FILE "ini/SystemFunction/Drop/DropItemType.xml"
#define DROP_SUBPACKAGE_CONFIG_FILE "ini/SystemFunction/Drop/DropSubPackage.xml"
#define DROP_MAIL                  "drop_mail"
#define DROP_ITEM_TYPE_ITEM       1  // 普通物品
#define DROP_ITEM_TYPE_SUBPACKAGE 2  // 子掉落包

#define HOURS_OF_A_DAY 24 // 一天24小时

#define LEFT_VALUE   1
#define RIGHT_VALUE  2
#define OPERATOR     3

#define VAR_NONE     0
#define VAR_ON_LEFT  1
#define VAR_ON_RIGHT 2

DropModule *		DropModule::m_pDropModule = NULL;
ContainerModule *	DropModule::m_pContainerBaseModule = NULL;
CapitalModule *     DropModule::m_pCapitalModule = NULL;

DropModule::DropItemMap DropModule::m_PackageItems;
DropModule::DropEntryMap DropModule::m_DropEntry;
DropModule::DropItemTypeMap DropModule::m_DropItemType;
DropModule::DropItemMap DropModule::m_SubpackageItems;  

//　重新载入掉落配置
int nx_reload_drop_config(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL != DropModule::m_pDropModule)
    {
        DropModule::m_pDropModule->LoadResource(pKernel);
    }

    return 1;
}
#ifndef FSROOMLOGIC_EXPORTS
// 测试掉落, 用法： drop NPC/宝箱CfgID count
int nx_test_drop_id(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (NULL == DropModule::m_pDropModule)
    {
        return 0;
    }

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_test_drop_id, 3);
    
    // 检查参数类型
    CHECK_ARG_OBJECT(state, nx_test_drop_id, 1);
    CHECK_ARG_STRING(state, nx_test_drop_id, 2);
    CHECK_ARG_INT(state, nx_test_drop_id, 3);

    // 获取参数
    PERSISTID player = pKernel->LuaToObject(state, 1);
    //掉落对象CFG
    const char *dropObjCfg = pKernel->LuaToString(state, 2);
    const char *dropObjCfgScript = pKernel->GetConfigProperty(dropObjCfg, "Script");
    std::vector<std::string> dropIDvec;
    if(strcmp(dropObjCfgScript, "BoxItem") == 0)
    {
        //宝箱
        CVarList itemEffectInfo;
		ToolItemModule::m_pToolItemModule->GetItemEffectInfo(pKernel, dropObjCfg, ITEM_EFFECT_GIFT, itemEffectInfo);
		LoopBeginCheck(a);
        for (int i = 0; i < int(itemEffectInfo.GetCount()); i += 3)
        {
			LoopDoCheck(a);
            dropIDvec.push_back(itemEffectInfo.StringVal(i));
        }
         
    }
    else if(strcmp(dropObjCfgScript, "NormalNpc") == 0 ||
            strcmp(dropObjCfgScript, "SecretNpc") == 0 ||
            strcmp(dropObjCfgScript, "EliteNpc") == 0)
    {

        dropIDvec.push_back(pKernel->GetConfigProperty(dropObjCfg, "DropID"));
    }
 
    
    //const char *drop_id = pKernel->LuaToString(state, 2);
    int drop_count  = pKernel->LuaToInt(state, 3);

    drop_count = (drop_count <= 0) ? 1 : drop_count;

    std::map<std::string, int> temp_items;
    LoopBeginCheck(asj);
    for (int i=0; i<drop_count; ++i)
    {
		LoopDoCheck(asj);
        CVarList list;
		LoopBeginCheck(asq);
        for(int d = 0; d < int(dropIDvec.size()); ++d)
        {
			LoopDoCheck(asq);
            DropModule::m_pDropModule->GetItemsFromDropID(pKernel, dropIDvec[d].c_str(), list, player);
        }

        int cnt = (int) list.GetCount()/2;

        int idx = 0;
		LoopBeginCheck(asp);
        for (int i=0; i<cnt; ++i)
        {
			LoopDoCheck(asp);
            std::string item_config = list.StringVal(idx);
            int cnt = list.IntVal(idx+1);
			if (ToolItemModule::IsCanWearItem(pKernel, item_config.c_str()))
            {
                cnt = 1;
            }
            idx += 2;
            
            std::map<std::string, int>::iterator find_it = temp_items.find(item_config);
            if (find_it == temp_items.end())
            {
                temp_items.insert(std::make_pair(item_config, cnt));
            }
            else
            {
                find_it->second += cnt;
            }
        }
    }
    
    // 写入varlist
    CVarList items_got;
    
    std::map<std::string, int>::iterator it = temp_items.begin();
    std::map<std::string, int>::iterator it_end = temp_items.end();
	LoopBeginCheck(a);
    for (; it != it_end; ++it)
    {
		LoopDoCheck(a);
        items_got << it->first.c_str() << it->second;
    }
 
    pKernel->LuaPushTable(state, items_got);

    return 1;
}
#endif

bool DropModule::Init(IKernel * pKernel)
{
	m_pDropModule = this;

    if (!LoadResource(pKernel))
    {
        return false;
    }

	m_pContainerBaseModule = (ContainerModule*)pKernel->GetLogicModule("ContainerModule");
	m_pCapitalModule = (CapitalModule*)pKernel->GetLogicModule("CapitalModule");

    Assert(m_pDropModule != NULL &&
        m_pContainerBaseModule != NULL &&
        m_pCapitalModule != NULL);

	pKernel->AddEventCallback("player", "OnLeaveScene", DropModule::OnPlayerLeaveScene);
	pKernel->AddIntCustomHook("player", CLIENT_CUSTOMMSG_PICK_UP_DROP_ITEM, DropModule::OnCustomPickUpDropItem);

	DECL_HEARTBEAT(DropModule::HB_CheckDropRecord);

#ifndef FSROOMLOGIC_EXPORTS
    DECL_LUA_EXT(nx_reload_drop_config);
    DECL_LUA_EXT(nx_test_drop_id);

	RELOAD_CONFIG_REG("DropConfig", DropModule::ReloadConfig);

	// 掉落限制
// 	DropLimitLogicSingleton::Instance()->Init(pKernel);
// 
// 	// 掉落分配
// 	DropAllocateSingleton::Instance()->Init(pKernel);
#endif

	return true;
}

bool DropModule::Shut(IKernel * pKernel)
{
	return true;
}

bool DropModule::LoadResource(IKernel * pKernel)
{
    if (!LoadDropItemTypeConfig(pKernel))
    {
        return false;
    }

    if (!LoadDropPackageConfig(pKernel))
    {
        return false;
    }

    if (!LoadSubpacketConfig(pKernel))
    {
        return false;
    }

    if (!LoadDropIDConfig(pKernel))
    {
        return false;
    }

    return true;
}

// 根据 @drop_id 得出掉落的物品的config_id, 放入 @drop_items
// 多个 drop_id 用逗号分隔
// @drop_items 由多组 [物品configID][物品数量] 组成 
int DropModule::GetItemsFromDropID(IKernel *pKernel, const char* drop_id,
                                   IVarList& output_items, const PERSISTID &player)
{
    CVarList drop_id_list;
    ::util_split_string(drop_id_list, drop_id, ",");

    int drop_id_count = (int)drop_id_list.GetCount();

    LoopBeginCheck(a)
    for (int i = 0; i < drop_id_count; ++i)
    {
        LoopDoCheck(a)
        
        // 一个掉落ID会包含多个DropEntry
        const std::vector<DropEntry> *drop_entry_list = GetDropEntry(drop_id_list.StringVal(i));
        if (drop_entry_list == NULL)
        {
            // 没找到此drop_id的配置
            continue;
        }

        std::vector<DropEntry>::const_iterator entry_it = drop_entry_list->begin();
        std::vector<DropEntry>::const_iterator entry_it_end = drop_entry_list->end();
        
        LoopBeginCheck(a1)
        for (; entry_it != entry_it_end; ++entry_it)
        {
            LoopDoCheck(a1)
            
            const DropEntry &drop_entry = *entry_it;

            // 取得此DropEntry 的掉落包中的物品
            const std::vector<DropItem> *item_list = GetPackageItems(drop_entry.drop_package);
            if (item_list == NULL)
            {
                // 此掉落包没有配置掉落物品
                break;
            }

            // 一个DropEntry 会掉落多次, 执行掉落次数
            LoopBeginCheck(a2)
            for (int j=0; j<drop_entry.drop_count; ++j)
            {
                LoopDoCheck(a2)
                
                // 执行掉落
                DropItemFromList(pKernel, item_list, &drop_entry, player, output_items);

            }

        }// [DropEntry] 循环结束

    }// [drop_id] 循环结束

    return 0;  
}

// 从列表中掉落物品
int DropModule::DropItemFromList(IKernel *pKernel, const std::vector<DropItem> * drop_items, 
                            const DropEntry *drop_entry, const PERSISTID& player,
                            IVarList &outpu_items)
{
    // 先过滤掉不可以参与掉落的物品
    std::vector<DropItem> base_list;
    FilterItems(pKernel, drop_items, player, base_list);

    // 从过滤后的列表中选择要产出的物品
    std::vector<DropItem> selected_list;
    if (drop_entry->drop_all)
    {
        // 列表中的物品全部掉落
        selected_list.insert(selected_list.begin(), base_list.begin(), base_list.end());
    }
    else
    {
        // 根据权重得出掉落物品
        SelectItemByWeight(base_list, selected_list);
    }

    // 检查产出的物品是否有特殊类型
    std::vector<DropItem>::const_iterator selected_it = selected_list.begin();
    std::vector<DropItem>::const_iterator selected_it_end = selected_list.end();
    
    LoopBeginCheck(a)
    for (; selected_it != selected_it_end; ++selected_it)
    {
        LoopDoCheck(a)
        
        const DropItem& item = *selected_it;
        if (item.item_config_id.length() == 0)
        {
            continue;
        }

        if (item.item_type == DROP_ITEM_TYPE_ITEM)
        {
            outpu_items << item.item_config_id.c_str();
            outpu_items << item.amount;
        }
        else if (item.item_type == DROP_ITEM_TYPE_SUBPACKAGE)
        {
            // 解析子包
            std::vector<DropItem> sub_pck_items;
            DropFromSubPackage(pKernel, item.item_config_id, player, sub_pck_items);
        }
    }

    return 0;
}

// 从子包中掉落, 掉落的物品放入result_list
int DropModule::DropFromSubPackage(IKernel *pKernel, const std::string& sub_pck_name, const PERSISTID& player,
                              std::vector<DropItem>& result_list)
{
    // 先获得子包中的物品
    const std::vector<DropItem> *drop_items = GetSubpackageItems(sub_pck_name);
    if (drop_items == NULL)
    {
        // 没有配置物品
        return 0;
    }

    // 先过滤掉不可以参与掉落的物品
    std::vector<DropItem> base_list;
    FilterItems(pKernel, drop_items, player, base_list);

    // 根据权重得出掉落物品
    SelectItemByWeight(base_list, result_list);

    return 0;
}

// 过滤不可以掉落的物品, 把可以参与掉落的物品放入@result_list
int DropModule::FilterItems(IKernel *pKernel, const std::vector<DropItem> * drop_items,
                           const PERSISTID& player, std::vector<DropItem>& result_list)
{
    std::vector<DropItem>::const_iterator it = drop_items->begin();
    std::vector<DropItem>::const_iterator it_end = drop_items->end();
    
    LoopBeginCheck(a)
    for (; it != it_end; ++it)
    {
        LoopDoCheck(a)
        
        const DropItem &drop_item = *it;

		// 检查是否符合掉落条件
		if (!MatchDropCondition(pKernel, drop_item, player))
		{
			continue;
		}

        if (drop_item.item_config_id.length() == 0)
        {
            // 空物品必须参与掉落
            result_list.push_back(drop_item);
            continue;
        }

        // 可以参与掉落
        result_list.push_back(drop_item);
    }

    return 0;
}

// 计算可掉落的物品
// 从@drop_tems 中掉落物品， 把掉落的物品放入@result_list
int DropModule::SelectItemByWeight(std::vector<DropItem>& drop_items,
                            std::vector<DropItem>& result_list)
{
    // 计算出权重
    int weight_sum = CalculateItemWeight(drop_items);

    // 获得随机因子
    int factor = GenerateDropFactor(0, weight_sum);
    
//#if defined(_DEBUG)
//    
//    std::string tt = util_int_as_string(factor).c_str();
//
//    CORE_TRACE(tt.c_str());
//
//#endif
    
    std::vector<DropItem>::const_iterator it = drop_items.begin();
    std::vector<DropItem>::const_iterator it_end = drop_items.end();
    
    LoopBeginCheck(a)
    for (; it != it_end; ++it)
    {
        LoopDoCheck(a)
        
        const DropItem &item = *it;

        if (factor < item.weight_min || factor > item.weight_max)
        {
            // 不可以掉落
            continue;
        }
    
        result_list.push_back(item);
    }

    return 0;
}

// 掉落条件的检查
bool DropModule::MatchDropCondition(IKernel *pKernel, const DropItem& drop_item,
                                    const PERSISTID& player)
{
    // 解析 '@Level>5,@Job=1' 逗号分隔的表达式是否同时成立

    if (drop_item.drop_condition.length() == 0)
    {
        return true;
    }

    // 首先把 @Level, @Job 这样的表达式转换成真实的值
    // 例如把表达式从 @Level>5 转成 10>5

    CVarList exp_list;
    ::util_split_string(exp_list, drop_item.drop_condition, ",");
    int exp_count = (int)exp_list.GetCount();
    
    LoopBeginCheck(a)
    for (int i=0; i<exp_count; ++i)
    {
        LoopDoCheck(a)
        
        const char *src_exp = exp_list.StringVal(i);

        // 这里多个布尔表达式直接的关系是And, 有一个false就返回false
        if (!BoolExp(pKernel, player, src_exp))
        {
            return false;
        }
    }

    return true;
}

// 生成一个介于 [min, max] 闭区间的随机数作为掉落因子
// 如果max是0, 返回0
int DropModule::GenerateDropFactor(int min, int max)
{
    if (max == 0)
    {
        return 0;
    }
    else
    {
        return util_random_int(max) + 1;
    }
}

// 计算掉落包中物品的权重
int DropModule::CalculateItemWeight(std::vector<DropItem>& drop_items)
{
    int weight_sum = 0;

    std::vector<DropItem>::iterator item_it = drop_items.begin();
    std::vector<DropItem>::iterator item_it_end = drop_items.end();

    LoopBeginCheck(a)
    for (; item_it != item_it_end; ++item_it)
    {
        LoopDoCheck(a)
        
        DropItem &item = *item_it;

        item.weight_min = weight_sum + 1;
        weight_sum += item.weight;
        
        item.weight_max = weight_sum;
    }

    return weight_sum;
}

// 计算布尔表达式的结果, TODO 临时方案，待优化
bool DropModule::BoolExp(IKernel *pKernel, const PERSISTID& player, const char* src_exp)
{
    // 求变量的值并带入表达式(例如把 @Level>10 转换成 5>10)

    std::string left_val;
    std::string right_val;
    std::string op;

    int state = LEFT_VALUE; // 读取状态
    int var_pos = VAR_NONE; // 变量的位置

    int exp_len = (int)strlen(src_exp);
    
    LoopBeginCheck(a)
    for (int j=0; j<exp_len; ++j)
    {
        LoopDoCheck(a)
        
        char c = *(src_exp + j);

        // 操作符过滤
        switch (c)
        {
        case '>':
        case '<':
        case '=':
        case '!':
            state = OPERATOR;
            break;
        default:
            break;
        }

        // 空格不计入操作数
        if (c == ' ')
        {
            continue;
        }

        // 当前读取状态: 左值, 右值, 操作符
        switch (state)
        {
        case LEFT_VALUE:
            if (c == '@')
            {
                var_pos = VAR_ON_LEFT;
            }
            else
            {
                left_val.append(1, c);
            }
            break;
        case RIGHT_VALUE:
            if (c == '@')
            {
                var_pos = VAR_ON_RIGHT;
            }
            else
            {
                right_val.append(1, c);
            }
            break;
        case OPERATOR:
            {
                op.append(1, c);
                state = RIGHT_VALUE; // 开始统计右值
            }
            break;
        }

    }// 单个表达式

    // 获得了一个完整的表达式

    // 取得变量类型和变量名, 如果没有变量, 就默认当作int
    int var_type = VTYPE_INT; 
    std::string var_name;
    std::string var_val;

    if (var_pos == VAR_ON_LEFT)
    {
        var_name = left_val;

        if (!SolveVariable(pKernel, player, var_name, var_val, &var_type))
        {
            return false;
        }

        return SolveBoolExp(var_type, var_val, op, right_val);
    }
    else if (var_pos == VAR_ON_RIGHT)
    {
        var_name = right_val;

        if (!SolveVariable(pKernel, player, var_name, var_val, &var_type))
        {
            return false;
        }

        return SolveBoolExp(var_type, left_val, op, var_val);
    }
    else
    {
        return SolveBoolExp(var_type, left_val, op, right_val);
    }
}

// 求解变量的值，结果存储在 @var_value中, 类型存放在@var_type 中
bool DropModule::SolveVariable(IKernel *pKernel, const PERSISTID& player,
                         const std::string& var_name, std::string& var_value, int *var_type)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return 0;
	}

    *var_type = pPlayerObj->GetAttrType(var_name.c_str());

    bool solve_ret = true;

    switch (*var_type)
    {
    case VTYPE_INT:
        {
            int val = pPlayerObj->QueryInt(var_name.c_str());

            var_value = StringUtil::IntAsString(val);
        }
        break;
    case VTYPE_INT64:
        {
            int64_t val = pPlayerObj->QueryInt64(var_name.c_str());
            var_value = StringUtil::Int64AsString(val);
        }
        break;
    case VTYPE_FLOAT:
        {
            float val = pPlayerObj->QueryFloat(var_name.c_str());
            var_value = StringUtil::FloatAsString(val);
        }
        break;
    case VTYPE_DOUBLE:
        {
            double val = pPlayerObj->QueryDouble(var_name.c_str());
            var_value = StringUtil::DoubleAsString(val);
        }
        break;
    case VTYPE_STRING:
        {
            var_value = pPlayerObj->QueryString(var_name.c_str());
        }
        break;
    case VTYPE_WIDESTR:
        solve_ret = false;
        break;
    default:
        solve_ret = false;
        break;
    }

    return solve_ret;
}

// 求解布尔表达式的值
bool DropModule::SolveBoolExp(int var_type, const std::string& left_val, const std::string& op,
                         const std::string& right_val)
{
    bool solve_ret = true;

    switch (var_type)
    {
    case VTYPE_INT:
    case VTYPE_INT64:
        {
            int64_t left = StringUtil::StringAsInt64(left_val.c_str());
            int64_t right = StringUtil::StringAsInt64(right_val.c_str());

            if (strcmp(op.c_str(), ">") == 0)
            {
                return left > right;
            }
            else if (strcmp(op.c_str(), ">=") == 0)
            {
                return left >= right;
            }
            else if (strcmp(op.c_str(), "<") == 0)
            {
                return left < right;
            }
            else if (strcmp(op.c_str(), "<=") == 0)
            {
                return left <= right;
            }
            else if (strcmp(op.c_str(), "=") == 0)
            {
                return left == right;
            }
            else if (strcmp(op.c_str(), "!=") == 0)
            {
                return left != right;
            }
            else
            {
                return false;
            }
        }
        break;
    case VTYPE_FLOAT:
    case VTYPE_DOUBLE:
        {
            double left = atof(left_val.c_str());
            double right = atof(right_val.c_str());


            if (strcmp(op.c_str(), ">") == 0)
            {
                return ((left - right) > FLT_EPSILON);
            }
            else if (strcmp(op.c_str(), ">=") == 0)
            {
                return ((left - right) >= FLT_EPSILON);
            }
            else if (strcmp(op.c_str(), "<") == 0)
            {
                return ((left - right) < FLT_EPSILON);
            }
            else if (strcmp(op.c_str(), "<=") == 0)
            {
                return ((left - right) <= FLT_EPSILON);
            }
            else if (strcmp(op.c_str(), "=") == 0)
            {
                return util_double_equal_zero(left - right);
            }
            else if (strcmp(op.c_str(), "!=") == 0)
            {
                return !util_double_equal_zero(left - right);
            }
            else
            {
                return false;
            }
        }
        break;
    case VTYPE_STRING:
        {
            solve_ret = false;
        }
        break;
    case VTYPE_WIDESTR:
        solve_ret = false;
        break;
    default:
        solve_ret = false;
        break;
    }

    return solve_ret;
}

//////////////////////////////////////////////////////////////////////////
// 以下是配置相关的接口
//////////////////////////////////////////////////////////////////////////

// 取得物品种类
int DropModule::GetDropItemType(std::string &type_name)
{
    DropItemTypeMap::const_iterator it = m_DropItemType.find(type_name);

    if (it == m_DropItemType.end())
    {
        return -1;
    }
    else
    {
        return it->second;
    }
}

// 根据掉落ID获取掉落包
const std::vector<DropModule::DropEntry> * DropModule::GetDropEntry(
    const char *drop_id)
{
    std::string search_key = drop_id;
    DropEntryMap::const_iterator it = m_DropEntry.find(search_key);
    if (it == m_DropEntry.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
}

// 根据掉落包获得掉落物品
const std::vector<DropModule::DropItem> * DropModule::GetPackageItems(
    const std::string &package_name)
{
    DropItemMap::const_iterator it = m_PackageItems.find(package_name);
    if (it == m_PackageItems.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
}

// 获取子包的掉落物品
const std::vector<DropModule::DropItem> * DropModule::GetSubpackageItems(
    const std::string &package_name)
{
    DropItemMap::const_iterator it = m_SubpackageItems.find(package_name);
    if (it == m_SubpackageItems.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
}

bool DropModule::LoadDropItemTypeConfig(IKernel *pKernel)
{
    m_DropItemType.clear();

    std::string res_path = pKernel->GetResourcePath();

    std::string the_config_file = res_path;
    the_config_file.append(DROP_ITEM_TYPE_CONFIG_FILE);

    // 掉落物品类型的
    // <Property ID="item" IntValue="1" />

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a)
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a)
        
        const char *section = sec_list.StringVal(i);
        std::string type_name = section;
        int type_id = xml.ReadInteger(section, "IntValue", -1);

        if (type_name.length() == 0 || type_id == -1)
        {
            std::string err_msg = the_config_file;
            err_msg.append(" invalid type.");
            ::extend_warning(LOG_ERROR, err_msg.c_str());
            return false;
        }

        m_DropItemType.insert(DropItemTypeMap::value_type(type_name, type_id));
    }

    return true;
}

bool DropModule::LoadDropPackageConfig(IKernel *pKernel)
{
    m_PackageItems.clear();

    std::string res_path = pKernel->GetResourcePath();

    std::string the_config_file = res_path;
    the_config_file.append(DROP_PACKAGE_CONFIG_FILE);

    // 掉落包
    // <Property ID="1" PackageName="drop_pck01" ItemType="item" ItemID="" Amount="100" Weight="90" />

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a)
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a)
        
        const char *section = sec_list.StringVal(i);
        std::string package_name = xml.ReadString(section, "PackageName", "");
        std::string drop_item_type = xml.ReadString(section, "ItemType", "item");
        std::string drop_item_id = xml.ReadString(section, "ItemID", "");
        std::string drop_condition = xml.ReadString(section, "DropCondition", "");
        int drop_amount = xml.ReadInteger(section, "Amount", 0);
        int drop_weight = xml.ReadInteger(section, "Weight", 0);

        if (package_name.length() == 0 || drop_amount == -1)
        {
            std::string err_msg = the_config_file;
            err_msg.append(":");
            err_msg.append(package_name);
            err_msg.append(" invalid drop package.");
            ::extend_warning(LOG_ERROR, err_msg.c_str());

            return false;
        }

        int item_type = GetDropItemType(drop_item_type);
        if (item_type == -1)
        {
            std::string err_msg = the_config_file;
            err_msg.append(":");
            err_msg.append(package_name);
            err_msg.append(" invalid drop package.");
            ::extend_warning(LOG_ERROR, err_msg.c_str());

            return false;
        }

        DropItem drop_item;
        drop_item.item_type = item_type;
        drop_item.item_config_id = drop_item_id;
        drop_item.drop_condition = drop_condition;
        drop_item.amount = drop_amount;
        drop_item.weight = drop_weight;
        drop_item.weight_min = 0;
        drop_item.weight_max = 0;

        DropItemMap::iterator it = m_PackageItems.find(package_name);
        if (it == m_PackageItems.end())
        {
            std::vector<DropItem> item_list;
            item_list.push_back(drop_item);

            m_PackageItems.insert(DropItemMap::value_type(package_name, item_list));
        }
        else
        {
            std::vector<DropItem> &item_list = it->second;
            item_list.push_back(drop_item);
        }
    }

    return true;
}


// 子掉落包
bool DropModule::LoadSubpacketConfig(IKernel *pKernel)
{
    m_SubpackageItems.clear();

    std::string res_path = pKernel->GetResourcePath();

    std::string the_config_file = res_path;
    the_config_file.append(DROP_SUBPACKAGE_CONFIG_FILE);

    // 掉落包
    // <Property ID="1" PackageName="drop_pck01" ItemID="Equi0001" Weight="90" />

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a)
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a)
        
        const char *section = sec_list.StringVal(i);
        std::string package_name = xml.ReadString(section, "PackageName", "");
        std::string drop_item_type = xml.ReadString(section, "ItemType", "item");
        std::string drop_item_id = xml.ReadString(section, "ItemID", "");
        std::string drop_condition = xml.ReadString(section, "DropCondition", "");
        int drop_amount = xml.ReadInteger(section, "Amount", 0);
        int drop_weight = xml.ReadInteger(section, "Weight", 0);

        if (package_name.length() == 0)
        {
            std::string err_msg = the_config_file;
            err_msg.append(":");
            err_msg.append(package_name);
            err_msg.append(" invalid sub package.");
            ::extend_warning(LOG_ERROR, err_msg.c_str());

            return false;
        }

        int item_type = GetDropItemType(drop_item_type);
        if (item_type == -1)
        {
            std::string err_msg = the_config_file;
            err_msg.append(":");
            err_msg.append(package_name);
            err_msg.append(" invalid sub package.");
            ::extend_warning(LOG_ERROR, err_msg.c_str());

            return false;
        }

        DropItem drop_item;
        drop_item.item_type = item_type;
        drop_item.item_config_id = drop_item_id;
        drop_item.drop_condition = drop_condition;
        drop_item.amount = drop_amount;
        drop_item.weight = drop_weight;
        drop_item.weight_min = 0;
        drop_item.weight_max = 0;

        DropItemMap::iterator it = m_SubpackageItems.find(package_name);
        if (it == m_SubpackageItems.end())
        {
            std::vector<DropItem> item_list;
            item_list.push_back(drop_item);

            m_SubpackageItems.insert(DropItemMap::value_type(package_name, item_list));
        }
        else
        {
            std::vector<DropItem> &item_list = it->second;
            item_list.push_back(drop_item);
        }
    }

    return true;
}

bool DropModule::LoadDropIDConfig(IKernel *pKernel)
{
    m_DropEntry.clear();

    std::string res_path = pKernel->GetResourcePath();

    std::string the_config_file = res_path;
    the_config_file.append(DROP_ID_CONFIG_FILE);

    // 掉落ID
    // <Property ID="1" DropID="boss_drop" DropPackage="drop_pck01" OutputAllInPackage="1" DropCount="2" />

    CXmlFile xml(the_config_file.c_str());
    if (!xml.LoadFromFile())
    {
        std::string err_msg = the_config_file;
        err_msg.append(" does not exists.");
        ::extend_warning(LOG_ERROR, err_msg.c_str());
        return false;
    }

    CVarList sec_list;
    xml.GetSectionList(sec_list);

    size_t sec_count = sec_list.GetCount();
    
    LoopBeginCheck(a)
    for (size_t i=0; i<sec_count; i++)
    {
        LoopDoCheck(a)
        
        const char *section = sec_list.StringVal(i);
        std::string drop_id = xml.ReadString(section, "DropID", "");
        std::string drop_pck_id = xml.ReadString(section, "DropPackage", "");
        int drop_all = xml.ReadInteger(section, "DropAll", 0);
        int drop_count = xml.ReadInteger(section, "DropCount", 0);

        if (drop_id.length() == 0 || drop_pck_id.length() == 0)
        {
            std::string err_msg = the_config_file;
            err_msg.append(":");
            err_msg.append(drop_id);
            err_msg.append(" invalid drop id.");
            ::extend_warning(LOG_ERROR, err_msg.c_str());

            return false;
        }

        DropEntry drop_entry;
        drop_entry.drop_package = drop_pck_id;
        drop_entry.drop_all = drop_all;
        drop_entry.drop_count = drop_count;

        DropEntryMap::iterator it = m_DropEntry.find(drop_id);
        if (it == m_DropEntry.end())
        {
            std::vector<DropEntry> pck_list;
            pck_list.push_back(drop_entry);

            m_DropEntry.insert(DropEntryMap::value_type(drop_id, pck_list));
        }
        else
        {
            std::vector<DropEntry> &pck_list = it->second;
            pck_list.push_back(drop_entry);
        }
    }

    return true;
}

void DropModule::ReloadConfig(IKernel* pKernel)
{
	m_pDropModule->LoadResource(pKernel);
	
#ifndef FSROOMLOGIC_EXPORTS
// 	DropLimitLogicSingleton::Instance()->LoadResource(pKernel);
// 
// 	DropAllocateSingleton::Instance()->LoadResource(pKernel);
#endif
}

int DropModule::OnPlayerLeaveScene( IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args )
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	if (!pKernel->Exists(self) || !pKernel->Exists(sender))
	{
		return 0;
	}

	if ( pKernel->Type(self) != TYPE_PLAYER )
	{
		return 0;
	}

	//切场景，清空玩家身上的掉落物品表,并移除更新冷却时间的心跳函数
	IRecord* pRecord = pSelfObj->GetRecord(ITEM_DROP_RECORD);
	if (NULL != pRecord)
	{
		pRecord->ClearRow();
		DELETE_HEART_BEAT(pKernel, self, "DropModule::HB_CheckDropRecord");
	}

	return 0;
}

// 分配掉落奖励
void DropModule::AllocateDropAward(IKernel* pKernel, const PERSISTID& self, const PERSISTID& deadnpc, const IVarList& killsers)
{
#ifndef FSROOMLOGIC_EXPORTS
//	DropAllocateSingleton::Instance()->AllocateAward(pKernel, self, deadnpc, killsers);
#endif
}

//掉落物品保存在玩家掉落表"item_drop_record"中
//@itemid+@itemnum拼接字段
//@itemnum字段
//@itemcdtime字段
bool DropModule::SaveItemsToPLayerDropRecord( IKernel *pKernel, const PERSISTID& player, std::string& item_id, int item_num )
{
	//判断玩家是否存在
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return false;
	}

	if (pPlayerObj->QueryInt(FIELD_PROP_ONLINE) == OFFLINE)
	{
		return false;
	}

	//获取玩家掉落属性表
	IRecord* pRecord = pPlayerObj->GetRecord(ITEM_DROP_RECORD);
	if (NULL == pRecord)
	{
		return false;
	}

	int drop_item_record_rows = pRecord->GetRows();
	if(drop_item_record_rows >= pRecord->GetRowMax())
	{
		//掉落表满处理
		return false;
	}

	//查找当前玩家掉落表，是否已有相同的装备
	//已有记录，修改表数据，刷新重置cd时间
	int item_row_index = pRecord->FindString(ITEM_DROP_RECORD_ITEM_ID, item_id.c_str());
	if( item_row_index != -1 )
	{
		int64_t new_item_num = item_num + pRecord->QueryInt64(item_row_index, ITEM_DROP_RECORD_ITEM_COUNT);
		pRecord->SetInt64(item_row_index, ITEM_DROP_RECORD_ITEM_COUNT, new_item_num);
		pRecord->SetInt(item_row_index, ITEM_DROP_RECORD_TIEM_CD_TIME, ITEM_DROP_DEFAULT_CD_TIME);
	}
	else //没有记录，新增一行掉落装备记录，cd时间为60秒
	{
		CVarList DropItemRecord;
		DropItemRecord << item_id.c_str() << int64_t(item_num) << ITEM_DROP_DEFAULT_CD_TIME;

		pRecord->AddRowValue(-1, DropItemRecord);
	}

	//添加掉落物品冷却时间心跳
	ADD_HEART_BEAT(pKernel, player, "DropModule::HB_CheckDropRecord", 5000);

	return true;
}

//物品掉落表的心跳回调函数，每5秒更新一次掉落表的物品，超过60秒没有拾取的物品自动清除
int DropModule::HB_CheckDropRecord( IKernel* pKernel, const PERSISTID& self, int slice )
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	IRecord* pRecord = pSelfObj->GetRecord(ITEM_DROP_RECORD);
	if (NULL == pRecord)
	{
		return 1;
	}


	int sceneID = pKernel->GetSceneId();
// 	if (GroupSceneBase::GetSceneType(pKernel, sceneID) != GROUP_SCENE_NOT_SECRET)
// 	{//秘境物品不处理超时
// 		return 0;
// 	}

	//更新掉落表中物品的cd冷却时间
	int row_num = pRecord->GetRows();
	
	LoopBeginCheck(a)
	for(int row_index = 0; row_index < row_num; ++row_index)
	{
	    LoopDoCheck(a)
	    
		int drop_item_cd_time = pRecord->QueryInt(row_index, ITEM_DROP_RECORD_TIEM_CD_TIME);
		drop_item_cd_time -= 5;
		drop_item_cd_time = drop_item_cd_time < 0 ? 0 : drop_item_cd_time;

		pRecord->SetInt(row_index, ITEM_DROP_RECORD_TIEM_CD_TIME, drop_item_cd_time);
	}

	//如果掉落表中的表项，cd时间已经到0，则删除相应的表项
	int delete_row_index = pRecord->FindInt(ITEM_DROP_RECORD_TIEM_CD_TIME, 0);
	
	LoopBeginCheck(b)
	while(delete_row_index != -1)
	{
		LoopDoCheck(b)
		pRecord->RemoveRow(delete_row_index);
		delete_row_index = pRecord->FindInt(ITEM_DROP_RECORD_TIEM_CD_TIME, 0);
	}

	//掉落表已空，删除心跳函数
	row_num = pRecord->GetRows();
	if(row_num == 0)
	{
		DELETE_HEART_BEAT(pKernel, self, "DropModule::HB_CheckDropRecord");
	}

	return 1;
}

//真实掉落，客户端拾取掉落物品，自定义消息回调函数
int DropModule::OnCustomPickUpDropItem(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	if (args.GetCount() < 3 ||
		args.GetType(1) != VTYPE_STRING ||
		args.GetType(2) != VTYPE_INT)
	{
		//客户端上发的消息，参数错误
		return 0;
	}

	// 如果玩家不存在
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

	std::string test_drop_item_id = args.StringVal(1);
	int count = args.IntVal(2);
	// 参数保护
	if (count <= 0)
	{
		return 0;
	}

	std::string rec_item_id = test_drop_item_id;

	//在玩家掉落表中查找，是否有对应的掉落装备或物品
	IRecord* pRecord = pSelfObj->GetRecord(ITEM_DROP_RECORD);
	if (NULL == pRecord)
	{
		return 0;
	}

	int find_row = pRecord->FindString(ITEM_DROP_RECORD_ITEM_ID, rec_item_id.c_str());

	//没找到对应的掉落物品
	if(find_row == -1)
	{
		return 0;
	}

	//找到，更新掉落表项
	int cd_time = pRecord->QueryInt(find_row, ITEM_DROP_RECORD_TIEM_CD_TIME);
	int64_t test_count = pRecord->QueryInt64(find_row, ITEM_DROP_RECORD_ITEM_COUNT) - count;

	if(cd_time != 0 && test_count >= 0)
	{


// 		if (strcmp(test_drop_item_id.c_str(), C_FRONTIERSCORE) == 0)
// 		{
// #ifndef FSROOMLOGIC_EXPORTS
// 			//增加边境积分 不是边境场景返回
// 			if (pKernel->GetSceneId() != EnvirValueModule::EnvirQueryInt(ENV_VALUE_FRONTIER_SCENE_ID))
// 			{
// 				return 0;
// 			}
// 			//FrontierModule::ms_Instance->OnPlayGainScore(pKernel, self, count);
// #endif
// 		}	

		//添加金币
		if(strcmp(test_drop_item_id.c_str(), "COPPER") == 0)
		{
			m_pCapitalModule->IncCapital(pKernel, self, CAPITAL_COPPER, count, FUNCTION_EVENT_ID_KILL_MONSTER_PICKUP);
			::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7014, CVarList() << count);

//             int sceneType = GroupSceneBase::GetSceneType(pKernel, pKernel->GetSceneId());
//             if (sceneType != GROUP_SCENE_NOT_SECRET)
//             {
//                 //在秘境中掉落
//                 IRecord* secret_rec = pSelfObj->GetRecord(SECRET_DROP_RECORD);
//                 if (NULL != secret_rec)
//                 {
//                     int row = secret_rec->FindString(ITEM_DROP_CONFIG_ID, "COPPER");
//                     if (row < 0)
//                     {
//                         secret_rec->AddRowValue(-1, CVarList() << "COPPER" << count);
//                     }
//                     else
//                     {
//                         int before = secret_rec->QueryInt(row, ITEM_DROP_AMOUNT);
//                         secret_rec->SetInt(row, ITEM_DROP_AMOUNT, before + count);
//                     }
//                 }
//             }
            
		}
		else
		{	
			CVarList item_list;
			StringUtil::SplitString(item_list, test_drop_item_id, "-");
			
			std::string item_id = item_list.StringVal(0);
			
			int amount = 0;

			if (item_list.GetCount() == 1)
			{
				amount = count;
			}
			else
			{
				amount = StringUtil::StringAsInt(item_list.StringVal(1));
				
			}



			

			//添加相应物品到玩家背包
			PERSISTID container = pKernel->GetChild(self, ITEM_BOX_NAME_WSTR);
			int nBindState = FunctionEventModule::GetItemBindState(FUNCTION_EVENT_ID_KILL_MONSTER_PICKUP);
			int num = m_pContainerBaseModule->TryAddItem(pKernel, container, item_id.c_str(), amount, nBindState);
			if(num <= 0)
			{
				// 背包空间不足
				::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_7004, CVarList());
				return 0;
			}
			else
			{
				// 放到对应容器中(货币直接添加)
				int putCnt = m_pContainerBaseModule->PlaceItem(pKernel, container, item_id.c_str(), amount, FUNCTION_EVENT_ID_KILL_MONSTER_PICKUP, true, nBindState);
//                 int sceneType = GroupSceneBase::GetSceneType(pKernel, pKernel->GetSceneId());
// 
// #ifndef FSROOMLOGIC_EXPORTS
//                 if (sceneType != GROUP_SCENE_NOT_SECRET )
//                 {
//                     //在秘境中掉落
//                     IRecord* secret_rec = pSelfObj->GetRecord(SECRET_DROP_RECORD);
//                     if (secret_rec != NULL)
//                     {
//                         int row = secret_rec->FindString(ITEM_DROP_CONFIG_ID, item_id.c_str());
// 						if (!ToolItemModule::IsCanWearItem(pKernel, item_id.c_str()) && row >= 0)
//                         {
//                             //道具
//                             int beforeAcount = secret_rec->QueryInt(row, ITEM_DROP_AMOUNT);
//                             secret_rec->SetInt(row, ITEM_DROP_AMOUNT, beforeAcount + amount);
//                         }
//                         else
//                         {
//                             secret_rec->AddRowValue(-1, CVarList() << item_id << amount);
//                         }
//                     }
//                 }
// #endif // FSROOMLOGIC_EXPORTS

			}
		}

		//如果数量减为0，删除对应的表项
		if(test_count == 0)
		{
			pRecord->RemoveRow(find_row);
		}
		else//更新表项中相应物品的数量
		{
			pRecord->SetInt64(find_row, ITEM_DROP_RECORD_ITEM_COUNT, test_count);
		}
	}
	else//掉落表中有相应物品，但参数不对
	{

	}

	return 0;
}

//玩家真实掉落物品接口
int DropModule::PlayerDropItems( IKernel* pKernel, const PERSISTID& self, const IVarList& args )
{
// 	if (args.GetType(0) != VTYPE_OBJECT ||
// 		args.GetType(1) != VTYPE_STRING ||
// 		args.GetType(2) != VTYPE_INT    ||
// 		args.GetType(3) != VTYPE_INT)
// 	{
// 		//掉落参数错误
// 		return 0;
// 	}

	int nIndex = 0;
	PERSISTID drop_player = args.ObjectVal(nIndex++);
	// 如果玩家不存在
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	int nItemCount = args.IntVal(nIndex++);

	//将怪物掉落的物品，暂存在玩家的掉落表中,等待玩家拾取
	//std::string ItemId(item_id);
	//由于装备的品质用数量来标示，所以将装备id与装备数量与装备是否锁定状态拼接，来唯一标示所掉落的装备
	//格式为“@itemid-@itemnum”
	CVarList saved_items;
	
	LoopBeginCheck(a)
	for (int i = 0;i < nItemCount;++i)
	{
	    LoopDoCheck(a)
	    
		const char* itemID = args.StringVal(nIndex++);
		int count = args.IntVal(nIndex++);
		std::string tmp(itemID);
		std::string item_id_and_color_level = tmp;

		// 放入的是装备
#ifndef FSROOMLOGIC_EXPORTS
		if (ToolItemModule::IsCanWearItem(pKernel, itemID))
		{
			count = 1;
			item_id_and_color_level += "-" + StringUtil::IntAsString(count);
		}
#endif
		if (count <= 0)
		{
			continue;
		}
		SaveItemsToPLayerDropRecord(pKernel, self, item_id_and_color_level, count);

		saved_items << item_id_and_color_level << count;
	}

	// msg 的格式: 
	//       [MSG_ID][NPC_ID][CONFIG_ID][ITEM_UNIQUE_ID][SAVED_AMOUNT][COLOR_LEVEL]...[CONFIG_ID][ITEM_UNIQUE_ID][SAVED_AMOUNT][COLOR_LEVEL]
	CVarList drop_msg;
	drop_msg << SERVER_CUSTOMMSG_ITEM_DROP;
	drop_msg << drop_player << ""; //死亡的玩家
	drop_msg << saved_items;

	pKernel->Custom(self, drop_msg);
	return 1;
}

bool DropModule::IsDropPkgExist(const char *pkg)
{
	DropEntryMap::const_iterator it = m_DropEntry.find(pkg);
	if (it == m_DropEntry.end())
	{
		return false;
	}
	return true;
}

void DropModule::SendDropRecordItemByMail(IKernel*pKernel, const PERSISTID& self)
{
	// 如果玩家不存在
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL){
		return;
	}

	IRecord* pRecord = pSelfObj->GetRecord(ITEM_DROP_RECORD);
	if (NULL == pRecord){
		return;
	}

	IRecord* secret_rec = pSelfObj->GetRecord(SECRET_DROP_RECORD);
	if (secret_rec == NULL)
	{
		return;
	}
	

	std::string awardStr;
	for (int i = 0; i < pRecord->GetRows(); i++)
	{

		int64_t count = pRecord->QueryInt64(i, ITEM_DROP_RECORD_ITEM_COUNT);
		if (count != 0)
		{
			const char* itemIDInfo = pRecord->QueryString(i, ITEM_DROP_RECORD_ITEM_ID);
			CVarList info;
			util_split_string(info, itemIDInfo, "-");
			if (info.GetCount() >= 2)
			{
				const char* itemID = info.StringVal(0);
				awardStr += itemID + std::string(":") + StringUtil::Int64AsString(count*info.IntVal(1)) +std::string(",");

				int row = secret_rec->FindString(ITEM_DROP_CONFIG_ID, itemID);
				if (row < 0 || std::strcmp(pKernel->GetConfigProperty(itemID, FIELD_PROP_SCRIPT), "Equipment") == 0)
				{
					secret_rec->AddRowValue(-1, CVarList() << itemID << (int)count);
				}
				else
				{
					int before = secret_rec->QueryInt(row, ITEM_DROP_AMOUNT);
					secret_rec->SetInt(row, ITEM_DROP_AMOUNT, before + (int)count);
				}
			}

		}

	}


	pRecord->ClearRow();

#ifndef FSROOMLOGIC_EXPORTS
	if (!awardStr.empty())
	{
// 		RewardModule::AwardEx award;
// 		award.name = DROP_MAIL;
// 		award.srcFunctionId = FUNCTION_EVENT_ID_KILL_MONSTER_PICKUP;
// 		if (RewardModule::ParseAwardData(awardStr, award))
// 		{
// 			RewardModule::m_pRewardInstance->RewardPlayer(pKernel, self, &award);
// 		}
// 		else
// 		{
// 			::extend_warning( LOG_ERROR, "[DropModule::SendDropRecordItemByMail] drop award config error[award:%s]", awardStr.c_str() );
// 		}
	}
#endif



}

