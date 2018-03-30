//--------------------------------------------------------------------
// 文件名:      VirtualRecModule.h
// 内  容:      虚拟表
// 说  明:		
// 创建日期:    2014年10月20日
// 创建人:        
// 修改人:        ( )
//    :       
//--------------------------------------------------------------------
#ifndef _VirtualRecModule_H_
#define _VirtualRecModule_H_

#include "Fsgame/Define/header.h"
#include <vector>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::vector<int>>                              RecColTypeMap;
typedef std::pair<std::unordered_map<std::string, std::vector<int>>::iterator, bool>  PairRecColTypeMap;
typedef std::vector<int>                                                             RecColType;
typedef std::string                                                                  RecName;

class VirtualRecModule : public ILogicModule
{
public:
    //初始化
    virtual bool Init(IKernel* pKernel);

    //释放
    virtual bool Shut(IKernel* pKernel);

public:
    //下发表格数据
    virtual bool SendVirtualRecord(IKernel* pKernel, const PERSISTID& self,
                                   const char* virtual_rec, const IVarList& args);

    //通知客户端清除表格数据
    virtual bool ClearVirtualRecord(IKernel* pKernel, const PERSISTID& self,
                                    const char* virtual_rec);

    //更新表格某行某列数据
    virtual bool UpdateVirtualRecord(IKernel* pKernel, const PERSISTID& self,
                                     const char* virtual_rec, int row, int col, const IVarList& args);

    //更新表格某行数据
    virtual bool UpdateVirtualRecordRow(IKernel* pKernel, const PERSISTID& self,
                                        const char* virtual_rec, int row, const IVarList& args);

    //删除表格某行数据
    virtual bool RemoveVirtualRecordRow(IKernel* pKernel, const PERSISTID& self,
                                        const char* virtual_rec, int row);

public:
    static VirtualRecModule* m_pVirtualRecModule;

protected:
    bool LoadResource(IKernel* pKernel);

    RecColTypeMap m_mapVirtualRecStruct;//虚拟表格的结构
};

#endif