//--------------------------------------------------------------------
// 文件名:		IniFile.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2006年1月30日
// 创建人:		 
//    :	   
//--------------------------------------------------------------------

#include "IniFile.h"
#include "../public/AutoMem.h"
#include "../public/Inlines.h"
#include "../public/CoreFile.h"
#include "../public/Portable.h"

// CIniFile

CIniFile* CIniFile::NewInstance(const char* filename)
{
	return NEW CIniFile(filename);
}

CIniFile::CIniFile()
{
	m_nReadRawFile = 0;
	m_pBuffer = NULL;
	m_nLength = 0;
}

CIniFile::CIniFile(const char* filename): m_strFileName(filename)
{
	Assert(filename != NULL);

	m_nReadRawFile = 0;
	m_pBuffer = NULL;
	m_nLength = 0;
}

CIniFile::CIniFile(const char* filename, int read_raw_file)
: m_strFileName(filename)
{
	Assert(filename != NULL);

	m_nReadRawFile = read_raw_file;
	m_pBuffer = NULL;
	m_nLength = 0;
}

CIniFile::~CIniFile()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
	}
}

void CIniFile::Release()
{
	delete this;
}

void CIniFile::SetFileName(const char* filename)
{
	Assert(filename != NULL);

	m_strFileName = filename;
}

const char* CIniFile::GetFileName() const
{
	return m_strFileName.c_str();
}

bool CIniFile::LoadFromFile()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
		m_nLength = 0;
	}

	m_Sections.clear();
	m_Items.clear();

	FILE* fp;

	if (m_nReadRawFile)
	{
		fp = Port_FileOpen(m_strFileName.c_str(), "rb");
	}
	else
	{
		fp = core_file::fopen(m_strFileName.c_str(), "rb");
	}

	if (NULL == fp)
	{
		return false;
	}

	size_t file_size;

	if (m_nReadRawFile)
	{
		fseek(fp, 0, SEEK_END);

		file_size = ftell(fp);

		fseek(fp, 0, SEEK_SET);
	}
	else
	{
		core_file::fseek(fp, 0, SEEK_END);

		file_size = core_file::ftell(fp);

		core_file::fseek(fp, 0, SEEK_SET);
	}

	size_t buffer_size = file_size + 2;

	char* buffer = NEW char[buffer_size];

	if (m_nReadRawFile)
	{
		if (fread(buffer, sizeof(char), file_size, fp) != file_size)
		{
			fclose(fp);
			delete[] buffer;
			return false;
		}

		fclose(fp);
	}
	else
	{
		if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
		{
			core_file::fclose(fp);
			delete[] buffer;
			return false;
		}

		core_file::fclose(fp);
	}

	buffer[file_size] = '\n';
	buffer[file_size + 1] = 0;
	
	return Build(buffer, buffer_size);
}

bool CIniFile::LoadFromString(const char* str, size_t size)
{
	Assert(str != NULL);
	
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
		m_nLength = 0;
	}
	
	m_Sections.clear();
	m_Items.clear();
	
	char* buffer = NEW char[size + 2];
	
	memcpy(buffer, str, size);
	
	buffer[size] = '\n';
	buffer[size + 1] = 0;

	return Build(buffer, size + 2);
}

bool CIniFile::Build(char* buffer, size_t buffer_size)
{
	Assert(buffer != NULL);
	
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
				pSection->nNamePos = buffer_size - 1;
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
				pItem->nNamePos = line - buffer;
				pItem->nHash = GetHashValue(line);
				pItem->nValuePos = buffer_size - 1;
			}
			else
			{
				*equal = 0;

				// 键名
				char* key = line;
				
				// 清除键名后多余的空格
				cur = equal - 1;

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

				pItem->nNamePos = key - buffer;
				pItem->nHash = GetHashValue(key);
				pItem->nValuePos = val - buffer;
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
				pSection->nNamePos = line - buffer;
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

	m_pBuffer = buffer;
	m_nLength = buffer_size;

	return true;
}

bool CIniFile::SaveToFile() const
{
	FILE* fp = Port_FileOpen(m_strFileName.c_str(), "wb");

	if (NULL == fp)
	{
		return false;
	}

	for (size_t i = 0; i < m_Sections.size(); ++i)
	{
		const section_t* pSection = &m_Sections[i];

		fprintf(fp, "[%s]\r\n", m_pBuffer + pSection->nNamePos);

		const size_t item_end = pSection->nItemStart + pSection->nItemCount;
		
		for (size_t k = pSection->nItemStart; k < item_end; ++k)
		{
			fprintf(fp, "%s=%s\r\n", m_pBuffer + m_Items[k].nNamePos, 
				m_pBuffer + m_Items[k].nValuePos);
		}

		fprintf(fp, "\r\n");
	}

	fclose(fp);

	return true;
}

const CIniFile::section_t* CIniFile::GetSection(const char* name) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValue(name);
	const size_t sect_num = m_Sections.size();

	for (size_t i = 0; i < sect_num; ++i)
	{
		if ((m_Sections[i].nHash == hash)
			&& (stricmp(m_pBuffer + m_Sections[i].nNamePos, name) == 0))
		{
			return &m_Sections[i];
		}
	}

	return NULL;
}

const CIniFile::item_t* CIniFile::GetItem(const section_t* pSection, 
	const char* name) const
{
	Assert(pSection != NULL);
	Assert(name != NULL);

	unsigned int hash = GetHashValue(name);
	const size_t item_end = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < item_end; ++i)
	{
		if ((m_Items[i].nHash == hash)
			&& (stricmp(m_pBuffer + m_Items[i].nNamePos, name) == 0))
		{
			return &m_Items[i];
		}
	}

	return NULL;
}

bool CIniFile::AddSection(const char* section)
{
	Assert(section != NULL);

	if (GetSection(section) != NULL)
	{
		return false;
	}
	
	m_Sections.push_back(section_t());

	section_t& data = m_Sections.back();

	data.nNamePos = AddToBuffer(section);
	data.nHash = GetHashValue(section);
	data.nItemStart = m_Items.size();
	data.nItemCount = 0;

	return true;
}

bool CIniFile::FindSectionIndex(const char* section, size_t& index) const
{
	Assert(section != NULL);

	unsigned int hash = GetHashValue(section);
	const size_t sect_num = m_Sections.size();

	for (size_t i = 0; i < sect_num; ++i)
	{
		if ((m_Sections[i].nHash == hash)
			&& (stricmp(m_pBuffer + m_Sections[i].nNamePos, section) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool CIniFile::FindSection(const char* section) const
{
	Assert(section != NULL);

	return (GetSection(section) != NULL);
}

bool CIniFile::DeleteSection(const char* section)
{
	Assert(section != NULL);

	size_t sect_index;

	if (!FindSectionIndex(section, sect_index))
	{
		return false;
	}
	
	section_t* pSection = &m_Sections[sect_index];

	for (size_t k = sect_index + 1; k < m_Sections.size(); ++k)
	{
		m_Sections[k].nItemStart -= pSection->nItemCount;
	}

	m_Items.remove_from(pSection->nItemStart, pSection->nItemCount);
	m_Sections.remove(sect_index);

	return true;
}

size_t CIniFile::GetSectionCount() const
{
	return m_Sections.size();
}

size_t CIniFile::GetSectionList(IVarList& result) const
{
	result.Clear();

	const size_t sect_num = m_Sections.size();

	for (size_t i = 0; i < sect_num; ++i)
	{
		result.AddString(m_pBuffer + m_Sections[i].nNamePos);
	}

	return result.GetCount();
}

bool CIniFile::FindItem(const char* section, const char* key) const
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

bool CIniFile::DeleteItem(const char* section, const char* key)
{
	Assert(section != NULL);
	Assert(key != NULL);

	size_t sect_index;

	if (!FindSectionIndex(section, sect_index))
	{
		return false;
	}
	
	section_t* pSection = &m_Sections[sect_index];

	unsigned int hash = GetHashValue(key);
	const size_t item_end = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < item_end; ++i)
	{
		if ((m_Items[i].nHash == hash)
			&& (stricmp(m_pBuffer + m_Items[i].nNamePos, key) == 0))
		{
			for (size_t k = sect_index + 1; k < m_Sections.size(); ++k)
			{
				--m_Sections[k].nItemStart;
			}

			m_Items.remove(i);
			--pSection->nItemCount;
			return true;
		}
	}

	return false;
}

size_t CIniFile::GetItemCount(const char* section) const
{
	section_t* pSection = (section_t*)GetSection(section);

	if (NULL == pSection)
	{
		return 0;
	}

	return pSection->nItemCount;
}

size_t CIniFile::GetItemList(const char* section, IVarList& result) const
{
	result.Clear();

	section_t* pSection = (section_t*)GetSection(section);

	if (NULL == pSection)
	{
		return 0;
	}

	const size_t item_end = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < item_end; ++i)
	{
		result.AddString(m_pBuffer + m_Items[i].nNamePos);
	}

	return result.GetCount();
}

size_t CIniFile::GetItemValueList(const char* section, const char* key, 
	IVarList& result) const
{
	result.Clear();

	section_t* pSection = (section_t*)GetSection(section);

	if (NULL == pSection)
	{
		return 0;
	}

	const size_t item_end = pSection->nItemStart + pSection->nItemCount;

	for (size_t i = pSection->nItemStart; i < item_end; ++i)
	{
		if (stricmp(m_pBuffer + m_Items[i].nNamePos, key) == 0)
		{
			result.AddString(m_pBuffer + m_Items[i].nValuePos);
		}
	}

	return result.GetCount();
}

const char* CIniFile::GetData(const char* section, const char* key) const
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

	return m_pBuffer + pItem->nValuePos;
}

int CIniFile::ReadInteger(const char* section, const char* key, int def) const
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

char* CIniFile::ReadString(const char* section, const char* key, char* value, 
	size_t maxlen) const
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

const char* CIniFile::ReadString(const char* section, const char* key, 
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

float CIniFile::ReadFloat(const char* section, const char* key, 
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

size_t CIniFile::AddToBuffer(const char* val)
{
	Assert(val != NULL);

	const size_t val_size = strlen(val) + 1;
	const size_t new_size = m_nLength + val_size;

	char* p = NEW char[new_size];

	if (m_pBuffer)
	{
		memcpy(p, m_pBuffer, m_nLength);
		delete[] m_pBuffer;
	}

	size_t old_size = m_nLength;

	memcpy(p + old_size, val, val_size);

	m_pBuffer = p;
	m_nLength = new_size;

	return old_size;
}

bool CIniFile::SetData(const char* section, const char* key, const char* data)
{
	Assert(section != NULL);
	Assert(key != NULL);
	Assert(data != NULL);

	size_t sect_index;

	if (!FindSectionIndex(section, sect_index))
	{
		sect_index = m_Sections.size();

		section_t sect;
		
		sect.nNamePos = AddToBuffer(section);
		sect.nHash = GetHashValue(section);
		sect.nItemStart = m_Items.size();
		sect.nItemCount = 0;

		m_Sections.push_back(sect);
	}

	section_t* pSection = &m_Sections[sect_index];
	item_t* pItem = (item_t*)GetItem(pSection, key);

	if (NULL == pItem)
	{
		size_t item_index = pSection->nItemStart + pSection->nItemCount;

		item_t item;
		
		item.nNamePos = AddToBuffer(key);
		item.nHash = GetHashValue(key);
		item.nValuePos = m_nLength - 1;

		m_Items.insert(item_index, item);

		pItem = &m_Items[item_index];

		++pSection->nItemCount;

		for (size_t k = sect_index + 1; k < m_Sections.size(); ++k)
		{
			++m_Sections[k].nItemStart;
		}
	}

	if (strcmp(m_pBuffer + pItem->nValuePos, data) != 0)
	{
		pItem->nValuePos = AddToBuffer(data);
	}

	return true;
}

bool CIniFile::WriteInteger(const char* section, const char* key, int value)
{
	char buf[32];

	SafeSprintf(buf, sizeof(buf), "%d", value);

	return SetData(section, key, buf);
}

bool CIniFile::WriteString(const char* section, const char* key, 
	const char* value)
{
	return SetData(section, key, value);
}

bool CIniFile::WriteFloat(const char* section, const char* key, float value)
{
	char buf[64];

	SafeSprintf(buf, sizeof(buf), "%f", double(value));

	return SetData(section, key, buf);
}

bool CIniFile::AddData(const char* section, const char* key, const char* data)
{
	Assert(section != NULL);
	Assert(key != NULL);
	Assert(data != NULL);

	size_t sect_index;

	if (!FindSectionIndex(section, sect_index))
	{
		sect_index = m_Sections.size();

		section_t sect;
		
		sect.nNamePos = AddToBuffer(section);
		sect.nHash = GetHashValue(section);
		sect.nItemStart = m_Items.size();
		sect.nItemCount = 0;

		m_Sections.push_back(sect);
	}

	section_t* pSection = &m_Sections[sect_index];
	
	size_t item_index = pSection->nItemStart + pSection->nItemCount;

	item_t item;
	
	item.nNamePos = AddToBuffer(key);
	item.nHash = GetHashValue(key);
	item.nValuePos = AddToBuffer(data);

	m_Items.insert(item_index, item);

	++pSection->nItemCount;

	for (size_t k = sect_index + 1; k < m_Sections.size(); ++k)
	{
		++m_Sections[k].nItemStart;
	}
		
	return true;
}

bool CIniFile::AddInteger(const char* section, const char* key, int value)
{
	char buf[32];

	SafeSprintf(buf, sizeof(buf), "%d", value);

	return AddData(section, key, buf);
}

bool CIniFile::AddString(const char* section, const char* key, 
	const char* value)
{
	return AddData(section, key, value);
}

bool CIniFile::AddFloat(const char* section, const char* key, float value)
{
	char buf[64];

	SafeSprintf(buf, sizeof(buf), "%f", double(value));

	return AddData(section, key, buf);
}

size_t CIniFile::GetMemoryUsage() const
{
	size_t size = sizeof(int) + sizeof(char*) + sizeof(size_t); 
	
	size += m_strFileName.get_memory_usage();
	size += m_Sections.get_memory_usage();
	size += m_Items.get_memory_usage();
	size += m_nLength;
	
	return size;
}
