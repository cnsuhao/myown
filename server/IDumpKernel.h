//--------------------------------------------------------------------
// 文件名:		IDumpKernel.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年4月7日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IDUMPKERNEL_H
#define _SERVER_IDUMPKERNEL_H

#include "../public/Macros.h"
#include "../public/IVarList.h"

// 数据转储逻辑接口版本号
#define DUMP_LOGIC_MODULE_VERSION 0x3002

// 数据库字段类型定义
enum DUMP_FIELD_TYPE_ENUM
{
	DUMP_FIELD_TYPE_NULL,
	DUMP_FIELD_TYPE_INT,
	DUMP_FIELD_TYPE_FLOAT,
	DUMP_FIELD_TYPE_DOUBLE,
	DUMP_FIELD_TYPE_STRING,
	DUMP_FIELD_TYPE_WIDESTR,
	DUMP_FIELD_TYPE_BINARY,
	DUMP_FIELD_TYPE_DATE,
};

class IDumpKernel;

// 定时器函数
typedef int (__cdecl* DUMP_TIMER_FUNC)(IDumpKernel* pDumpKernel, 
	void* pContext, int time) ;
	
// 数据转储核心接口

class IDumpKernel
{
public:
	virtual ~IDumpKernel() = 0;
	
	/// \brief 查找定时器
	/// \param name 定时器名称
	virtual bool FindTimer(const char* name, void* pContext) = 0;
	/// \brief 添加定时器
	/// \param name 定时器名称
	/// \param func 定时器函数指针
	/// \param time 定时时长
	/// \param count 定时次数
	virtual bool AddTimer(const char* name, DUMP_TIMER_FUNC func, 
		void* pContext, int time, int count) = 0;
	/// \brief 删除定时器
	/// \param name 定时器名称
	virtual bool RemoveTimer(const char* name, void* pContext) = 0;

	/// \brief 发送对控制命令的回应消息
	/// \param args 消息参数
	virtual bool SendResponse(const IVarList& args) = 0;

	/// \brief 运行存储过程
	/// \param access_name 数据连接名
	/// \param stored_proc_name 存储过程名
	/// \param args 参数名(string)、参数类型(int)、是否返回参数(int)、参数值
	virtual bool RunStoredProc(const char* access_name, 
		const char* stored_proc_name, const IVarList& args) = 0;

	/// \brief 执行SQL语句
	/// \param access_name 数据连接名
	/// \param sql 内容
	virtual bool ExecuteSql(const char* access_name, const wchar_t* sql) = 0;

	/// \brief 获得数据表的相关信息
	/// \param access_name 数据连接名
	/// \param table_name 数据表名
	/// \param time_field 时间字段名
	virtual bool ReadTableInfo(const char* access_name, 
		const char* table_name, const char* time_field) = 0; 

	/// \brief 批量读取数据表中的记录信息
	/// \param access_name 数据连接名
	/// \param table_name 数据表名
	/// \param time_field 时间字段名
	/// \param key_field 关键字段名
	/// \param beg_time 起始时间
	/// \param end_time 结束时间
	virtual bool ReadBatch(const char* access_name, const char* table_name, 
		const char* time_field, const char* key_field, double beg_time, 
		double end_time, int max_key_num) = 0;

	/// \brief 从数据库读取多个记录
	/// \param access_name 数据连接名
	/// \param table_name 数据表名
	/// \param key_field 关键字段名
	/// \param keys 关键值表
	virtual bool ReadRecord(const char* access_name, const char* table_name,
		const char* key_field, const IVarList& keys) = 0;

	/// \brief 向数据库写入记录
	/// \param access_name 数据连接名
	/// \param table_name 数据表名
	/// \param key_field 关键字段名
	/// \param key_value 关键值
	/// \param field_num 字段数量
	/// \param field_types 字段类型
	/// \param field_values 字段值
	virtual bool WriteRecord(const char* access_name, const char* table_name, 
		const char* key_field, const wchar_t* key_value, int field_num, 
		int field_types[], const char* field_names[], 
		const IVarList& field_values) = 0;

	virtual bool QuerySql(const char* access_name, const char* table_name, const char* key_field, 
		const char* condition, int nMaxKeyNums) = 0;

	virtual bool SendServerMsg(int index, const IVarList& msg) = 0;

	virtual bool GetTableSchem(const char* access_name, const char* table_name) = 0;
	
	/// \brief 获得配置资源文件路径
	virtual const char* GetResourcePath() = 0;

	virtual bool IsAccessIdle() = 0;

	virtual bool Quit() = 0;
};

inline IDumpKernel::~IDumpKernel() {}

#endif // _SERVER_IDUMPKERNEL_H

