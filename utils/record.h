//--------------------------------------------------------------------
// 文件名:		record.h
// 内  容:		表相关工具
// 说  明:		
//				
// 创建日期:	2015年07月31日
// 整理日期:	2015年07月31日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#ifndef __Record_OPT_H__
#define __Record_OPT_H__

#include "FsSnsLogic/ISnsLogic.h"
#include "../server/ISnsData.h"
#include "../server/IPubData.h"
#include "../server/IPubKernel.h"

// 创建表格
bool _create_record(ISnsKernel *pKernel,    
					ISnsData* pPubData, 	   
					const char* recName, 	   
					int row_max, 	   
					const IVarList& col_types,   
					const bool IsSave = false);

// 创建表格
bool _create_record(IPubKernel *pKernel, 	   
					IPubData* pPubData,   
					const char* recName,    
					int row_max,    
					const IVarList& col_types,   
					const bool IsSave = false);
//保存旧表格
bool _create_pub_record_safe(IPubData* pPubData, const char* recordName,
	IVarList& cols, IVarList& newColDefaultValue, int rowMax, int recordKey = -1, bool isSave = true);

// 判断表结构是否改变
bool check_record_changed(IRecord* pRecord, IVarList& cols, int rowMax);

// 判断表结构是否一致
bool is_same_record(IRecord* pDestRecord, IRecord* pSrcRecord);

// 公共域表结构修改
// 只支持列后面增加新列
bool change_pub_record(IPubData* pPubData, const char* recordName, 
					   IVarList& cols, IVarList& newColDefaultValue, int rowMax, int recordKey = -1,bool isSave = true);

#endif //__Record_OPT_H__
