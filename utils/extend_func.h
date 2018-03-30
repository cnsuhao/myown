// --------------------------------------------------------------
//  文件名:     extend_func.h
//  内  容:     扩展函数集
//  说  明:     游戏逻辑中会较多被使用到的功能函数集合
//  创建日期:   2009年2月1日
//  创建人:     冯红文
//     :      
// --------------------------------------------------------------
#ifndef _GAME_extend_func_H
#define _GAME_extend_func_H

#include "../public/IVarList.h"
#include "../SDK/tinyxml/tinyxml.h"
#include "../SDK/rapidxml/rapidxml.hpp"
#include "../public/Inlines.h"
#include <string>
#include "../FsGame/Define/LogDefine.h"

#ifdef FSROOMLOGIC_EXPORTS
#include "../server/IRoomKernel.h"
#else
#include "../server/IKernel.h"
#endif

#define ZERO    (1.0e-7f)
#define PI      (3.14159265f)
#define PI2     (6.2831853f)
#define PI_2    (1.570796325f)
#define PI_3    (1.0471975512f)
#define PI_4    (0.7853981625f)
#define PI_6    (0.5235987756f)

#ifndef INT_MAX
#define INT_MAX (2147483647)
#endif

using namespace rapidxml;

// FastString 定义
typedef TFastStr<char, 128> fast_string;
typedef TFastStr<wchar_t, 64> fast_wstring;

// 周期定义
enum CycleType
{
	CYCLE_TYPE_NONE		= 0,
	CYCLE_TYPE_DAY		= 1,	// 每天
	CYCLE_TYPE_WEEK		= 2,	// 每周
	CYCLE_TYPE_MONTH	= 3,	// 每月
	CYCLE_TYPE_YEAR		= 4,	// 每年
};

// 比较
template <class T> bool compare(const T & var1, const T & var2, const char * op)
{
    if (stricmp(op, "==") == 0)
    {
        return var1 == var2;
    }
    else if (stricmp(op, ">=") == 0)
    {
        return var1 >= var2;
    }
    else if (stricmp(op, "<=") == 0)
    {
        return var1 <= var2;
    }
    else if (stricmp(op, ">") == 0)
    {
        return var1 > var2;
    }
    else if (stricmp(op, "<") == 0)
    {
        return var1 < var2;
    }
    else if (stricmp(op, "!=") == 0)
    {
        return var1 != var2;
    }
    return false;
}

// 比较对象的属性是否满足条件
bool compare_property(IKernel * pKernel, const PERSISTID & obj, const char * prop,
                      const char * compare_value, const char * op);

// 设置日志级别
bool extend_log_level(IKernel* pKernel, LogLevelEnum level, const char* IncluedInfo);

// 输出警告信息
void extend_warning(IKernel * pKernel, const char * info);

// 输出警告信息
void extend_warning(LogLevelEnum level, const char* format, ...);

// 公共数据模块输出警告信息
void extend_warning_pub(LogLevelEnum level, const char* format, ...);

// 字符串类型的对象类型（"TYPE_PLAYER", "TYPE_SCENE"...）转换为整型
int convert_class_type(const char * sz_classtype);

// 字符串类型的属性类型（"string", "widestr", "int", "int64" ...）转换为整型
int convert_var_type(const char * sz_vartype);

// 字符串类型的字符串类型(安全检测)
const char* convert_string(const char * szstring, const char* defaultstring);

// 字符串类型的布尔类型（"true", "false"）转换为布尔型
bool convert_boolean(const char * szboolean);

// 字符串类型的整数类型转换为整型
int convert_int(const char * szint, int default_int = 0);

// 字符串类型的整数类型转换为64位整型
int64_t convert_int64(const char * szint64, int64_t default_int64);

// 字符串类型的浮点型转换为浮点型
float convert_float(const char * szfloat, float default_float = 0.0f);

// 字符串类型的浮点型转换为双精度浮点型
double convert_double(const char * szdouble, double default_double);

// 从XML文件中为逻辑类创建属性
// xmlfile是一个以服务器的资源路径为相对路径的文件名
bool createclass_from_xml(IKernel * pKernel, int index, const char * xmlfile);

// 获取第一个指定类型的父对象
PERSISTID find_firsttype_parent(IKernel * pKernel, const PERSISTID & obj, int type);

// 获得某个游戏对象（npc或player对象）主人的函数
PERSISTID get_sceneobj_master(IKernel * pKernel, const PERSISTID & self);
IGameObj* get_sceneobj_master(IKernel* pKernel, IGameObj* pNpcObj);

// 获取宠物主人
PERSISTID get_pet_master(IKernel* pKernel, const PERSISTID& self);

// 执行逻辑模块的脚本
bool run_logic_lua_script(IKernel * pKernel, const char * script, const char * func,
                          const IVarList & args, int res_num = 0, IVarList * res = NULL);

// 在某个范围内获得随机的可行走点
bool get_random_position(IKernel * pKernel, float center_x, float center_z, float radius,
                         float & random_x, float & random_y, float & random_z, float & random_orient);

// 保存到XML格式的字符串中（自定义格式，没有XML文件常用的头信息等）
bool save_to_string(IKernel * pKernel, const PERSISTID & self, fast_string& szContent,
                    bool bIncludeUnsave, bool bIncudeInvisible);


bool save_to_xml(IKernel * pKernel, const PERSISTID & self, TiXmlElement & element,
                 bool bIncludeUnsave, bool bIncudeInvisible);

bool out_put_xml(IKernel * pKernel, const PERSISTID & self, const char* file);

bool output_object_xml(IKernel * pKernel, const PERSISTID & self, TiXmlElement* root);

// 获取玩家外观信息
bool player_info_to_string(IKernel * pKernel, const PERSISTID & player, fast_string & szContent);
// 取玩家属性面板上的属性
bool player_prop_to_string(IKernel * pKernel, const PERSISTID & player, fast_string & szContent);
// 取一件装备属性
bool player_equip_to_string(IKernel * pKernel, const PERSISTID & equip, fast_string & szContent);
// 取一件物品属性
bool player_item_to_string(IKernel * pKernel, const PERSISTID & item, fast_string & szContent);

// 从上面保存的XML内容的字符器创建物品
PERSISTID load_from_string(IKernel * pKernel, const PERSISTID & parent, const std::string & szContent, int index = 0);

PERSISTID load_from_xml(IKernel * pKernel, const PERSISTID & parent, const TiXmlElement & element, int index = 0);

// 对象属性值为空或者0
bool is_save_prop_null(const char * prop);

// 生成道具类物品唯一ID
void gen_item_unique_id(IKernel * pKernel, const PERSISTID & item);

// 收到客户端点击菜单时验证菜单有效性
bool IsFuncidValid(IKernel * pKernel, const PERSISTID & player, const PERSISTID & npc, int funcid);

// 新建菜单的时候注册一下，以方便客户端返回的验证
bool RegFuncid(IKernel * pKernel, const PERSISTID & player, const PERSISTID & npc, int funcid);

// 清空注册的菜单funcid
bool ClearRegistedFuncid(IKernel * pKernel, const PERSISTID & player);

// 获得与当前高度最接近的层高度
float GetCurFloorHeight(IKernel * pKernel, float x, float y, float z, PERSISTID self = PERSISTID());

//// 获得物体上与当前高度最接近的层高度
float GetCurFloorHeightFromObj(IKernel * pKernel, const PERSISTID & obj, float x, float y, float z);

// 获取x,z位置从下往上第一层层高
float GetHeight(IKernel * pKernel, float x, float z, PERSISTID self = PERSISTID());

// 获取x, y, z位置下第一层的层高
float GetRootFLoorHeight(IKernel * pKernel, float x, float y, float z);

// 获取x,z位置从下往上第一层站立层层高
float GetFirstStandHeight(IKernel * pKernel, float x, float z);

// 获取x,z位置从下往上第一层可行走层高
float GetWalkHeight(IKernel * pKernel, float x, float z, int nFloor);

// 获得有效的可站立高度
float get_valid_height(IKernel * pKernel, float dx, float dy, float dz);

// 获取x坐标包括移动物体上的坐标
float GetPosiX(IKernel * pKernel, const PERSISTID & self, bool bRealPos = false);

// 获取y坐标包括移动物体上的坐标
float GetPosiY(IKernel * pKernel, const PERSISTID & self, bool bRealPos = false);

// 获取z坐标包括移动物体上的坐标
float GetPosiZ(IKernel * pKernel, const PERSISTID & self, bool bRealPos = false);

// 获取角度包括移动物体上的坐标
float GetOrient(IKernel * pKernel, const PERSISTID & self, bool bRealPos = false);

// 获取坐标包括移动物体上的坐标
// 获取坐标包括移动物体上的坐标
void GetLocation(IKernel * pKernel, const PERSISTID & self, float & x, float & y, float & z, float & orient, bool bRealPos = false);

// 获取link之后的真实坐标
bool GetRealPos(IKernel* pKernel,PERSISTID self,float& x,float& y,float& z,float& orient);

// obj转换为string
fast_string ObjToString(const PERSISTID & obj);

// 字符串转换为obj
PERSISTID StringToObj(const fast_string & str);

//获取角色编号。当前的编号规则是6位整数，从左到右分别表示为：第一位阵营编号，第二、三位种族编号、第四、五位职业编号、第六位性别编号
int GetRoleIndex(IKernel* pKernel, const PERSISTID& player);

//检查是否在对象前方
bool CheckObjectFront(float fStartX,float fStartZ,float fStartOrient,float fEndX,float fEndZ);

// 检查当前时间与参数时间相隔的分钟数
int GetPassMinutes(const int64_t& date);

//文件流
char* GetFileBuff(const char* file_name);

//查询XML属性
const char* QueryXmlAttr(xml_node<>* pItem, const char* name, const char* def = NULL);

//规整化坐标
void GetCenterPos(IKernel* pKernel, float& centerx, float& centerz);

//解析字符串时间 [小时：分钟：秒数]转成秒
int util_analyzing_time_str(const char* pTimeStr);

//解析字符串时间 [星期：小时：分钟：秒数]转成秒
bool util_analyzing_week_time_str(const char* pTimeStr, int& iWeek, int& iSec);

//拼接特定字符串
std::string util_get_string(const char* pUid, const int iNation, const int iType);

//解析字符 [uid：国家：类型]
bool util_analyzing_uid_nation_type(const char* pStr, std::string& uid, int& nation, int& type, time_t& time);

//拼接特定字符串[国家：类型]
std::string util_get_string(const int iNation, const int iType);

//拼接特定字符串
std::string util_get_string(const char* pStr, const int iNation, const int iType, const time_t iCurTime);

// 获取周期时间
// nNextCyc 正数 下几个周期 负数 上几个周期
int64_t get_cycle_time(int nCycType, int nDay, int nSec, int nNextCyc = 0);

//#ifdef _DEBUG
#define OPEN_COST_TIME 
#ifdef OPEN_COST_TIME

#include <Windows.h>
#include <vector>

class CostTime
{
public:
	CostTime(const char* flag):m_flag(flag)
	{
		::QueryPerformanceFrequency(&m_frequency);
		::QueryPerformanceCounter(&m_counter);
		m_beginTime = double(m_counter.QuadPart) / double(m_frequency.QuadPart);
		m_endTime = m_beginTime;
	}

	~CostTime()
	{
		::QueryPerformanceFrequency(&m_frequency);
		::QueryPerformanceCounter(&m_counter);
		m_endTime = double(m_counter.QuadPart) / double(m_frequency.QuadPart);
		double lasttime = m_endTime - m_beginTime;
		char strtemp[256];
		SPRINTF_S(strtemp, "%s Last Time %f\r\n", m_flag.c_str(), lasttime);
		m_vecCostTime.push_back(strtemp);
	}
private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_counter;
	double m_beginTime;
	double m_endTime;
	std::string m_flag;
public:
	static std::vector<std::string> m_vecCostTime;
};

// 记录整个作用域的耗时
#define RECORD_COST_TIME(flag) CostTime costTime_##flag(#flag)
// 记录代码段耗时
#define RECORD_COST_TIME_BEGIN(flag) CostTime costTime_##flag(#flag)
#define RECORD_COST_TIME_END(flag) costTime_##flag.~costTime_##flag()
// 输出耗时 记录耗时别包含此逻辑
#define TRACE_COST_TIME(pKernel) \
{\
	for(int i = 0; i < (int)CostTime::m_vecCostTime.size(); ++i) \
{\
	pKernel->Trace(CostTime::m_vecCostTime[i].c_str()); \
}\
	CostTime::m_vecCostTime.clear(); \
}

#endif // OPEN_COST_TIME
//#endif // _DEBUG
#endif // _GAME_extend_func_H

