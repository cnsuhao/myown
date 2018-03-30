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
#include "PubModule.h"
#include "FsGame/Define/PubDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "utils/string_util.h"

PubModule * PubModule::m_pPubModule = NULL;

//脚本函数
// 原型：bool nx_set_pub_property(string pubspace, widestr pubdata, string prop, value)
// 功能：设置公共数据的属性的值
int nx_set_pub_property(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_pub_property, 4);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_set_pub_property, 1);
	CHECK_ARG_WIDESTR(state, nx_set_pub_property, 2);
	CHECK_ARG_STRING(state, nx_set_pub_property, 3);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * prop = pKernel->LuaToString(state, 3);

	CVarList value;
	if (pKernel->LuaIsInt(state, 4))
	{
		value.AddInt(pKernel->LuaToInt(state, 4));
	}
	else if (pKernel->LuaIsInt64(state, 4))
	{
		value.AddInt64(pKernel->LuaToInt64(state, 4));
	}
	else if (pKernel->LuaIsFloat(state, 4))
	{
		value.AddFloat(pKernel->LuaToFloat(state, 4));
	}
	else if (pKernel->LuaIsDouble(state, 4))
	{
		value.AddDouble(pKernel->LuaToDouble(state, 4));
	}
	else if (pKernel->LuaIsString(state, 4))
	{
		value.AddString(pKernel->LuaToString(state, 4));
	}
	else if (pKernel->LuaIsWideStr(state, 4))
	{
		value.AddWideStr(pKernel->LuaToWideStr(state, 4));
	}
	else if (pKernel->LuaIsObject(state, 4))
	{
		value.AddObject(pKernel->LuaToObject(state, 4));
	}

	pKernel->LuaPushBool(state, PubModule::SetProp(pKernel, pubspace, pubdata, prop, CVarList() << value));
	return 1;
}

// 原型：bool nx_inc_pub_property(string pubspace, widestr pubdata, string prop, value)
// 功能：设置公共数据的属性的值
int nx_inc_pub_property(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_inc_pub_property, 4);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_inc_pub_property, 1);
	CHECK_ARG_WIDESTR(state, nx_inc_pub_property, 2);
	CHECK_ARG_STRING(state, nx_inc_pub_property, 3);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * prop = pKernel->LuaToString(state, 3);

	CVarList value;
	if (pKernel->LuaIsInt(state, 4))
	{
		value.AddInt(pKernel->LuaToInt(state, 4));
	}
	else if (pKernel->LuaIsInt64(state, 4))
	{
		value.AddInt64(pKernel->LuaToInt64(state, 4));
	}
	else if (pKernel->LuaIsFloat(state, 4))
	{
		value.AddFloat(pKernel->LuaToFloat(state, 4));
	}
	else if (pKernel->LuaIsDouble(state, 4))
	{
		value.AddDouble(pKernel->LuaToDouble(state, 4));
	}
	else if (pKernel->LuaIsString(state, 4))
	{
		value.AddString(pKernel->LuaToString(state, 4));
	}
	else if (pKernel->LuaIsWideStr(state, 4))
	{
		value.AddWideStr(pKernel->LuaToWideStr(state, 4));
	}
	else if (pKernel->LuaIsObject(state, 4))
	{
		value.AddObject(pKernel->LuaToObject(state, 4));
	}

	pKernel->LuaPushBool(state, PubModule::IncProp(pKernel, pubspace, pubdata, prop, value));
	return 1;
}

// 原型：bool nx_clear_pub_rec(string pubspace, widestr pubdata, string rec)
// 功能：清除公共数据的表格
int nx_clear_pub_rec(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_clear_pub_rec, 3);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_clear_pub_rec, 1);
	CHECK_ARG_WIDESTR(state, nx_clear_pub_rec, 2);
	CHECK_ARG_STRING(state, nx_clear_pub_rec, 3);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * rec = pKernel->LuaToString(state, 3);

	pKernel->LuaPushBool(state, PubModule::ClearRec(pKernel, pubspace, pubdata, rec));
	return 1;
}

// 原型：bool nx_remove_pub_rec_row(string pubspace, widestr pubdata, string rec, int row)
// 功能：删除公共数据的表格的一行
int nx_remove_pub_rec_row(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_remove_pub_rec_row, 4);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_remove_pub_rec_row, 1);
	CHECK_ARG_WIDESTR(state, nx_remove_pub_rec_row, 2);
	CHECK_ARG_STRING(state, nx_remove_pub_rec_row, 3);
	CHECK_ARG_INT(state, nx_remove_pub_rec_row, 4);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * rec = pKernel->LuaToString(state, 3);
	int row = pKernel->LuaToInt(state, 4);

	pKernel->LuaPushBool(state, PubModule::RemoveRecRow(pKernel, pubspace, pubdata, rec, row));
	return 1;
}

// 原型：bool nx_add_pub_rec_row_value(string pubspace, widestr pubdata, string rec, ...)
// 功能：添加公共数据的表格的一行数据
int nx_add_pub_rec_row_value(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	int arg_count = pKernel->LuaGetArgCount(state);
	if (arg_count < 4)
	{
		pKernel->LuaErrorHandler(state, "nx_add_pub_rec_row_value args less than 4");
		pKernel->LuaPushBool(state, false);
		return 1;
	}

	// 检查参数类型
	CHECK_ARG_STRING(state, nx_add_pub_rec_row_value, 1);
	CHECK_ARG_WIDESTR(state, nx_add_pub_rec_row_value, 2);
	CHECK_ARG_STRING(state, nx_add_pub_rec_row_value, 3);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * rec = pKernel->LuaToString(state, 3);

	// 表对象
	IPubSpace * pPubSpace = pKernel->GetPubSpace(pubspace);
	if (pPubSpace == NULL)
	{
		return 0;
	}

	IPubData * pPubData = pPubSpace->GetPubData(pubdata);
	if (pPubData == NULL)
	{
		return 0;
	}

	IRecord* record = pPubData->GetRecord(rec);
	if( record == NULL )
	{
		return 0;
	}

	// 表列数
	int cols = record->GetCols();
	if( arg_count - 3 != cols )
	{
		return 0;
	}

	CVarList vList;
	LoopBeginCheck(a);
	for( int i = 4; i <= arg_count; ++i )
	{
		LoopDoCheck(a);
		switch( record->GetColType(i - 4) )
		{
		case VTYPE_INT:
			vList << pKernel->LuaToInt(state, i);
			break;
		case VTYPE_INT64:
			vList << pKernel->LuaToInt64(state, i);
			break;
		case VTYPE_FLOAT:
			vList << pKernel->LuaToFloat(state, i);
			break;
		case VTYPE_DOUBLE:
			vList << pKernel->LuaToDouble(state, i);
			break;
		case VTYPE_STRING:
			vList << pKernel->LuaToString(state, i);
			break;
		case VTYPE_WIDESTR:
			vList << pKernel->LuaToWideStr(state, i);
			break;
		case VTYPE_OBJECT:
			vList << pKernel->LuaToObject(state, i);
			break;
		default:
			break;
		}
	}

	pKernel->LuaPushBool(state, PubModule::AddRecRowValue(pKernel, pubspace, pubdata, rec, vList));

	return 1;
}

// 原型：bool nx_set_pub_rec_value(string pubspace, widestr pubdata, string rec, int row, int col, value)
// 功能：修改公共数据的表格的某行某列的数据
int nx_set_pub_rec_value(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_pub_rec_value, 6);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_set_pub_rec_value, 1);
	CHECK_ARG_WIDESTR(state, nx_set_pub_rec_value, 2);
	CHECK_ARG_STRING(state, nx_set_pub_rec_value, 3);
	CHECK_ARG_INT(state, nx_set_pub_rec_value, 4);
	CHECK_ARG_INT(state, nx_set_pub_rec_value, 5);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * rec = pKernel->LuaToString(state, 3);
	int row = pKernel->LuaToInt(state, 4);
	int col = pKernel->LuaToInt(state, 5);
	CVarList value;
	if (pKernel->LuaIsInt(state, 6))
	{
		value.AddInt(pKernel->LuaToInt(state, 6));
	}
	else if (pKernel->LuaIsInt64(state, 6))
	{
		value.AddInt64(pKernel->LuaToInt64(state, 6));
	}
	else if (pKernel->LuaIsFloat(state, 6))
	{
		value.AddFloat(pKernel->LuaToFloat(state, 6));
	}
	else if (pKernel->LuaIsDouble(state, 6))
	{
		value.AddDouble(pKernel->LuaToDouble(state, 6));
	}
	else if (pKernel->LuaIsString(state, 6))
	{
		value.AddString(pKernel->LuaToString(state, 6));
	}
	else if (pKernel->LuaIsWideStr(state, 6))
	{
		value.AddWideStr(pKernel->LuaToWideStr(state, 6));
	}
	else if (pKernel->LuaIsObject(state, 6))
	{
		value.AddObject(pKernel->LuaToObject(state, 6));
	}

	pKernel->LuaPushBool(state, PubModule::SetRecValue(pKernel, pubspace, pubdata, rec, row, col, value));
	return 1;
}

// 原型：bool nx_inc_pub_rec_value(string pubspace, widestr pubdata, string rec, int row, int col, value)
// 功能：修改公共数据的表格的某行某列的数据
int nx_inc_pub_rec_value(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_inc_pub_rec_value, 6);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_inc_pub_rec_value, 1);
	CHECK_ARG_WIDESTR(state, nx_inc_pub_rec_value, 2);
	CHECK_ARG_STRING(state, nx_inc_pub_rec_value, 3);
	CHECK_ARG_INT(state, nx_inc_pub_rec_value, 4);
	CHECK_ARG_INT(state, nx_inc_pub_rec_value, 5);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * rec = pKernel->LuaToString(state, 3);
	int row = pKernel->LuaToInt(state, 4);
	int col = pKernel->LuaToInt(state, 5);

	CVarList value;
	if (pKernel->LuaIsInt(state, 6))
	{
		value.AddInt(pKernel->LuaToInt(state, 6));
	}
	else if (pKernel->LuaIsInt64(state, 6))
	{
		value.AddInt64(pKernel->LuaToInt64(state, 6));
	}
	else if (pKernel->LuaIsFloat(state, 6))
	{
		value.AddFloat(pKernel->LuaToFloat(state, 6));
	}
	else if (pKernel->LuaIsDouble(state, 6))
	{
		value.AddDouble(pKernel->LuaToDouble(state, 6));
	}
	else if (pKernel->LuaIsString(state, 6))
	{
		value.AddString(pKernel->LuaToString(state, 6));
	}
	else if (pKernel->LuaIsWideStr(state, 6))
	{
		value.AddWideStr(pKernel->LuaToWideStr(state, 6));
	}
	else if (pKernel->LuaIsObject(state, 6))
	{
		value.AddObject(pKernel->LuaToObject(state, 6));
	}
 
	pKernel->LuaPushBool(state, PubModule::IncRecValue(pKernel, pubspace, pubdata, rec, row, col, value));
	return 1;
}

// 原型：bool nx_remove_pub_rec_row_bykey(string pubspace, widestr pubdata, string rec, int keycol, keyvalue)
// 功能：根据关健值删除公共数据的表格的一行
int nx_remove_pub_rec_row_bykey(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_remove_pub_rec_row_bykey, 5);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_remove_pub_rec_row_bykey, 1);
	CHECK_ARG_WIDESTR(state, nx_remove_pub_rec_row_bykey, 2);
	CHECK_ARG_STRING(state, nx_remove_pub_rec_row_bykey, 3);
	CHECK_ARG_INT(state, nx_remove_pub_rec_row_bykey, 4);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * rec = pKernel->LuaToString(state, 3);
	int keycol = pKernel->LuaToInt(state, 4);

	CVarList keyvalue;
	if (pKernel->LuaIsInt(state, 5))
	{
		keyvalue.AddInt(pKernel->LuaToInt(state, 5));
	}
	else if (pKernel->LuaIsInt64(state, 5))
	{
		keyvalue.AddInt64(pKernel->LuaToInt64(state, 5));
	}
	else if (pKernel->LuaIsFloat(state, 5))
	{
		keyvalue.AddFloat(pKernel->LuaToFloat(state, 5));
	}
	else if (pKernel->LuaIsDouble(state, 5))
	{
		keyvalue.AddDouble(pKernel->LuaToDouble(state, 5));
	}
	else if (pKernel->LuaIsString(state, 5))
	{
		keyvalue.AddString(pKernel->LuaToString(state, 5));
	}
	else if (pKernel->LuaIsWideStr(state, 5))
	{
		keyvalue.AddWideStr(pKernel->LuaToWideStr(state, 5));
	}
	else if (pKernel->LuaIsObject(state, 5))
	{
		keyvalue.AddObject(pKernel->LuaToObject(state, 5));
	}

	pKernel->LuaPushBool(state, PubModule::RemoveRecRowByKey(pKernel, pubspace, pubdata, rec, keycol, keyvalue));
	return 1;
}

// 原型：bool nx_set_pub_rec_value_bykey(string pubspace, widestr pubdata, string rec, int keycol, keyvalue, int col, value)
// 功能：根据关健值修改公共数据的表格的某行某列的数据
int nx_set_pub_rec_value_bykey(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_set_pub_rec_value_bykey, 7);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_set_pub_rec_value_bykey, 1);
	CHECK_ARG_WIDESTR(state, nx_set_pub_rec_value_bykey, 2);
	CHECK_ARG_STRING(state, nx_set_pub_rec_value_bykey, 3);
	CHECK_ARG_INT(state, nx_set_pub_rec_value_bykey, 4);
	CHECK_ARG_INT(state, nx_set_pub_rec_value_bykey, 6);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * rec = pKernel->LuaToString(state, 3);
	int keycol = pKernel->LuaToInt(state, 4);
	int col = pKernel->LuaToInt(state, 6);

	CVarList keyvalue;
	if (pKernel->LuaIsInt(state, 5))
	{
		keyvalue.AddInt(pKernel->LuaToInt(state, 5));
	}
	else if (pKernel->LuaIsInt64(state, 5))
	{
		keyvalue.AddInt64(pKernel->LuaToInt64(state, 5));
	}
	else if (pKernel->LuaIsFloat(state, 5))
	{
		keyvalue.AddFloat(pKernel->LuaToFloat(state, 5));
	}
	else if (pKernel->LuaIsDouble(state, 5))
	{
		keyvalue.AddDouble(pKernel->LuaToDouble(state, 5));
	}
	else if (pKernel->LuaIsString(state, 5))
	{
		keyvalue.AddString(pKernel->LuaToString(state, 5));
	}
	else if (pKernel->LuaIsWideStr(state, 5))
	{
		keyvalue.AddWideStr(pKernel->LuaToWideStr(state, 5));
	}
	else if (pKernel->LuaIsObject(state, 5))
	{
		keyvalue.AddObject(pKernel->LuaToObject(state, 5));
	}

	CVarList value;
	if (pKernel->LuaIsInt(state, 7))
	{
		value.AddInt(pKernel->LuaToInt(state, 7));
	}
	else if (pKernel->LuaIsInt64(state, 7))
	{
		value.AddInt64(pKernel->LuaToInt64(state, 7));
	}
	else if (pKernel->LuaIsFloat(state, 7))
	{
		value.AddFloat(pKernel->LuaToFloat(state, 7));
	}
	else if (pKernel->LuaIsDouble(state, 7))
	{
		value.AddDouble(pKernel->LuaToDouble(state, 7));
	}
	else if (pKernel->LuaIsString(state, 7))
	{
		value.AddString(pKernel->LuaToString(state, 7));
	}
	else if (pKernel->LuaIsWideStr(state, 7))
	{
		value.AddWideStr(pKernel->LuaToWideStr(state, 7));
	}
	else if (pKernel->LuaIsObject(state, 7))
	{
		value.AddObject(pKernel->LuaToObject(state, 7));
	}

	pKernel->LuaPushBool(state, PubModule::SetRecValueByKey(pKernel, pubspace, pubdata, rec, keycol, keyvalue, col, value));
	return 1;
}

// 原型：bool nx_inc_pub_rec_value_bykey(string pubspace, widestr pubdata, string rec, int keycol, keyvalue, int col, value)
// 功能：根据关健值修改公共数据的表格的某行某列的数据
int nx_inc_pub_rec_value_bykey(void * state)
{
	IKernel * pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_inc_pub_rec_value_bykey, 7);
	// 检查参数类型
	CHECK_ARG_STRING(state, nx_inc_pub_rec_value_bykey, 1);
	CHECK_ARG_WIDESTR(state, nx_inc_pub_rec_value_bykey, 2);
	CHECK_ARG_STRING(state, nx_inc_pub_rec_value_bykey, 3);
	CHECK_ARG_INT(state, nx_inc_pub_rec_value_bykey, 4);
	CHECK_ARG_INT(state, nx_inc_pub_rec_value_bykey, 6);

	// 获取参数
	const char * pubspace = pKernel->LuaToString(state, 1);
	const wchar_t * pubdata = pKernel->LuaToWideStr(state, 2);
	const char * rec = pKernel->LuaToString(state, 3);
	int keycol = pKernel->LuaToInt(state, 4);
	int col = pKernel->LuaToInt(state, 6);

	CVarList keyvalue;
	if (pKernel->LuaIsInt(state, 5))
	{
		keyvalue.AddInt(pKernel->LuaToInt(state, 5));
	}
	else if (pKernel->LuaIsInt64(state, 5))
	{
		keyvalue.AddInt64(pKernel->LuaToInt64(state, 5));
	}
	else if (pKernel->LuaIsFloat(state, 5))
	{
		keyvalue.AddFloat(pKernel->LuaToFloat(state, 5));
	}
	else if (pKernel->LuaIsDouble(state, 5))
	{
		keyvalue.AddDouble(pKernel->LuaToDouble(state, 5));
	}
	else if (pKernel->LuaIsString(state, 5))
	{
		keyvalue.AddString(pKernel->LuaToString(state, 5));
	}
	else if (pKernel->LuaIsWideStr(state, 5))
	{
		keyvalue.AddWideStr(pKernel->LuaToWideStr(state, 5));
	}
	else if (pKernel->LuaIsObject(state, 5))
	{
		keyvalue.AddObject(pKernel->LuaToObject(state, 5));
	}

	CVarList value;
	if (pKernel->LuaIsInt(state, 7))
	{
		value.AddInt(pKernel->LuaToInt(state, 7));
	}
	else if (pKernel->LuaIsInt64(state, 7))
	{
		value.AddInt64(pKernel->LuaToInt64(state, 7));
	}
	else if (pKernel->LuaIsFloat(state, 7))
	{
		value.AddFloat(pKernel->LuaToFloat(state, 7));
	}
	else if (pKernel->LuaIsDouble(state, 7))
	{
		value.AddDouble(pKernel->LuaToDouble(state, 7));
	}
	else if (pKernel->LuaIsString(state, 7))
	{
		value.AddString(pKernel->LuaToString(state, 7));
	}
	else if (pKernel->LuaIsWideStr(state, 7))
	{
		value.AddWideStr(pKernel->LuaToWideStr(state, 7));
	}
	else if (pKernel->LuaIsObject(state, 7))
	{
		value.AddObject(pKernel->LuaToObject(state, 7));
	}

	pKernel->LuaPushBool(state, PubModule::IncRecValueByKey(pKernel, pubspace, pubdata, rec, keycol, keyvalue, col, value));
	return 1;
}

bool PubModule::Init(IKernel * pKernel)
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

	m_pPubModule = this;

	Assert(m_pPubModule != NULL);

	DECL_LUA_EXT(nx_set_pub_property);
	DECL_LUA_EXT(nx_inc_pub_property);
	DECL_LUA_EXT(nx_clear_pub_rec);
	DECL_LUA_EXT(nx_remove_pub_rec_row);
	DECL_LUA_EXT(nx_add_pub_rec_row_value);
	DECL_LUA_EXT(nx_set_pub_rec_value);
	DECL_LUA_EXT(nx_inc_pub_rec_value);
	DECL_LUA_EXT(nx_remove_pub_rec_row_bykey);
	DECL_LUA_EXT(nx_set_pub_rec_value_bykey);
	DECL_LUA_EXT(nx_inc_pub_rec_value_bykey);

	return true;
}

IPubData* PubModule::GetPubData(IKernel * pKernel, const wchar_t* domain_name)
{
	IPubSpace * pPubSpace = pKernel->GetPubSpace(PUBSPACE_DOMAIN);
	if (NULL == pPubSpace)
	{
		return NULL;
	}

	if (StringUtil::CharIsNull(domain_name))
	{
		return NULL;
	}

	return pPubSpace->GetPubData(domain_name);
}

bool PubModule::SetProp(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
						const char * prop, const IVarList & value)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_SET_PROP << prop;
	switch (value.GetType(0))
	{
	case VTYPE_INT:
		msg << value.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << value.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << value.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << value.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << value.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << value.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << value.ObjectVal(0);
		break;
	default:
		break;
	}

	return pKernel->SendPublicMessage(msg);
}

bool PubModule::IncProp(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
						const char * prop, const IVarList & value)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_INC_PROP << prop;
	switch (value.GetType(0))
	{
	case VTYPE_INT:
		msg << value.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << value.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << value.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << value.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << value.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << value.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << value.ObjectVal(0);
		break;
	default:
		break;
	}

	return pKernel->SendPublicMessage(msg);
}

//操作表格
bool PubModule::AddRec(IKernel * pKernel, const char * pubspace_name, const wchar_t * pubdata_name, 
	const char * rec, int row_max, int cols, const IVarList& col_types)
{
	CVarList msg;
	msg << pubspace_name << pubdata_name << PUBDATA_MSG_ADD_REC << rec << row_max << cols << col_types;

	return pKernel->SendPublicMessage(msg);
}

bool PubModule::ClearRec(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
						 const char * rec)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_CLEAR_REC << rec;

	return pKernel->SendPublicMessage(msg);
}

bool PubModule::RemoveRecRow(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
							 const char * rec, int row)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_REMOVE_REC_ROW << rec << row;

	return pKernel->SendPublicMessage(msg);
}

bool PubModule::AddRecRowValue(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
							   const char * rec, const IVarList& args)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_ADD_REC_ROW_VALUE << rec << args;

	return pKernel->SendPublicMessage(msg);
}

bool PubModule::SetRecValue(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
							const char * rec, int row, int col, const IVarList & value)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_SET_REC_VALUE << rec << row << col;
	switch (value.GetType(0))
	{
	case VTYPE_INT:
		msg << value.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << value.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << value.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << value.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << value.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << value.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << value.ObjectVal(0);
		break;
	default:
		break;
	}

	return pKernel->SendPublicMessage(msg);
}

bool PubModule::IncRecValue(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
							const char * rec, int row, int col, const IVarList & value)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_INC_REC_VALUE << rec << row << col;
	switch (value.GetType(0))
	{
	case VTYPE_INT:
		msg << value.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << value.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << value.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << value.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << value.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << value.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << value.ObjectVal(0);
		break;
	default:
		break;
	}

	return pKernel->SendPublicMessage(msg);
}

//按关健值操作表格
bool PubModule::RemoveRecRowByKey(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
								  const char * rec, int keycol, const IVarList & key)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_REMOVE_REC_ROW_BYKEY << rec << keycol;
	switch (key.GetType(0))
	{
	case VTYPE_INT:
		msg << key.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << key.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << key.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << key.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << key.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << key.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << key.ObjectVal(0);
		break;
	default:
		break;
	}

	return pKernel->SendPublicMessage(msg);
}

bool PubModule::SetRecValueByKey(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
								 const char * rec, int keycol, const IVarList & key, int col, const IVarList & value)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_SET_REC_VALUE_BYKEY << rec << keycol;
	switch (key.GetType(0))
	{
	case VTYPE_INT:
		msg << key.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << key.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << key.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << key.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << key.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << key.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << key.ObjectVal(0);
		break;
	default:
		break;
	}

	msg << col;

	switch (value.GetType(0))
	{
	case VTYPE_INT:
		msg << value.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << value.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << value.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << value.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << value.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << value.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << value.ObjectVal(0);
		break;
	default:
		break;
	}

	return pKernel->SendPublicMessage(msg);
}

bool PubModule::IncRecValueByKey(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata,
								 const char * rec, int keycol, const IVarList & key, int col, const IVarList & value)
{
	CVarList msg;
	msg << pubspace << pubdata << PUBDATA_MSG_INC_REC_VALUE_BYKEY << rec << keycol;
	switch (key.GetType(0))
	{
	case VTYPE_INT:
		msg << key.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << key.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << key.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << key.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << key.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << key.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << key.ObjectVal(0);
		break;
	default:
		break;
	}

	msg << col;

	switch (value.GetType(0))
	{
	case VTYPE_INT:
		msg << value.IntVal(0);
		break;
	case VTYPE_INT64:
		msg << value.Int64Val(0);
		break;
	case VTYPE_FLOAT:
		msg << value.FloatVal(0);
		break;
	case VTYPE_DOUBLE:
		msg << value.DoubleVal(0);
		break;
	case VTYPE_STRING:
		msg << value.StringVal(0);
		break;
	case VTYPE_WIDESTR:
		msg << value.WideStrVal(0);
		break;
	case VTYPE_OBJECT:
		msg << value.ObjectVal(0);
		break;
	default:
		break;
	}

	return pKernel->SendPublicMessage(msg);
}

int PubModule::GetPubRecColType(IKernel * pKernel, const char * pubspace, const wchar_t * pubdata, const char * rec, int keycol)
{
	if (StringUtil::CharIsNull(pubspace) 
		|| StringUtil::CharIsNull(pubdata) 
		|| StringUtil::CharIsNull(rec))
	{
		return VTYPE_UNKNOWN;
	}

	IPubSpace* pPubSpace = pKernel->GetPubSpace(pubspace);
	if (pPubSpace == NULL)
	{
		return VTYPE_UNKNOWN;
	}

	IPubData* pPubData = pPubSpace->GetPubData(pubdata);
	if (pPubData == NULL)
	{
		return VTYPE_UNKNOWN;
	}

	IRecord* pRecord = pPubData->GetRecord(rec);
	if (pRecord == NULL)
	{
		return VTYPE_UNKNOWN;
	}
	
	return pRecord->GetColType(keycol);
}