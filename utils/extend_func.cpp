#include "extend_func.h"
#include "util_func.h"
#include "../public/VarList.h"
#include "math/fm_fixedpoint.h"
#include "string_util.h"
#include "../server/IPubKernel.h"

//#include "FsGame/Define/NpcTypeDefine.h"
#include <time.h>
#include <math.h>
#include <set>
#include <map>
#include <fstream>

#ifdef _WIN32
#include "../system/WinPortable.h"
#else
#include "../system/LinuxPortable.h"
#endif // _WIN32

// 日志级别
int g_logLevel = LOG_WARNING;
std::string g_loginfo_include = "";
IKernel* g_pKernel = NULL;
IPubKernel* g_pPubKernel = NULL;

const int ONE_WEEK_DAYS = 7;	// 一周天数
const int ONE_DAY_SECOND = 86400;	// 一天秒数

//#define npc_prop_write_txt
#ifdef npc_prop_write_txt
#include <vector>
#include <set>
#include <direct.h>

#include <algorithm>
bool READ_FLAG = false;

struct  NPC_WRITE_PROP
{
    std::string prop_name;
    std::string pub;
    std::string rea;
    std::string pri;
    std::string sav;
    std::string typ;
    std::string des;
    bool operator < (const NPC_WRITE_PROP npc1)
    {
        if (pub < npc1.pub)
        {
            return false;
        }

        return true;
    }
};

std::vector<NPC_WRITE_PROP> npc_wr_vec;
std::vector<NPC_WRITE_PROP> npc_wr_rec_vec;
std::set<std::string> npc_file_set;

void GetFile(TiXmlElement * pElement);

void get_npc_file_vec(const char * logicclass)
{
    TiXmlDocument doc(logicclass);

    doc.LoadFile();
    TiXmlNode * pNode = doc.FirstChild("logicclass");
    TiXmlElement * pElement = pNode->FirstChildElement("class");
    while (pElement != NULL)
    {
        if (strcmp("TYPE_NPC", pElement->Attribute("type")) == 0)
        {
            break;
        }
        pElement = pElement->NextSiblingElement("class");
    }

    GetFile(pElement);
    return;
}

void GetFile(TiXmlElement * pElement)
{


    npc_file_set.insert(pElement->Attribute("id"));
    TiXmlElement * pele = pElement->FirstChildElement("class");

    while (pele != NULL)
    {
        GetFile(pele);
        pele = pele->NextSiblingElement("class");
    }
    return;
}



#endif
#include "public/Inlines.h"




const int MAX_INT_VALUE = 0x7FFFFFFF;
const float MAX_FLOAT_VALUE = 99999999.0f;

bool write_comp_data(const char * xmlfile, const char * name, int type, bool bpublic, bool bprivate)
{
    const char * public_prop = "D:\\bgproject\\visible\\public_prop.txt";
    const char * private_prop = "D:\\bgproject\\visible\\private_prop.txt";
    const char * public_pivate_prop = "D:\\bgproject\\visible\\public_pivate_prop.txt";
    const char * public_record = "D:\\bgproject\\visible\\public_record.txt";
    const char * private_record = "D:\\bgproject\\visible\\private_record.txt";
    const char * public_pivate_record = "D:\\bgproject\\visible\\public_pivate_record.txt";
    const char * fileName = NULL;
    static std::set<std::string> key_map;
    if (type == 0)
    {
        if (bpublic && bprivate)
        {
            fileName = public_pivate_prop;
        }
        else if (bpublic)
        {
            fileName = public_prop;
        }
        else if (bprivate)
        {
            fileName = private_prop;
        }
    }
    else
    {
        if (bpublic && bprivate)
        {
            fileName = public_pivate_record;
        }
        else if (bpublic)
        {
            fileName = public_record;
        }
        else if (bprivate)
        {
            fileName = private_record;
        }
    }
    if (fileName == NULL)
    {
        return true;
    }
    if (key_map.find(name) != key_map.end())
    {
        return true;
    }
    key_map.insert(name);
    std::ofstream fout(fileName, std::ios::app);
    if (!fout.good())
    {
        return false;
    }
    char line[256];
    SPRINTF_S(line, "%s\t %s\n", name, xmlfile);
    fout << line;
    fout.close();
    return true;
}

//比较对象的属性是否满足条件
bool compare_property(IKernel * pKernel, const PERSISTID & obj, const char * prop,
                      const char * compare_value, const char * op)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(obj);
	if (pSelfObj == NULL)
	{
		return false;
	}

    if (!pSelfObj->FindAttr(prop))
    {
        return false;
    }

    if (op == NULL)
    {
        return false;
    }

    //处理包含关系 "1;2;3n;3n+2"表示，属性为1,2或者3的倍数则满足条件
    if (strcmp(op, "E") == 0)
    {
        if (pSelfObj->GetAttrType(prop) == VTYPE_INT)
        {
            int prop_value = pSelfObj->QueryInt(prop);
            if (prop_value == 0) // 属性查找不到可能会出现为“0”情况
            {
                return false;
            }

            CVarList value_element;
            std::string str_compare_value = compare_value;
            util_split_string(value_element, str_compare_value, ";");
            for (size_t i = 0; i < value_element.GetCount(); ++i)
            {
                std::string str_value = value_element.StringVal(i);
                CVarList value_word;
                util_split_string(value_word, str_value, "+");
                if (value_word.GetCount() == 1) // 形式为“3”、“3n”类
                {
                    CVarList value_stem;
                    util_split_string(value_stem, str_value, "n");
                    if (value_stem.GetCount() == 1)
                    {
                        // 形式为“3”类
                        int temp = atoi(str_value.c_str());
                        if (temp != 0)
                        {
                            if (prop_value % temp == 0)
                            {
                                return true;
                            }
                        }
                    }
                    else if (value_stem.GetCount() == 2)
                    {
                        // 形式为“3n”类
                        int temp = atoi(value_stem.StringVal(0));
                        if (temp != 0)
                        {
                            if (prop_value % temp == 0)
                            {
                                return true;
                            }
                        }
                    }
                }
                else if (value_word.GetCount() == 2) // 形式为“3n+1”
                {
                    std::string str_one = value_word.StringVal(0);
                    int str_two = atoi(value_word.StringVal(1));
                    int value = prop_value - str_two;

                    CVarList value_stem;
                    util_split_string(value_stem, str_one, "n");
                    if (value_stem.GetCount() == 2)
                    {
                        // 形式为3n类
                        int temp = atoi(value_stem.StringVal(0));
                        if (temp != 0)
                        {
                            if (value % temp == 0)
                            {
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
        }
        return false;
    }

    switch (pSelfObj->GetAttrType(prop))
    {
        case VTYPE_INT:
            return ::compare(pSelfObj->QueryInt(prop), ::atoi(compare_value), op);
            break;
        case VTYPE_INT64:
            return ::compare(pSelfObj->QueryInt64(prop), ::_atoi64(compare_value), op);
            break;
        case VTYPE_FLOAT:
            return ::compare(pSelfObj->QueryFloat(prop), (float)::atof(compare_value), op);
            break;
        case VTYPE_DOUBLE:
            return ::compare(pSelfObj->QueryDouble(prop), ::atof(compare_value), op);
            break;
        case VTYPE_STRING:
            return ::compare(std::string(pSelfObj->QueryString(prop)), std::string(compare_value), op);
            break;
        case VTYPE_WIDESTR:
			return ::compare(std::wstring(pSelfObj->QueryWideStr(prop)), ::util_string_as_widestr(compare_value), op);
            break;
        default:
            return false;
    }
}

bool extend_log_level(IKernel* pKernel, LogLevelEnum level, const char* IncluedInfo)
{
	if (!g_pKernel)
	{
		g_pKernel = pKernel;
	}

	if (level < LOG_TRACE || level > LOG_CRITICAL)
	{
		return false;
	}
	g_logLevel = level;
	g_loginfo_include = IncluedInfo;
	return true;
}

void extend_warning(IKernel * pKernel, const char * info)
{
    //#ifdef _DEBUG
    //  pKernel->Echo(info);
    //#endif
    pKernel->Trace(info);
}

void extend_warning(LogLevelEnum level, const char* format, ...)
{
	if (g_pKernel == NULL)
	{
		return;
	}

	if (level < g_logLevel)
	{
		return;
	}
	std::string strFormat;
	switch (level)
	{
	case LOG_TRACE_PERFORMANCE:
		strFormat = "[Trace-Performance]";
		break;
	case LOG_TRACE:
		strFormat = "[Trace-Logic]";
		break;
	case LOG_INFO:
		strFormat = "[Info-Logic]";
		break;
	case LOG_WARNING:
		strFormat = "[Warning-Logic]";
		break;
	case LOG_ERROR:
		strFormat = "[Error-Logic]";
		break;
	case LOG_CRITICAL:
		strFormat = "[Critical-Logic]";
		break;
	default:
		return;
	}

	strFormat += format;
	char buff[1024] = {0};
	va_list args;
	va_start(args, format);
	vsnprintf(buff, 1023, strFormat.c_str(), args);
	va_end(args);

	std::string sbuff = buff;
	if (level == LOG_TRACE &&
		!g_loginfo_include.empty() &&
		!sbuff.empty() &&
		sbuff.find(g_loginfo_include) == std::string::npos)
	{
		return;
	}
	g_pKernel->Trace(buff);
}

void extend_warning_pub(LogLevelEnum level, const char* format, ...)
{
	if (g_pPubKernel == NULL)
	{
		return;
	}

	if (level < g_logLevel)
	{
		return;
	}
	std::string strFormat;
	switch (level)
	{
	case LOG_TRACE_PERFORMANCE:
		strFormat = "[Trace-Performance]";
		break;
	case LOG_TRACE:
		strFormat = "[Trace-PubLogic]";
		break;
	case LOG_INFO:
		strFormat = "[Info-PubLogic]";
		break;
	case LOG_WARNING:
		strFormat = "[Warning-PubLogic]";
		break;
	case LOG_ERROR:
		strFormat = "[Error-PubLogic]";
		break;
	case LOG_CRITICAL:
		strFormat = "[Critical-PubLogic]";
		break;
	default:
		return;
	}

	strFormat += format;
	char buff[1024] = {0};
	va_list args;
	va_start(args, format);
	vsnprintf(buff, 1023, strFormat.c_str(), args);
	va_end(args);

	std::string sbuff = buff;
	if (level == LOG_TRACE &&
		!g_loginfo_include.empty() &&
		!sbuff.empty() &&
		sbuff.find(g_loginfo_include) == std::string::npos)
	{
		return;
	}
	g_pPubKernel->Trace(buff);
}


int convert_class_type(const char * sz_classtype)
{
    int classtype = 0;
    if (stricmp(sz_classtype, "TYPE_PLAYER") == 0)
    {
        classtype = TYPE_PLAYER;
    }
    else if (stricmp(sz_classtype, "TYPE_SCENE") == 0)
    {
        classtype = TYPE_SCENE;
    }
    else if (stricmp(sz_classtype, "TYPE_NPC") == 0)
    {
        classtype = TYPE_NPC;
    }
    else if (stricmp(sz_classtype, "TYPE_ITEM") == 0)
    {
        classtype = TYPE_ITEM;
    }
    else if (stricmp(sz_classtype, "TYPE_HELPER") == 0)
    {
        classtype = TYPE_HELPER;
    }
    else if (stricmp(sz_classtype, "TYPE_WEAKBOX") == 0)
    {
        classtype = TYPE_WEAKBOX;
    }
    else
    {
        throw;
    }

    return classtype;
}

int convert_var_type(const char * sz_vartype)
{
    int vartype = 0;
    if (stricmp(sz_vartype, "BOOL") == 0)
    {
        vartype = VTYPE_BOOL;
    }
    else if (stricmp(sz_vartype, "INT") == 0 ||
             stricmp(sz_vartype, "BYTE") == 0 ||
             stricmp(sz_vartype, "WORD") == 0 ||
             stricmp(sz_vartype, "DWORD") == 0)
    {
        vartype = VTYPE_INT;
    }
    else if (stricmp(sz_vartype, "INT64") == 0)
    {
        vartype = VTYPE_INT64;
    }
    else if (stricmp(sz_vartype, "FLOAT") == 0)
    {
        vartype = VTYPE_FLOAT;
    }
    else if (stricmp(sz_vartype, "DOUBLE") == 0)
    {
        vartype = VTYPE_DOUBLE;
    }
    else if (stricmp(sz_vartype, "STRING") == 0)
    {
        vartype = VTYPE_STRING;
    }
    else if (stricmp(sz_vartype, "WIDESTR") == 0)
    {
        vartype = VTYPE_WIDESTR;
    }
    else if (stricmp(sz_vartype, "OBJECT") == 0)
    {
        vartype = VTYPE_OBJECT;
    }
    else if (stricmp(sz_vartype, "POINTER") == 0)
    {
        vartype = VTYPE_POINTER;
    }
    else if (stricmp(sz_vartype, "USERDATA") == 0)
    {
        vartype = VTYPE_USERDATA;
    }
    //else if (stricmp(sz_vartype, "TABLE") == 0)
    //{
    //    vartype = VTYPE_TABLE;
    //}
    else
    {
        Assert(false);
    }

    return vartype;
}

const char* convert_string(const char * szstring, const char* defaultstring)
{
	if (szstring == NULL)
	{
		return defaultstring == NULL ? "" : defaultstring;
	}

	return szstring;
}

bool convert_boolean(const char * szboolean)
{
    if (szboolean == NULL)
    {
        return false;
    }

    if (stricmp(szboolean, "TRUE") == 0)
    {
        return true;
    }

    return false;
}

//字符串类型的整数类型转换为整型
int convert_int(const char * szint, int default_int)
{
    if (szint == NULL)
    {
        return default_int;
    }

    return ::atoi(szint);
}

//字符串类型的整数类型转换为64位整型
int64_t convert_int64(const char * szint64, int64_t default_int64)
{
    if (szint64 == NULL)
    {
        return default_int64;
    }

    return ::_atoi64(szint64);
}

//字符串类型的浮点型转换为浮点型
float convert_float(const char * szfloat, float default_float)
{
    if (szfloat == NULL)
    {
        return default_float;
    }

    return (float)::atof(szfloat);
}

// 字符串类型的浮点型转换为双精度浮点型
double convert_double(const char * szdouble, double default_double)
{
    if (szdouble == NULL)
    {
        return default_double;
    }

    return ::atof(szdouble);
}

bool createclass_add_property(IKernel * pKernel, const char * xmlfile, int index, const char * name,
                              int type, bool bsaving, bool bpublic, bool bprivate, bool breal, const char * vistype)
{
    int bresult = -1;
    if (bpublic || bprivate)
    {
        bresult = pKernel->AddVisible(index, name, type, vistype, bpublic, bprivate, bsaving);
    }
    else
    {
        bresult = pKernel->Add(index, name, type, bsaving);
    }

    write_comp_data(xmlfile, name, 0, bpublic, bprivate);
    if (-1 == bresult)
    {
        return false;
    }

    if (breal)
    {
        pKernel->SetRealtime(index, name, true);
    }

    return true;
}
//#define _write_xml_desc
#ifdef _write_xml_desc
typedef struct _ClassNode
{
    char name[128];
    char type[64];
    char desc[256];
} ClassNode;
ClassNode class_node_list[256];
static int node_cout = 0;
bool read_xml = false;

int read_attribute_nodes(TiXmlElement * pElement, char * type)
{
    if (!pElement)
    {
        return 0;
    }
    TiXmlAttribute * pAttrib = pElement->FirstAttribute();
    if (!pAttrib)

    {
        return 0;
    }
    if (node_cout >= 256)
    {
        return -1;
    }
    int i = 0;
    while (pAttrib)
    {
        if (strcmp(pAttrib->Name(), "id") == 0)
        {
            SPRINTF_S(class_node_list[node_cout].name, "%s", pAttrib->Value());
        }
        else if (strcmp(pAttrib->Name(), "type") == 0)
        {
            strcpy(type, pAttrib->Value());
        }
        else if (strcmp(pAttrib->Name(), "desc") == 0)
        {
            SPRINTF_S(class_node_list[node_cout].desc, "%s", pAttrib->Value());
        }
        i++;
        pAttrib = pAttrib->Next();
    }
    if (strcmp(type, "") != 0)
    {
        SPRINTF_S(class_node_list[node_cout].type, "%s", type);
    }
    node_cout++;
    return i;
}

void read_class_nodes(TiXmlNode * pParent, char * type)
{
    if (!pParent)
    {
        return;
    }
    TiXmlNode * pChild;
    int t = pParent->Type();
    int num = 0;
    if (t == TiXmlNode::TINYXML_ELEMENT)
    {
        num = read_attribute_nodes(pParent->ToElement(), type);
    }
    for (pChild = pParent->FirstChild("class"); pChild != 0; pChild = pChild->NextSibling("class"))
    {
        read_class_nodes(pChild, type);
    }
}

void read_xml_file(const char * pFilename)
{
    TiXmlDocument doc(pFilename);
    bool loadOkay = doc.LoadFile();
    if (loadOkay)
    {

        TiXmlNode * root = doc.FirstChild("logicclass");
        if (root)
        {
            TiXmlNode * pChild;
            for (pChild = root->FirstChild("class"); pChild != 0; pChild = pChild->NextSibling("class"))
            {
                char type[64] = {'\0'};
                int t = pChild->Type();
                int num = 0;
                if (t == TiXmlNode::TINYXML_ELEMENT)
                {
                    num = read_attribute_nodes(pChild->ToElement(), type);
                }
                read_class_nodes(pChild, type);
            }
        }
    }
}
int find_name_index(IKernel * pKernel, int index)
{
    int ret = -1;
    for (int i = 0; i < node_cout; i++)
    {
        int _index = pKernel->GetClassIndex(class_node_list[i].name);
        if (_index == index)
        {
            ret = i;
            break;
        }
    }
    return ret;
}
#endif //_write_xml_desc

//从XML文件中为逻辑类创建属性
//xmlfile是一个以服务器的资源路径为相对路径的文件名
bool createclass_from_xml(IKernel * pKernel, int index, const char * xmlfile)
{

    std::string xmlpath = pKernel->GetResourcePath();
    xmlpath += xmlfile;

#ifdef npc_prop_write_txt
    if (!READ_FLAG)
    {
        std::string logic_class_file =   std::string(pKernel->GetResourcePath()) +
                                         std::string("logic_class.xml");
        get_npc_file_vec(logic_class_file.c_str());
        READ_FLAG = true;
        char dir[256];
        _getcwd(dir, 255);
        _chdir(pKernel->GetResourcePath());
        _mkdir("npc_prop_list");
        _chdir(dir);
    }
    static int class_index = -1;
    FILE * fp = NULL;
    if (index != class_index)
    {
        size_t pos = xmlpath.find_last_of('/');
        size_t endpos = xmlpath.find_last_of('.');
        std::string class_name = xmlpath.substr(pos + 1, endpos - pos - 1);
        if (npc_file_set.find(class_name) != npc_file_set.end())
        {
            class_index = index;

            std::string  out_file = std::string(pKernel->GetResourcePath()) + std::string("npc_prop_list/") +
                                    class_name + ".txt";
            fp = fopen(out_file.c_str(), "w+");
        }
    }



#endif




#ifdef _write_xml_desc
    if (!read_xml)
    {
        std::string logic_class_file =   std::string(pKernel->GetResourcePath()) +
                                         std::string("/logic_class.xml");
        read_xml_file(logic_class_file.c_str());
        read_xml = true;
    }
    int node_index = find_name_index(pKernel, index);
    char buff[512];
    SPRINTF_S(buff, "c:/struct/%s_%s_%s_propertys.txt", class_node_list[node_index].name,
            class_node_list[node_index].type, class_node_list[node_index].desc);
    FILE * fp = fopen(buff, "a+");

#endif

    //判断文件是否存在
    TiXmlDocument doc(xmlpath.c_str());
    if (!doc.LoadFile())
    {
        ::extend_warning(pKernel, "[error]Load xml file failed!");
        ::extend_warning(pKernel, xmlfile);
        return false;
    }

    TiXmlNode * pNodeRoot = doc.FirstChild("object");
    if (pNodeRoot == NULL)
    {
        return false;
    }

    //内部属性
    TiXmlNode * pNodeInternals = pNodeRoot->FirstChild("internals");
    if (pNodeInternals != NULL)
    {
        TiXmlElement * pNode = pNodeInternals->FirstChildElement("property");
        while (pNode != NULL)
        {
            //设置内部的属性是否可保存
            if (!pKernel->SetSaving(index, pNode->Attribute("name"),
                                    convert_boolean(pNode->Attribute("save"))))
            {
                extend_warning(pKernel, "[Error](createclass_from_xml)set saving failed");
                extend_warning(pKernel, xmlfile);
                extend_warning(pKernel, pNode->Attribute("name"));
            }

            //设置内部的属性是否可视
            if (!pKernel->SetVisible(index, pNode->Attribute("name"),
                                     pNode->Attribute("type"),
                                     convert_boolean(pNode->Attribute("public")),
                                     convert_boolean(pNode->Attribute("private"))))
            {
                extend_warning(pKernel, "[Error](createclass_from_xml)set visible failed");
                extend_warning(pKernel, xmlfile);
                extend_warning(pKernel, pNode->Attribute("name"));
            }

            write_comp_data(xmlfile, pNode->Attribute("name"), 0, convert_boolean(pNode->Attribute("public")), convert_boolean(pNode->Attribute("private")));

            //设置内部的属性是否实时
            if (!pKernel->SetRealtime(index, pNode->Attribute("name"),
                                      convert_boolean(pNode->Attribute("realtime"))))
            {
                extend_warning(pKernel, "[Error](createclass_from_xml)set reailtime failed");
                extend_warning(pKernel, xmlfile);
                extend_warning(pKernel, pNode->Attribute("name"));
            }
#ifdef npc_prop_write_txt
            if (class_index != -1)
            {
                NPC_WRITE_PROP tem;
                tem.prop_name = pNode->Attribute("name") ? pNode->Attribute("name") : std::string();
                tem.pub = pNode->Attribute("public") ? pNode->Attribute("public") : std::string("false");
                tem.pri = pNode->Attribute("private") ? pNode->Attribute("private") : std::string("false");
                tem.sav = pNode->Attribute("save") ? pNode->Attribute("save") : std::string("false");
                tem.typ = pNode->Attribute("type") ? pNode->Attribute("type") : std::string();
                tem.des = pNode->Attribute("desc") ? pNode->Attribute("desc") : std::string();
                tem.rea = pNode->Attribute("realtime") ? pNode->Attribute("realtime") : std::string("false");
                npc_wr_vec.push_back(tem);
            }

#endif




#ifdef _write_xml_desc
            if (fp != NULL)
            {
                char line[256];
                SPRINTF_S(line, "%s\t%s\t%s\t%s\n", pNode->Attribute("name"), pNode->Attribute("type"),
                        pNode->Attribute("save"), pNode->Attribute("desc"));
                fputs(line, fp);
            }
#endif
            pNode = pNode->NextSiblingElement("property");
        }
    }

    //基础属性
    TiXmlNode * pNodeProperties = pNodeRoot->FirstChild("properties");
    if (pNodeProperties != NULL)
    {
        TiXmlElement * pNode = pNodeProperties->FirstChildElement("property");
        while (pNode != NULL)
        {
            //添加属性
            if (!createclass_add_property(pKernel, xmlfile, index,
                                          pNode->Attribute("name"),
                                          convert_var_type(pNode->Attribute("type")),
                                          convert_boolean(pNode->Attribute("save")),
                                          convert_boolean(pNode->Attribute("public")),
                                          convert_boolean(pNode->Attribute("private")),
                                          convert_boolean(pNode->Attribute("realtime")),
                                          pNode->Attribute("type")))
            {
                extend_warning(pKernel, "[Error](createclass_from_xml)add property failed");
                extend_warning(pKernel, xmlfile);
                extend_warning(pKernel, pNode->Attribute("name"));
            }

#ifdef npc_prop_write_txt
            if (class_index != -1)
            {
                NPC_WRITE_PROP tem;
                tem.prop_name = pNode->Attribute("name") ? pNode->Attribute("name") : std::string();
                tem.pub = pNode->Attribute("public") ? pNode->Attribute("public") : std::string("false");
                tem.pri = pNode->Attribute("private") ? pNode->Attribute("private") : std::string("false");
                tem.sav = pNode->Attribute("save") ? pNode->Attribute("save") : std::string("false");
                tem.typ = pNode->Attribute("type") ? pNode->Attribute("type") : std::string();
                tem.des = pNode->Attribute("desc") ? pNode->Attribute("desc") : std::string();
                tem.rea = pNode->Attribute("realtime") ? pNode->Attribute("realtime") : std::string("false");
                npc_wr_vec.push_back(tem);
            }
#endif
#ifdef _write_xml_desc
            if (fp != NULL)
            {
                char line[256];
                SPRINTF_S(line, "%s\t%s\t%s\t%s\n", pNode->Attribute("name"), pNode->Attribute("type"),
                        pNode->Attribute("save"), pNode->Attribute("desc"));
                fputs(line, fp);
            }
#endif
            pNode = pNode->NextSiblingElement("property");
        }
    }
#ifdef _write_xml_desc
    if (fp != NULL)
    {
        fclose(fp);
    }
#endif

#ifdef _write_xml_desc
    if (fp != NULL)
    {
        fclose(fp);
    }
    SPRINTF_S(buff, "c:/struct/%s_%s_%s_records.txt", class_node_list[node_index].name,
            class_node_list[node_index].type, class_node_list[node_index].desc);
    fp = fopen(buff, "a+");
#endif
    //表格属性
    TiXmlNode * pNodeRecords = pNodeRoot->FirstChild("records");
    if (pNodeRecords != NULL)
    {
        TiXmlElement * pNode = pNodeRecords->FirstChildElement("record");
        while (pNode != NULL)
        {
            const char * name = pNode->Attribute("name");
            int cols = ::atoi(pNode->Attribute("cols"));
            int max_rows = ::atoi(pNode->Attribute("maxrows"));
            bool saved = convert_boolean(pNode->Attribute("save"));
            //添加表格
            if (!pKernel->AddRecord(index, name, cols, max_rows, saved))
            {
                extend_warning(pKernel, "[Error](createclass_from_xml)add record failed");
                extend_warning(pKernel, xmlfile);
                extend_warning(pKernel, name);
            }
#ifdef npc_prop_write_txt
            if (class_index != -1)
            {
                NPC_WRITE_PROP tem;
                tem.prop_name = pNode->Attribute("name") ? pNode->Attribute("name") : std::string();
                tem.pub = pNode->Attribute("public") ? pNode->Attribute("public") : std::string("false");
                tem.pri = pNode->Attribute("private") ? pNode->Attribute("private") : std::string("false");
                tem.sav = pNode->Attribute("save") ? pNode->Attribute("save") : std::string("false");
                tem.typ = pNode->Attribute("type") ? pNode->Attribute("type") : std::string();
                tem.des = pNode->Attribute("desc") ? pNode->Attribute("desc") : std::string();
                tem.rea = pNode->Attribute("realtime") ? pNode->Attribute("realtime") : std::string("false");
                npc_wr_rec_vec.push_back(tem);
            }
#endif
#ifdef _write_xml_desc
            if (fp != NULL)
            {
                char line[256];
                SPRINTF_S(line, "%s", "################################################################\n");
                fputs(line, fp);
                SPRINTF_S(line, "name\t[%s]\nsave\t[%s]\nmaxrows\t[%s]\ncol\t[%s]\n", pNode->Attribute("name"),
                        pNode->Attribute("save"),
                        pNode->Attribute("maxrows"),
                        pNode->Attribute("cols"));
                fputs(line, fp);
            }
#endif
            //设置表格列的数据类型
            int col = 0;
            TiXmlElement * pColumn = pNode->FirstChildElement("column");
            while (pColumn != NULL)
            {
                if (!pKernel->SetRecordColType(index, name, col,
                                               convert_var_type(pColumn->Attribute("type"))))
                {
                    extend_warning(pKernel, "[Error](createclass_from_xml)set record col type failed");
                    extend_warning(pKernel, xmlfile);
                    extend_warning(pKernel, name);
                }
#ifdef _write_xml_desc
                if (fp != NULL)
                {
                    char line[256];
                    SPRINTF_S(line, "%d\t%s\t%s\n", col, pColumn->Attribute("type"), pColumn->Attribute("desc"));
                    fputs(line, fp);
                }
#endif
                pColumn = pColumn->NextSiblingElement("column");
                col++;
            }

#ifdef _write_xml_desc
            if (fp != NULL)
            {
                char line[256];
                SPRINTF_S(line, "desc\t[%s]\n", pNode->Attribute("desc"));
                fputs(line, fp);
            }
#endif
            //设置表格是否可视
            bool bpublic = convert_boolean(pNode->Attribute("public"));
            bool bprivate = convert_boolean(pNode->Attribute("private"));
            if (bpublic || bprivate)
            {
                write_comp_data(xmlfile, name, 1, bpublic, bprivate);
                if (pKernel->SetRecordVisible(index, name, bpublic, bprivate))
                {
                    int col = 0;
                    TiXmlElement * pColumn = pNode->FirstChildElement("column");
                    while (pColumn != NULL)
                    {
                        if (!pKernel->SetRecordColVisType(index, name, col,
                                                          pColumn->Attribute("type")))
                        {
                            extend_warning(pKernel, "[Error](createclass_from_xml)set record col vis type failed");
                            extend_warning(pKernel, xmlfile);
                            extend_warning(pKernel, name);
                        }
                        pColumn = pColumn->NextSiblingElement("column");
                        col++;
                    }
                }
                else
                {
                    extend_warning(pKernel, "[Error](createclass_from_xml)set record visible failed");
                    extend_warning(pKernel, xmlfile);
                    extend_warning(pKernel, name);
                }
            }
            pNode = pNode->NextSiblingElement("record");
        }
    }

#ifdef _write_xml_desc
    if (fp != NULL)
    {
        fclose(fp);
    }
#endif
    //外包含属性
    TiXmlNode * pNodeIncludes = pNodeRoot->FirstChild("includes");
    if (pNodeIncludes != NULL)
    {
        TiXmlElement * pNode = pNodeIncludes->FirstChildElement("path");
        while (pNode != NULL)
        {
            const char * path = pNode->Attribute("name");
            //从其他XML文件创建属性
            if (!createclass_from_xml(pKernel, index, path))
            {
                extend_warning(pKernel, "[Error](createclass_from_xml)create include file failed");
                extend_warning(pKernel, xmlfile);
                extend_warning(pKernel, pNode->Attribute("name"));
            }

            pNode = pNode->NextSiblingElement("path");
        }
    }
#ifdef npc_prop_write_txt
    if (fp != NULL && class_index != -1)
    {
        std::sort(npc_wr_vec.begin(), npc_wr_vec.end());
        std::sort(npc_wr_rec_vec.begin(), npc_wr_rec_vec.end());
        for (size_t i = 0; i < npc_wr_vec.size(); i++)
        {
            std::string line = npc_wr_vec[i].prop_name + "\t";
            line += npc_wr_vec[i].pub + "\t";
            line += npc_wr_vec[i].pri + "\t";
            line += npc_wr_vec[i].sav + "\t";
            line += npc_wr_vec[i].rea + "\t";
            line += npc_wr_vec[i].des + "\n";
            fputs(line.c_str(), fp);
        }
        fputs("*********************************records***********\
				  **********************\n", fp);
        for (size_t i = 0; i < npc_wr_rec_vec.size(); i++)
        {
            std::string line = npc_wr_rec_vec[i].prop_name + "\t";
            line += npc_wr_rec_vec[i].pub + "\t";
            line += npc_wr_rec_vec[i].pri + "\t";
            line += npc_wr_rec_vec[i].sav + "\t";
            line += npc_wr_rec_vec[i].rea + "\t";
            line += npc_wr_rec_vec[i].des + "\n";
            fputs(line.c_str(), fp);
        }
        npc_wr_vec.clear();
        npc_wr_rec_vec.clear();
        fclose(fp);
        fp = NULL;
        class_index = -1;
    }

#endif
    return true;
}

//获取第一个指定类型的父对象
PERSISTID find_firsttype_parent(IKernel * pKernel, const PERSISTID & obj, int type)
{
    PERSISTID parent = pKernel->Parent(obj);
    while (pKernel->Exists(parent))
    {
        if (pKernel->Type(parent) == type)
        {
            return parent;
        }

        parent = pKernel->Parent(parent);
    }

    return parent;
}

//获得某个游戏对象（npc或player对象）主人的函数
PERSISTID get_sceneobj_master(IKernel * pKernel, const PERSISTID & self)
{
    PERSISTID master = self;

	IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (pSelfObj != NULL)
    {
        // 有主人的NPC
        if (pSelfObj->FindAttr("Master"))
        {
            //被攻击者是宠物
            if ( pSelfObj->GetClassType() == TYPE_NPC )
            {
                master = pSelfObj->QueryObject("Master");
            }
        }
    }


    return master;
}

IGameObj* get_sceneobj_master(IKernel* pKernel, IGameObj* pNpcObj)
{
	if (!pNpcObj || pNpcObj->GetClassType() != TYPE_NPC)
	{
		return pNpcObj;
	}

	if (!pNpcObj->FindAttr("Master"))
	{
		return pNpcObj;
	}

	PERSISTID master = pNpcObj->QueryObject("Master");
	IGameObj* pMasterObj = pKernel->GetGameObj(master);
	if (!pMasterObj)
	{
		pMasterObj = pNpcObj;
	}
	return pMasterObj;
}

PERSISTID get_pet_master(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return PERSISTID();
	}
	PERSISTID master = self;
	const char* szSelfScript = "";
	if (pSelfObj->GetClassType() == TYPE_NPC)
	{
		szSelfScript = pKernel->GetScript(self);
	}

	if (strcmp(szSelfScript, "PetNpc") == 0)
	{
		master = get_sceneobj_master(pKernel, self);
	}
	return master;
}

//执行逻辑模块的脚本
bool run_logic_lua_script(IKernel * pKernel, const char * script, const char * func, const IVarList & args,
                          int res_num, IVarList * res)
{
    if (!pKernel->LuaFindScript(script))
    {
        if (!pKernel->LuaLoadScript(script))
        {
            return false;
        }
    }

    if (!pKernel->LuaFindFunc(script, func))
    {
        return false;
    }

    return pKernel->LuaRunFunc(script, func, args, res_num, res);
}

//在某个范围内获得随机的可行走点
bool get_random_position(IKernel * pKernel, float center_x, float center_z, float radius,
                         float & random_x, float & random_y, float & random_z, float & random_orient)
{
    //尝试次数
    int try_count = 0;
    //是否成功
    bool bfound = false;
    //随机取周围的点
    while (try_count < 20)
    {
        random_x = center_x + ::util_random_float(radius * 2) / 2;
        random_z = center_z + ::util_random_float(radius * 2) / 2;

        if (pKernel->CanWalk(random_x, random_z))
        {
            //寻找到正确的位置了
            bfound = true;
            break;
        }

        try_count++;
    }

    if (!bfound)
    {
        //未找到
        random_x = center_x;
        random_z = center_z;
    }

    random_y = pKernel->GetWalkHeight(random_x, random_z);
    random_orient = PI2 * ::util_random_float(1.0);

    return true;
}

//保存到XML格式的字符串中（自定义格式，没有XML文件常用的头信息等）
bool save_to_string(IKernel * pKernel, const PERSISTID & self, fast_string & szContent,
					bool bIncludeUnsave, bool bIncudeInvisible)
{
	TiXmlElement element("Object");

	if (!save_to_xml(pKernel, self, element, bIncludeUnsave, bIncudeInvisible))
	{
		return false;
	}

	TiXmlPrinter printer;
	element.Accept(&printer);

	if (strlen(printer.CStr()) <= 0)
	{
		return false;
	}

	szContent = printer.CStr();
	return true;
}



bool save_to_xml(IKernel * pKernel, const PERSISTID & self, TiXmlElement & element,
                 bool bIncludeUnsave, bool bIncudeInvisible)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

    // 首先保存该物品的配置文件和创建脚本
    element.SetAttribute("Config", pKernel->GetConfig(self));

    TiXmlElement elementProperties("Properties");

    // 保存应该保存的属性
    CVarList res;
    pSelfObj->GetAttrList(res);

    size_t size = res.GetCount();

    for (size_t i = 0; i < size; ++i)
    {
        const char * prop = res.StringVal(i);

        // 需保存的属性总是要导出
        if (!pSelfObj->GetAttrSaving(prop))
        {
            // 是否包含无需保存的属性
            if (!bIncludeUnsave)
            {
                continue;
            }

            // 是否包含不可视的属性
            if (!bIncudeInvisible && !pSelfObj->GetAttrVisible(prop))
            {
                continue;
            }
        }

        switch (pSelfObj->GetAttrType(prop))
        {
            case VTYPE_INT:
            {
                int value = pSelfObj->QueryInt(prop);

                if (value != 0)
                {
                    elementProperties.SetAttribute(prop, value);
                }
                else if (is_save_prop_null(prop))
                {
                    elementProperties.SetAttribute(prop, value);
                }

            }
            break;

            case VTYPE_INT64:
            {
                __int64 value = pSelfObj->QueryInt64(prop);

                if (value != 0)
                {
                    elementProperties.SetAttribute(prop, ::util_int64_as_string(value).c_str());
                }
                else if (is_save_prop_null(prop))
                {
                    elementProperties.SetAttribute(prop, ::util_int64_as_string(value).c_str());
                }
            }
            break;

            case VTYPE_FLOAT:
            {
                float value = pSelfObj->QueryFloat(prop);

                if (value < 0.0f || value > 0.0f)
                {
                    elementProperties.SetDoubleAttribute(prop, value);
                }
                else if (is_save_prop_null(prop))
                {
                    elementProperties.SetDoubleAttribute(prop, value);
                }
            }
            break;

            case VTYPE_DOUBLE:
            {
                double value = pSelfObj->QueryDouble(prop);

                if (value < 0.0f || value > 0.0f)
                {
                    elementProperties.SetDoubleAttribute(prop, value);
                }
                else if (is_save_prop_null(prop))
                {
                    elementProperties.SetDoubleAttribute(prop, value);
                }
            }
            break;

            case VTYPE_STRING:
            {
                const char * value = pSelfObj->QueryString(prop);

                if (strlen(value) > 0)
                {
                    elementProperties.SetAttribute(prop, value);
                }
                else if (is_save_prop_null(prop))
                {
                    elementProperties.SetAttribute(prop, value);
                }
            }
            break;

            case VTYPE_WIDESTR:
            {
                const wchar_t * value = pSelfObj->QueryWideStr(prop);
                if (wcslen(value) > 0)
                {
                    elementProperties.SetAttribute(prop, ::util_widestr_as_string(value).c_str());
                }
                else if (is_save_prop_null(prop))
                {
                    elementProperties.SetAttribute(prop, ::util_widestr_as_string(value).c_str());
                }
            }
            break;
        }
    }

    element.InsertEndChild(elementProperties);


    // 保存应该保存的属性
    TiXmlElement elementRecords("Records");

    res.Clear();
    pSelfObj->GetRecordList(res);
    size = res.GetCount();

    for (size_t i = 0; i < size; ++i)
    {
        const char * recname = res.StringVal(i);
        if (!recname || recname[0] == 0)
        {
            continue;
        }

		// 获取表
		IRecord* pRecord = pSelfObj->GetRecord(recname);
		if (pRecord == NULL)
		{
			continue;
		}

        // 需保存的属性总是要导出
        if (!pSelfObj->GetRecordSaving(recname))
        {
            // 是否包含无需保存的属性
            if (!bIncludeUnsave)
            {
                continue;
            }

            // 是否包含不可视的属性
            if (!bIncudeInvisible && !pSelfObj->GetRecordVisible(recname))
            {
                continue;
            }
        }

        // 表格大小
        int rows = pRecord->GetRows();
        int cols = pRecord->GetCols();

        // 如果为空不保存
        if (rows <= 0 || cols <= 0)
        {
            continue;
        }

        // 保存该表
        TiXmlElement * elementRecord = elementRecords.InsertEndChild(TiXmlElement("Record"))->ToElement();

        // 设置表名
        elementRecord->SetAttribute("name", recname);
        // 设置表的行数
        char strrows[32];
        SPRINTF_S(strrows, "%d", rows);
        elementRecord->SetAttribute("rows", strrows);

        for (int r = 0; r < rows; ++r)
        {
            // 表中添加一行
            TiXmlElement * elementRow = elementRecord->InsertEndChild(TiXmlElement("Row"))->ToElement();

            // 设置行数
            char row[32];
            SPRINTF_S(row, "%d", r);
            elementRow->SetAttribute("row", row);

            for (int c = 0; c < cols; ++c)
            {
                // 列数作为属性名，属性名只能为字符串，加入前缀"c"
                char col[32];
                SPRINTF_S(col, "c%d", c);

                // 保存数据
                switch (pRecord->GetColType(c))
                {
                    case VTYPE_INT:
                    {
                        int nValue = pRecord->QueryInt(r, c);
                        elementRow->SetAttribute(col, nValue);
                    }
                    break;
                    case VTYPE_INT64:
                    {
                        __int64 value = pRecord->QueryInt64(r, c);
                        elementRow->SetAttribute(col, ::util_int64_as_string(value).c_str());
                    }
                    break;
                    case VTYPE_FLOAT:
                    {
                        float value = pRecord->QueryFloat(r, c);
                        elementRow->SetDoubleAttribute(col, value);
                    }
                    break;
                    case VTYPE_DOUBLE:
                    {
                        double value = pRecord->QueryDouble(r, c);
                        elementRow->SetDoubleAttribute(col, value);
                    }
                    break;
                    case VTYPE_STRING:
                    {
                        const char * value = pRecord->QueryString(r, c);
                        elementRow->SetAttribute(col, value);
                    }
                    break;
                    case VTYPE_WIDESTR:
                    {
                        const wchar_t * value = pRecord->QueryWideStr(r, c);
                        elementProperties.SetAttribute(col, ::util_widestr_as_string(value).c_str());
                    }
                    break;
                    case VTYPE_OBJECT:
                    {
                        PERSISTID obj = pRecord->QueryObject(r, c);
                        fast_string value = ObjToString(obj);
                        elementRow->SetAttribute(col, value.c_str());
                    }
                    break;
                    default:
                        break;
                }
            }
        }
    }

    element.InsertEndChild(elementRecords);

    return true;
}

bool out_put_xml(IKernel* pKernel, const PERSISTID& self, const char* file)
{
	TiXmlDocument doc;

	doc.InsertEndChild(TiXmlDeclaration("1.0", "gb2312", ""));

	TiXmlElement* element = doc.InsertEndChild(TiXmlElement("nx_obj"))->ToElement();

	output_object_xml(pKernel, self, element);

	if (!doc.SaveFile(file))
	{
		return false;
	}

	return true;
}

bool output_object_xml(IKernel* pKernel, const PERSISTID& self, TiXmlElement* root)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	TiXmlElement* object = root->InsertEndChild(TiXmlElement("object"))->ToElement();

	object->SetAttribute("version", int(util_get_time_64()));

	int nType = pKernel->Type(self);

	if (nType == TYPE_PLAYER)
	{
		object->SetAttribute("type","TYPE_PLAYER");
		object->SetAttribute("config", ::util_widestr_as_utf8(pSelfObj->QueryWideStr("Name")).c_str());
	}
	else if (nType == TYPE_NPC)
	{
		object->SetAttribute("type","TYPE_NPC");
		object->SetAttribute("config", pKernel->GetConfig(self));
	}
	else if (nType == TYPE_ITEM)
	{
		object->SetAttribute("type","TYPE_ITEM");
		object->SetAttribute("config", pKernel->GetConfig(self));
		object->SetAttribute("Script", pKernel->GetScript(self));
	}

	// 属性
	TiXmlElement* properties = object->InsertEndChild(TiXmlElement("properties"))->ToElement();

	CVarList res;
	pSelfObj->GetAttrList(res);
	properties->SetAttribute("count", int(res.GetCount()));

	for (size_t i = 0; i < res.GetCount(); ++i)
	{
		TiXmlElement* property = properties->InsertEndChild(TiXmlElement("property"))->ToElement();

		const char* prop = res.StringVal(i);
		unsigned char type = pSelfObj->GetAttrType(prop);
		property->SetAttribute("name", prop);

		// 是否保存
		property->SetAttribute("Save", pSelfObj->GetAttrSaving(prop));

		// 是否可视
		property->SetAttribute("Visible", pSelfObj->GetAttrVisible(prop));

		// 是否可视
		property->SetAttribute("Public", pSelfObj->GetAttrPublicVisible(prop));

		switch (type)
		{
		case VTYPE_INT:
			{
				int value = pSelfObj->QueryInt(prop);
				property->SetAttribute("Type", "int");
				property->SetAttribute("value", value);
			}
			break;
		case VTYPE_INT64:
			{
				__int64 value = pSelfObj->QueryInt64(prop);
				property->SetAttribute("Type", "int64");
				property->SetAttribute("value", ::util_int64_as_string(value).c_str());
			}
			break;
		case VTYPE_FLOAT:
			{
				float value = pSelfObj->QueryFloat(prop);
				property->SetAttribute("Type", "float");
				property->SetDoubleAttribute("value", value);
			}
			break;
		case VTYPE_DOUBLE:
			{
				double value = pSelfObj->QueryDouble(prop);
				property->SetAttribute("Type", "double");
				property->SetDoubleAttribute("value", value);
			}
			break;
		case VTYPE_STRING:
			{
				const char* value = pSelfObj->QueryString(prop);
				property->SetAttribute("Type", "string");
				property->SetAttribute("value", value);
			}
			break;
		case VTYPE_WIDESTR:
			{
				const wchar_t* value = pSelfObj->QueryWideStr(prop);
				property->SetAttribute("Type", "wstring");
				fast_string utf8_val = util_widestr_as_utf8(value);
				property->SetAttribute("value", utf8_val.c_str());
			}
			break;
		case VTYPE_OBJECT:
			{
				PERSISTID obj = pSelfObj->QueryObject(prop);
				property->SetAttribute("Type", "Object");
				fast_string value = ::util_int64_as_string(obj.nIdent) +"-"+ util_int64_as_string(obj.nSerial);
				property->SetAttribute("value", value.c_str());
			}
			break;
		default:
			break;
		}
	}

	// 表格
	TiXmlElement* records = object->InsertEndChild(TiXmlElement("records"))->ToElement();

	CVarList res1;
	pSelfObj->GetRecordList(res1);
	records->SetAttribute("count", int(res1.GetCount()));

	for (size_t i = 0; i < res1.GetCount(); ++i)
	{
		TiXmlElement* record = records->InsertEndChild(TiXmlElement("record"))->ToElement();

		const char* recname = res1.StringVal(i);
		if (StringUtil::CharIsNull(recname))
		{
			continue;
		}

		// 获取表
		IRecord* pRecord = pSelfObj->GetRecord(recname);
		if (pRecord == NULL)
		{
			continue;
		}

		record->SetAttribute("name",    recname);
		record->SetAttribute("row_max", pRecord->GetRowMax());
		record->SetAttribute("rows",    pRecord->GetRows());
		record->SetAttribute("cols",    pRecord->GetCols());
		record->SetAttribute("Save",    pSelfObj->GetRecordSaving(recname));
		record->SetAttribute("Visible", pSelfObj->GetRecordVisible(recname));
		record->SetAttribute("Public", pSelfObj->GetRecordPublicVisible(recname));

		TiXmlElement* coltype = record->InsertEndChild(TiXmlElement("coltype"))->ToElement();

		int nCols = pRecord->GetCols();
		int nRows = pRecord->GetRows();

		for (int k = 0; k < nCols; ++k)
		{
			int nColType = pRecord->GetColType(k);

			char buf[32];
			SPRINTF_S(buf, "c%d", k);

			switch (nColType)
			{
			case VTYPE_INT:
				coltype->SetAttribute(buf, "int");
				break;
			case VTYPE_INT64:
				coltype->SetAttribute(buf, "int64");
				break;
			case VTYPE_FLOAT:
				coltype->SetAttribute(buf, "float");
				break;
			case VTYPE_DOUBLE:
				coltype->SetAttribute(buf, "double");
				break;
			case VTYPE_STRING:
				coltype->SetAttribute(buf, "string");
				break;
			case VTYPE_WIDESTR:
				coltype->SetAttribute(buf, "widestr");
				break;
			case VTYPE_OBJECT:
				coltype->SetAttribute(buf, "object");
				break;
			default:
				break;
			}
		}

		for (int r = 0; r < nRows; ++r)
		{
			TiXmlElement* row = record->InsertEndChild(TiXmlElement("row"))->ToElement();

			for (int c = 0; c < nCols; ++c)
			{
				char col[32];

				SPRINTF_S(col, "c%d", c);
				int nColType = pRecord->GetColType(c);

				switch (nColType)
				{
				case VTYPE_INT:
					{
						int nValue = pRecord->QueryInt(r, c);
						row->SetAttribute(col, nValue);	
					}
					break;
				case VTYPE_INT64:
					{
						__int64 value = pRecord->QueryInt64(r, c);
						row->SetAttribute(col, ::util_int64_as_string(value).c_str());
					}
					break;
				case VTYPE_FLOAT:
					{
						float value = pRecord->QueryFloat(r, c);
						row->SetDoubleAttribute(col, value);
					}
					break;
				case VTYPE_DOUBLE:
					{
						double value = pRecord->QueryDouble(r, c);
						row->SetDoubleAttribute(col, value);	
					}
					break;
				case VTYPE_STRING:
					{
						const char* value = pRecord->QueryString(r, c);
						row->SetAttribute(col, value);	
					}
					break;
				case VTYPE_WIDESTR:
					{
						const wchar_t* value = pRecord->QueryWideStr(r, c);
						fast_string utf8_val = util_widestr_as_utf8(value);
						row->SetAttribute(col, utf8_val.c_str());
					}
					break;
				case VTYPE_OBJECT:
					{
						PERSISTID obj = pRecord->QueryObject(r, c);
						fast_string value = ::util_int64_as_string(obj.nIdent) +"-"+ util_int64_as_string(obj.nSerial);
						row->SetAttribute(col, value.c_str());
					}
					break;
				default:
					break;
				}
			}
		}
	}

	// 容器和子对象
	TiXmlElement* container = object->InsertEndChild(TiXmlElement("container"))->ToElement();

	CVarList ResChild;
	pKernel->GetChildList(self, 0, ResChild);

	container->SetAttribute("count", int(ResChild.GetCount()));

	for (size_t i = 0; i < ResChild.GetCount(); ++i)
	{
		TiXmlElement* child = container->InsertEndChild(TiXmlElement("child"))->ToElement();

		PERSISTID childobj = ResChild.ObjectVal(i);

		if (!output_object_xml(pKernel,childobj, child))
		{
			return false;
		}
	}

	return true;
}

bool player_info_to_string(IKernel * pKernel, const PERSISTID & player, fast_string & szContent)
{
	szContent = "";

	static const std::string szRoleInfos[] = {
		"Job", "Level", "BattleAbility", "Sex", "Weapon", "Cloth", 
	};

	IGameObj *pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)
	{
		return false;
	}

	for (int i = 0; i < sizeof(szRoleInfos)/sizeof(std::string); ++i)
	{
		if (!pPlayer->FindAttr(szRoleInfos[i].c_str()))
		{
			continue;
		}

		int nType = pPlayer->GetAttrType(szRoleInfos[i].c_str());
		if (nType == VTYPE_INT)
		{
			int nValue = pPlayer->QueryInt(szRoleInfos[i].c_str());
			szContent += szRoleInfos[i].c_str();
			szContent += ",";
			szContent += util_int64_as_string(nValue).c_str();
		}
		else if (nType == VTYPE_STRING)
		{
			const char * szValue = pPlayer->QueryString(szRoleInfos[i].c_str());
			szContent += szRoleInfos[i].c_str();
			szContent += ",";
			szContent += szValue;
		}
		else if (nType == VTYPE_INT64)
		{
			__time64_t nValue64 = pPlayer->QueryInt64(szRoleInfos[i].c_str());
			szContent += szRoleInfos[i].c_str();
			szContent += ",";
			szContent += util_int64_as_string(nValue64).c_str();
		}
		else
		{
			szContent += szRoleInfos[i].c_str();
			szContent += ",";
			szContent += "";
		}
		szContent += ";";
	}

	return true;
}

// 取玩家属性面板上的属性
bool player_prop_to_string(IKernel * pKernel, const PERSISTID & player, fast_string & szContent)
{
	szContent = "";

	static const std::string szRoleProps[] = {
		"Str", "Dex", "Sta", 
		"HP", "MaxHP", "MinAttack", "MaxAttack", "Defend",
		"Hit", "Miss", "Crit", "Toug", "RunSpeed"
	};

	IGameObj *pPlayer = pKernel->GetGameObj(player);
	if (NULL == pPlayer)
	{
		return false;
	}

	for (int i = 0; i < sizeof(szRoleProps)/sizeof(std::string); ++i)
	{
		if (!pPlayer->FindAttr(szRoleProps[i].c_str()))
		{
			continue;
		}

		int nType = pPlayer->GetAttrType(szRoleProps[i].c_str());
		if (nType == VTYPE_INT)
		{
			int nValue = pPlayer->QueryInt(szRoleProps[i].c_str());
			szContent += szRoleProps[i].c_str();
			szContent += ",";
			szContent += util_int64_as_string(nValue).c_str();
		}
		else if (nType == VTYPE_FLOAT)
		{
			float fValue = pPlayer->QueryFloat(szRoleProps[i].c_str());
			szContent += szRoleProps[i].c_str();
			szContent += ",";
			szContent += util_int64_as_string((int64_t)fValue).c_str();
		}
		else if (nType == VTYPE_STRING)
		{
			const char * szValue = pPlayer->QueryString(szRoleProps[i].c_str());
			szContent += szRoleProps[i].c_str();
			szContent += ",";
			szContent += szValue;
		}
		else if (nType == VTYPE_INT64)
		{
			__time64_t nValue64 = pPlayer->QueryInt64(szRoleProps[i].c_str());
			szContent += szRoleProps[i].c_str();
			szContent += ",";
			szContent += util_int64_as_string(nValue64).c_str();
		}
		else
		{
			szContent += szRoleProps[i].c_str();
			szContent += ",";
			szContent += "";
		}
		szContent += ";";
	}

	return true;
}

// 取一件装备属性
bool player_equip_to_string(IKernel * pKernel, const PERSISTID & equip, fast_string & szContent)
{
	szContent = "";

	static const std::string strEquipProps[] = {
		"Config", "ColorLevel", "BattleAbility", "AddPackage", "ModifyAddPackage"
	};

	IGameObj *pEquip = pKernel->GetGameObj(equip);
	if (NULL == pEquip)
	{
		return false;
	}

	for (int i = 0; i < sizeof(strEquipProps)/sizeof(std::string); ++i)
	{
		if (!pEquip->FindAttr(strEquipProps[i].c_str()))
		{
			continue;
		}

		int nType = pEquip->GetAttrType(strEquipProps[i].c_str());
		if (nType == VTYPE_INT)
		{
			int nValue = pEquip->QueryInt(strEquipProps[i].c_str());
			szContent += strEquipProps[i].c_str();
			szContent += ",";
			szContent += util_int64_as_string(nValue).c_str();
		}
		else if (nType == VTYPE_STRING)
		{
			const char * szValue = pEquip->QueryString(strEquipProps[i].c_str());
			szContent += strEquipProps[i].c_str();
			szContent += ",";
			szContent += szValue;
		}
		else if (nType == VTYPE_INT64)
		{
			__time64_t nValue64 = pEquip->QueryInt64(strEquipProps[i].c_str());
			szContent += strEquipProps[i].c_str();
			szContent += ",";
			szContent += util_int64_as_string(nValue64).c_str();
		}
		else
		{
			szContent += strEquipProps[i].c_str();
			szContent += ",";
			szContent += "";
		}
		szContent += ";";
	}
	
	return true;
}

// 取一件物品属性
bool player_item_to_string(IKernel * pKernel, const PERSISTID & item, fast_string & szContent)
{
	IGameObj *pItem = pKernel->GetGameObj(item);
	if (NULL == pItem)
	{
		return false;
	}

	// 保存应该保存的属性
	CVarList res;
	pItem->GetAttrList(res);

	size_t size = res.GetCount();

	for (size_t i = 0; i < size; ++i)
	{
		const char * prop = res.StringVal(i);

		// 不保存并且不可视
		if (!pItem->GetAttrSaving(prop) && 
			!pItem->GetAttrVisible(prop))
		{
			continue;
		}

		switch (pItem->GetAttrType(prop))
		{
		case VTYPE_INT:
			{
				int value = pItem->QueryInt(prop);
				if (value != 0)
				{
					szContent += prop;
					szContent += ",";
					szContent += util_int64_as_string(value).c_str();
					szContent += ";";
				}
			}
			break;

		case VTYPE_INT64:
			{
				__int64 value = pItem->QueryInt64(prop);
				if (value != 0)
				{
					szContent += prop;
					szContent += ",";
					szContent += util_int64_as_string(value).c_str();
					szContent += ";";
				}
			}
			break;

		case VTYPE_FLOAT:
			{
				float value = pItem->QueryFloat(prop);
				if (FloatEqual(value, 0.0f))
				{
					szContent += prop;
					szContent += ",";
					szContent += StringUtil::FloatAsString(value).c_str();
					szContent += ";";
				}
			}
			break;

		case VTYPE_DOUBLE:
			{
				double value = pItem->QueryDouble(prop);

				if (DoubleEqual(value, 0.0f))
				{
					szContent += prop;
					szContent += ",";
					szContent += StringUtil::DoubleAsString(value).c_str();
					szContent += ";";
				}
			}
			break;

		case VTYPE_STRING:
			{
				const char * value = pItem->QueryString(prop);
				if (!StringUtil::CharIsNull(value))
				{
					szContent += prop;
					szContent += ",";
					szContent += value;
					szContent += ";";
				}
			}
			break;

		case VTYPE_WIDESTR:
			{
				const wchar_t * value = pItem->QueryWideStr(prop);
				if (!StringUtil::CharIsNull(value))
				{
					szContent += prop;
					szContent += ",";
					szContent += ::util_widestr_as_string(value).c_str();
					szContent += ";";
				}
			}
			break;
		}
	}

	return true;
}

//从上面保存的XML内容的字符器创建物品
PERSISTID load_from_string(IKernel * pKernel, const PERSISTID & parent, const std::string & szContent, int index /*= 0*/)
{
    TiXmlElement element("Object");
    element.Parse(szContent.c_str(), NULL, TIXML_DEFAULT_ENCODING);

    return load_from_xml(pKernel, parent, element, index);
}

PERSISTID load_from_xml(IKernel * pKernel, const PERSISTID & parent, const TiXmlElement & element, int index /*= 0*/)
{
    PERSISTID obj;

    const char * szconfig = element.Attribute("Config");
    const char * szscript = "";

    if (szconfig == NULL)
    {
        const char * perror = "element.Attribute(\"Config\"); error == NULL ";
        pKernel->Trace(perror);
        ::extend_warning(pKernel, perror);
        return obj;
    }

    // 创建物品
    obj = pKernel->CreateFromConfig(parent, szscript, szconfig);

    if (!pKernel->Exists(obj))
    {
        pKernel->Trace("load_from_xml error:");
        ::extend_warning(pKernel, szscript);
        ::extend_warning(pKernel, szconfig);

        return obj;
    }

    // 放置到正确的位置
    if (index > 0 && index <= pKernel->GetCapacity(parent))
    {
        if (!pKernel->PlacePos(obj, parent, index))
        {
            pKernel->DestroySelf(obj);
            return obj;
        }

    }

    // 需要保存的属性
    const TiXmlElement * pelementProperties = (const TiXmlElement *)element.FirstChild("Properties");
    const TiXmlAttribute * pAttr = NULL;
    if (pelementProperties)
    {
        pAttr = pelementProperties->FirstAttribute();
    }

	IGameObj* pItemObj = pKernel->GetGameObj(obj);
	if (pItemObj == NULL)
	{
		return obj;
	}

    while (pAttr != NULL)
    {
        if (pItemObj->FindAttr(pAttr->Name()))
        {
            int type = pItemObj->GetAttrType(pAttr->Name());

            switch (type)
            {
                case VTYPE_INT:
                    pItemObj->SetInt(pAttr->Name(), pAttr->IntValue());
                    break;
                case VTYPE_INT64:
                    pItemObj->SetInt64(pAttr->Name(), ::_atoi64(pAttr->Value()));
                    break;
                case VTYPE_FLOAT:
                    pItemObj->SetFloat(pAttr->Name(), (const float)pAttr->DoubleValue());
                    break;
                case VTYPE_DOUBLE:
                    pItemObj->SetDouble(pAttr->Name(), pAttr->DoubleValue());
                    break;
                case VTYPE_STRING:
                    pItemObj->SetString(pAttr->Name(), pAttr->Value());
                    break;
                case VTYPE_WIDESTR:
                {
                    pItemObj->SetWideStr(pAttr->Name(), ::util_string_as_widestr(pAttr->Value()).c_str());
                }
                break;
            }
        }

        pAttr = pAttr->Next();
    }

    // 需要保存的表格
    const TiXmlElement * pelementRecords = element.FirstChildElement("Records");
    const TiXmlElement * pelementRecord  = NULL;
    if (pelementRecords)
    {
        pelementRecord = pelementRecords->FirstChildElement("Record");
    }

    while (pelementRecord != NULL)
    {
        // 取得表明
        const char * rec_name = pelementRecord->Attribute("name");
		if (StringUtil::CharIsNull(rec_name))
		{
			continue;
		}

		IRecord* pRecord = pItemObj->GetRecord(rec_name);
        if (pRecord == NULL)
        {
            pelementRecord = pelementRecord->NextSiblingElement("Record");
            continue;
        }

        // 表格大小
        int rows = pRecord->GetRows();
        int cols = pRecord->GetCols();
        int row_max = pRecord->GetRowMax();
        int rec_rows = ::atoi(pelementRecord->Attribute("rows"));

        // 数据出错
        if (rec_rows <= 0 || rec_rows > row_max)
        {
            pelementRecord = pelementRecord->NextSiblingElement("Record");
            continue;
        }

        // 增加表格行数
        if (rec_rows > rows)
        {
            for (int i = 0; i < rec_rows - rows; ++i)
            {
                pRecord->AddRow(-1);
            }
        }

        rows = pRecord->GetRows();

        // 取得表中行
        const TiXmlElement * pelementRow = pelementRecord->FirstChildElement("Row");

        while (pelementRow != NULL)
        {
            // 行元素中"row"属性表示该行的行数, 其他属性表示该行某列的值
            int row = ::atoi(pelementRow->Attribute("row"));
            if (row < 0 || row >= rows)
            {
                pelementRow = pelementRow->NextSiblingElement("Row");
                continue;
            }

            const TiXmlAttribute * pAttr = pelementRow->FirstAttribute();
            while (pAttr != NULL)
            {
                const char * prop = pAttr->Name();
                // 如果是行数属性，跳过
                if (strcmp(prop, "row") == 0)
                {
                    pAttr = pAttr->Next();
                    continue;
                }

                // 属性格式："c1" "c2"...
                // 去除'c'就是列数
                int col = ::atoi(prop + 1);
                if (col < 0 || col >= cols)
                {
                    pAttr = pAttr->Next();
                    continue;
                }

                // 填表
                switch (pRecord->GetColType(col))
                {
                    case VTYPE_INT:
                        pRecord->SetInt(row, col, pAttr->IntValue());
                        break;
                    case VTYPE_INT64:
                        pRecord->SetInt64(row, col, ::_atoi64(pAttr->Value()));
                        break;
                    case VTYPE_FLOAT:
                        pRecord->SetFloat(row, col, (const float)pAttr->DoubleValue());
                        break;
                    case VTYPE_DOUBLE:
                        pRecord->SetDouble(row, col, pAttr->DoubleValue());
                        break;
                    case VTYPE_STRING:
                        pRecord->SetString(row, col, pAttr->Value());
                        break;
                    case VTYPE_WIDESTR:
                        pRecord->SetWideStr(row, col, ::util_string_as_widestr(pAttr->Value()).c_str());
                        break;
                    case VTYPE_OBJECT:
                    {
                        PERSISTID objValue = StringToObj(pAttr->Value());

                        pRecord->SetObject(row, col, objValue);
                    }
                    break;
                }

                pAttr = pAttr->Next();
            }

            pelementRow = pelementRow->NextSiblingElement("Row");
        }

        pelementRecord = pelementRecord->NextSiblingElement("Record");
    }

    return obj;
}


// 对象属性值为空或者0
bool is_save_prop_null(const char * prop)
{
    static std::set<std::string> PropSet;
    if (PropSet.empty())
    {
        //      PropSet.insert("PropPack");                 // 物品固定属性包
    }

    if (PropSet.find(prop) == PropSet.end())
    {
        return false;
    }

    return true;
}


//生成唯一ID
void gen_item_unique_id(IKernel * pKernel, const PERSISTID & item)
{
	IGameObj* pItemObj = pKernel->GetGameObj(item);
	if (pItemObj == NULL)
	{
		return;
	}

    IGameObj* pSceneObj = pKernel->GetSceneObj();
    int serial_no = 0;
    if (pSceneObj == NULL)
    {
        //场景不存在，用全局的静态变量
        static int static_serial_no = 0;
        serial_no = static_serial_no++;
        if (static_serial_no >= 10000)
        {
            static_serial_no = 0;
        }
    }
    else
    {
        if (!pSceneObj->FindData("ItemSerialNo"))
        {
            pSceneObj->AddDataInt("ItemSerialNo", 0);
        }

        serial_no = pSceneObj->QueryDataInt("ItemSerialNo");
        //下一次使用的序号
        int next_serial_no = serial_no + 1;
        if (next_serial_no >= 10000)
        {
            next_serial_no = 0;
        }

        pSceneObj->SetDataInt("ItemSerialNo", next_serial_no);
    }

    //服务器号
#ifdef FSROOMLOGIC_EXPORTS
	int server_id = pKernel->GetRoomId();
#else
	int server_id = pKernel->GetServerId();
#endif

    //场景号
    int scene_id = pKernel->GetSceneId();

    //时间
    unsigned int now = (unsigned int)::time(NULL);

    char strUniqueID[256];
    //生成物品的流水号（服务器号场景号时间流水号）
    ::SPRINTF_S(strUniqueID, "%04d-%03d-%010d-%04d",
              server_id, scene_id, now, serial_no);

    //设置物品的流水号
    pItemObj->SetString("UniqueID", strUniqueID);
}


// 收到客户端点击菜单时验证菜单有效性
bool IsFuncidValid(IKernel * pKernel, const PERSISTID & player, const PERSISTID & npc, int funcid)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return false;
	}

	// 获取表
	IRecord* pRecord = pPlayerObj->GetRecord("Menu_Verify_Rec");
	if (pRecord == NULL)
	{
		return false;
	}

    // 检测funcid是否注册过
    int row = pRecord->FindInt(0, funcid);
    if (row < 0)
    {
        return false;
    }

    return true;
}

// 新建菜单的时候注册一下，以方便客户端返回的验证
bool RegFuncid(IKernel * pKernel, const PERSISTID & player, const PERSISTID & npc, int funcid)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return false;
	}

	// 获取表
	IRecord* pRecord = pPlayerObj->GetRecord("Menu_Verify_Rec");
	if (pRecord == NULL)
	{
		return false;
	}

    // 查找看看是否已经在注册表格中，如果已经注册，则说明funcid重复
    int row = pRecord->FindInt(0, funcid);
    if (row >= 0)
    {
        return false;
    }

    return pRecord->AddRowValue(-1, CVarList() << funcid << 0 << 0) > 0;
}

// 清空注册的菜单funcid
bool ClearRegistedFuncid(IKernel * pKernel, const PERSISTID & player)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (pPlayerObj == NULL)
	{
		return false;
	}

	// 获取表
	IRecord* pRecord = pPlayerObj->GetRecord("Menu_Verify_Rec");
	if (pRecord == NULL)
	{
		return false;
	}

    return pRecord->ClearRow();
}

// 获得与当前高度最接近的层高度
float GetCurFloorHeight(IKernel * pKernel, float x, float y, float z, PERSISTID self)
{
    if (pKernel->Exists(self))
    {
        PERSISTID temp = pKernel->GetLinkObject(self);
        if (pKernel->Exists(temp))
        {
            return GetCurFloorHeightFromObj(pKernel, temp, x, y, z);
        }
    }

    // 遍历所有层比较距离
    float dis_min = (float)MAX_INT_VALUE;

    // 目标层高
    float real_y = y;

    // 遍历层
    int count = pKernel->GetFloorCount(x, z);

    for (int i = 0; i < count; ++i)
    {
        // 判断层是否存在
        if (!pKernel->GetFloorExists(x, z, i))
        {
            continue;
        }

        // 当前层高度
        float floor_y = pKernel->GetFloorHeight(x, z, i);

        // 判断当前层和高度的差距
        float dis = fabs(y - floor_y);

        if (dis < dis_min)
        {
            dis_min = dis;
            real_y = floor_y;
        }
    }

    return real_y;
}

// 获得物体上与当前高度最接近的层高度
float GetCurFloorHeightFromObj(IKernel* pKernel, const PERSISTID & obj, float x, 
							   float y, float z)
{
	if (!pKernel->Exists(obj))
	{
		return MAX_FLOAT_VALUE;
	}

	// 遍历所有层比较距离
	float dis_min = (float)MAX_INT_VALUE;

	// 目标层高
	float real_y = y;

	// 遍历层
	int count = pKernel->GetFloorCount(x, z);

	for (int i = 0; i < count; ++i)
	{
		// 判断层是否存在
		if (!pKernel->GetFloorExists(x, z, i))
		{
			continue;
		}

		// 当前层高度
		float floor_y = pKernel->GetFloorHeight(x, z, i);

		// 判断当前层和高度的差距
		float dis = fabs(y - floor_y);

		if (dis < dis_min)
		{
			dis_min = dis;
			real_y = floor_y;
		}
	}

	return real_y;
}

// 获取x, y, z位置下第一层的层高
float GetRootFLoorHeight(IKernel * pKernel, float x, float y, float z)
{
    // 目标层高
    float real_y = -MAX_FLOAT_VALUE;

    // 遍历层
    int count = pKernel->GetFloorCount(x, z);

    for (int i = 0; i < count; ++i)
    {
        // 判断层是否存在
        if (!pKernel->GetFloorExists(x, z, i))
        {
            continue;
        }

        // 当前层高度
        float floor_y = pKernel->GetFloorHeight(x, z, i);

        // 判断当前层和高度的差距
        if (floor_y > real_y && floor_y < (y + 0.1f))
        {
            real_y = floor_y;
        }
    }

    return real_y;
}



// 获取x,z位置从下往上第一层站立层层高
float GetFirstStandHeight(IKernel * pKernel, float x, float z)
{
    // 目标层高
    float real_y = MAX_FLOAT_VALUE;

    // 遍历层
    int count = pKernel->GetFloorCount(x, z);

    for (int i = 0; i < count; ++i)
    {
        // 判断层是否存在
        if (!pKernel->GetFloorExists(x, z, i) ||
            !pKernel->GetFloorCanStand(x, z, i))
        {
            continue;
        }

        // 当前层高度
        float floor_y = pKernel->GetFloorHeight(x, z, i);

        // 判断当前层和高度的差距
        if (floor_y < real_y)
        {
            real_y = floor_y;
        }
    }

    return real_y;
}

// 获取x,z位置从下往上第一层可行走层高
float GetWalkHeight(IKernel * pKernel, float x, float z, int nFloor)
{
    // 目标层高
    float real_y = MAX_FLOAT_VALUE;

    // 遍历层
    int count = pKernel->GetFloorCount(x, z);
	if (nFloor < 0 || nFloor > count)
	{
		nFloor = 0;
	}

    for (int i = 0; i < count; ++i)
    {
        // 判断层是否存在
        if (!pKernel->GetFloorExists(x, z, i) ||
            !pKernel->GetFloorCanMove(x, z, i)
			|| nFloor != i)
        {
            continue;
        }

        // 当前层高度
		real_y = pKernel->GetFloorHeight(x, z, i);

        // 判断当前层和高度的差距
// 		if (floor_y < real_y)
// 		{
// 			real_y = floor_y;
//         }
    }

    return real_y;
}

// 获得有效的可站立高度
float get_valid_height(IKernel * pKernel, float dx, float dy, float dz)
{
    if (pKernel->GetWalkWaterExists(dx, dz))
    {
        // 水面高度
        float water_y = pKernel->GetWalkWaterHeight(dx, dz);
        // 水底地面高度
        float ground_y = pKernel->GetFloorHeight(dx, dz, 0);

        if (dy < ground_y)
        {
            // 不可低于地面
            return ground_y;
        }
        else if (dy < (water_y + 0.5f))
        {
            // 人在水中
            return dy;
        }
    }

    int floor_num = pKernel->GetFloorCount(dx, dz);

    // 查找最接近的层高度
    float min_delta = 1e8;
    float near_height = 0.0f;

    for (int i = 0; i < floor_num; ++i)
    {
        if (pKernel->GetFloorExists(dx, dz, i))
        {
            float floor_y = pKernel->GetFloorHeight(dx, dz, i);

            // 高度差绝对值
            float delta = floor_y - dy;

            if (delta < 0.0f)
            {
                delta = -delta;
            }

            if (delta < min_delta)
            {
                min_delta = delta;
                // 记录最接近的层高度
                near_height = floor_y;
            }
        }
    }

    if (min_delta < 1.0f)
    {
        return near_height;
    }

    return dy;
}

//// 获取x坐标包括移动物体上的坐标
//float GetPosiX(IKernel * pKernel, const PERSISTID & self, bool bRealPos)
//{
//    if (pKernel->Exists(pKernel->GetLinkObject(self)))
//    {
//        float x, y, z, o;
//        if (bRealPos)
//        {
//            GetRealPos(pKernel, self, x, y, z, o);
//        }
//        else
//        {
//            pKernel->GetLinkPosition(self, x, y, z, o);
//        }
//
//        return x;
//    }
//
//    return pKernel->GetPosiX(self);
//}

//// 获取y坐标包括移动物体上的坐标
//float GetPosiY(IKernel * pKernel, const PERSISTID & self, bool bRealPos)
//{
//    if (pKernel->Exists(pKernel->GetLinkObject(self)))
//    {
//        float x, y, z, o;
//        if (bRealPos)
//        {
//            GetRealPos(pKernel, self, x, y, z, o);
//        }
//        else
//        {
//            pKernel->GetLinkPosition(self, x, y, z, o);
//        }
//
//        return y;
//    }
//
//    return pKernel->GetPosiY(self);
//}

//// 获取z坐标包括移动物体上的坐标
//float GetPosiZ(IKernel * pKernel, const PERSISTID & self, bool bRealPos)
//{
//    if (pKernel->Exists(pKernel->GetLinkObject(self)))
//    {
//        float x, y, z, o;
//        if (bRealPos)
//        {
//            GetRealPos(pKernel, self, x, y, z, o);
//        }
//        else
//        {
//            pKernel->GetLinkPosition(self, x, y, z, o);
//        }
//
//        return z;
//    }
//
//    return pKernel->GetPosiZ(self);
//}

//// 获取角度包括移动物体上的坐标
//float GetOrient(IKernel * pKernel, const PERSISTID & self, bool bRealPos)
//{
//    if (pKernel->Exists(pKernel->GetLinkObject(self)))
//    {
//        float x, y, z, o;
//        if (bRealPos)
//        {
//            GetRealPos(pKernel, self, x, y, z, o);
//        }
//        else
//        {
//            pKernel->GetLinkPosition(self, x, y, z, o);
//        }
//
//        return o;
//    }
//
//    return pKernel->GetOrient(self);
//}
bool GetRealPos(IKernel* pKernel,PERSISTID self,float& x,float& y,float& z,float& orient)
{
	PERSISTID linkID = pKernel->GetLinkObject(self);
	if (!pKernel->Exists(linkID))
	{
		return false;
	}

	len_t linkx,linky,linkz,linko;
	pKernel->GetLinkPosition(self,linkx,linky,linkz,linko);

	// 保持与连接物体的相对位置
	double len = sqrt(linkx * linkx + linkz * linkz);
	double angle = acos(linkz / len);

	if (Port_IsNaN((float)angle))		
	{
		angle = 0.0f;
	}

	if (linkx < 0.0f)
	{
		angle = -angle;
	}

	len_t px,py,pz,po;
	pKernel->GetLocation(linkID,px,py,pz,po);

	angle += po;

	x = px + len_t(sin(angle) * len);
	y = py + linky;
	z = pz + len_t(cos(angle) * len);
	orient = po + linko;

	return true;
}
//// 获取坐标包括移动物体上的坐标
void GetLocation(IKernel * pKernel, const PERSISTID & self, float & x, float & y, float & z, float & orient, bool bRealPos)
{
    if (pKernel->Exists(pKernel->GetLinkObject(self)))
    {
        if (bRealPos)
        {
            GetRealPos(pKernel, self, x, y, z, orient);
        }
        else
        {
            pKernel->GetLinkPosition(self, x, y, z, orient);
        }

        return;
    }

    pKernel->GetLocation(self, x, y, z, orient);
}


// obj转换为string
fast_string ObjToString(const PERSISTID & obj)
{
    char str[128];

    ::SPRINTF_S(str, "%d-%d", obj.nIdent, obj.nSerial);

    return fast_string(str);
}

// 字符串转换为obj
PERSISTID StringToObj(const fast_string & str)
{
    PERSISTID obj = PERSISTID();

    char buf[128];

    ::strncpy(buf, str.c_str(), sizeof(buf));

    char * splitter = ::strchr(buf, '-');

    if (NULL == splitter)
    {
        return obj;
    }

    *splitter = 0;

    obj.nIdent = ::atoi(buf);
    obj.nSerial = ::atoi(splitter + 1);

    return obj;
}

int GetRoleIndex(IKernel* pKernel, const PERSISTID& player)
{
	IGameObj* pPlayerObj = pKernel->GetGameObj(player);
	if (!pPlayerObj)
	{
		return -1;
	}

	if (!pKernel->Exists(player) || pKernel->Type(player) != TYPE_PLAYER)
	{
		return -1;
	}

	return pPlayerObj->QueryInt("Job");
}

bool CheckObjectFront(float fStartX,float fStartZ,float fStartOrient,float fEndX,float fEndZ)
{
	float fTargetOrient = ::util_dot_angle(fStartX,fStartZ,fEndX,fEndZ);
	float fOrientDiff = FMABS(fStartOrient - fTargetOrient);
	if (fOrientDiff > (PI * 0.5f) && fOrientDiff < (PI * 1.5f))
	{
		return false;
	}

	return true;
}

// 检查当前时间与参数时间相隔的分钟数
int GetPassMinutes(const int64_t& date)
{
	time_t now = ::time(NULL);

	return (int)(now-date)/60;
}

char* GetFileBuff(const char* file_name)
{
	if (NULL == file_name)
	{
		return NULL;
	}

	int file_size=0;

	FILE* pfile = fopen(file_name, "rb");

	if (NULL == pfile)
	{
		return NULL;
	}

	fseek(pfile, 0, SEEK_END);

	file_size = ftell(pfile);

	fseek(pfile, 0, SEEK_SET);

	if (file_size <= 0)
	{
		return NULL;
	}

	char* pfilebuffer = new char[file_size+2];

	::memset(pfilebuffer, 0x00, file_size+2);

	fread(pfilebuffer, 1, file_size, pfile);

	fclose(pfile);

	return pfilebuffer;
}

const char* QueryXmlAttr(xml_node<>* pItem, const char* name, const char* def)
{
	Assert(pItem != NULL);

	xml_attribute<>* pAttr = pItem->first_attribute(name);

	return pAttr == NULL ? def : pAttr->value();
}

//获得中心点
void GetCenterPos(IKernel* pKernel, float& centerx, float& centerz)
{
	int iGridIndex = pKernel->GetGridIndexByPos(centerx, centerz);
	pKernel->GetPosByIndex(iGridIndex, centerx, centerz);
}

//解析字符串时间 [小时：分钟：秒数]转成秒
int util_analyzing_time_str(const char* pTimeStr)
{
	if (NULL == pTimeStr)
	{
		return 0;
	}

	if (std::strcmp(pTimeStr, "") == 0)
	{
		return 0;
	}

	//解析数据
	CVarList value_list;;
	util_split_string(value_list, pTimeStr, ":");
	if ( value_list.GetCount() != 3 )
	{
		return 0;
	}

	int iH = atoi(value_list.StringVal(0));
	int iM = atoi(value_list.StringVal(1));
	int iS = atoi(value_list.StringVal(2));

	return 3600 * iH + 60 * iM + iS;
}

//解析字符串时间 [星期：小时：分钟：秒数]转成秒
bool util_analyzing_week_time_str(const char* pTimeStr, int& iWeek, int& iSec)
{
	iWeek = -1;
	iSec = 0;

	if (NULL == pTimeStr)
	{
		return false;
	}

	if (std::strcmp(pTimeStr, "") == 0)
	{
		return false;
	}

	//解析数据
	CVarList value_list;;
	util_split_string(value_list, pTimeStr, ":");
	if ( value_list.GetCount() != 4 )
	{
		return false;
	}

	iWeek = atoi(value_list.StringVal(0));
	int iH = atoi(value_list.StringVal(1));
	int iM = atoi(value_list.StringVal(2));
	int iS = atoi(value_list.StringVal(3));

	iSec = 3600 * iH + 60 * iM + iS;

	return true;
}

//拼接特定字符串
std::string util_get_string(const char* pStr, const int iNation, const int iType)
{
	std::string outStr = std::string(pStr) + ":" + std::string(util_int_as_string(iNation).c_str()) + ":" + std::string(util_int_as_string(iType).c_str());

	return outStr;
}

//解析字符 [uid：国家：类型]
bool util_analyzing_uid_nation_type(const char* pStr, std::string& uid, int& nation, int& type, time_t& time)
{
	uid = "";
	nation = 0;
	type = 0;

	if (NULL == pStr)
	{
		return false;
	}

	if (std::strcmp(pStr, "") == 0)
	{
		return false;
	}

	//解析数据
	CVarList value_list;;
	util_split_string(value_list, pStr, ":");
	if ( value_list.GetCount() != 4 )
	{
		return false;
	}

	uid = value_list.StringVal(0);
	nation = atoi(value_list.StringVal(1));
	type = atoi(value_list.StringVal(2));
	time = atol(value_list.StringVal(3));

	return true;
}

//拼接特定字符串
std::string util_get_string(const int iNation, const int iType)
{
	std::string outStr = std::string(util_int_as_string(iNation).c_str()) + ":" + std::string(util_int_as_string(iType).c_str());

	return outStr;
}

//拼接特定字符串
std::string util_get_string(const char* pStr, const int iNation, const int iType, const time_t iCurTime)
{
	std::string outStr = std::string(pStr) + ":" + std::string(util_int_as_string(iNation).c_str()) + ":" + std::string(util_int_as_string(iType).c_str()) + ":" + std::string(util_int64_as_string(iCurTime).c_str());

	return outStr;
}

// 获取周期时间
// nNextCyc 正数 下几个周期 负数 上几个周期
int64_t get_cycle_time(int nCycType, int nDay, int nSec, int nNextCyc /*= 0*/)
{
	time_t curTime = ::time(NULL);
	tm structTime;
	util_get_local_time(&curTime, &structTime);

	// 第几天转换为天数(第1天 已经过了的天数为0)
	// 支持负数用来计算周期内倒数的天数
	if (nDay > 0)
	{
		--nDay;
	}
	else if (nDay < 0)
	{
		// 计算周期内倒数天数 在下一个周期基础上减去|nDay|天
		++nNextCyc;
	}

	switch (nCycType)
	{
	case CYCLE_TYPE_YEAR:	// 每年
		{
			// 处理周期
			structTime.tm_year += nNextCyc;

			// 处理日期
			structTime.tm_mon = 0;
			structTime.tm_mday = 1;
			structTime.tm_hour = 0;
			structTime.tm_min = 0;
			structTime.tm_sec = 0;
		}

		break;
	case CYCLE_TYPE_MONTH:	// 每月
		{
			// 处理周期
			structTime.tm_mon += nNextCyc;

			// 处理日期
			structTime.tm_mday = 1;
			structTime.tm_hour = 0;
			structTime.tm_min = 0;
			structTime.tm_sec = 0;
		}
		break;
	case CYCLE_TYPE_WEEK:	// 每周
		{
			// 处理日期
			structTime.tm_mday -= structTime.tm_wday;
			structTime.tm_mday += (nNextCyc * ONE_WEEK_DAYS);
			structTime.tm_hour = 0;
			structTime.tm_min = 0;
			structTime.tm_sec = 0;
		}
		break;
	case CYCLE_TYPE_DAY:	// 每天
		{
			// 处理天数
			nDay = 0;

			// 处理日期
			structTime.tm_mday += nNextCyc; 
			structTime.tm_hour = 0;
			structTime.tm_min = 0;
			structTime.tm_sec = 0;
		}
		break;
	}

	return mktime(&structTime) + (nDay * ONE_DAY_SECOND) + nSec;
}