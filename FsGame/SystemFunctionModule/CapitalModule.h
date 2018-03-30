//--------------------------------------------------------------------
// 文件名:		CapitalModule.h
// 内  容:		角色资金管理
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __CapitalModule_H__
#define __CapitalModule_H__

#include "Fsgame/Define/header.h"
#include <vector>
#include <string>

#include "FsGame/Define/CapitalDefine.h"
#include "FsGame/Define/LogDefine.h"

//监视货币日增加返回结果
enum CAPITAL_MONITOR
{
    CAPITAL_MONITOR_NORMAL = 0,
    CAPITAL_MONITOR_WARN,
    CAPITAL_MONITOR_TOPLIMIT,
};

class LogModule;

class CapitalModule : public ILogicModule
{
public:
	//初始化
	virtual bool Init(IKernel* pKernel);
	//释放
	virtual bool Shut(IKernel* pKernel);

	//从Rule/CapitalDefine.ini文件中读取资金类型的定义;
	bool LoadResource(IKernel* pKernel);

	// 读取兑换规则配置
	bool LoadExchangeRuleConfig(IKernel* pKernel);

	// 货币是否已定义
	bool IsCapitalDefined(const std::string& name);
	bool IsCapitalDefined(const int& type);

	// 根据货币名字查询货币类型
	int GetCapitalType(const std::string& name);
	// 根据货币类型查询货币名字
	std::string GetCapitalName(const int& type);

	//对外接口
	//获得资金类型的种类数量
	virtual int GetCapitalTypeCount(IKernel* pKernel);

	//获得某种类型的资金的当前值(strType:资金的类型，见CapitalDefine.h文件中的定义)
	virtual __int64 GetCapital(IKernel* pKernel, const PERSISTID& self, int capital_type);

	//增加某种类型的资金的当前值
	// log_type-记录日志类型
	// comment - 备注信息
	virtual bool IncCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value,
		EmFunctionEventId log_type, const wchar_t* comment = L"");

	//减少某种类型的资金的当前值
	virtual int DecCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value,
		int logType, const char* itemConfigId = "", const char* itemInstanceId = "", int itemCount = 0,
		int cmd = -1, const IVarList* var = NULL, const char * reason="");

	//是否能够增加某种类型的资金的当前值
	//如果未达最大值，返回实际可增加的值数量
	//钻石在此方法中判断失败，不可以在游戏服务器中主动添加
	virtual bool CanIncCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value);

	//是否能够减少某种类型的资金的当前值
	//如果未达最小值，返回实际可减少的值数量
	virtual bool CanDecCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value);

	//是否是有效的资金类型
	virtual bool IsValid(IKernel* pKernel, const PERSISTID& self, int capital_type);

	//获取某种类型资金的最大值
	virtual __int64 GetMaxValue(IKernel* pKernel, int capital_type);

	//获取某种类型资金的属性名称
	virtual const char* GetCapitalProp(int capital_type);
	//监视货币日增加量合法性
	const CAPITAL_MONITOR CapitalMonitor(IKernel *pKernel, const PERSISTID &self, const int capitalType, const __int64 addNum, __int64 &normalAddNum, __int64 &abnormalAddNum);
	//今日是否可以增加货币
	virtual bool IsDailyCanAddCapital(IKernel *pKernel, const PERSISTID &self,int capital_type);
#ifndef FSROOMLOGIC_EXPORTS
	// 直接添多个货币值，错误的直接跳过，返回成功的个数
	int AddCapital(IKernel *pKernel, const PERSISTID &player, const IVarList &capital_list, EmFunctionEventId capitalEventID);
	// 直接添多个货币值，错误的直接跳过，返回成功的个数
	int AddCapital(IKernel *pKernel, const PERSISTID &player, const char* pszCaptialInfo, EmFunctionEventId capitalEventID);
#endif
private:

    //0点重置当天累计增加资金记录值
    static int ResetDayToplimit(IKernel *pKernel, const PERSISTID &self, int slice);

	//设置每日增加资金上限
	bool SetCapitalMonitorMaxVal(const int capitalType, const __int64 warnDayVal, const __int64 maxDayVal);
public:

	static CapitalModule* m_pCapitalModule;
#ifndef FSROOMLOGIC_EXPORTS
	static LogModule* m_pLogModule;
#endif // FSROOMLOGIC_EXPORTS
protected:
	//资金类型定义
	struct S_CAPTITALTYPE
	{
		std::string     prop;       //属性名称
		__int64         maxvalue;   //最大值
        __int64         warnDayVal; //日警告线
        __int64         maxDayVal;  //日最大值
        S_CAPTITALTYPE():prop(""),
                         maxvalue(0),
                         warnDayVal(0),
                         maxDayVal(0)
        {

        }
	};
	std::vector<S_CAPTITALTYPE>     m_vecCaptialType;

	enum CAPITAL //资产表列类型
	{
		CAPITAL_NAME = 0,//资产名称
		CAPITAL_TYPE = 1 , //资产类型
		CAPITAL_VALUE = 2,//资产数量
	};

	// 货币规则
	struct CapitalExchangeRule 
	{
		int				nExchangeId;		// 兑换规则id
		std::string		strSrcCapital;		// 消耗原货币类型
		std::string		strDesCapital;		// 获得的目标货币类型
		int				nExchangeRate;		// 兑换比例
	};

	typedef std::vector<CapitalExchangeRule>  CapitalExRuleVec;

	CapitalExRuleVec m_vecExchangeRules;

private:
	// 执行实际的增加金钱
	bool Inner_IncCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value);

	// 执行实际的扣除金钱
	int Inner_DecCapital(IKernel* pKernel, const PERSISTID& self, int capital_type, __int64 value);

	//设置资金值
	virtual bool SetCaptialValue(IKernel* pKernel, const PERSISTID& self, const int capital_type, const __int64 newvalue);

   

    //玩家从数据库恢复数据完成（玩家上线）
    static int OnPlayerRecover(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

    //GM设置每日增加资金上限
    static int OnSetCapitalMonitorMaxVal(IKernel *pKernel, const PERSISTID &self, const PERSISTID &sender, const IVarList &args);

	static void ReloadConfig(IKernel* pKernel);

	// 响应客户端消息
	static int OnCustomExchangeCapital(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 查询兑换规则
	const CapitalExchangeRule* QueryExchangeRule(int nExchangeId);
};

#endif //__CapitalModule_H__