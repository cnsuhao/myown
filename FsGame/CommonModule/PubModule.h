//--------------------------------------------------------------------
// 文件名:		PubModule.h
// 内  容:		公共数据操作
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __PubModule_H__
#define __PubModule_H__

#include "Fsgame/Define/header.h"

class PubModule : public ILogicModule
{
public:
	virtual bool Init(IKernel * pKernel);

	virtual bool Shut(IKernel * pKernel)
	{
		return true;
	}

public:
	// 获取公共数据空间
	static  IPubData* GetPubData(IKernel * pKernel, const wchar_t* domain_name);
	//对外接口
	static bool SetProp(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * prop, const IVarList & value);
	static bool IncProp(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * prop, const IVarList & value);

	//操作表格
	static bool AddRec(IKernel * pKernel, const char * pubspace_name, const wchar_t * pubdata_name,
		const char * rec, int row_max, int cols, const IVarList& col_types);
	static bool ClearRec(IKernel * pKernel, const char * pubspace_name, const wchar_t * pubdata_name,
		const char * rec);
	static bool RemoveRecRow(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * rec, int row);
	static bool AddRecRowValue(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * rec, const IVarList& args);
	static bool SetRecValue(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * rec, int row, int col, const IVarList & value);
	static bool IncRecValue(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * rec, int row, int col, const IVarList & value);


	//按关健值操作表格
	static bool RemoveRecRowByKey(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * rec, int keycol, const IVarList & key);
	static bool SetRecValueByKey(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * rec, int keycol, const IVarList & key, int col, const IVarList & value);
	static bool IncRecValueByKey(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
		const char * rec, int keycol, const IVarList & key, int col, const IVarList & value);

	// 查询表格
	static int GetPubRecColType(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata, 
		const char * rec, int keycol);

public:
	static PubModule* m_pPubModule;
};

#endif //__PubModule_H__
