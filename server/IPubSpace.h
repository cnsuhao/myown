//--------------------------------------------------------------------
// 文件名:		IPubSpace.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月11日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IPUBSPACE_H
#define _SERVER_IPUBSPACE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"
#include "IPubData.h"

// 公共数据空间接口

class IPubSpace
{
public:
	virtual ~IPubSpace() = 0;
	
	/// \brief 获得公共数据空间名
	virtual const char* GetSpaceName() = 0;
	/// \brief 获得数据库表名
	virtual const char* GetTableName() = 0;
	/// \brief 是否可以修改数据
	virtual bool GetCanModify() = 0;

	/// \brief 查找公共数据项
	/// \param name 公共数据项名
	virtual bool FindPubData(const wchar_t* name) = 0;
	/// \brief 获得公共数据项数量
	virtual int GetPubDataCount() = 0;
	/// \brief 获得公共数据项名称列表
	/// \param result 返回的公共数据项名列表
	virtual int GetPubDataList(IVarList& result) = 0;
	/// \brief 获得指定名字的公共数据项
	/// \param name 公共数据项名
	virtual IPubData* GetPubData(const wchar_t* name) = 0;
	
	/// \brief 添加公共数据项
	/// \param name 公共数据项名
	virtual bool AddPubData(const wchar_t* name, bool save) = 0;
	/// \brief 删除公共数据项
	/// \param name 公共数据项名
	virtual bool RemovePubData(const wchar_t* name) = 0;
};

inline IPubSpace::~IPubSpace() {}

#endif // _SERVER_IPUBSPACE_H

