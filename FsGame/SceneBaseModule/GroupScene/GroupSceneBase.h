//--------------------------------------------------------------------
// 文件名:      GroupSceneBase.h
// 内  容:      分组场景基础类
// 说  明:
// 创建日期:    2015年8月4日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------
#ifndef _GROUP_SCENE_BASE_H_
#define _GROUP_SCENE_BASE_H_

#include "Fsgame/Define/header.h"
//#include "Fsgame/Define/GroupSceneDefine.h"

class GroupSceneBase : public ILogicModule
{
public:
    //初始化
    virtual bool Init(IKernel* pKernel);
    //关闭
    virtual bool Shut(IKernel* pKernel);

public:
    //获得场景类型
    static int GetSceneType(IKernel* pKernel, int sceneid);
    //获得配置信息
   // CfgSecretInfo* GetResInfo(int sceneID);
    //根据类型获取配置信息
    void GetResInfoByType(CVarList& list);
    //设置groupid
    bool SetGroupID(IKernel* pKernel,const PERSISTID& self,const int groupID);
    //加载配置
    bool LoadResource(IKernel* pKernel);
    //获得陷阱NPC配置
    void GetTrapConfig(int sceneID, std::string& trapNpcConfig, int& trapAmount);
public:
    static GroupSceneBase* m_pGroupSceneBase;
};
#endif;