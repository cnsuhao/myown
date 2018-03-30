//--------------------------------------------------------------------
// 文件名:       StaticDataQueryModule.cpp
// 内  容:        实体属性查询
// 说  明:
// 创建日期:    2010年6月21日
// 创建人:       huangmw@snailgame.net
//    :       
//--------------------------------------------------------------------
#include "StaticDataQueryModule.h"
#include "utils/util_ini.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include "utils/XmlFile.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "CommonModule/ReLoadConfigModule.h"
#endif

static const char* DEFAULT_STRING = "";

IKernel* StaticDataQueryModule::m_pKernel = NULL;

//类型名数组，用于错误信息
static const char* TYPE_NAME[VTYPE_MAX] =
{
    "VTYPE_UNKNOWN", //VTYPE_UNKNOWN,   // 未知
    "VTYPE_BOOL", //VTYPE_BOOL,     // 布尔
    "VTYPE_INT", //VTYPE_INT,       // 32位整数
    "VTYPE_INT64", //VTYPE_INT64,   // 64位整数
    "VTYPE_FLOAT", //VTYPE_FLOAT,   // 单精度浮点数
    "VTYPE_DOUBLE", //VTYPE_DOUBLE, // 双精度浮点数
    "VTYPE_STRING", //VTYPE_STRING, // 字符串
    "VTYPE_WIDESTR", //VTYPE_WIDESTR,   // 宽字符串
    "VTYPE_OBJECT", //VTYPE_OBJECT, // 对象号
    "VTYPE_POINTER", //VTYPE_POINTER,   // 指针
    "VTYPE_USERDATA", //VTYPE_USERDATA, // 用户数据
    //"VTYPE_TABLE" //VTYPE_TABLE,  // 表
};

StaticDataQueryModule* StaticDataQueryModule::m_pInstance = NULL;

//----------------------------------------------------------------------
//服务端脚本函数

// 原型：bool nx_reload_all_static_data()
// 功能：重新加载所有的静态数据表
int nx_reload_all_static_data(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    if (StaticDataQueryModule::m_pInstance != NULL)
    {
        pKernel->LuaPushBool(state, StaticDataQueryModule::m_pInstance->LoadResource());
    }
    else
    {
        pKernel->LuaPushBool(state, false);
    }

    return 1;
}

// 原型：bool nx_reload_static_data(int iClassType)
// 功能：重新加载指定的静态数据表
int nx_reload_static_data(void* state)
{
    IKernel* pKernel = LuaExtModule::GetKernel(state);

    // 检查参数数量
    CHECK_ARG_NUM(state, nx_reload_static_data, 1);

    // 检查参数类型
    CHECK_ARG_INT(state, nx_reload_static_data, 1);

    int iClassType = pKernel->LuaToInt(state, 1);

    if (StaticDataQueryModule::m_pInstance != NULL)
    {
        pKernel->LuaPushBool(state, StaticDataQueryModule::m_pInstance->ReloadStaticData(iClassType));
    }
    else
    {
        pKernel->LuaPushBool(state, false);
    }

    return 1;
}

bool StaticDataQueryModule::Init(IKernel* pKernel)
{
    m_pKernel = pKernel;

    DECL_LUA_EXT(nx_reload_all_static_data);
    DECL_LUA_EXT(nx_reload_static_data);

    m_pInstance = this;
#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("StaticDataConfig", StaticDataQueryModule::ReloadConfig);
#endif
    // 加载资源
    LoadResource();

    return true;
}

bool StaticDataQueryModule::Shut(IKernel* pKernel)
{
    FreeResource();
    return true;
}

//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
//重新加载指定的静态属性表
bool StaticDataQueryModule::ReloadStaticData(const int iClassType)
{
    //获取静态属性类型的字符串名
    char buff[32];
    SPRINTF_S(buff, "%d", iClassType);
    const char* pTypeName = buff;

    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return false;
    }

    //判断静态数据类型是否有效
    if (iIndex >= m_vecStaticTable.size())
    {
        ::extend_warning(LOG_ERROR,
                         "[StaticDataQueryModule::ReloadStaticData] "
                         "iClassType is valid : %s",
                         pTypeName);
        return false;
    }

    //获得配置文件路径
    std::string config_path = GetResourcePath();

    config_path += STATIC_DATA_FILE;

    //判断是否加载失败
    INI ini(config_path.c_str());

    if (!ini.LoadFromFile())
    {
        ::extend_warning(LOG_ERROR,
                         "[StaticDataQueryModule::ReloadStaticData] No found file : %s",
                         config_path.c_str());
        return false;
    }

    //先执行清理
    FreeStaticData(iClassType);

    //重新分配内存
    m_vecStaticTable[iIndex] = new StaticTable;

    //取静态数据表的路径
    std::string path = ini.ReadString(buff, "Resource", "");

    //重新加载静态表
    return LoadStaticData(iClassType, path.c_str());
}


// 直接通过列属性查，如果只是单次查询，则直接用列名查即可
// 查询数据列是否存在表中,失败返回-1, 成功返回列序号（从0开始）
int StaticDataQueryModule::FindCol(const int iClassType, const char* pColName)
{
    //指针非空
    if (pColName == NULL)
    {
        return -1;
    }

    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return -1;
    }

    //判断静态数据类型是否有效
    //if (iClassType < 0 || iClassType >= static_cast<int>(m_ColIndexMaps.size()))
    if (iIndex >= m_vecStaticTable.size())
    {
#ifdef _DEBUG
        //输出错误信息
        ::extend_warning(LOG_ERROR,
                         "[StaticDataQueryModule::FindCol] "
                         "iClassType is invalid: [%d] pColName: [%s]",
                         iClassType, pColName);
#endif

        return -1;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    if (NULL == pStaticTable)
    {
        return -1;
    }

    //通过属性名找列索引
    size_t iColIndex;

    if (!pStaticTable->colIndexList.GetData(pColName, iColIndex))
    {
        //没有找到该属性
        return -1;
    }

    //返回属性名对应的列索引
    return int(iColIndex);
}

//查找属性名对应的数据类型
int StaticDataQueryModule::GetType(const int iClassType, const char* pColName)
{
    //指针非空
    if (pColName == NULL)
    {
        return VTYPE_UNKNOWN;
    }

    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return VTYPE_UNKNOWN;
    }

    //判断静态数据类型是否有效
    if (iIndex >= m_vecStaticTable.size())
    {
#ifdef _DEBUG
        //输出错误信息
        ::extend_warning(LOG_ERROR,
                         "[StaticDataQueryModule::GetType] "
                         "iClassType is invalid: [%d] pColName: [%s]",
                         iClassType, pColName);
#endif

        return VTYPE_UNKNOWN;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    if (NULL == pStaticTable)
    {
        return VTYPE_UNKNOWN;
    }

    //通过属性名找列索引
    size_t col_index;

    if (!pStaticTable->colIndexList.GetData(pColName, col_index))
    {
        //没有找到该属性
        return VTYPE_UNKNOWN;
    }

    //判断列索引是否有效
    if (col_index >= pStaticTable->nColNum)
    {
#ifdef _DEBUG
        //输出错误信息
        ::extend_warning(LOG_WARNING,
                         "[StaticDataQueryModule::GetType]"
                         "col index is invalid: [%u] iClassType: [%d] pColName: [%s]",
                         col_index, iClassType, pColName);
#endif

        return VTYPE_UNKNOWN;
    }

    //返回类型表记录的数据类型
    return pStaticTable->pColTypes[col_index];
}

const char* StaticDataQueryModule::GetColName(const int iClassType, const int nCol)
{
    if (nCol < 0 ||
        nCol > GetDataCount(iClassType))
    {
        return "";
    }

    // 获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return "";
    }

    // 判断静态数据类型是否有效
    if (iIndex >= m_vecStaticTable.size())
    {
#ifdef _DEBUG
        //输出警告信息
        ::extend_warning(LOG_WARNING,
                         "[StaticDataQueryModule::GetColName]: iClassType is invalid: [%d] Col: [%d]",
                         iClassType, nCol);
#endif
        return "";
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];
    if (NULL == pStaticTable)
    {
        return "";
    }

    // 遍历找列名
    LoopBeginCheck(a);
    for (TStringPod<char, size_t>::iterator iter = pStaticTable->colIndexList.Begin();
         iter != pStaticTable->colIndexList.End();
         ++iter)
    {
        LoopDoCheck(a); 
        if ((int)iter.GetData() == nCol)
        {
            return iter.GetKey();
        }
    }

    return "";
}

// 查询数据整型
int StaticDataQueryModule::QueryInt(const int iClassType, const std::string& strConfigID, const char* pColName)
{
    StaticData* pResult = NULL;

    if (GetData(iClassType, strConfigID, pColName, &pResult, VTYPE_INT))
    {
        if (pResult)
        {
            if (pResult->JudgeEffective())
            {
                return pResult->iValue;
            }
        }
    }

    return 0;
}

int64_t StaticDataQueryModule::QueryInt64(const int iClassType, const std::string& strConfigID, const char* pColName)
{
    StaticData* pResult = NULL;

    if (GetData(iClassType, strConfigID, pColName, &pResult, VTYPE_INT64))
    {
        if (pResult)
        {
            if (pResult->JudgeEffective())
            {
                return pResult->i64Value;
            }
        }
    }

    return (int64_t)0;
}

// 查询数据浮点型
float StaticDataQueryModule::QueryFloat(const int iClassType, const std::string& strConfigID, const char* pColName)
{
    StaticData* pResult = NULL;

    if (GetData(iClassType, strConfigID, pColName, &pResult, VTYPE_FLOAT))
    {
        if (pResult)
        {
            if (pResult->JudgeEffective())
            {
                return pResult->fValue;
            }
        }
    }

    return 0.0f;
}

// 查询数据字符串型,失败返回""，成功返回字符串指针，可以认为是静态，不一定要付给std::string,只要不要改变内容即可
const char* StaticDataQueryModule::QueryString(const int iClassType, const std::string&strConfigID, const char* pColName)
{
    StaticData* pResult = NULL;

    if (GetData(iClassType, strConfigID, pColName, &pResult, VTYPE_STRING))
    {
        if (pResult)
        {
            if (pResult->JudgeEffective())
            {
                return pResult->pStrValue;
            }
        }
    }

    return DEFAULT_STRING;
}


// 直接通过列序号查询数据,多次查询时，可以用列序号优化，省去MAP列过程.获取数据列类型
int StaticDataQueryModule::GetType(const int iClassType, const std::string& strConfigID, const int col)
{
    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return VTYPE_UNKNOWN;
    }

    //静态数据类型有效
    if (iIndex >= m_vecStaticTable.size())
    {
#ifdef _DEBUG
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::GetType] "
            "iClassType is invalid: [%d] index_row: [%s] col: [%d]",
            iClassType, strConfigID.c_str(), col);
#endif

        return VTYPE_UNKNOWN;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    if (NULL == pStaticTable)
    {
        return VTYPE_UNKNOWN;
    }

    //列索引有效
    if (size_t(col) >= pStaticTable->nColNum)
    {
#ifdef _DEBUG
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::GetType] "
            "col is invalid: [%d] iClassType: [%d] index_row: [%s]",
            col, iClassType, strConfigID.c_str());
#endif

        return VTYPE_UNKNOWN;
    }

    return pStaticTable->pColTypes[col];
}

// 查询数据整型
int StaticDataQueryModule::QueryInt(const int iClassType, const std::string& strConfigID, const int col)
{
    StaticData* pResult = NULL;

    if (GetData(iClassType, strConfigID, col, &pResult, VTYPE_INT))
    {
        if (pResult)
        {
            if (pResult->JudgeEffective())
            {
                return pResult->iValue;
            }
        }
    }

    return 0;
}

int64_t StaticDataQueryModule::QueryInt64(const int iClassType, const std::string& strConfigID, const int col)
{
    StaticData* pResult = NULL;

    if (GetData(iClassType, strConfigID, col, &pResult, VTYPE_INT64))
    {
        if (pResult)
        {
            if (pResult->JudgeEffective())
            {
                return pResult->i64Value;
            }
        }
    }

    return (int64_t)0;
}

// 查询数据浮点型
float StaticDataQueryModule::QueryFloat(const int iClassType, const std::string& strConfigID, const int col)
{
    StaticData* pResult = NULL;

    if (GetData(iClassType, strConfigID, col, &pResult, VTYPE_FLOAT))
    {
        if (pResult)
        {
            if (pResult->JudgeEffective())
            {
                return pResult->fValue;
            }
        }
    }

    return 0.0f;
}

// 查询数据字符串型,失败返回NULL，成功返回字符串指针，可以认为是静态，不一定要付给std::string,只要不要改变内容即可
const char* StaticDataQueryModule::QueryString(const int iClassType, const std::string& strConfigID, const int col)
{
    StaticData* pResult = NULL;

    if (GetData(iClassType, strConfigID, col, &pResult, VTYPE_STRING))
    {
        if (pResult)
        {
            if (pResult->JudgeEffective())
            {
                return pResult->pStrValue;
            }
        }
    }

    return DEFAULT_STRING;
}

//查询当前数据定义的count
int StaticDataQueryModule::GetDataCount(const int iClassType)
{
    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return 0;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];
    if (NULL == pStaticTable)
    {
        return 0;
    }

    int count = (int)pStaticTable->colIndexList.GetCount();//属性数


    return count;
}

//查询当前数据的最大行数
size_t StaticDataQueryModule::GetMaxRows(const int iClassType)
{
    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return 0;
    }


    StaticTable* pStaticTable = m_vecStaticTable[iIndex];
    if (NULL == pStaticTable)
    {
        return 0;
    }

    return  pStaticTable->vecRowData.size();
}


bool StaticDataQueryModule::GetOneRowInData(const int iClassType, const std::string& strConfigID, 
											StaticData** result, size_t& resultnum)
{
    size_t iIndex = 0;
    if (!GetIndexUseClassType(iClassType, iIndex))
    {
        return false;
    }
    //判断静态数据类型是否有效
    if (iIndex >= m_vecStaticTable.size())
    {
#ifdef _DEBUG
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::GetOneRowInData] "
            "iClassType is invalid: [%d] index_row: [%s] col",
            iClassType, strConfigID.c_str());
#endif

        return false;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    if (NULL == pStaticTable)
    {
        return false;
    }

    if (!pStaticTable->GetIndexUseRowIndex(strConfigID, iIndex))
    {
#ifdef _DEBUG
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::GetOneRowInData] "
            "strConfigID is invalid: [%d] index_row: [%s] col",
            iClassType, strConfigID.c_str());
#endif
        return false;
    }
    //判断行索引是否有效
    if (iIndex >= pStaticTable->vecRowData.size())
    {
#ifdef _DEBUG
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::GetOneRowInData] "
            "iIndex is invalid: [%d] index_row: [%s] col",
            iClassType, strConfigID.c_str());
#endif
        return false;
    }

    *result = pStaticTable->vecRowData[iIndex];
    resultnum = pStaticTable->colIndexList.GetCount();
    return true;
}

bool StaticDataQueryModule::GetOneRowData(const int iClassType, const std::string& strConfigID, 
										  IVarList& propNameList, IVarList& propValueList, int defType/* = VTYPE_INT*/)
{
	size_t	nDataSize = 0;
	StaticData * pDataList = NULL;

	GetOneRowInData(iClassType, strConfigID, &pDataList, nDataSize);
	const TStringPod<char, size_t>* pNameList = GetColNameToColIndex(iClassType);

	if (NULL == pDataList || NULL == pNameList)
	{
		return false;
	}

	int ival = 0;
	int64_t i64val = 0;
	float fval = 0.0f;
    
	// 遍历找列名
    LoopBeginCheck(b);
	for (TStringPod<char, size_t>::iterator iter = pNameList->Begin();
		iter != pNameList->End(); ++iter)
	{
        LoopDoCheck(b); 
		const char * prop = iter.GetKey();
		size_t index = iter.GetData();

		if (index >= nDataSize)
		{
			continue;
		}

		propNameList << prop;
		
		ival = 0;
		i64val = 0;
		fval = 0.0f;

		if (pDataList[index].JudgeEffective())
		{
			ival = pDataList[index].iValue;
			i64val = pDataList[index].i64Value;
			fval = pDataList[index].fValue;
		}

		if (defType == VTYPE_FLOAT)
		{
			propValueList<< fval;
		}
		else if (defType == VTYPE_INT)
		{
		    propValueList<< ival;
		}
		else if (defType == VTYPE_INT64)
		{
			propValueList<< i64val;
		}
	}

	return true;
}

bool StaticDataQueryModule::GetOneRowDataStr(const int iClassType, const std::string& strConfigID, 
											 IVarList& propNameList, IVarList& propValueList)
{
	size_t	nDataSize = 0;
	StaticData * pDataList = NULL;

	GetOneRowInData(iClassType, strConfigID, &pDataList, nDataSize);
	const TStringPod<char, size_t>* pNameList = GetColNameToColIndex(iClassType);

	if (NULL == pDataList || NULL == pNameList)
	{
		return false;
	}

	// 遍历找列名
    LoopBeginCheck(c);
	for (TStringPod<char, size_t>::iterator iter = pNameList->Begin();
		iter != pNameList->End(); ++iter)
	{
        LoopDoCheck(c);
		const char * prop = iter.GetKey();
		size_t index = iter.GetData();

		if (index >= nDataSize)
		{
			continue;
		}

		if (pDataList[index].JudgeEffective())
		{
			const char* szVal = pDataList[index].pStrValue;

			propNameList << prop;
			propValueList<< szVal;
		}
	}

	return true;
}

const TStringPod<char, size_t>* StaticDataQueryModule::GetColNameToColIndex(const int iClassType)
{
    size_t iIndex = 0;
    if (!GetIndexUseClassType(iClassType, iIndex))
    {
        return false;
    }
    return &m_vecStaticTable[iIndex]->colIndexList;
}

// 查询数据整型
int  StaticDataQueryModule::QueryInt(const int iClassType, int  strConfigID, const char* pColName)
{
    return QueryInt(iClassType, util_int64_as_string(strConfigID).c_str(), pColName);
}

int64_t StaticDataQueryModule::QueryInt64(const int iClassType, int  strConfigID, const char* pColName)
{
    return QueryInt64(iClassType, util_int64_as_string(strConfigID).c_str(), pColName);
}

// 查询数据浮点型
float  StaticDataQueryModule::QueryFloat(const int iClassType, int strConfigID, const char* pColName)
{
    return QueryFloat(iClassType, util_int64_as_string(strConfigID).c_str(), pColName);
}

// 查询数据字符串型,失败返回""，成功返回字符串指针，可以认为是静态，
// 不一定要付给std::string,只要不要改变内容即可
const char*  StaticDataQueryModule::QueryString(const int iClassType, int strConfigID, const char* pColName)
{
    return QueryString(iClassType, util_int64_as_string(strConfigID).c_str(), pColName);
}

// 查询数据整型
int StaticDataQueryModule::QueryInt(const int iClassType, int strConfigID, const int col)
{
    return QueryInt(iClassType, util_int64_as_string(strConfigID).c_str(), col);
}

int64_t StaticDataQueryModule::QueryInt64(const int iClassType, int strConfigID, const int col)
{
    return QueryInt64(iClassType, util_int64_as_string(strConfigID).c_str(), col);
}

// 查询数据浮点型
float StaticDataQueryModule::QueryFloat(const int iClassType, int strConfigID, const int col)
{
    return QueryFloat(iClassType, util_int64_as_string(strConfigID).c_str(), col);
}

// 查询数据字符串型,失败返回NULL，成功返回字符串指针，可以认为是静态，
// 不一定要付给std::string,只要不要改变内容即可
const char* StaticDataQueryModule::QueryString(const int iClassType, int strConfigID, const int col)
{
    return QueryString(iClassType, util_int64_as_string(strConfigID).c_str(), col);
}

// 直接通过列序号查询数据,多次查询时，可以用列序号优化，省去MAP列过程
// 获取数据列类型
int StaticDataQueryModule::GetType(const int iClassType, int strConfigID, const int col)
{
    return GetType(iClassType, util_int64_as_string(strConfigID).c_str(), col);
}
//////////////////////////////////////////////////////////////////////////







//////////////////////////////////////////////////////////////////////////
//取得指定表指定行指定属性的数据
bool StaticDataQueryModule::GetData(const int iClassType, const std::string& strConfigID, const char* pColName, StaticData** ppVar, int data_type)
{
    //指针非空
    if (pColName == NULL || ppVar == NULL)
    {
        return false;
    }

    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return false;
    }

    //判断静态数据类型是否有效
    if (iIndex >= m_vecStaticTable.size())
    {
#ifdef _DEBUG
        //非法静态表，输出错误信息
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::GetType] "
            "iClassType is invalid: [%d] index_row: [%s] pColName: [%s]",
            iClassType, strConfigID.c_str(), pColName);
#endif

        return false;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    if (NULL == pStaticTable)
    {
        return false;
    }

    //获取DataRows数组索引
    bFind = pStaticTable->GetIndexUseRowIndex(strConfigID, iIndex);
    if (!bFind)
    {
        return false;
    }

    //判断行索引是否有效
    if (iIndex >= pStaticTable->vecRowData.size())
    {
#ifdef _DEBUG
        //非法行索引，输出错误信息
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::GetType]"
            "index_row is invalid: [%s] iClassType: [%d] pColName: [%s]",
            strConfigID.c_str(), iClassType, pColName);
#endif

        return false;
    }

    size_t iColIndex;

    if (!pStaticTable->colIndexList.GetData(pColName, iColIndex))
    {
        //没有找到该属性
        return false;
    }

    //判断列索引是否有效
    if (iColIndex >= pStaticTable->nColNum)
    {
#ifdef _DEBUG
        //非法列索引，输出错误信息
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::GetType]"
            "col index is invalid: [%u] iClassType: [%d] index_row: [%d] pColName: [%s]",
            iColIndex, iClassType, iColIndex, pColName);
#endif

        return false;
    }

    //判断请求的数据类型与静态表中的类型是否匹配
    int iPropType = GetType(iClassType, strConfigID, (int)iColIndex);

    if (iPropType != data_type)
    {
#ifdef _DEBUG
        //数据类型不匹配，输出错误
        if (iPropType < VTYPE_MAX && data_type < VTYPE_MAX)
        {
            ::extend_warning(
                LOG_WARNING,
                "[StaticDataQueryModule::GetData] Type Unmatch: "
                "iClassType : %d index_row : %s pColName : %s"
                "col type is [%s] not [%s]",
                iClassType,
                strConfigID.c_str(),
                pColName,
                TYPE_NAME[iPropType],
                TYPE_NAME[data_type]);
        }
        else
        {
            ::extend_warning(
                LOG_WARNING,
                "[StaticDataQueryModule::GetData] Type Unmatch: "
                "iClassType : %d index_row : %s pColName : %s"
                "col type is [%d] not [%d]",
                iClassType,
                strConfigID.c_str(),
                pColName,
                iPropType,
                data_type);
        }
#endif

        return false;
    }

    //设置查找到的数据
    *ppVar = &pStaticTable->vecRowData[iIndex][iColIndex];

    return true;
}

//取得指定表指定行指定列的数据
bool StaticDataQueryModule::GetData(const int iClassType, const std::string& strConfigID, const int col, StaticData** ppVar, int data_type)
{
    //指针非空
    if (ppVar == NULL)
    {
        return false;
    }

    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return false;
    }

    //静态数据类型有效
    if (iIndex >= m_vecStaticTable.size())
    {
#ifdef _DEBUG
        ::extend_warning(
            LOG_WARNING,
            "[StaticDataQueryModule::GetData] "
            "iClassType is invalid: [%d] index_row: [%s] col: [%d]",
            iClassType, strConfigID.c_str(), col);
#endif

        return false;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    if (NULL == pStaticTable)
    {
        return false;
    }

    //获取DataRows数组索引
    bFind = pStaticTable->GetIndexUseRowIndex(strConfigID, iIndex);
    if (!bFind)
    {
        return false;
    }

    //行索引有效
    if (iIndex >= pStaticTable->vecRowData.size())
    {
#ifdef _DEBUG
        ::extend_warning(
            LOG_WARNING,
            "[StaticDataQueryModule::GetData]"
            "index_row is invalid: [%s] iClassType: [%d] col: [%d]",
            strConfigID.c_str(), iClassType, col);
#endif

        return false;
    }

    //列索引有效
    if (size_t(col) >= pStaticTable->nColNum)
    {
#ifdef _DEBUG
        ::extend_warning(
            LOG_WARNING,
            "[StaticDataQueryModule::GetData]"
            "col is invalid: [%d] iClassType: [%d] index_row: [%s]",
            col, iClassType, strConfigID.c_str());
#endif

        return false;
    }

    //类型匹配
    int expect_type = GetType(iClassType, strConfigID, col);

    if (expect_type != data_type)
    {
#ifdef _DEBUG
        if (expect_type < VTYPE_MAX && data_type < VTYPE_MAX)
        {
            ::extend_warning(
                LOG_WARNING,
                "[StaticDataQueryModule::GetData] Type Unmatch: "
                "iClassType : [%d] index_row : [%s] col : [%d] "
                "col type is [%s] not [%s]",
                iClassType,
                strConfigID.c_str(),
                col,
                TYPE_NAME[expect_type],
                TYPE_NAME[data_type]);
        }
        else
        {
            ::extend_warning(
                LOG_WARNING,
                "[StaticDataQueryModule::GetData] Type Unmatch: "
                "iClassType : [%d] index_row : [%s] col : [%d] "
                "col type is [%d] not [%d]",
                iClassType,
                strConfigID.c_str(),
                col,
                expect_type,
                data_type);
        }
#endif

        return false;
    }

    *ppVar = &pStaticTable->vecRowData[iIndex][col];

    return true;
}

//载入资源
bool StaticDataQueryModule::LoadResource()
{
    //先执行清理
    FreeResource();

    //获得路径
    std::string strConfigPath =  GetResourcePath();

    strConfigPath += STATIC_DATA_FILE;

    //加载失败
    CXmlFile ini(strConfigPath.c_str());
    if (!ini.LoadFromFile())
    {
        extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::LoadResource] No found file: [%s]",
            strConfigPath.c_str());
        return false;
    }

    //读取所有的静态数据表文件

    CVarList sectionList;
    ini.GetSectionList(sectionList);

    //设置默认的表格数
    size_t nClassNum = sectionList.GetCount();

    m_vecStaticTable.resize(nClassNum, NULL);


    LoopBeginCheck(d);
    for (size_t i = 0; i < nClassNum; ++i)
    {
        LoopDoCheck(d);
        int iClassType = CONVERT_INT(sectionList.StringVal(i), 0);

        if (iClassType < 0)
        {
            //出错日志
            ::extend_warning(
                LOG_ERROR,
                "[StaticDataQueryModule::LoadResource] iClassType is valid : %s",
                sectionList.StringVal(i));
            continue;
        }

        m_vecStaticTable[i] = new StaticTable;

        //取静态数据表的路径
        std::string strFilePath = ini.ReadString(sectionList.StringVal(i), "Resource", "");


        //类型(是否需要加载)
        std::string strType = ini.ReadString(sectionList.StringVal(i), "Type", "share");
        if (strType == "client")
        {
            ::extend_warning(LOG_INFO, "[StaticDataQueryModule::LoadResource] %s only client need load", strFilePath.c_str());

            continue;
        }

        //类型配置错误了
        if (strType != "client" &&  strType !=  "share" && strType != "server")
        {
            ::extend_warning(LOG_ERROR, "[StaticDataQueryModule::LoadResource] File:%s Type:%s Error", strFilePath.c_str(), strType.c_str());
        }


        //设置iClassType和m_StaticTables中索引对应关系
        SetIndexUseClassType(iClassType, i);

        //载入静态数据表
        LoadStaticData(iClassType, strFilePath.c_str());
    }

    return true;
}

//载入静态数据表
bool StaticDataQueryModule::LoadStaticData(const int iClassType, const char* path)
{
    // 获得路径
    std::string strFilePath = GetResourcePath();
    strFilePath += path;


    //加载失败
    INI ini(strFilePath.c_str());

    std::string log;
    if (!ini.LoadFromFile(log))
    {
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::LoadStaticData] %s",
            log.c_str());
        return false;
    }

    //处理定义属性索引的段，段名[-1]
    bool bIndex = LoadIndexDefineSection(iClassType, path, ini);

    //处理定义数据类型的段，段名[0]
    if (!LoadColTypeDefineSection(iClassType, path, ini, bIndex))
    {
        ::extend_warning(
            LOG_ERROR,
            "[StaticDataQueryModule::LoadStaticData] "
            "No found section : [0] FileName: [%s]",
            path);
        return false;
    }

    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return false;
    }

    //开始读取静态数据表
    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    //先把ini 的段数量设为表的行数量
    size_t section_count = ini.GetSectionCount();
    size_t row_num = section_count;
    size_t col_num = pStaticTable->nColNum;
    int* colTypes = pStaticTable->pColTypes;

    //设置表的行数量
    std::vector<StaticData*>& vecDataRow = pStaticTable->vecRowData;

    // 初始化表行数据
    vecDataRow.resize(row_num, NULL);

    LoopBeginCheck(e);
    for (size_t r = 0; r < row_num; ++r)
    {
        LoopDoCheck(e);
        StaticData* pRowData = new StaticData[col_num];

        LoopBeginCheck(f);
        for (size_t icol = 0; icol < col_num; ++icol)
        {
            LoopDoCheck(f);
            pRowData[icol].iValue = _INVALID_VALUE;
        }

        vecDataRow[r] = pRowData;
    }

    //循环处理每个段
    LoopBeginCheck(g);
    for (size_t i = 0; i < section_count; ++i)
    {
        LoopDoCheck(g);
        const char* section_name = ini.GetSectionByIndex(i);

        if ((strcmp(section_name, "-1") == 0)
            || (strcmp(section_name, "0") == 0))
        {
            //定义索引和类型的段不处理
            continue;
        }

        std::string iRowIndex = section_name;

        //设置index_row和DataRows中索引对应关系
        if (!pStaticTable->SetIndexUseRowIndex(iRowIndex, i))
        {
            //iRowIndex重复，输出错误
            ::extend_warning(
                LOG_WARNING,
                "[StaticDataQueryModule::SetIndexUseRowIndex] "
                "Repeat(iClassType : [%d], RowIndex : [%s])",
                iClassType, iRowIndex.c_str());
        }

        StaticData* pRowData = vecDataRow[i];

        size_t item_num = ini.GetSectionItemCount(i);

        LoopBeginCheck(h);
        for (size_t k = 0; k < item_num; ++k)
        {
            LoopDoCheck(h);
            const char* key = ini.GetSectionItemKey(i, k);
            const char* val = ini.GetSectionItemValue(i, k);

            //设置列的数据
            size_t iColIndex;

            if (!pStaticTable->colIndexList.GetData(key, iColIndex))
            {
                ::extend_warning(
                    LOG_ERROR,
                    "[StaticDataQueryModule::LoadStaticData] "
                    "No found col_index : [%s] FileName: [%s]",
                    key, path);
                continue;
            }

            if (iColIndex >= col_num)
            {
                ::extend_warning(
                    LOG_ERROR,
                    "[StaticDataQueryModule::LoadStaticData]"
                    "col_index not in order : [%s] FileName: [%s]",
                    key, path);
                continue;
            }

            //根据类型设置值
            switch (colTypes[iColIndex])
            {
                case VTYPE_INT :
                {
                    pRowData[iColIndex].iValue = atoi(val);
                }
                break;
                case VTYPE_INT64 :
                    {
                        std::string str_val = val;
                        pRowData[iColIndex].i64Value = util_string_as_int64(str_val);
                    }
                    break;
                case VTYPE_FLOAT :
                {
                    pRowData[iColIndex].fValue = (float)atof(val);
                }
                break;
                case VTYPE_STRING :
                {
                    const size_t count = strlen(val) + 1;

                    pRowData[iColIndex].pStrValue = NEW char[count];

                    memcpy(pRowData[iColIndex].pStrValue, val, count);
                }
                break;
                default :
                {
                    const size_t count = strlen(val) + 1;

                    pRowData[iColIndex].pStrValue = NEW char[count];

                    memcpy(pRowData[iColIndex].pStrValue, val, count);
                }
                break;
            }
        }
    }

    return true;
}

//清理资源
bool StaticDataQueryModule::FreeResource()
{
    if (!m_mapClassTypeToIndex.empty())
    {
        std::map<int, size_t>::iterator itFind = m_mapClassTypeToIndex.begin();

        LoopBeginCheck(i);
        for (; itFind != m_mapClassTypeToIndex.end(); ++ itFind)
        {
            LoopDoCheck(i);
            int iClassType = itFind->first;
            FreeStaticData(iClassType);
        }

        m_vecStaticTable.clear();
        m_mapClassTypeToIndex.clear();
    }

    return true;
}

//清理静态属性表
bool StaticDataQueryModule::FreeStaticData(const int iClassType)
{
    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return false;
    }

    //判断静态数据类型是否有效
    if (iIndex >= m_vecStaticTable.size())
    {
        return false;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    if (NULL == pStaticTable)
    {
        return false;
    }

    int* col_types = pStaticTable->pColTypes;
    size_t col_num = pStaticTable->nColNum;
    std::vector<StaticData*>& data_rows = pStaticTable->vecRowData;

    LoopBeginCheck(j);
    for (size_t r = 0; r < data_rows.size(); ++r)
    {
        LoopDoCheck(j);
        LoopBeginCheck(k);
        for (size_t c = 0; c < col_num; ++c)
        {
            LoopDoCheck(k);
            if (col_types[c] == VTYPE_STRING)
            {
                //字符串类型的数据需删除堆空间
                if (data_rows[r][c].JudgeEffective())
                {
                    delete[] data_rows[r][c].pStrValue;
                    data_rows[r][c].iValue = _INVALID_VALUE;
                }
            }
        }

        delete[] data_rows[r];
        data_rows[r] = NULL;
    }

    delete(pStaticTable->pColTypes);
    pStaticTable->pColTypes = NULL;

    delete(pStaticTable);
    m_vecStaticTable[iIndex] = NULL;

    return true;
}

//载入定义属性索引的段
bool StaticDataQueryModule::LoadIndexDefineSection(const int iClassType, const char* path, const INI& ini)
{
    //处理定义列索引的段，段名[-1]
    size_t sect_index;

    if (!ini.FindSectionIndex("-1", sect_index))
    {
        return false;
    }

    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return false;
    }


    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    size_t item_num = ini.GetSectionItemCount(sect_index);

    LoopBeginCheck(l);
    for (size_t i = 0; i < item_num; ++i)
    {
        LoopDoCheck(l);
        const char* key = ini.GetSectionItemKey(sect_index, i);
        const char* val = ini.GetSectionItemValue(sect_index, i);

        int iColIndex = atoi(val);

        if (iColIndex < 0)
        {
            ::extend_warning(
                LOG_ERROR,
                "[StaticDataQueryModule::LoadIndexDefineSection]col_index is invalid : [%s] FileName: [%s]",
                key, path);
        }

        pStaticTable->colIndexList.Add(key, iColIndex);
    }

    return true;
}

// 载入定义列数据类型的段
bool StaticDataQueryModule::LoadColTypeDefineSection(const int iClassType, const char* path, const CXmlFile& ini, bool bIndex)
{
    //处理定义列数据类型的段，段名[0]
    size_t sect_index;

    if (!ini.FindSectionIndex("0", sect_index))
    {
        return false;
    }

    //获取m_StaticTables数组索引
    size_t iIndex = 0;
    bool bFind = GetIndexUseClassType(iClassType, iIndex);
    if (!bFind)
    {
        return false;
    }

    StaticTable* pStaticTable = m_vecStaticTable[iIndex];

    size_t item_num = ini.GetSectionItemCount(sect_index);
    size_t col_num = item_num;

    pStaticTable->nColNum = col_num;
    pStaticTable->pColTypes = new int[col_num];

    LoopBeginCheck(m);
    for (size_t i = 0; i < col_num; ++i)
    {
        LoopDoCheck(m);
        pStaticTable->pColTypes[i] = VTYPE_STRING;
    }

    LoopBeginCheck(n);
    for (size_t i = 0; i < item_num; ++i)
    {
        LoopDoCheck(n);
        const char* key = ini.GetSectionItemKey(sect_index, i);
        const char* val = ini.GetSectionItemValue(sect_index, i);

        int iColType = atoi(val);
        int iColIndex = int(i);

        // 列数如果和实际总的列总不一致则报错，但仍兼容
        if (bIndex)
        {
            //有[-1] 段的话，查找属性名对应的索引
            iColIndex = FindCol(iClassType, key);

            if (iColIndex < 0)
            {
                ::extend_warning(
                    LOG_ERROR,
                    "[StaticDataQueryModule::LoadStaticData] "
                    "No found col_index : [%s] FileName: [%s]",
                    key, path);
                continue;
            }
            else if (iColIndex >= (int)col_num)
            {
                ::extend_warning(
                    LOG_ERROR,
                    "[StaticDataQueryModule::LoadStaticData]"
                    "col_index not in order: [%s] FileName: [%s]",
                    key, path);

                size_t new_col_num = iColIndex + 1;

                int* col_types = new int[new_col_num];

                memcpy(col_types, pStaticTable->pColTypes,
                       sizeof(int) * col_num);

                LoopBeginCheck(o);
                for (size_t c = col_num; c < new_col_num; ++c)
                {
                    LoopDoCheck(o);
                    col_types[c] = VTYPE_UNKNOWN;
                }

                col_num = new_col_num;

                delete[] pStaticTable->pColTypes;
                pStaticTable->nColNum = col_num;
                pStaticTable->pColTypes = col_types;
                continue;
            }
        }
        else
        {
            // 没有[-1] 段的话，处理索引表
            pStaticTable->colIndexList.Add(key, iColIndex);
        }

        // 插入数据类型表
        pStaticTable->pColTypes[iColIndex] = iColType;
    }

    return true;
}

//获取资源路径
const char* StaticDataQueryModule::GetResourcePath()
{
    if (m_pKernel)
    {
        return m_pKernel->GetResourcePath();
    }

    return DEFAULT_STRING;
}

//设置iClassType和m_StaticTables中索引对应关系
bool StaticDataQueryModule::SetIndexUseClassType(int iClassType, size_t iIndex)
{
    std::map<int, size_t>::iterator itFind = m_mapClassTypeToIndex.find(iClassType);
    if (itFind != m_mapClassTypeToIndex.end())
    {
        //iClassType重复，输出错误
        ::extend_warning(
            LOG_WARNING,
            "[StaticDataQueryModule::SetIndexUseClassType]"
            "Repeat(iClassType : %d,iIndex : %d)",
            iClassType, iIndex);
        return false;
    }

    m_mapClassTypeToIndex[iClassType] = iIndex;
    return true;
}

//获取m_StaticTables数组索引
bool StaticDataQueryModule::GetIndexUseClassType(int iClassType, size_t& iIndex)
{
    std::map<int, size_t>::iterator itFind = m_mapClassTypeToIndex.find(iClassType);
    if (itFind == m_mapClassTypeToIndex.end())
    {
        return false;
    }

    iIndex = itFind->second;
    return true;
}

void StaticDataQueryModule::ReloadConfig(IKernel *pKernel)
{
	m_pInstance->LoadResource();
}

//////////////////////////////////////////////////////////////////////////