//--------------------------------------------------------------------
// 文件名:		IDumpCallee.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年4月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IDUMPCALLEE_H
#define _SERVER_IDUMPCALLEE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"

// 数据转储回调接口

class IDumpKernel;

class IDumpCallee
{
public:
	virtual ~IDumpCallee() = 0;

	/// \brief 数据转储服务器启动
	/// \param pDumpKernel 核心指针，下同
	virtual int OnDumperStart(IDumpKernel* pDumpKernel) = 0;

	/// \brief 服务器每帧调用
	/// \param frame_ticks 此帧的毫秒数
	virtual int OnFrameExecute(IDumpKernel* pDumpKernel, int frame_ticks) = 0;

	/// \brief 服务器关闭通知
	virtual int OnServerClose(IDumpKernel* pDumpKernel) = 0;

	/// \brief 收到上层的控制命令
	/// \param args 命令参数
	virtual int OnReceiveCommand(IDumpKernel* pDumpKernel, 
		const IVarList& args) = 0;

	/// \brief 返回运行存储过程结果
	/// \param access_name 数据连接名
	/// \param stored_proc_name 存储过程名
	/// \param succeed 是否运行成功
	/// \param result 返回结果参数
	virtual int OnRunStoredProc(IDumpKernel* pDumpKernel,
		const char* access_name, const char* stored_proc_name, 
		int succeed, const IVarList& result) = 0;

	/// \brief 返回数据表的相关信息
	/// \param access_name 数据连接名
	/// \param table_name 数据表名
	/// \param record_num 记录总数
	/// \param first_write_time 最先写入时间
	/// \param last_write_time 最后写入时间
	virtual int OnReadTableInfo(IDumpKernel* pDumpKernel,
		const char* access_name, const char* table_name, int succeed,
		int record_num, double first_write_time, double last_write_time) = 0;

	/// \brief 返回批量读取的结果记录数量
	/// \param access_name 数据连接名
	/// \param table_name 数据表名
	/// \param beg_time 起始时间
	/// \param end_time 结束时间
	/// \param last_time 实际返回记录的最后时间
	/// \param record_num 记录数量
	/// \param key_field 关键字段名
	/// \param keys 记录的关键字
	virtual int OnReadBatch(IDumpKernel* pDumpKernel, 
		const char* access_name, const char* table_name, int succeed,
		double beg_time, double end_time, double last_time, int record_num, 
		const char* key_field, const IVarList& keys) = 0;

	/// \brief 读取数据库记录
	/// \param access_name 数据连接名
	/// \param table_name 数据表名
	/// \param key_field 关键字段名
	/// \param key_total 一次读取的记录数量
	/// \param key_index 本次读取记录的顺序值
	/// \param field_num 字段数量
	/// \param field_type 字段类型
	/// \param field_values 字段数据
	virtual int OnReadRecord(IDumpKernel* pDumpKernel, 
		const char* access_name, const char* table_name, int succeed, 
		const char* key_field, int key_total, int key_index, int field_num, 
		int field_type[], const char* field_names[], 
		const IVarList& field_values) = 0;

	/// \brief 返回写入数据记录结果
	/// \param access_name 数据连接名
	/// \param table_name 数据表名
	/// \param key_field 关键字段名
	/// \param key_value 关键值
	/// \param succeed 是否成功
	virtual int OnWriteRecord(IDumpKernel* pDumpKernel, 
		const char* access_name, const char* table_name, int succeed,
		const char* key_field, const wchar_t* key_value) = 0;

	virtual int OnQuerySql(IDumpKernel* pDumpKernel, const char* access_name, 
		const char* table_name, int succeed, const char* key_field, int key_num, 
		const IVarList& keys) = 0;

	virtual int OnTableColNames(IDumpKernel* pDumpKernel, const char* access_name,
		const char* table_name, int succeed, int key_nums, const IVarList& keys) = 0;

	virtual int OnRecvGameServerMsg(IDumpKernel* pDumpKernel, int index, const IVarList& msg) = 0;

	virtual int OnServerConnected(IDumpKernel* pDumpKernel, int index) = 0;
};

inline IDumpCallee::~IDumpCallee() {}

#endif // _SERVER_IDUMPCALLEE_H

