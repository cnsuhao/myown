//-------------------------------------------------------------------
// 文 件 名：IDumpManager.h
// 内    容：
// 说    明：
// 创建日期：2013年12月12日
// 创 建 人：Liangxk
// 版权所有：苏州蜗牛电子有限公司
//-------------------------------------------------------------------


#if !defined __IDUMPMANAGER_H__
#define __IDUMPMANAGER_H__

#include "idumpentity.h"
#include <windows.h>
#include "dump_version.h"

//#if defined _DEBUG
/// 开启循环测试宏
//#define START_LOOP_TEST
//#endif // _DEBUG

// 单次最多读取角色数据
#define READ_ROLE_MAX_ONCE 200 

// access处理中的角色数量最大值
#define WAIT_READ_ROLE_COUNT 0x4000

enum entity_status
{
	ENTITY_STATUS_UNKNOWN,
	ENTITY_STATUS_STARTED,
	ENTITY_STATUS_RUNNING,
	ENTITY_STATUS_STOPED,
};



class IDumpManager : public IDumpEntity
{
public:
	virtual bool Init(const IVarList& args) = 0;
	virtual bool Shut(/* = 0 */) = 0;
	virtual bool LoadConfig(const char* path) = 0;
	

    //-----------------------------------------------------------
    /// \brief       发送网络消息
    /// \param       [in] name		网络名称
    /// \param       [in] index		网络索引
    /// \param       [in] serial	网络序列号
    /// \param       [in] msg		消息内容
    /// \note        
	virtual bool SendCustomMsg(const char* name, int index, int serial, const IVarList& msg) = 0;

    //-----------------------------------------------------------
    /// \brief       注册接收网络消息的模块
    /// \param       [in] strConnName	网络名称
    /// \param       [in] pEntity		实体对象
    /// \note        
	virtual bool AddModuleConnect(const char* strConnName, IDumpEntity* pEntity) = 0;

	/// 警告
	virtual void LogWarning(const char* module, const char* info, ...) = 0;

	/// 错误 异常
	virtual void LogError(const char* module, const char* info, ...) = 0;

	/// 性能
	virtual void LogPerf(const char* module, const char* info, ...) = 0;

	/// 输出 
	virtual void LogTrace(const char* module, const char* info, ...) = 0;
	virtual void LogEcho(const char* module, const char* info, ...) = 0;
	virtual void LogEchoTrace(const char* module, const char* info, ...) = 0;


    //-----------------------------------------------------------
    /// \brief       创建并初始化读取二进制队列
    /// \param       [in] module		模块
    /// \param       [in] access_name	访问器名称
    /// \param       [in] strName		队列名称
    /// \param       [in] table			二进制所在数据库表名
    /// \param       [in] key			表的主键
	/// \param       [in] query_fields	需要查询的所有字段 如:'role,save_data'
    /// \param       [in] isRole		true表示查询角色, false表示查询公会和公共数据
    /// \note        
	virtual bool InitReadBinQueue(PERSISTID module, const char* access_name, const char* strName, 
		const char* table, const char* key, const char* query_fields, bool isRole) = 0;

    //-----------------------------------------------------------
    /// \brief       移除创建的读取二进制队列
    /// \param       [in] strName
    /// \note        
	virtual bool RemoveRequestRoleList(const char* strName) = 0;

    //-----------------------------------------------------------
    /// \brief       想读取二进制队列中压入查询的名称
    /// \param       [in] strListName	队列名称
    /// \param       [in] name_list		名称列表
    /// \param       [in] begin_index	name_list开始下标
    /// \param       [in] end_index		name_list结束下标
    /// \note        
	virtual bool PushRoleName(const char* strListName, const IVarList& name_list, int begin_index, int end_index) = 0;


    //-----------------------------------------------------------
    /// \brief       注册执行sql结束后的回调函数
    /// \param       [in] module		模块
    /// \param       [in] cmd			sql命令id
    /// \param       [in] access_name	访问器名称
    /// \param       [in] callback		回调函数名称
    /// \note        
	virtual bool RegistExeSqlCallBack(PERSISTID module, int cmd, const char* access_name, const char* callback) = 0;

    //-----------------------------------------------------------
    /// \brief       执行sql语句, 不返回sql执行结果
    /// \param       [in] cmd			sql命令id
    /// \param       [in] sql			sql语句
    /// \note        
	virtual bool PushExecuteSql(int cmd, const char* sql) = 0;

    //-----------------------------------------------------------
    /// \brief       执行查询语句, 返回sql执行结果
    /// \param       [in] cmd			sql命令id
    /// \param       [in] sql			sql语句
    /// \note        
	virtual bool PushQuerySql(int cmd, const char* sql) = 0;

    //-----------------------------------------------------------
    /// \brief       执行写表格操作, sql语句将放入队列中等待执行
    /// \param       [in] cmd			sql命令id
	/// \param       [in] sql			需要更新的sql数据 如:"`key`='value',...,`keyn`='valuen'"
    /// \param       [in] key			主键名称
    /// \param       [in] value			主键匹配的值 用于where部分
    /// \param       [in] table			表名
    /// \note        
	virtual bool PushWriteSql(int cmd, const char* sql, const char* key, const char* value, const char* table) = 0;

    //-----------------------------------------------------------
    /// \brief       执行写表操作, 但sql不放入队列, 直接发给访问器, 如果访问器繁忙, 则返回false
    /// \param       [in] cmd
    /// \param       [in] sql
    /// \param       [in] key
    /// \param       [in] value
    /// \param       [in] table
    /// \note        
	virtual bool PushWriteSqlDirect(int cmd, const char* sql, const char* key, const char* value, const char* table) = 0;


	virtual bool OnTimer() = 0;

	/// 增加帧循环

    //-----------------------------------------------------------
    /// \brief       注册模块的帧循环
    /// \param       [in] module		模块
	/// \param       [in] ms			间隔时间(毫秒)
	/// \note        模块需要实现虚函数 virtual void OnUserLoop()
	virtual bool AddExecuteCallback(PERSISTID module, int ms = 0) = 0;

    //-----------------------------------------------------------
    /// \brief       移除模块的帧循环
    /// \param       [in] module
    /// \param       [in] strFunc
    /// \note        
	virtual bool RemoveExecuteCallback(PERSISTID module, const char* strFunc) = 0;

	virtual bool Start() = 0;
	virtual bool Stop() = 0;

	virtual void RunCommand(const char* cmd) = 0;

	const char* GetVersion() const { return CURRENT_TOOLSET_VERSION; }
};

#define DPLOG_WARNNING(mgr,s1,...) (mgr)->LogWarning(__FUNCTION__,s1,__VA_ARGS__)
#define DPLOG_PERF(mgr,s1,...) (mgr)->LogPerf(__FUNCTION__,s1,__VA_ARGS__)
#define DPLOG_ERROR(mgr,s1,...) (mgr)->LogError(__FUNCTION__,s1,__VA_ARGS__)
#define DPLOG_TRACE(mgr,s1,...) (mgr)->LogTrace(__FUNCTION__,s1,__VA_ARGS__)
#define DPLOG_ECHO(mgr,s1,...) (mgr)->LogEcho(__FUNCTION__,s1,__VA_ARGS__)
#define DPLOG_ECHOTRACE(mgr,s1,...) (mgr)->LogEchoTrace(__FUNCTION__,s1,__VA_ARGS__)
#endif // __IDUMPMANAGER_H__
