//--------------------------------------------------------------------
// 文件名:		ISnsData.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月11日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_ISNSDATA_H
#define _SERVER_ISNSDATA_H

#include "../public/Macros.h"
#include "../public/IVarList.h"
#include "IRecord.h"

// 公共数据接口

class ISnsData
{
public:
	virtual ~ISnsData() = 0;

	/// \brief 获得数据名称
	//virtual const wchar_t* GetName() = 0;
	/// \brief 获得唯一标识符
	virtual const char* GetUid() = 0;
	/// \brief 是否需要保存
	virtual bool GetSave() = 0;
	/// \brief 是否可以修改数据
	virtual bool GetCanModify() = 0;
	
	//增加属性
	virtual bool AddAttr(const char * attr_name, int type, bool saving = false) = 0;
	//移除属性
	virtual bool RemoveAttr(const char * attr_name) = 0;	

	/// \brief 设置属性是否保存
	/// \param name 属性名
	/// \param value 是否保存
	virtual bool SetAttrSave(const char* name, bool value) = 0;

	/// \brief 查找属性
	/// \param name 属性名
	virtual bool FindAttr(const char* name) = 0;
	/// \brief 获得属性类型
	/// \param name 属性名
	virtual int GetAttrType(const char* name) = 0;
	/// \brief 获得属性是否保存
	/// \param name 属性名
	virtual bool GetAttrSave(const char* name) = 0;
	/// \brief 获得属性数量
	virtual int GetAttrCount() = 0;
	/// \brief 获得属性名称列表
	/// \param result 返回的属性名列表
	virtual int GetAttrList(IVarList& result) = 0;
	/// \brief 查询属性值
	/// \param name 属性名
	virtual int QueryAttrInt(const char* name) = 0;
	virtual int64_t QueryAttrInt64(const char* name) = 0;
	virtual float QueryAttrFloat(const char* name) = 0;
	virtual double QueryAttrDouble(const char* name) = 0;
	virtual const char* QueryAttrString(const char* name) = 0;
	virtual const wchar_t* QueryAttrWideStr(const char* name) = 0;
	virtual PERSISTID  QueryAttrObject(const char* name) = 0;

	/// \brief 设置属性值
	/// \param name 属性名
	/// \param value 属性值
	virtual bool SetAttrInt(const char* name, int value) = 0;
	virtual bool SetAttrInt64(const char* name, int64_t value) = 0;
	virtual bool SetAttrFloat(const char* name, float value) = 0;
	virtual bool SetAttrDouble(const char* name, double value) = 0;
	virtual bool SetAttrString(const char* name, const char* value) = 0;
	virtual bool SetAttrWideStr(const char* name, const wchar_t* value) = 0;
	virtual bool SetAttrObject(const char* name, const PERSISTID& value) = 0;

	/// \brief 添加表
	/// \param name 表名
	/// \param row_max 最大行数（为0表示无限制）
	/// \param cols 列数
	/// \param col_types 列类型列表

	virtual bool AddRecord(const char* name, int row_max, int cols, 
		const IVarList& col_types) = 0;
	/// \brief 删除表
	/// \param name 表名
	virtual bool RemoveRecord(const char* name) = 0;
	/// \brief 为表设置一个列索引键（列类型必须为字符串或宽字符串）
	/// \param name 表格名
	/// \param col 索引列号
	/// \param case_insensitive 是否大小写不敏感
	virtual bool SetRecordKey(const char* name, int col,
		bool case_insensitive = false) = 0;
	/// \brief 设置表是否保存
	/// \param name 表名
	/// \param value 是否保存
	virtual bool SetRecordSave(const char* name, bool value) = 0;

	/// \brief 表插入或添加一行
	/// \param name 表名
	/// \param row 行号（为-1表示添加新行）
	virtual int AddRecordRow(const char* name, int row) = 0;
	/// \brief 表插入或添加一行并初始化
	/// \param name 表名
	/// \param row 行号（为-1表示添加新行）
	/// \param value 行数据
	virtual int AddRecordRowValue(const char* name, int row, 
		const IVarList& value) = 0;
	/// \brief 表删除一行
	/// \param name 表名
	/// \param row 行号
	virtual bool RemoveRecordRow(const char* name, int row) = 0;
	/// \brief 表清空
	/// \param name 表名
	virtual bool ClearRecordRow(const char* name) = 0;
	
	/// \brief 获得表格访问接口
	/// \param name 表格名
	virtual IRecord* GetRecord(const char* name) = 0;

	/// \brief 表是否存在
	/// \param name 表名
	virtual bool FindRecord(const char* name) = 0;
	/// \brief 获得表数量
	virtual int GetRecordCount() = 0;
	/// \brief 获得表名称列表
	/// \param result 返回的表格名列表
	virtual int GetRecordList(IVarList& result) = 0;
	/// \brief 获得表是否保存
	/// \param name 表名
	virtual bool GetRecordSave(const char* name) = 0;
	/// \brief 获得列类型
	/// \param name 表名
	/// \param col 列号
	virtual int GetRecordColType(const char* name, int col) = 0;
	/// \brief 获得表最大行数
	/// \param name 表名
	virtual int GetRecordMax(const char* name) = 0;
	/// \brief 获得表行数
	/// \param name 表名
	virtual int GetRecordRows(const char* name) = 0;
	/// \brief 获得表列数
	/// \param name 表名
	virtual int GetRecordCols(const char* name) = 0;

	/// \brief 查找指定列相符的表行
	/// \param name 表名
	/// \param col 列号
	/// \param value 查找的数值
	virtual int FindRecordInt(const char* name, int col, int value) = 0;
	virtual int FindRecordInt64(const char* name, int col, int64_t value) = 0;
	virtual int FindRecordFloat(const char* name, int col, float value) = 0;
	virtual int FindRecordDouble(const char* name, int col, double value) = 0;
	virtual int FindRecordString(const char* name, int col, 
		const char* value) = 0;
	virtual int FindRecordWideStr(const char* name, int col, 
		const wchar_t* value) = 0;
	virtual int FindRecordObject(const char* name, int col, 
		const PERSISTID& value) = 0;
	/// \brief 大小写不敏感查找
	/// \param name 表名
	/// \param col 列号
	/// \param value 查找的数值
	virtual int FindRecordStringCI(const char* name, int col, 
		const char* value) = 0;
	virtual int FindRecordWideStrCI(const char* name, int col, 
		const wchar_t* value) = 0;

	/// \brief 查询一行表数据
	/// \param name 表名
	/// \param row 行号
	/// \param value 返回的行数据
	virtual bool QueryRecordRowValue(const char* name, int row, 
		IVarList& value) = 0;
	/// \brief 查询表数据
	/// \param name 表名
	/// \param row 行号
	/// \param col 列号
	virtual int QueryRecordInt(const char* name, int row, int col) = 0;
	virtual int64_t QueryRecordInt64(const char* name, int row, int col) = 0;
	virtual float QueryRecordFloat(const char* name, int row, int col) = 0;
	virtual double QueryRecordDouble(const char* name, int row, int col) = 0;
	virtual const char* QueryRecordString(const char* name, 
		int row, int col) = 0;
	virtual const wchar_t* QueryRecordWideStr(const char* name, 
		int row, int col) = 0;
	virtual PERSISTID QueryRecordObject(const char* name, 
		int row, int col) = 0;

	/// \brief 设置一行表数据
	/// \param name 表名
	/// \param row 行号
	/// \param value 行数据
	virtual bool SetRecordRowValue(const char* name, int row, 
		const IVarList& value) = 0;
	/// \brief 设置表数据
	/// \param name 表名
	/// \param row 行号
	/// \param col 列号
	/// \param value 数值
	virtual bool SetRecordInt(const char* name, int row, int col, 
		int value) = 0;
	virtual bool SetRecordInt64(const char* name, int row, int col, 
		int64_t value) = 0;
	virtual bool SetRecordFloat(const char* name, int row, int col, 
		float value) = 0;
	virtual bool SetRecordDouble(const char* name, int row, int col, 
		double value) = 0;
	virtual bool SetRecordString(const char* name, int row, int col, 
		const char* value) = 0;
	virtual bool SetRecordWideStr(const char* name, int row, int col, 
		const wchar_t* value) = 0;
	virtual bool SetRecordObject(const char* name, int row, int col, 
		const PERSISTID& value) = 0;
};

inline ISnsData::~ISnsData() {}

#endif // _SERVER_IPUBDATA_H

