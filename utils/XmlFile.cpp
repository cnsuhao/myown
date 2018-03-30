//--------------------------------------------------------------------
// 文件名:      XmlFile.cpp
// 内  容:
// 说  明:
// 创建日期:    2011年1月4日
// 创建人:      zj
//    :       
//--------------------------------------------------------------------


#include "XmlFile.h"
#include "public/Inlines.h"
#include "public/CoreFile.h"
#include <windows.h>

CXmlFile::CXmlFile()
{
}

CXmlFile::CXmlFile(const char* filename):
    m_strFileName(filename)
{
    Assert(filename != NULL);
}

CXmlFile::~CXmlFile()
{
    Release();
}

void CXmlFile::SetFileName(const char* filename)
{
    Assert(filename != NULL);

    m_strFileName = filename;
}

const char* CXmlFile::GetFileName() const
{
    return m_strFileName.c_str();
}

bool CXmlFile::LoadFromFile(std::string& errorLog)
{
    Release();
    errorLog.clear();

    errorLog += m_strFileName;

    FILE* file = core_file::fopen(m_strFileName.c_str(), "rb");
    if (file == NULL)
    {
        errorLog += ": file is not exist!";
        return false;
    }

    core_file::fseek(file, 0, SEEK_END);
    long lengths = core_file::ftell(file);
    core_file::fseek(file, 0, SEEK_SET);
    char* pXmlValue = NEW char[lengths + 1 ];
    memset(pXmlValue, 0, lengths + 1);
    size_t result = core_file::fread(pXmlValue, sizeof(char), lengths, file);
    if (result == 0)
    {
        delete[] pXmlValue;
        errorLog += ": file is empty!";
        return false;
    }

    pXmlValue[lengths] = '\0';
    core_file::fclose(file);

    try
    {
        rapidxml::xml_document<>  doc;

        //将文件加载到数据表
        doc.clear();
        doc.parse<0>(pXmlValue);

        rapidxml::xml_node<char>* pNodeRoot = doc.first_node("Object");
        if (pNodeRoot == NULL)
        {
            errorLog += ": file has no Ojbect or capitalization errors!";
            return false;
        }

        rapidxml::xml_node<char>* pNodeProperty = pNodeRoot->first_node("Property");

        if (NULL == pNodeProperty)
        {
            errorLog += ": file has no Property or capitalization errors!";
            return true;
        }

        while (pNodeProperty)
        {
            rapidxml::xml_attribute<char>* attr = pNodeProperty->first_attribute();

            if (attr)
            {
                const char* name = attr->value();

                size_t hash_value = GetHashValue(name);

                // 缺省名字为空的段
                section_t_xml section = section_t_xml(name, (unsigned int)hash_value, m_XmlItems.size(), 0);

                rapidxml::xml_attribute<char>* pXmlAttribut =  attr->next_attribute();

                while (pXmlAttribut)
                {
                    const char* itemname = pXmlAttribut->name();
                    const char* itemvalue = pXmlAttribut->value();
                    item_t_xml item = item_t_xml(itemname, GetHashValue(itemname), itemvalue);

                    m_XmlItems.push_back(item);

                    section.m_nItemCount++;
                    pXmlAttribut =  pXmlAttribut->next_attribute();

                }

                m_sectionMap.insert(std::make_pair(hash_value,
                                                   m_XmlSections.size()));

                m_XmlSections.push_back(section);
            }

            pNodeProperty = pNodeProperty->next_sibling("Property");
        }

        delete[] pXmlValue;
    }
    catch (rapidxml::parse_error& e)
    {
        // TODO: 正式版考虑删除弹屏
        char msg[512] = {0};
        _snprintf(msg, 511,
                  "File: [%s] \nParse error: [%s] \nPosition: %s",
                  m_strFileName.c_str(),
                  e.what(),
                  e.where<char>());
        MessageBox(NULL, msg, "文件出错啦!", MB_OK | MB_ICONERROR);
        delete[] pXmlValue;
        errorLog += ": parse exception!";
        return false;
    }
    return true;


}

bool CXmlFile::LoadFromFile()
{
    std::string log;
    return LoadFromFile(log);
}

bool CXmlFile::LoadFromFile_Rapid()
{
    Release();

    FILE* file = core_file::fopen(m_strFileName.c_str(), "rb");
    if (file == NULL)
    {
        return false;
    }

    core_file::fseek(file, 0, SEEK_END);
    long lengths = core_file::ftell(file);
    core_file::fseek(file, 0, SEEK_SET);
    char* pXmlValue = new char[lengths + 1 ];
    memset(pXmlValue, 0, lengths + 1);
    size_t result = core_file::fread(pXmlValue, sizeof(char), lengths, file);
    if (result == 0)
    {
        delete[] pXmlValue;
        return false;
    }

    pXmlValue[lengths] = '\0';
    core_file::fclose(file);

    rapidxml::xml_document<>  doc;

    //将文件加载到数据表
    doc.clear();
    doc.parse<0>(pXmlValue);

    rapidxml::xml_node<char>* pNodeRoot = doc.first_node("Object");
    if (pNodeRoot == NULL)
    {
        delete[] pXmlValue;
        return false;
    }

    rapidxml::xml_node<char>* pNodeProperty = pNodeRoot->first_node("Property");

    if (NULL == pNodeProperty)
    {
        delete[] pXmlValue;
        return true;
    }

    while (pNodeProperty)
    {
        rapidxml::xml_attribute<char>* attr = pNodeProperty->first_attribute();

        if (attr)
        {
            std::string name = attr->value();

            // 缺省名字为空的段
            section_t_xml section = section_t_xml(name, GetHashValue(name.c_str()), m_XmlItems.size(), 0);

            rapidxml::xml_attribute<char>* pXmlAttribut =  attr->next_attribute();

            while (pXmlAttribut)
            {
                std::string itemname = pXmlAttribut->name();
                std::string itemvalue = pXmlAttribut->value();
                item_t_xml item = item_t_xml(itemname, GetHashValue(itemname.c_str()), itemvalue);

                m_XmlItems.push_back(item);

                section.m_nItemCount++;
                pXmlAttribut =  pXmlAttribut->next_attribute();

            }
            m_XmlSections.push_back(section);
        }

        pNodeProperty = pNodeProperty->next_sibling("Property");
    }

    delete[] pXmlValue;

    return true;
}

bool CXmlFile::LoadFromString(const char* str, size_t size)
{
    Assert(str != NULL);

    rapidxml::xml_document<>  doc;

    //将文件加载到数据表
    doc.clear();
    doc.parse<0>((char*)str);

    rapidxml::xml_node<char>* pNodeRoot = doc.first_node("Object");
    if (pNodeRoot == NULL)
    {
        return false;
    }

    rapidxml::xml_node<char>* pNodeProperty = pNodeRoot->first_node("Property");

    if (NULL == pNodeProperty)
    {
        return true;
    }

    while (pNodeProperty)
    {
        rapidxml::xml_attribute<char>* attr = pNodeProperty->first_attribute();

        if (attr)
        {
            std::string name = attr->value();

            // 缺省名字为空的段
            section_t_xml section = section_t_xml(name, GetHashValue(name.c_str()), m_XmlItems.size(), 0);

            rapidxml::xml_attribute<char>* pXmlAttribut =  attr->next_attribute();

            while (pXmlAttribut)
            {
                std::string itemname = pXmlAttribut->name();
                std::string itemvalue = pXmlAttribut->value();
                item_t_xml item = item_t_xml(itemname, GetHashValue(itemname.c_str()), itemvalue);

                m_XmlItems.push_back(item);

                section.m_nItemCount++;
                pXmlAttribut =  pXmlAttribut->next_attribute();

            }

            m_XmlSections.push_back(section);
        }

        pNodeProperty = pNodeProperty->next_sibling("Property");
    }


    return true;
}

bool CXmlFile::FindSection(const char* section_name) const
{
    Assert(section_name != NULL);

    return (GetSection(section_name) != NULL);
}

bool CXmlFile::FindSectionIndex(const char* section_name, size_t& index) const
{
    Assert(section_name != NULL);

    unsigned int hash = GetHashValue(section_name);

    const size_t SIZE1 = m_XmlSections.size();

    for (size_t i = 0; i < SIZE1; ++i)
    {
        if ((m_XmlSections[i].m_nHash == hash)
            && (stricmp(m_XmlSections[i].m_strName.c_str(), section_name) == 0))
        {
            index = i;
            return true;
        }
    }

    return false;
}

bool CXmlFile::FindSectionItemIndex(size_t sect_index, const char* key,
                                    size_t& item_index) const
{
    Assert(sect_index < m_XmlSections.size());
    Assert(key != NULL);

    const section_t_xml section = m_XmlSections[sect_index];

    unsigned int hash = GetHashValue(key);

    const size_t SIZE1 = section.m_nItemStart + section.m_nItemCount;

    for (size_t i = section.m_nItemStart; i < SIZE1; ++i)
    {
        if ((m_XmlItems[i].m_nHash == hash)
            && (stricmp(m_XmlItems[i].m_strName.c_str(), key) == 0))
        {
            item_index = i - section.m_nItemStart;
            return true;
        }
    }

    return false;
}

size_t CXmlFile::GetSectionList(IVarList& result) const
{
    result.Clear();

    const size_t SIZE1 = m_XmlSections.size();

    for (size_t i = 0; i < SIZE1; ++i)
    {
        result.AddString(m_XmlSections[i].m_strName.c_str());
    }

    return result.GetCount();
}

size_t CXmlFile::GetSectionItemCount(size_t sect_index) const
{
    Assert(sect_index < m_XmlSections.size());

    const section_t_xml section = m_XmlSections[sect_index];

    return section.m_nItemCount;
}

const char* CXmlFile::GetSectionItemKey(size_t sect_index,
                                        size_t item_index) const
{
    Assert(sect_index < m_XmlSections.size());

    const section_t_xml section = m_XmlSections[sect_index];

    Assert(item_index < section.m_nItemCount);

    return m_XmlItems[section.m_nItemStart + item_index].m_strName.c_str();
}

const char* CXmlFile::GetSectionItemValue(size_t sect_index,
                                          size_t item_index) const
{
    Assert(sect_index < m_XmlSections.size());

    const section_t_xml section = m_XmlSections[sect_index];

    Assert(item_index < section.m_nItemCount);

    return m_XmlItems[section.m_nItemStart + item_index].m_strValue.c_str();
}

bool CXmlFile::FindItem(const char* section_name, const char* key) const
{
    Assert(section_name != NULL);
    Assert(key != NULL);

    const section_t_xml* section = GetSection(section_name);

    if (NULL == section)
    {
        return false;
    }

    return (GetItem(section, key) != NULL);
}

size_t CXmlFile::GetItemCount(const char* section_name) const
{
    const section_t_xml* section = GetSection(section_name);

    if (NULL == section)
    {
        return 0;
    }

    return section->m_nItemCount;
}

size_t CXmlFile::GetItemList(const char* section_name, IVarList& result) const
{
    result.Clear();

    const section_t_xml* section = GetSection(section_name);

    if (NULL == section)
    {
        return 0;
    }

    const size_t SIZE1 = section->m_nItemStart + section->m_nItemCount;

    for (size_t i = section->m_nItemStart; i < SIZE1; ++i)
    {
        result.AddString(m_XmlItems[i].m_strName.c_str());
    }

    return result.GetCount();
}


size_t CXmlFile::GetSectionCount() const
{
    return m_XmlSections.size();
}

const char* CXmlFile::GetSectionByIndex(size_t index) const
{
    Assert(index < m_XmlSections.size());

    return m_XmlSections[index].m_strName.c_str();
}

int CXmlFile::ReadInteger(const char* section_name, const char* key, int def) const
{
    const char* str = GetData(section_name, key);

    if (str && strcmp(str, "") != 0)
    {
        return atoi(str);
    }
    else
    {
        return def;
    }
}

__int64 CXmlFile::ReadInt64(const char* section_name, const char* key, __int64 def) const
{
    const char* str = GetData(section_name, key);

    if (str && strcmp(str, "") != 0)
    {
        return ::_atoi64(str);
    }
    else
    {
        return def;
    }
}
const char* CXmlFile::GetData(const char* section_name, const char* key) const
{
    Assert(section_name != NULL);
    Assert(key != NULL);

    const section_t_xml* section = GetSection(section_name);

    if (NULL == section)
    {
        return NULL;
    }

    const item_t_xml* item = GetItem(section, key);

    if (NULL == item)
    {
        return NULL;
    }

    return item->m_strValue.c_str();
}

size_t CXmlFile::GetItemValueList(const char* section_name, const char* key, IVarList& result) const
{
    result.Clear();

    const section_t_xml* section = GetSection(section_name);

    if (NULL == section)
    {
        return NULL;
    }

    const size_t SIZE1 = section->m_nItemStart + section->m_nItemCount;

    for (size_t i = section->m_nItemStart; i < SIZE1; ++i)
    {
        if (stricmp(m_XmlItems[i].m_strName.c_str(), key) == 0)
        {
            result.AddString(m_XmlItems[i].m_strValue.c_str());
        }
    }

    return result.GetCount();
}

const CXmlFile::section_t_xml* CXmlFile::GetSection(const char* section_name) const
{
    Assert(section_name != NULL);

    unsigned int hash = GetHashValue(section_name);

    const size_t SIZE1 = m_XmlSections.size();

    if (SIZE1 == m_sectionMap.size())
    {
        // 进行优化查找
        std::pair<SectionMapTypeConstItr, SectionMapTypeConstItr> ii =
            m_sectionMap.equal_range(hash);

        for (SectionMapTypeConstItr i = ii.first; i != ii.second; ++i)
        {
            if ((m_XmlSections[i->second].m_nHash == hash)
                && (stricmp(m_XmlSections[i->second].m_strName.c_str(), section_name) == 0))
            {
                return &m_XmlSections[i->second];
            }
        }
    }
    else
    {
        for (size_t i = 0; i < SIZE1; ++i)
        {
            if ((m_XmlSections[i].m_nHash == hash)
                && (stricmp(m_XmlSections[i].m_strName.c_str(), section_name) == 0))
            {
                return &m_XmlSections[i];
            }
        }
    }

    return NULL;
}


const CXmlFile::item_t_xml* CXmlFile::GetItem(const section_t_xml* section, const char* name) const
{
    Assert(name != NULL);
    Assert(section != NULL);


    unsigned int hash = GetHashValue(name);

    const size_t SIZE1 = section->m_nItemStart + section->m_nItemCount;

    for (size_t i = section->m_nItemStart; i < SIZE1; ++i)
    {
        if ((m_XmlItems[i].m_nHash == hash)
            && (stricmp(m_XmlItems[i].m_strName.c_str(), name) == 0))
        {
            return &m_XmlItems[i];
        }
    }

    return NULL;
}

char* CXmlFile::ReadString(const char* section_name, const char* key, char* value, size_t maxlen) const
{
    Assert(value != NULL);
    Assert(maxlen > 0);

    const char* str = GetData(section_name, key);

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

const char* CXmlFile::ReadString(const char* section_name, const char* key, const char* def) const
{
    Assert(def != NULL);

    const char* str = GetData(section_name, key);

    if (str && strcmp(str, "") != 0)
    {
        return str;
    }
    else
    {
        return def;
    }
}

float CXmlFile::ReadFloat(const char* section_name, const char* key, float def) const
{
    const char* str = GetData(section_name, key);

    if (str && strcmp(str, "") != 0)
    {
        return (float)atof(str);
    }
    else
    {
        return def;
    }
}

void CXmlFile::Release()
{
    m_XmlSections.clear();
    m_XmlItems.clear();

    m_sectionMap.clear();
}

bool CXmlFile::AddSection(const char* section_name)
{
    Assert(section_name != NULL);

    if (NULL != GetSection(section_name))
    {
        return false;
    }

    std::string name = section_name;
    section_t_xml section = section_t_xml(name, GetHashValue(section_name), m_XmlItems.size(), 0);
    m_XmlSections.push_back(section);

    return true;
}

bool CXmlFile::DeleteSection(const char* section_name)
{
    std::vector<item_t_xml>::iterator item_It_begin;
    std::vector<item_t_xml>::iterator item_It_end;
    std::vector<section_t_xml>::iterator section_It_earse;

    Assert(section_name != NULL);

    size_t sect_index;

    if (!FindSectionIndex(section_name, sect_index))
    {
        return false;
    }

    section_t_xml section = m_XmlSections[sect_index];

    const size_t xml_section_size = m_XmlSections.size();
    for (size_t k = sect_index + 1; k < xml_section_size; ++k)
    {
        m_XmlSections[k].m_nItemStart -= section.m_nItemCount;
    }

    std::vector<item_t_xml>::iterator item_it = m_XmlItems.begin();
    for (size_t i = 0; i < m_XmlItems.size(); ++i)
    {
        if (i == section.m_nItemStart)
        {
            item_It_begin = item_it;
        }

        if (i == (section.m_nItemStart + section.m_nItemCount))
        {
            item_It_end = item_it;
        }
        ++item_it;
    }

    if (section.m_nItemStart + section.m_nItemCount == m_XmlItems.size())
    {
        item_It_end = m_XmlItems.end();
    }

    std::vector<section_t_xml>::iterator section_it = m_XmlSections.begin();

    for (size_t i = 0; i < xml_section_size; ++i)
    {
        if (i == sect_index)
        {
            section_It_earse = section_it;
            break;
        }

        ++section_it;
    }

    if (section.m_nItemCount > 0)
    {
        m_XmlItems.erase(item_It_begin, item_It_end);
    }

    m_XmlSections.erase(section_It_earse);

    return true;
}

bool CXmlFile::DeleteItem(const char* section_name, const char* key)
{
    Assert(section_name != NULL);
    Assert(key != NULL);

    size_t sect_index;

    if (!FindSectionIndex(section_name, sect_index))
    {
        return false;
    }

    section_t_xml* section = &m_XmlSections[sect_index];

    unsigned int hash = GetHashValue(key);
    const size_t item_end = section->m_nItemStart + section->m_nItemCount;

    std::vector<item_t_xml>::iterator item_it;

    item_it = m_XmlItems.begin();

    for (size_t i = section->m_nItemStart; i < item_end; ++i)
    {
        if ((m_XmlItems[i].m_nHash == hash)
            && (stricmp(m_XmlItems[i].m_strName.c_str(), key) == 0))
        {
            const size_t xml_section_size = m_XmlSections.size();
            for (size_t k = sect_index + 1; k < xml_section_size; ++k)
            {
                --m_XmlSections[k].m_nItemStart;
            }

            m_XmlItems.erase(item_it);
            --section->m_nItemCount;

            return true;
        }

        ++item_it;
    }


    return false;
}

bool CXmlFile::WriteInteger(const char* section_name, const char* key, int value)
{
    char buf[32];

    SPRINTF_S(buf, "%d", value);

    return SetData(section_name, key, buf);
}

bool CXmlFile::WriteString(const char* section_name, const char* key, const char* value)
{
    return SetData(section_name, key, value);
}

bool CXmlFile::WriteFloat(const char* section_name, const char* key, float value)
{
    char buf[32];

    SPRINTF_S(buf, "%f", double(value));

    return SetData(section_name, key, buf);

}

bool CXmlFile::AddInteger(const char* section_name, const char* key, int value)
{
    char buf[32];

    SPRINTF_S(buf, "%d", value);

    return AddData(section_name, key, buf);
}

bool CXmlFile::AddString(const char* section_name, const char* key, const char* value)
{
    return AddData(section_name, key, value);
}

bool CXmlFile::AddFloat(const char* section_name, const char* key, float value)
{
    char buf[32];

    SPRINTF_S(buf, "%f", double(value));

    return AddData(section_name, key, buf);

}

bool CXmlFile::AddData(const char* section_name, const char* key, const char* data)
{
    Assert(section_name != NULL);
    Assert(key != NULL);
    Assert(data != NULL);

    size_t sect_index;

    if (!FindSectionIndex(section_name, sect_index))
    {
        sect_index = m_XmlSections.size();

        AddSection(section_name);
    }

    section_t_xml* section = &m_XmlSections[sect_index];

    size_t item_index = section->m_nItemStart + section->m_nItemCount;

    std::string item_name = key;
    std::string item_value = data;

    item_t_xml item = item_t_xml(item_name, GetHashValue(item_name.c_str()), item_value);

    m_XmlItems.insert(m_XmlItems.begin() + item_index, 1, item);

    ++section->m_nItemCount;

    const size_t xml_section_size = m_XmlSections.size();
    for (size_t k = sect_index + 1; k < xml_section_size; ++k)
    {
        ++m_XmlSections[k].m_nItemStart;
    }

    return true;
}

bool CXmlFile::SetData(const char* section_name, const char* key, const char* data)
{
    Assert(section_name != NULL);
    Assert(key != NULL);
    Assert(data != NULL);

    size_t sect_index;

    if (!FindSectionIndex(section_name, sect_index))
    {
        sect_index = m_XmlSections.size();

        AddSection(section_name);

    }

    section_t_xml section = m_XmlSections[sect_index];
    item_t_xml* item = const_cast<item_t_xml*>(GetItem(&section, key));

    if (NULL == item)
    {
        AddData(section_name, key, data);
    }
    else
    {
        item->m_strValue = data;
    }

    return true;

}

bool CXmlFile::SaveToFile() const
{
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<char>* pNodeRoot = doc.allocate_node(rapidxml::node_element, "Object");
    doc.append_node(pNodeRoot);

    rapidxml::xml_node<char>* pItemElm = NULL;

    const size_t xml_section_size = m_XmlSections.size();
    for (size_t i = 0; i < xml_section_size; i ++)
    {
        pItemElm = doc.allocate_node(rapidxml::node_element, "Property");
        pItemElm->append_attribute(doc.allocate_attribute("ID", m_XmlSections[i].m_strName.c_str()));
        pNodeRoot->append_node(pItemElm);

        size_t start_index = m_XmlSections[i].m_nItemStart;
        size_t end_index = m_XmlSections[i].m_nItemStart + m_XmlSections[i].m_nItemCount;

        for (size_t j = start_index; j < end_index; j ++)
        {
            pItemElm->append_attribute(doc.allocate_attribute(m_XmlItems[j].m_strName.c_str(), m_XmlItems[j].m_strValue.c_str()));
        }
    }

//     std::ofstream out(m_strFileName.c_str());
//     out << "<?xml version=\"1.0\" encoding=\"gb2312\"?>\n";
//     out << doc;
//     out.close();

    return true;
}
