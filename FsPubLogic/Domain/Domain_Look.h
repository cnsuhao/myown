//---------------------------------------------------------
//文件名:       Domain_Look.h
//内  容:       查看信息
//说  明: 
    
//创建日期: 
//创建人:            
//---------------------------------------------------------

#ifndef PUB_DOMAIN_SHOWOFF_H
#define PUB_DOMAIN_SHOWOFF_H

#include "IDomainData.h"
#include "FsGame/Define/PubDefine.h"

#include <string>

class Domain_Look : public IDomainData
{
public:

    Domain_Look();

    virtual ~Domain_Look();

public:

    // 公共数据区名
    virtual const std::wstring & GetDomainName(IPubKernel* pPubKernel);

    //是否保存到数据库
    virtual bool GetSave(){return true;}

    virtual int OnCreate(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    virtual int OnLoad(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    /// 处理来自场景服务器的消息
    virtual int OnMessage(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

private:

    IRecord * GetShowoffRecord(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

    // 保存物品信息到表中
    bool SaveShowoffItem(IPubKernel* pPubKernel, IPubSpace* pPubSpace,
        int source_id, int scene_id, const IVarList& msg);

    // 创建存放展示的物品表
    bool CreateShowoffRecord(IPubKernel* pPubKernel, IPubSpace* pPubSpace);

public:

    static Domain_Look* m_pShowoffInstance;

};

#endif