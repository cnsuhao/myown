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

#include "record.h"
#include "extend_func.h"
#include "string_util.h"

// 创建表格
bool _create_record(ISnsKernel* pKernel, 
					ISnsData* pPubData,
					const char* recName,
					int row_max, 
					const IVarList& col_types,
					const bool IsSave)
{
	//常规检查
	if (pPubData->FindRecord(recName))
	{
		bool ReBuild = false;
		IRecord* pRec = pPubData->GetRecord(recName);
		if (NULL == pRec)
		{
			ReBuild = true;
		}
		else
		{
			// 旧数据表列数不对，删除旧表
			if (pRec->GetRowMax() != row_max)
			{
				ReBuild = true;
			}

			if (pPubData->GetRecordCols(recName) != (int)col_types.GetCount())
			{
				ReBuild = true;
			}
			else
			{
				LoopBeginCheck(a)
				for (int i = 0; i< (int)col_types.GetCount(); ++i)
				{
					LoopDoCheck(a)
					int iType = pPubData->GetRecordColType(recName, i);
					if (iType != col_types.IntVal(i))
					{
						ReBuild = true;
					}
				}
			}
		}

		// 旧数据表列数不对，删除旧表
		if (ReBuild)
		{
			pPubData->RemoveRecord(recName);
			pPubData->AddRecord(recName, row_max, (int)col_types.GetCount(), col_types);
		}
	}
	else
	{
		pPubData->AddRecord(recName, row_max, (int)col_types.GetCount(), col_types);
	}

	pPubData->SetRecordSave(recName, IsSave);

	return true;
}

// 创建表格
bool _create_record(IPubKernel* pKernel, 
					IPubData* pPubData,
					const char* recName,
					int row_max, 
					const IVarList& col_types,
					const bool IsSave)
{
	//常规检查
	if (pPubData->FindRecord(recName))
	{
		bool ReBuild = false;
		IRecord* pRec = pPubData->GetRecord(recName);
		if (NULL == pRec)
		{
			ReBuild = true;
		}
		else
		{
			// 旧数据表列数不对，删除旧表
			if (pRec->GetRowMax() != row_max)
			{
				ReBuild = true;
			}

			if (pPubData->GetRecordCols(recName) != (int)col_types.GetCount())
			{
				ReBuild = true;
			}
			else
			{
				LoopBeginCheck(b)
				for (int i = 0; i< (int)col_types.GetCount(); ++i)
				{
					LoopDoCheck(b)
					int iType = pPubData->GetRecordColType(recName, i);
					if (iType != col_types.IntVal(i))
					{
						ReBuild = true;
					}
				}
			}
		}

		// 旧数据表列数不对，删除旧表
		if (ReBuild)
		{
			pPubData->RemoveRecord(recName);
			pPubData->AddRecord(recName, row_max, (int)col_types.GetCount(), col_types);
		}
	}
	else
	{
		pPubData->AddRecord(recName, row_max, (int)col_types.GetCount(), col_types);
	}

	pPubData->SetRecordSave(recName, IsSave);

	return true;
}
bool _create_pub_record_safe(IPubData* pPubData, const char* recordName,
	IVarList& cols, IVarList& newColDefaultValue, int rowMax, int recordKey, bool isSave)
{



	//常规检查
	if (pPubData->FindRecord(recordName))
	{
		change_pub_record(pPubData, recordName, cols, newColDefaultValue, rowMax, recordKey, isSave);
	}
	else
	{
		pPubData->AddRecord(recordName, rowMax, (int)cols.GetCount(), cols);
		if (recordKey != -1){
			pPubData->SetRecordKey(recordName, recordKey);
		}
		pPubData->SetRecordSave(recordName, isSave);
	}

	return true;


}

// 检测表结构是否改变
bool check_record_changed(IRecord* pRecord, IVarList& cols, int rowMax)
{
	if (pRecord == NULL)
	{
		return false;
	}

	// 最大行数改变
	if (pRecord->GetRowMax() != rowMax)
	{
		return true;
	}

	// 最大列数改变
	int colsCount = pRecord->GetCols();
	if ( colsCount != static_cast<int>(cols.GetCount()))
	{
		return true;
	}

	// 列数类型不一致
	LoopBeginCheck(a);
	for (int i = 0; i < colsCount; ++i)
	{
		LoopDoCheck(a);

		if (pRecord->GetColType(i) != cols.IntVal(i))
		{
			return true;
		}
	}

	return false;
}

// 判断表结构是否一致
bool is_same_record(IRecord* pDestRecord, IRecord* pSrcRecord)
{
	if (pDestRecord == NULL
		|| pSrcRecord == NULL)
	{
		return false;
	}

	// 最大行数
	if (pDestRecord->GetRowMax() != pSrcRecord->GetRowMax())
	{
		return false;
	}

	// 最大列数
	int colsCount = pDestRecord->GetCols();
	if ( colsCount != pSrcRecord->GetCols())
	{
		return false;
	}

	// 列数类型不一致
	LoopBeginCheck(a);
	for (int i = 0; i < colsCount; ++i)
	{
		LoopDoCheck(a);

		if (pDestRecord->GetColType(i) != pSrcRecord->GetColType(i))
		{
			return false;
		}
	}

	return true;
}

// 公共域表结构修改
// 只支持列后面增加新列
bool change_pub_record(IPubData* pPubData, const char* recordName, 
	IVarList& cols, IVarList& newColDefaultValue, int rowMax, int recordKey, bool isSave)
{
	if (pPubData == NULL
		|| StringUtil::CharIsNull(recordName))
	{
		return false;
	}

	// 原始表
	IRecord* pRecord = pPubData->GetRecord(recordName);
	if (pRecord == NULL)
	{
		return false;
	}

	// 表结构是否一致
	if (!check_record_changed(pRecord, cols, rowMax))
	{
		return true;
	}

	// 建立临时表
	std::string tempRecordName = "temp_";
	tempRecordName.append(recordName);
	if (!pPubData->AddRecord(tempRecordName.c_str(), 
		rowMax, static_cast<int>(cols.GetCount()), cols))
	{
		return false;
	}

	// 获取临时表
	IRecord* pTempRecord = pPubData->GetRecord(tempRecordName.c_str());
	if (pTempRecord == NULL)
	{
		return false;
	}

	// 将原始表数据转移到临时表中
	CVarList rowValueList;
	LoopBeginCheck(a);
	int rows = pRecord->GetRows();
	for (int i = 0; i < rows; ++i)
	{
		LoopDoCheck(a);

		// 取原始行数据
		rowValueList.Clear();
		pRecord->QueryRowValue(i, rowValueList);

		// 增加新数据
		rowValueList << newColDefaultValue;

		// 添加到临时表中
		pTempRecord->AddRowValue(-1, rowValueList);
	}

	// 删除原始表
	if (!pPubData->RemoveRecord(recordName))
	{
		return false;
	}

	// 重建原始表
	if (!pPubData->AddRecord(recordName, 
		rowMax, static_cast<int>(cols.GetCount()), cols))
	{
		return false;
	}

	// 重新获取原始表
	pRecord = pPubData->GetRecord(recordName);
	if (pRecord == NULL)
	{
		return false;
	}
	if (recordKey != -1){
		pPubData->SetRecordKey(recordName, recordKey);
	}
	pPubData->SetRecordSave(recordName, isSave);

	// 将临时表数据转移到新的表中
	LoopBeginCheck(b);
	rows = pTempRecord->GetRows();
	for (int i = 0; i < rows; ++i)
	{
		LoopDoCheck(b);

		// 取临时表行数据
		rowValueList.Clear();
		pTempRecord->QueryRowValue(i, rowValueList);

		// 添加到新表中
		pRecord->AddRowValue(-1, rowValueList);
	}

	// 删除临时表
	pPubData->RemoveRecord(tempRecordName.c_str());

	return true;
}
