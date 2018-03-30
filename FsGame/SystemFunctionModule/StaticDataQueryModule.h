//--------------------------------------------------------------------
// 文件名:      StaticDataQueryModule.h
// 内  容:      实体属性查询
// 说  明:
// 创建日期:    2010年6月21日
// 创建人:      huangmw@snailgame.net
//    :       
//--------------------------------------------------------------------
#ifndef STATICDATA_QUERY_MODULE_H
#define STATICDATA_QUERY_MODULE_H

class IVarList;
class CXmlFile;

#include "Fsgame/Define/header.h"
#include "utils/StringPod.h"
#include "utils/XmlFile.h"
#include <vector>
#include <string>

#define INI CXmlFile
#define CONVERT_INT convert_int
#define STATIC_DATA_FILE "ini/SystemFunction/static_data.xml";

class StaticDataQueryModule: public ILogicModule
{
public:
    // 初始化
    virtual bool Init(IKernel* pKernel);

    // 关闭
    virtual bool Shut(IKernel* pKernel);

    //重新加载指定的静态属性表
    bool ReloadStaticData(const int iClassType);

    // 直接通过列属性查，如果只是单次查询，则直接用列名查即可
    // 查询数据列是否存在表中,失败返回-1, 成功返回列序号（从0开始）
    int FindCol(const int iClassType, const char* pColName);

    // 获取数据列类型
    int GetType(const int iClassType, const char* pColName);

    // 查询数据列名
    const char* GetColName(const int iClassType, const int nCol);

    // 查询数据整型
    int QueryInt(const int iClassType, const std::string&  strConfigID, const char* pColName);
    int64_t QueryInt64(const int iClassType, const std::string&  strConfigID, const char* pColName);

    // 查询数据浮点型
    float QueryFloat(const int iClassType, const std::string& strConfigID, const char* pColName);

    // 查询数据字符串型,失败返回""，成功返回字符串指针，可以认为是静态，
    // 不一定要付给const std::string&,只要不要改变内容即可
    const char* QueryString(const int iClassType, const std::string& strConfigID, const char* pColName);

    // 直接通过列序号查询数据,多次查询时，可以用列序号优化，省去MAP列过程
    // 获取数据列类型
    int GetType(const int iClassType, const std::string& strConfigID, const int col);

    // 查询数据整型
    int QueryInt(const int iClassType, const std::string& strConfigID, const int col);
    int64_t QueryInt64(const int iClassType, const std::string& strConfigID, const int col);

    // 查询数据浮点型
    float QueryFloat(const int iClassType, const std::string& strConfigID, const int col);

    // 查询数据字符串型,失败返回NULL，成功返回字符串指针，可以认为是静态，
    // 不一定要付给const std::string&,只要不要改变内容即可
    const char* QueryString(const int iClassType, const std::string& strConfigID, const int col);

    //查询当前数据定义列数的count
    virtual int GetDataCount(const int iClassType);

    //查询当前数据的最大行数
    virtual size_t GetMaxRows(const int iClassType);

    //静态表中的数据类型
    struct StaticData;
    //获取配置文件中的一行数据,后两个参数为输出值
    bool GetOneRowInData(const int iClassType, const std::string& strConfigID, StaticData** result, size_t& resultnum);

	//获得配置文件中的一行数据,输出到CVarList中
	bool GetOneRowData(const int iClassType, const std::string& strConfigID, IVarList& propNameList, IVarList& propValueList, int defType = VTYPE_INT);

	//获得配置文件中的一行数据,输出到CVarList中
	bool GetOneRowDataStr(const int iClassType, const std::string& strConfigID, IVarList& propNameList, IVarList& propValueList);

    //获取配置文件的属性名和列号的索引信息
    const TStringPod<char, size_t>* GetColNameToColIndex(const int iClassType);


    //兼容////////////////////////////////////////////////////////////
    // 直接通过列序号查询数据,多次查询时，可以用列序号优化，省去MAP列过程
    // 获取数据列类型
    int GetType(const int iClassType, int strConfigID, const int col);

    // 查询数据整型
    int QueryInt(const int iClassType, int  strConfigID, const char* pColName);
    int64_t QueryInt64(const int iClassType, int  strConfigID, const char* pColName);

    // 查询数据浮点型
    float QueryFloat(const int iClassType, int strConfigID, const char* pColName);

    // 查询数据字符串型,失败返回""，成功返回字符串指针，可以认为是静态，
    // 不一定要付给const std::string&,只要不要改变内容即可
    const char* QueryString(const int iClassType, int strConfigID, const char* pColName);


    // 查询数据整型
    int QueryInt(const int iClassType, int strConfigID, const int col);
    int64_t QueryInt64(const int iClassType, int strConfigID, const int col);

    // 查询数据浮点型
    float QueryFloat(const int iClassType, int strConfigID, const int col);

    // 查询数据字符串型,失败返回NULL，成功返回字符串指针，可以认为是静态，
    // 不一定要付给const std::string&,只要不要改变内容即可
    const char* QueryString(const int iClassType, int strConfigID, const int col);

public:
    //载入资源
    bool LoadResource();

private:


    //清理资源
    bool FreeResource();

    //清理静态属性表
    bool FreeStaticData(const int iClassType);

    //载入静态数据表
    bool LoadStaticData(const int iClassType, const char* path);

    //载入定义属性索引的段
    bool LoadIndexDefineSection(const int iClassType, const char* path, const CXmlFile& ini);


    //载入定义列数据类型的段
    bool LoadColTypeDefineSection(const int iClassType, const char* path, const CXmlFile& ini, bool bIndex);

    //根据类型设置变量的默认值
    //bool SetDefaultVaule(StaticData &var, int type);

    //取得指定表指定行指定属性的数据
    bool GetData(const int iClassType, const std::string& strConfigID, const char* pColName, StaticData** ppVar, int data_type);

    //取得指定表指定行指定列的数据
    bool GetData(const int iClassType, const std::string& strConfigID, const int col, StaticData** ppVar, int data_type);

    //获取资源路径
    const char* GetResourcePath();

    //设置iClassType和m_StaticTables中索引对应关系
    bool SetIndexUseClassType(int iClassType, size_t iIndex);

    //获取m_StaticTables数组索引
    bool GetIndexUseClassType(int iClassType, size_t& iIndex);

	static void ReloadConfig(IKernel *pKernel);
public:
    //静态表中的数据类型
    enum {_INVALID_VALUE = INT_MAX};
    struct StaticData
    {
        union
        {
            int iValue;
            int64_t i64Value;
            float fValue;
            char* pStrValue;
        };
        bool JudgeEffective() const
        {
            if (iValue != _INVALID_VALUE)
            {
                return true;
            }
            return false;
        }
    };
private:
    // 静态数据表
    struct StaticTable
    {
        StaticTable()
        {
            nColNum = 0;
            pColTypes = NULL;
            mapConfigIDToIndex.clear();
        }

        size_t nColNum;
        int* pColTypes;
        TStringPod<char, size_t> colIndexList; //列名在表的列号
        std::vector<StaticData*> vecRowData;

        std::map<std::string, size_t> mapConfigIDToIndex;//index_row和DataRows中索引对应关系

        //设置index_row和DataRows中索引对应关系
        bool SetIndexUseRowIndex(const std::string& strConfigID, size_t iIndex)
        {
            std::map<std::string, size_t>::iterator itFind = mapConfigIDToIndex.find(strConfigID);
            if (itFind != mapConfigIDToIndex.end())
            {
                return false;
            }

            mapConfigIDToIndex[strConfigID] = iIndex;
            return true;
        }

        //获取DataRows数组索引
        bool GetIndexUseRowIndex(const std::string& strConfigID, size_t& iIndex)
        {
            std::map<std::string, size_t>::iterator itFind = mapConfigIDToIndex.find(strConfigID);
            if (itFind == mapConfigIDToIndex.end())
            {
                return false;
            }

            iIndex = itFind->second;
            return true;
        }
    };



private:
    std::vector<StaticTable*> m_vecStaticTable;

    //iClassType和m_StaticTables中索引对应关系
    std::map<int, size_t> m_mapClassTypeToIndex;

public:
    static StaticDataQueryModule* m_pInstance;

    static IKernel* m_pKernel;
};

#endif
