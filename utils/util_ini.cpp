//--------------------------------------------------------------------
// 文件名:		CUtilIni.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2006年1月30日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------

#include "util_ini.h"
#include "../public/AutoMem.h"
#include "../public/Inlines.h"

// CUtilIni

CUtilIni* CUtilIni::NewInstance(const char* filename)
{
	return NEW CUtilIni(filename);
}

CUtilIni::CUtilIni()
{
	m_pContent = NULL;
}

CUtilIni::CUtilIni(const char* filename): m_strFileName(filename)
{
	Assert(filename != NULL);

	m_pContent = NULL;
}

CUtilIni::~CUtilIni()
{
	if (m_pContent)
	{
		delete[] m_pContent;
        m_pContent = NULL;
	}
}

void CUtilIni::Release()
{
	delete this;
}

void CUtilIni::SetFileName(const char* filename)
{
	Assert(filename != NULL);

	m_strFileName = filename;
}

const char* CUtilIni::GetFileName() const
{
	return m_strFileName.c_str();
}

bool CUtilIni::LoadFromFile()
{
	if (m_pContent)
	{
		delete[] m_pContent;
        m_pContent = NULL;
	}
	
	m_Sections.clear();

	FILE* fp;
	
	fp = fopen(m_strFileName.c_str(), "rb");

	if (NULL == fp)
	{
		return false;
	}

	size_t file_size;
	
	fseek(fp, 0, SEEK_END);

	file_size = ftell(fp);

	fseek(fp, 0, SEEK_SET);

	size_t buffer_size = file_size + 2;

	char* buffer = NEW char[buffer_size];

	if (fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		fclose(fp);
		delete[] buffer;
        buffer = NULL;
		return false;
	}

	fclose(fp);

	buffer[file_size] = '\n';
	buffer[file_size + 1] = 0;

	size_t char_num = 0;
	bool section_begin = false;
	char* line = buffer;
	section_t* pSection = NULL;

	for (char* p = buffer; *p; ++p)
	{
		if ((*p == '\r') || (*p == '\n'))
		{
			*p = 0;

			section_begin = false;

			if (0 == char_num)
			{
				continue;
			}

			if (line[0] == ';')
			{
				// 忽略注释
				char_num = 0;
				continue;
			}

			if ((char_num >= 2) && (line[0] == '/') && (line[1] == '/'))
			{
				// 忽略注释
				char_num = 0;
				continue;
			}

			if (NULL == pSection)
			{
				// 缺省名字为空的段
				m_Sections.push_back(section_t());

				pSection = &m_Sections.back();
				pSection->strName = "";
				pSection->nHash = GetHashValue("");
				pSection->nItemStart = m_Items.size();
				pSection->nItemCount = 0;
			}

			// 清除后面多余的空格
			char* cur = line + char_num - 1;

			while (cur >= line)
			{
				if ((*cur == ' ') || (*cur == '\t'))
				{
					*cur = 0;
					--cur;
				}
				else
				{
					break;
				}
			}

			char* equal = strchr(line, '=');

			m_Items.push_back(item_t());

			item_t* pItem = &m_Items.back();

			if (NULL == equal)
			{
				pItem->strName = line;
				pItem->nHash = GetHashValue(line);
				pItem->strValue = "";
			}
			else
			{
				*equal = 0;
				
				// 键名
				char* key = line;
				
				// 清除键名后多余的空格
				char* cur = equal - 1;

				while (cur >= key)
				{
					if ((*cur == ' ') || (*cur == '\t'))
					{
						*cur = 0;
						--cur;
					}
					else
					{
						break;
					}
				}

				// 键值
				char* val = equal + 1;

				// 清除键值前多余的空格
				while (*val)
				{
					if ((*val == ' ') || (*val == '\t'))
					{
						*val = 0;
						++val;
					}
					else
					{
						break;
					}
				}

				pItem->strName = key;
				pItem->nHash = GetHashValue(key);
				pItem->strValue = val;
			}

			pSection->nItemCount++;

			char_num = 0;

			continue;
		}
		else if (*p == '[')
		{
			if (0 == char_num)
			{
				section_begin = true;
				*p = 0;
				line = p + 1;
				continue;
			}
		}
		else if (*p == ']')
		{
			if (section_begin)
			{
				*p = 0;
				
				m_Sections.push_back(section_t());

				pSection = &m_Sections.back();
				pSection->strName = line;
				pSection->nHash = GetHashValue(line);
				pSection->nItemStart = m_Items.size();
				pSection->nItemCount = 0;

				char_num = 0;
				section_begin = false;
				continue;
			}
		}
		else if ((*p == ' ') || (*p == '\t'))
		{
			if (0 == char_num)
			{
				// 忽略起始的空格
				continue;
			}
		}

		if (0 == char_num)
		{
			line = p;
		}

		++char_num;
	}
	
	m_pContent = buffer;

	return true;
}

const CUtilIni::section_t* CUtilIni::GetSection(const char* name) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValue(name);

	const size_t SIZE1 = m_Sections.size();
	
	for (size_t i = 0; i < SIZE1; ++i)
	{
		if ((m_Sections[i].nHash == hash)
			&& (stricmp(m_Sections[i].strName, name) == 0))
		{
			return &m_Sections[i];
		}
	}

	return NULL;
}

const CUtilIni::item_t* CUtilIni::GetItem(const section_t* pSection, 
										  const char* name) const
{
	Assert(pSection != NULL);
	Assert(name != NULL);

	unsigned int hash = GetHashValue(name);

	const size_t SIZE1 = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < SIZE1; ++i)
	{
		if ((m_Items[i].nHash == hash)
			&& (stricmp(m_Items[i].strName, name) == 0))
		{
			return &m_Items[i];
		}
	}

	return NULL;
}

bool CUtilIni::FindSection(const char* section) const
{
	Assert(section != NULL);

	return (GetSection(section) != NULL);
}

bool CUtilIni::FindSectionIndex(const char* section, size_t& index) const
{
	Assert(section != NULL);

	unsigned int hash = GetHashValue(section);

	const size_t SIZE1 = m_Sections.size();

	for (size_t i = 0; i < SIZE1; ++i)
	{
		if ((m_Sections[i].nHash == hash)
			&& (stricmp(m_Sections[i].strName, section) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool CUtilIni::FindSectionItemIndex(size_t sect_index, const char* key, 
									size_t& item_index) const
{
	Assert(sect_index < m_Sections.size());
	Assert(key != NULL);

	const section_t* pSection = &m_Sections[sect_index];

	unsigned int hash = GetHashValue(key);

	const size_t SIZE1 = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < SIZE1; ++i)
	{
		if ((m_Items[i].nHash == hash)
			&& (stricmp(m_Items[i].strName, key) == 0))
		{
			item_index = i - pSection->nItemStart;
			return true;
		}
	}

	return false;
}

size_t CUtilIni::GetSectionCount() const
{
	return m_Sections.size();
}

const char* CUtilIni::GetSectionByIndex(size_t index) const
{
	Assert(index < m_Sections.size());

	return m_Sections[index].strName;
}

size_t CUtilIni::GetSectionList(IVarList& result) const
{
	result.Clear();

	const size_t SIZE1 = m_Sections.size();

	for (size_t i = 0; i < SIZE1; ++i)
	{
		result.AddString(m_Sections[i].strName);
	}

	return result.GetCount();
}

size_t CUtilIni::GetSectionItemCount(size_t sect_index) const
{
	Assert(sect_index < m_Sections.size());

	const section_t* pSection = &m_Sections[sect_index];

	return pSection->nItemCount;
}

const char* CUtilIni::GetSectionItemKey(size_t sect_index, 
										size_t item_index) const
{
	Assert(sect_index < m_Sections.size());

	const section_t* pSection = &m_Sections[sect_index];

	Assert(item_index < pSection->nItemCount);

	return m_Items[pSection->nItemStart + item_index].strName;
}

const char* CUtilIni::GetSectionItemValue(size_t sect_index, 
										  size_t item_index) const
{
	Assert(sect_index < m_Sections.size());

	const section_t* pSection = &m_Sections[sect_index];

	Assert(item_index < pSection->nItemCount);

	return m_Items[pSection->nItemStart + item_index].strValue;
}

bool CUtilIni::FindItem(const char* section, const char* key) const
{
	Assert(section != NULL);
	Assert(key != NULL);

	const section_t* pSection = GetSection(section);

	if (NULL == pSection)
	{
		return false;
	}
	
	return (GetItem(pSection, key) != NULL);
}

size_t CUtilIni::GetItemCount(const char* section) const
{
	section_t* pSection = (section_t*)GetSection(section);

	if (NULL == pSection)
	{
		return 0;
	}

	return pSection->nItemCount;
}

size_t CUtilIni::GetItemList(const char* section, IVarList& result) const
{
	result.Clear();

	section_t* pSection = (section_t*)GetSection(section);

	if (NULL == pSection)
	{
		return 0;
	}

	const size_t SIZE1 = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < SIZE1; ++i)
	{
		result.AddString(m_Items[i].strName);
	}
	
	return result.GetCount();
}

size_t CUtilIni::GetItemValueList(const char* section, const char* key, 
								  IVarList& result) const
{
	result.Clear();

	section_t* pSection = (section_t*)GetSection(section);

	if (NULL == pSection)
	{
		return 0;
	}

	const size_t SIZE1 = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < SIZE1; ++i)
	{
		if (stricmp(m_Items[i].strName, key) == 0)
		{
			result.AddString(m_Items[i].strValue);
		}
	}
	
	return result.GetCount();
}

// 获得段下所有指定键的值列表
size_t CUtilIni::GetItemValueList(size_t sect_index, const char* key,
	IVarList& result) const
{
	result.Clear();

    if (sect_index >= m_Sections.size())
    {
        return 0;
    }

	const section_t* pSection = &m_Sections[sect_index];

	if (NULL == pSection)
	{
		return 0;
	}

    unsigned int  hash = GetHashValue(key);
	const size_t SIZE1 = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < SIZE1; ++i)
	{
        if (m_Items[i].nHash == hash && 
            stricmp(m_Items[i].strName, key) == 0)
		{
			result.AddString(m_Items[i].strValue);
		}
	}
	
	return result.GetCount();
}

const char* CUtilIni::GetData(const char* section, 
							  const char* key) const
{
	Assert(section != NULL);
	Assert(key != NULL);

	const section_t* pSection = GetSection(section);

	if (NULL == pSection)
	{
		return NULL;
	}
	
	const item_t* pItem = GetItem(pSection, key);

	if (NULL == pItem)
	{
		return NULL;
	}

	return pItem->strValue;
}

const char* CUtilIni::GetData(const size_t sect_index, 
							  const char* key) const
{
    Assert(sect_index < m_Sections.size());
	Assert(key != NULL);

	const section_t* pSection = &m_Sections[sect_index];

	if (NULL == pSection)
	{
		return NULL;
	}
	
	const item_t* pItem = GetItem(pSection, key);

	if (NULL == pItem)
	{
		return NULL;
	}

	return pItem->strValue;
}

int CUtilIni::ReadInteger(const char* section, const char* key, int def) const
{
	const char* str = GetData(section, key);

	if (str)
	{
		return atoi(str);
	}
	else
	{
		return def;
	}
}

char* CUtilIni::ReadString(const char* section, const char* key, 
						   char* value, size_t maxlen) const
{
	Assert(value != NULL);
	Assert(maxlen > 0);

	const char* str = GetData(section, key);

	if (str)
	{
		size_t len = strlen(str);

		if (len >= maxlen)
		{
			len = maxlen - 1;
		}
		
		memcpy(value, str, len);
		
		value[len] = 0;
	}

	return value;
}

const char* CUtilIni::ReadString(const char* section, const char* key, 
								 const char* def) const
{
	Assert(def != NULL);
	
	const char* str = GetData(section, key);

	if (str)
	{
		return str;
	}
	else
	{
		return def;
	}
}

float CUtilIni::ReadFloat(const char* section, const char* key, 
						  float def) const
{
	const char* str = GetData(section, key);

	if (str)
	{
		return (float)atof(str);
	}
	else
	{
		return def;
	}
}

// 读取键数据，当键不存在的时候返回缺省值(def)
int CUtilIni::ReadInteger(size_t sect_index, const char* key, int def) const
{
	const char* str = GetData(sect_index, key);

	if (str)
	{
		return atoi(str);
	}
	else
	{
		return def;
	}
}

char* CUtilIni::ReadString(size_t sect_index, const char* key, char* value, 
	size_t maxlen) const
{
	Assert(value != NULL);
	Assert(maxlen > 0);

	const char* str = GetData(sect_index, key);

	if (str)
	{
		size_t len = strlen(str);

		if (len >= maxlen)
		{
			len = maxlen - 1;
		}
		
		memcpy(value, str, len);
		
		value[len] = 0;
	}

	return value;
}

const char* CUtilIni::ReadString(size_t sect_index, const char* key, 
	const char* def) const
{
	Assert(def != NULL);
	
	const char* str = GetData(sect_index, key);

	if (str)
	{
		return str;
	}
	else
	{
		return def;
	}
}

float CUtilIni::ReadFloat(size_t sect_index, const char* key, float def) const
{
	const char* str = GetData(sect_index, key);

	if (str)
	{
		return (float)atof(str);
	}
	else
	{
		return def;
	}
}

