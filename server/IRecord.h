//--------------------------------------------------------------------
// 文件名:		IRecord.h
// 内  容:		表格操作接口
// 说  明:		
// 创建日期:	2010年2月21日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IRECORD_H
#define _SERVER_IRECORD_H

#include "../public/Macros.h"
#include "../public/Var.h"
#include "../public/VarList.h"

// 表格操作接口

class IRecordIndex;

class IRecord
{
public:
	virtual ~IRecord() = 0;

	/// \brief 获得表列数
	virtual int GetCols() const = 0;
	/// \brief 获得表行数
	virtual int GetRows() const = 0;
	/// \brief 获得表最大行数
	virtual int GetRowMax() const = 0;
	/// \brief 获得列数据类型
	/// \param col 列序号
	virtual int GetColType(int col) const = 0;
	
	/// \brief 添加一行，返回插入的行号
	/// \param row 行号（-1表示在最后添加）
	virtual int AddRow(int row) = 0;
	/// \brief 添加一行并初始化，返回插入的行号
	/// \param row 行号（-1表示在最后添加）
	/// \param value 行的数据
	virtual int AddRowValue(int row, const IVarList& value) = 0;
	/// \brief 删除一行
	/// \param row 行号（-1表示在最后删除）
	virtual bool RemoveRow(int row) = 0;
	/// \brief 清除所有表数据
	virtual bool ClearRow() = 0;
	
	/// \brief 写一行的表数据
	/// \param row 行号
	/// \param value 一行的数值
	virtual bool SetRowValue(int row, const IVarList& value) = 0;
	/// \brief 写表数据
	/// \param obj 对象
	/// \param row 行号
	/// \param col 列号
	/// \param value 数值
	virtual bool SetInt(int row, int col, int value) = 0;
	virtual bool SetInt64(int row, int col, int64_t value) = 0;
	virtual bool SetFloat(int row, int col, float value) = 0;
	virtual bool SetDouble(int row, int col, double value) = 0;
	virtual bool SetString(int row, int col, const char* value) = 0;
	virtual bool SetWideStr(int row, int col, const wchar_t* value) = 0;
	virtual bool SetObject(int row, int col, const PERSISTID& value) = 0;
	
	/// \brief 读一行表数据
	/// \param row 行号
	/// \param col 列号
	/// \param value 返回的一行表数据
	virtual bool QueryRowValue(int row, IVarList& value) = 0;
	/// \brief 读表数据
	/// \param row 行号
	/// \param col 列号
	virtual int QueryInt(int row, int col) = 0;
	virtual int64_t QueryInt64(int row, int col) = 0;
	virtual float QueryFloat(int row, int col) = 0;
	virtual double QueryDouble(int row, int col) = 0;
	virtual const char* QueryString(int row, int col) = 0;
	virtual const wchar_t* QueryWideStr(int row, int col) = 0;
	virtual PERSISTID QueryObject(int row, int col) = 0;
	
	/// \brief 查找指定列相符的某行，返回-1表示没有
	/// \param col 列号
	/// \param value 数值
	virtual int FindInt(int col, int value, int start_row = 0) = 0;
	virtual int FindInt64(int col, int64_t value, int start_row = 0) = 0;
	virtual int FindFloat(int col, float value, int start_row = 0) = 0;
	virtual int FindDouble(int col, double value, int start_row = 0) = 0;
	virtual int FindString(int col, const char* value, int start_row = 0) = 0;
	virtual int FindWideStr(int col, const wchar_t* value, 
		int start_row = 0) = 0;
	virtual int FindObject(int col, const PERSISTID& value, 
		int start_row = 0) = 0;
	/// \brief 不区分大小写查找
	/// \param col 列号
	/// \param value 数值
	virtual int FindStringCI(int col, const char* value, 
		int start_row = 0) = 0;
	virtual int FindWideStrCI(int col, const wchar_t* value, 
		int start_row = 0) = 0;


	// 添加索引
	virtual bool AddRecordIndex(const char* name){ return true; }
	// 删除索引
	virtual bool RemoveRecordIndex(const char* name){ return true; }
	// 获取索引接口
	virtual IRecordIndex* GetRecordIndex(const char* name) const { return NULL; }
};

inline IRecord::~IRecord() {}

#endif // _SERVER_IRECORD_H
