//--------------------------------------------------------------------
// 文件名:		ReadIni.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年1月30日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------

#ifndef __UTIL_INI_H__
#define __UTIL_INI_H__
#include "../public/Macros.h"
#include "../public/IVarList.h"
#include "extend_func.h"

#include <vector>
#include <string>

// CUtilIni 
// 读取配置文件

class CUtilIni
{
private:
	struct item_t
	{
		const char* strName;
		unsigned int nHash;
		const char* strValue;
	};
	
	struct section_t
	{
		const char* strName;
		unsigned int nHash;
		size_t nItemStart;
		size_t nItemCount;
	};

public:
	enum { READ_RAW_FILE = 1 };

	static CUtilIni* NewInstance(const char* filename);
	
public:
	CUtilIni();
	CUtilIni(const char* filename);
	~CUtilIni();

	// 删除
	void Release();

	// 设置文件名
	void SetFileName(const char* filename);
	// 获得文件名
	const char* GetFileName() const;
	// 加载文件
	bool LoadFromFile();

	// 查找段
	bool FindSection(const char* section) const;
	// 查找段的索引值
	bool FindSectionIndex(const char* section, size_t& index) const;
	// 查找指定段中的键索引值
	bool FindSectionItemIndex(size_t sect_index, const char* key, 
		size_t& item_index) const;
	// 获得段的数量
	size_t GetSectionCount() const;
	// 获得指定段名
	const char* GetSectionByIndex(size_t index) const;
	// 获得段名列表
	//size_t GetSectionList(IVarList& result) const;
	// 获得指定段下的键数量
	size_t GetSectionItemCount(size_t sect_index) const;
	// 获得指定段下的指定键名
	const char* GetSectionItemKey(size_t sect_index, size_t item_index) const;
	// 获得指定段下的指定键值
	const char* GetSectionItemValue(size_t sect_index, size_t item_index) const;

	// 获得段下所有指定键的值列表
	size_t GetItemValueList(size_t sect_index, const char* key,
		IVarList& result) const;

	// 读取键数据，当键不存在的时候返回缺省值(def)
	int ReadInteger(size_t sect_index, const char* key, int def) const;
	char* ReadString(size_t sect_index, const char* key, char* value, 
		size_t maxlen) const;
	const char* ReadString(size_t sect_index, const char* key, 
		const char* def) const;
	float ReadFloat(size_t sect_index, const char* key, float def) const;

    size_t GetSectionList(IVarList& result) const;
	// 查找段下的键
	bool FindItem(const char* section, const char* key) const;
	// 获得段下所有键的数量
	size_t GetItemCount(const char* section) const;
	// 获得段下所有键名列表
	size_t GetItemList(const char* section, IVarList& result) const;
	// 获得段下所有指定键的值列表
	size_t GetItemValueList(const char* section, const char* key,
		IVarList& result) const;
	
	// 读取键数据，当键不存在的时候返回缺省值(def)
	int ReadInteger(const char* section, const char* key, int def) const;
	char* ReadString(const char* section, const char* key, char* value, 
		size_t maxlen) const;
	const char* ReadString(const char* section, const char* key, 
		const char* def) const;
	float ReadFloat(const char* section, const char* key, float def) const;

private:
	const char* GetData(const char* section, const char* key) const;
    const char* GetData(const size_t sect_index, const char* key) const;

	const CUtilIni::section_t* GetSection(const char* name) const;
	const CUtilIni::item_t* GetItem(const section_t* pSection, 
		const char* name) const;

private:
	fast_string m_strFileName;
	char* m_pContent;
	std::vector<section_t> m_Sections;
	std::vector<item_t> m_Items;
};

#endif // __UTIL_INI_H__
