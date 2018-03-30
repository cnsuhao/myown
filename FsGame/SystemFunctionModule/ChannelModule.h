//----------------------------------------------------------
// 文件名:      ChannelModule.h
// 内  容:      频道系统接口
// 说  明:
// 创建日期:    2014年10月17日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------

#ifndef _ChannelModule_H_
#define _ChannelModule_H_

#include "Fsgame/Define/header.h"

class ChannelModule : public ILogicModule
{
public:
    //初始化
    virtual bool Init(IKernel * pKernel);
    virtual bool Shut(IKernel * pKernel);

    //加入频道
    bool JoinChannel(IKernel * pKernel, const PERSISTID & opplayer,
                     const char * channel_type, const wchar_t * channel_name);

    //离开频道
    bool LeaveChannel(IKernel * pKernel, const PERSISTID & opplayer,
                      const char * channel_type, const wchar_t * channel_name);

    //获得频道编号
    int GetChannel(IKernel * pKernel, const char * channel_type, const wchar_t * channel_name);

	//保存玩家聊天记录
	//bool SaveChannelInfo(IKernel *pKernel, const IVarList &args);

	//下发聊天消息
	static int OnCustomChannelInfo(IKernel* pKernel, const PERSISTID& self, 
							const PERSISTID& sender, const IVarList& args);

private:
	// 获取公共数据名称
	static const std::wstring & GetDomainName(IKernel * pKernel);

	//获取各频道聊天信息存储表名
	virtual const std::string GetChatRecordName(IPubData *pChannelData, const int channel, int subType = -1);

	//下发聊天历史消息
	//bool SendChannelToClient(IKernel* pKernel, const PERSISTID& self);


public:
    static ChannelModule * m_pChannelModule;
    static std::wstring m_domainName;               // 获取公共数据名称
};
#endif
