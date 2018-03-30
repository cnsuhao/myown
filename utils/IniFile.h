//--------------------------------------------------------------------
// 文件名:		IniFile.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年1月30日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------

#ifndef _UTILS_INIFILE_H
#define _UTILS_INIFILE_H

#include "../public/Macros.h"
#include "../public/IVarList.h"
#include "../public/FastStr.h"
#include "../utils/ArrayPod.h"

// 读写配置文件

class CIniFile
{
protected:
	struct item_t
	{
		size_t nNamePos;
		unsigned int nHash;
		size_t nValuePos;
	};

	struct section_t
	{
		size_t nNamePos;
		unsigned int nHash;
		size_t nItemStart;
		size_t nItemCount;
	};

public:
	enum { READ_RAW_FILE = 1 };

	static CIniFile* NewInstance(const char* filename);

public:
	CIniFile();
	CIniFile(const char* filename);
	CIniFile(const char* filename, int read_raw_file);
	~CIniFile();

	// 删除
	void Release();

	// 设置文件名
	void SetFileName(const char* filename);
	// 获得文件名
	const char* GetFileName() const;
	// 加载文件
	bool LoadFromFile();
	// 解析字符串
	bool LoadFromString(const char* str, size_t size);
	// 保存文件
	bool SaveToFile() const;

	// 添加段
	bool AddSection(const char* section);
	// 查找段
	bool FindSection(const char* section) const;
	// 删除段
	bool DeleteSection(const char* section);
	// 获得段的数量
	size_t GetSectionCount() const;
	// 获得段名列表
	size_t GetSectionList(IVarList& result) const;

	// 查找段下的键
	bool FindItem(const char* section, const char* key) const;
	// 删除键
	bool DeleteItem(const char* section, const char* key);
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

	// 改写键数据（如果段或键不存在则添加）
	bool WriteInteger(const char* section, const char* key, int value);
	bool WriteString(const char* section, const char* key, const char* value);
	bool WriteFloat(const char* section, const char* key, float value);

	// 添加有可能重复的键数据（如果段不存在则添加）
	bool AddInteger(const char* section, const char* key, int value);
	bool AddString(const char* section, const char* key, const char* value);
	bool AddFloat(const char* section, const char* key, float value);

	// 获得内存占用
	size_t GetMemoryUsage() const;
	
private:
	bool Build(char* buffer, size_t buffer_size);
	bool FindSectionIndex(const char* section, size_t& index) const;
	size_t AddToBuffer(const char* val);
	bool SetData(const char* section, const char* key, const char* data);
	bool AddData(const char* section, const char* key, const char* data);
	const char* GetData(const char* section, const char* key) const;
	const CIniFile::section_t* GetSection(const char* name) const;
	const CIniFile::item_t* GetItem(const section_t* pSection, 
		const char* name) const;

protected:
	TFastStr<char, 32> m_strFileName;
	int m_nReadRawFile;
	char* m_pBuffer;
	size_t m_nLength;
	TArrayPod<section_t, 8> m_Sections;
	TArrayPod<item_t, 8> m_Items;
};

#endif // _UTILS_INIFILE_H
