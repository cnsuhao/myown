//-------------------------------------------------------------------- 
// 文件名:		ISheet.h 
// 内  容:		
// 说  明:		
// 创建日期:	2006年4月6日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _SERVER_ISHEET_H
#define _SERVER_ISHEET_H

#include "../public/Macros.h"
#include "../public/IVarList.h"

// 只读数据表接口

class ISheet
{
public:
	virtual ~ISheet() = 0;
	
	/// \brief 是否可以被修改
	virtual void SetCanModify(bool value) = 0;
	virtual bool GetCanModify() const = 0;
	
	/// \brief 获得表名称
	virtual const char* GetName() const = 0;
	/// \brief 添加记录行
	/// \param key 键名
	/// \param row_data 一行数据
	virtual bool AddRow(const char* key, const IVarList& row_data) = 0;
	/// \brief 获得记录行数
	virtual size_t RowCount() const = 0;
	/// \brief 获得行的关键字
	/// \param row 行号
	virtual const char* GetKey(size_t row) const = 0;
	/// \brief 获得指定的行数据
	/// \param row 行号
	virtual const IVarList* GetRow(size_t row) const = 0;
	/// \brief 查找指定键名行数据
	/// \param key 键名
	virtual const IVarList* FindRow(const char* key) const = 0;
	
	/// \brief 添加子表
	/// \param name 子表名
	virtual ISheet* AddSubSheet(const char* name) = 0;
	/// \brief 获得子表数量
	virtual size_t SubSheetCount() const = 0;
	/// \brief 获得指定的子表
	/// \param index 子表索引
	virtual ISheet* GetSubSheet(size_t index) const = 0;
	/// \brief 查找子表
	/// \param name 子表名
	virtual ISheet* FindSubSheet(const char* name) const = 0;
};

inline ISheet::~ISheet() {}

#endif // _SERVER_ISHEET_H
*/
