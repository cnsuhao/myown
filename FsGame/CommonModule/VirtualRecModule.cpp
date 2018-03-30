//--------------------------------------------------------------------
// 文件名:      VirtualRecModule.h
// 内  容:      虚拟表
// 说  明:		
// 创建日期:    2014年10月20日
// 创建人:        
// 修改人:        ( )
//    :       
//--------------------------------------------------------------------
#include "VirtualRecModule.h"
#include "FsGame/Define//ServerCustomDefine.h"
#include "public/VarList.h"
#include "utils/util_ini.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/XmlFile.h"

VirtualRecModule* VirtualRecModule::m_pVirtualRecModule = NULL;

bool VirtualRecModule::Init(IKernel* pKernel)
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

    m_pVirtualRecModule = this;

    if (!LoadResource(pKernel))
    {
		Assert(false);

		return false;
    }

    return true;
}

bool VirtualRecModule::Shut(IKernel* pKernel)
{
    return true;
}

//下发表格数据
bool VirtualRecModule::SendVirtualRecord(IKernel* pKernel, const PERSISTID& self,
                                         const char* virtual_rec, const IVarList& args)
{
    if (TYPE_PLAYER != pKernel->Type(self))
    {
        return false;
    }

    RecColTypeMap::const_iterator it = m_mapVirtualRecStruct.find(virtual_rec);
    if (it == m_mapVirtualRecStruct.end())
    {
        return false;
    }

    const RecColType& vector_cols = (*it).second;

    const int cols = (int)vector_cols.size();
    if (0 == cols)
    {
        return false;
    }

    if (0 != (int)args.GetCount() % cols)
    {
        return false;//列数不对
    }

    //检查数据类型是否正确
    const int rows = (int)args.GetCount() / cols;

	LoopBeginCheck(a);
    for (int i = 0; i < rows; i++)
    {
		LoopDoCheck(a);

		LoopBeginCheck(b);
        for (int j = 0; j < cols; j++)
        {
			LoopDoCheck(b);

            if (args.GetType(i * cols + j) != vector_cols[j])
            {
                return false;
            }
        }
    }

    //向客户端发送消息
    CVarList msg;
    msg << SERVER_CUSTOMMSG_VIRTUALREC_ADD << virtual_rec << args;

    pKernel->Custom(self, msg);

    return true;
}

//开始等候异步操作的过程
bool VirtualRecModule::ClearVirtualRecord(IKernel* pKernel, const PERSISTID& self,
                                          const char* virtual_rec)
{
    if (TYPE_PLAYER != pKernel->Type(self))
    {
        return false;
    }

    RecColTypeMap::const_iterator it = m_mapVirtualRecStruct.find(virtual_rec);
    if (it == m_mapVirtualRecStruct.end())
    {
        return false;
    }

    //向客户端发送消息
    CVarList msg;
    msg << SERVER_CUSTOMMSG_VIRTUALREC_CLEAR << virtual_rec;
    pKernel->Custom(self, msg);

    return true;
}

//更新表格某行某列数据
bool VirtualRecModule::UpdateVirtualRecord(IKernel* pKernel, const PERSISTID& self,
                                           const char* virtual_rec, int row, int col, const IVarList& args)
{
    if (TYPE_PLAYER != pKernel->Type(self))
    {
        return false;
    }

    if (1 != args.GetCount())
    {
        return false;
    }

    RecColTypeMap::const_iterator it = m_mapVirtualRecStruct.find(virtual_rec);
    if (m_mapVirtualRecStruct.end() == it)
    {
        return false;
    }

    const RecColType& vector_cols = (*it).second;

    const int cols = (int)vector_cols.size();
    if (0 == cols)
    {
        return false;
    }

    if (col < 0 || col >= cols)
    {
        return false;    //列数不对
    }

    if (args.GetType(0) != vector_cols[col])
    {
        return false;
    }

    //向客户端发送消息
    CVarList msg;
    msg << SERVER_CUSTOMMSG_VIRTUALREC_UPDATE << virtual_rec << row << col << args;

    pKernel->Custom(self, msg);

    return true;
}

//更新表格某行数据
bool VirtualRecModule::UpdateVirtualRecordRow(IKernel* pKernel, const PERSISTID& self,
                                              const char* virtual_rec, int row, const IVarList& args)
{
    if (TYPE_PLAYER != pKernel->Type(self))
    {
        return false;
    }

    RecColTypeMap::const_iterator it = m_mapVirtualRecStruct.find(virtual_rec);
    if (m_mapVirtualRecStruct.end() == it)
    {
        return false;
    }

    const RecColType& vector_cols = (*it).second;

    const int cols = (int)vector_cols.size();
    if (0 == cols)
    {
        return false;
    }

    if ((int)args.GetCount() != cols)
    {
        return false;    //列数不对
    }

    //检查数据类型是否正确
	LoopBeginCheck(c);
    for (int j = 0; j < cols; j++)
    {
		LoopDoCheck(c);

        if (args.GetType(j) != vector_cols[j])
        {
            return false;
        }
    }

    //向客户端发送消息
    CVarList msg;
    msg << SERVER_CUSTOMMSG_VIRTUALREC_UPDATE_ROW << virtual_rec << row << args;

    pKernel->Custom(self, msg);

    return true;
}

//删除表格某行数据
bool VirtualRecModule::RemoveVirtualRecordRow(IKernel* pKernel, const PERSISTID& self,
                                              const char* virtual_rec, int row)
{
    if (TYPE_PLAYER != pKernel->Type(self))
    {
        return false;
    }

    RecColTypeMap::const_iterator it = m_mapVirtualRecStruct.find(virtual_rec);
    if (m_mapVirtualRecStruct.end() == it)
    {
        return false;
    }

    //向客户端发送消息
    CVarList msg;
    msg << SERVER_CUSTOMMSG_VIRTUALREC_REMOVE_ROW << virtual_rec << row;
    pKernel->Custom(self, msg);

    return true;
}

bool VirtualRecModule::LoadResource(IKernel* pKernel)
{
    m_mapVirtualRecStruct.clear();
    std::string path = pKernel->GetResourcePath();

    path += "ini/virtual_record.xml";
    CXmlFile ini(path.c_str());

    if (!ini.LoadFromFile())
    {
        return false;
    }

    const size_t section_count = ini.GetSectionCount();
	LoopBeginCheck(d);
    for (size_t i = 0; i < section_count; i++)
    {
		LoopDoCheck(d);

        const char* section = ini.GetSectionByIndex(i);
        RecColTypeMap::iterator it = m_mapVirtualRecStruct.find(section);
        if (it != m_mapVirtualRecStruct.end())
        {
            continue;
        }

        RecColType temp;
        PairRecColTypeMap pib = m_mapVirtualRecStruct.insert(make_pair(section, temp));
        if (!pib.second)
        {
            continue;
        }

        RecColType& vector_cols = (*pib.first).second;

        int cols = ini.ReadInteger(section, "Cols", 0);
		LoopBeginCheck(f);
        for (int j = 0; j < cols; j++)
        {
			LoopDoCheck(f);

            fast_string key = "ColType" + ::util_int_as_string(j);
            std::string type = ini.ReadString(section, key.c_str(), "");
            if (type == "WIDESTR")
            {
                vector_cols.push_back(VTYPE_WIDESTR);
            }
            else if (type == "STRING")
            {
                vector_cols.push_back(VTYPE_STRING);
            }
            else if (type == "INT")
            {
                vector_cols.push_back(VTYPE_INT);
            }
            else if (type == "INT64")
            {
                vector_cols.push_back(VTYPE_INT64);
            }
            else if (type == "FLOAT")
            {
                vector_cols.push_back(VTYPE_FLOAT);
            }
            else if (type == "DOUBLE")
            {
                vector_cols.push_back(VTYPE_DOUBLE);
            }
            else if (type == "OBJECT")
            {
                vector_cols.push_back(VTYPE_OBJECT);
            }
            else
            {
                vector_cols.push_back(VTYPE_INT);
            }
        }
    }

    return true;
}
