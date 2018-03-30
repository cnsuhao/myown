//--------------------------------------------------------------------
// 文件名:		NetWorldGmcc.h
// 内  容:		Gmcc World服务器通信协议
// 说  明:		
// 创建日期:		2016年9月17日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_WORLD_GMCC_H__
#define __NET_WORLD_GMCC_H__

#include "NetVarListAction.h"
#include "ProtocolsID.h"
#include "ExchGiftInfo.h"
#include "CmdInfo.h"
#include "MailData.h"
#include "NoticeInfo.h"
#include "NetWorldAction.h"
#include "utils/string_util.h"
#include "PlayerData.h"
#include "ServerInfo.h"
#include "GameInfo.h"
//--------------------------------------------------------------------
// 游戏服务器确认收到消息
//--------------------------------------------------------------------
class NetExtraGameAck : public NetVarListRequest
{
public:
	int64_t seq;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 1);

		size_t nIndex = 1;
		seq = args.Int64Val(nIndex++);
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt64(seq);
		return true;
	}
};

class NetExtraAction_GameAck : public NetVarListAction<PROTOCOL_ID_CONFIRM_ACK, NetExtraGameAck>
{

};

//--------------------------------------------------------------------
// 游戏服务器准备好了
//--------------------------------------------------------------------
class NetExtraGameReady : public NetVarListRequest
{
public:
	virtual bool LoadFromVarList(const IVarList& args)
	{
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		return true;
	}
};

class NetExtraAction_GameReady : public NetVarListAction<PROTOCOL_ID_GAME_READY, NetExtraGameReady>
{

};

//--------------------------------------------------------------------
// 请求游戏服务器准备好了
//--------------------------------------------------------------------
class NetExtraGameCheck : public NetVarListRequest
{
public:
	virtual bool LoadFromVarList(const IVarList& args)
	{
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		return true;
	}
};

class NetExtraAction_GameCheck : public NetVarListAction<PROTOCOL_ID_GAME_CHECK_READY, NetExtraGameCheck>
{

};

//--------------------------------------------------------------------
// 游戏请求gmcc服务器初始化信息
//--------------------------------------------------------------------
class NetExtraGameReqInitInfo : public NetVarListRequest
{
public:
	virtual bool LoadFromVarList(const IVarList& args)
	{
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		return true;
	}
};

class NetExtraAction_GameReqSrvInfo : public NetVarListAction<PROTOCOL_ID_GAME_REQ_SRV_INFO, NetExtraGameReqInitInfo>
{

};


//--------------------------------------------------------------------
// gmcc服务信息
//--------------------------------------------------------------------
class NetExtraGmccInfo : public NetVarListRequest
{
public:
	SrvInfo Info;

	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 6);

		size_t nIndex = 1;
		Info.Identity = args.Int64Val(nIndex++);
		Info.GameId = args.Int64Val(nIndex++);
		Info.ChannelId = args.Int64Val(nIndex++);
		Info.ProductionId = args.Int64Val(nIndex++);
		Info.ServerId = args.Int64Val(nIndex++);
		Info.AreaId = args.Int64Val(nIndex++);
		Info.DeployId = args.Int64Val(nIndex++);
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt64(Info.Identity);
		args.AddInt64(Info.GameId);
		args.AddInt64(Info.ChannelId);
		args.AddInt64(Info.ProductionId);
		args.AddInt64(Info.ServerId);
		args.AddInt64(Info.AreaId);
		args.AddInt64(Info.DeployId);
		return true;
	}
};

class NetExtraAction_GmccInfo : public NetVarListAction<RPOTOCOL_ID_GAME_INIT_INFO, NetExtraGmccInfo>
{

};

//--------------------------------------------------------------------
// 执行Gm命令
//--------------------------------------------------------------------
class NetExtraGM_Exec : public NetVarListRequest
{
public:
	CmdInfo cmd;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);

		size_t nIndex = 1;
		if (!SafeLoadString(cmd.operateRoleId, args, nIndex++))		// nIndex + 0
		{
			return false;
		}

		if (!SafeLoadString(cmd.roleId, args, nIndex++))			// nIndex + 1
		{
			return false;
		}

		if (!SafeLoadString(cmd.command, args, nIndex++))			// nIndex + 2
		{
			return false;
		}

		cmd.seq = args.Int64Val(nIndex++);						// nIndex + 3
		size_t nEndPos = args.GetCount();
		cmd.cmdArgs.clear();
		for (; nIndex < nEndPos; ++nIndex )
		{
			std::string param;
			if (!SafeLoadString(param, args, nIndex))
			{
				return false;
			}
			cmd.cmdArgs.push_back(param);
		}
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(cmd.operateRoleId.c_str());
		args.AddString(cmd.roleId.c_str());
		args.AddString(cmd.command.c_str());
		args.AddInt64(cmd.seq);
		size_t nSize = cmd.cmdArgs.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			args.AddString(cmd.cmdArgs[i].c_str());
		}
		return true;
	}
};

class NetExtraAction_GM_Exec : public NetVarListAction<PROTOCOL_ID_GM_EXEC, NetExtraGM_Exec>
{

};

//--------------------------------------------------------------------
// 执行Gm命令结果
//--------------------------------------------------------------------
class NetExtraGM_Reply : public NetVarListRequest
{
public:
	std::string roleid;				// 角色id
	int64_t		cmdid;				// 命令索引	index:1
	int			result;				// 执行结果	index:2
	std::string message;			// 返回字符串结果 index:3

	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);

		int nIndex = 1;
		cmdid = args.Int64Val(nIndex++);			// nIndex + 0
		roleid = args.StringVal(nIndex++);			// nIndex + 1
		result = args.IntVal(nIndex++);				// nIndex + 2

		if ( args.GetCount() > nIndex )
		{
			message = args.StringVal(nIndex++);
		}

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt64(cmdid);
		args.AddString(roleid.c_str());
		args.AddInt(result);
		args.AddString(message.c_str());
		return true;
	}
};

class NetExtraAction_GM_Reply : public NetVarListAction<PROTOCOL_ID_GM_REPLY, NetExtraGM_Reply>
{
public:
};


//--------------------------------------------------------------------
// 执行发送邮件
//--------------------------------------------------------------------
class NetExtra_Mail_Post : public NetVarListRequest
{
public:
	MailData data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 11);

		size_t nIndex = 1;
		data.GlobalMail = args.IntVal(nIndex++) != 0;	// nIndex + 0
		data.MailId = args.IntVal(nIndex++);			// nIndex + 1
		data.MailType = args.IntVal(nIndex++);			// nIndex + 2
		data.CreateTime = args.Int64Val(nIndex++);		// nIndex + 3
		data.Status = args.IntVal(nIndex++);			// nIndex + 4
		data.Period = args.IntVal(nIndex++);			// nIndex + 5

		if (!SafeLoadString(data.Title, args, nIndex++))// nIndex + 6
		{
			return false;
		}

		if (!SafeLoadString(data.Content, args, nIndex++)) // nIndex + 7
		{
			return false;
		}

		if (!SafeLoadString(data.senderId, args, nIndex++))// nIndex + 8
		{
			return false;
		}		
		
		int nRecvCount = args.IntVal(nIndex++);				// nIndex + 9
		data.ReciverIds.clear();
		for (int i = 0; i < nRecvCount; ++i)
		{
			std::string strRecv;
			if (!SafeLoadString(strRecv, args, nIndex++))	// nIndex + 10 + i
			{
				return false;
			}
			data.ReciverIds.push_back(strRecv);
		}


		data.HasAttach = args.IntVal(nIndex++) != 0;		// nIndex + 10 + nRecvCount
		if (!SafeLoadString(data.AttachInfo, args, nIndex++))// nIndex + 11 + nRecvCount
		{
			return false;
		}		
		return true;
	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(data.GlobalMail ? 1 : 0);
		args.AddInt64(data.MailId);
		args.AddInt(data.MailType);
		args.AddInt64(data.CreateTime);
		args.AddInt(data.Status);
		args.AddInt(data.Period);
		args.AddString(data.Title.c_str());
		args.AddString(data.Content.c_str());
		args.AddString(data.senderId.c_str());
		int nRecvCount = (int)data.ReciverIds.size();
		args.AddInt(nRecvCount);
		for (int i = 0; i < nRecvCount; ++i)
		{
			args.AddString(data.ReciverIds[i].c_str());
		}
		args.AddInt(data.HasAttach ? 1 : 0);
		args.AddString(data.AttachInfo.c_str());
		return true;
	}
};

class NetExtraAction_Mail_Post : public NetVarListAction<PROTOCOL_ID_MAIL_POST, NetExtra_Mail_Post>
{
public:

};

//--------------------------------------------------------------------
// 执行发送邮件结果
//--------------------------------------------------------------------
class NetExtra_Mail_Result : public NetVarListRequest
{
public:
	int32_t			MailId;							// 邮件id		index:1
	int32_t			Result;							// 结果			index:2
	bool			IsGlobal;						// 是否全局邮件	index:3
	std::map<std::string,int32_t>	Roles;			// 邮件执行结果	index:4<用户id, 结果值>

	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);
		Roles.clear();

		size_t nIndex = 1;
		MailId = args.IntVal(nIndex++);			// nIndex + 0
		Result = args.IntVal(nIndex++);			// nIndex + 1
		IsGlobal = args.IntVal(nIndex++) != 0;	// nIndex + 2
		int32_t nCount = args.IntVal(nIndex++);	// nIndex + 3
		if (nCount > 0)
		{
			for (int32_t i = 0; i < nCount; ++i )
			{
				const char* pszRole = args.StringVal(nIndex++);
				int32_t nResult = args.IntVal(nIndex++);
				if (NULL != pszRole && pszRole[0] != '\0')
				{
					Roles[pszRole] = nResult;
				}
			}
		}
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(MailId);
		args.AddInt(Result);
		args.AddInt(IsGlobal ? 1 : 0);
		args.AddInt((int32_t)Roles.size());

		for (std::map<std::string, int32_t>::iterator itr = Roles.begin(); itr != Roles.end(); ++itr)
		{
			args.AddString(itr->first.c_str());
			args.AddInt(itr->second);
		}

		return true;
	}
};

class NetExtraAction_Mail_Result : public NetVarListAction<PROTOCOL_ID_MAIL_RESULT, NetExtra_Mail_Result>
{
public:

};

//--------------------------------------------------------------------
// 同步公告请求
//--------------------------------------------------------------------
class NetExtra_Notice_Sync_Req : public NetVarListRequest
{
public:
	virtual bool LoadFromVarList(const IVarList& args)
	{
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		return true;
	}
};
class NetExtraAction_Notice_Sync : public NetVarListAction<PROTOCOL_ID_NOTICE_SYNC, NetExtra_Notice_Sync_Req>
{
};

//--------------------------------------------------------------------
// 删除公告
//--------------------------------------------------------------------
class NetExtra_Notice_Delete : public NetVarListRequest
{
public:
	int64_t NoticeId;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 1);

		size_t nIndex = 1;
		NoticeId = args.Int64Val(nIndex++);
		return true;
	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt64(NoticeId);
		return true;
	}
};
class NetExtraAction_Notice_Delete : public NetVarListAction<PROTOCOL_ID_NOTICE_DELETE, NetExtra_Notice_Delete>
{
};

//--------------------------------------------------------------------
// 执行发送公告
//--------------------------------------------------------------------
class NetExtra_Notice_Post : public NetVarListRequest
{
public:
	NoticeData data;

	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 7);

		size_t nIndex = 1;
		if (!SafeLoadString(data.Name, args, nIndex++))	// nIndex + 0
		{
			return false;
		}
		if (!SafeLoadString(data.Content, args, nIndex++))	// nIndex + 1
		{
			return false;
		}
		data.NoticeId = args.IntVal(nIndex++);		// nIndex + 2
		data.CreateTime = args.Int64Val(nIndex++);	// nIndex + 3
		data.EndTime = args.Int64Val(nIndex++);		// nIndex + 4
		data.Interval = args.Int64Val(nIndex++);	// nIndex + 5
		data.Loop = args.IntVal(nIndex++);			// nIndex + 6
		data.Type = args.IntVal(nIndex++);			// nIndex + 7

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(data.Name.c_str());
		args.AddString(data.Content.c_str());
		args.AddInt64(data.NoticeId);
		args.AddInt64(data.CreateTime);
		args.AddInt64(data.EndTime);
		args.AddInt64(data.Interval);
		args.AddInt(data.Loop);
		args.AddInt(data.Type);
		return true;
	}
};
class NetExtraAction_Notice_Post : public NetVarListAction<PROTOCOL_ID_NOTICE_POST, NetExtra_Notice_Post>
{
};

//--------------------------------------------------------------------
// 执行发送公告结果
//--------------------------------------------------------------------
class NetExtra_Notice_Result : public NetVarListRequest
{
public:
	int32_t			NoticeId;		// 公告id		index:1
	int32_t			Result;			// 公告执行结果	index:2

	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 2);

		int nIndex = 1;
		NoticeId = args.IntVal(nIndex++);	// nIndex + 0
		Result = args.IntVal(nIndex++);		// nIndex + 1
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(NoticeId);
		args.AddInt(Result);
		return true;
	}
};

class NetExtraAction_Notice_Result : public NetVarListAction<PROTOCOL_ID_NOTICE_RESULT, NetExtra_Notice_Result>
{
public:
};

//--------------------------------------------------------------------
// 兑换礼包
//--------------------------------------------------------------------
class NetExtra_Exch_Gift : public NetWorldRequest
{
public:
	ExchGiftInfo info;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);
		int nIndex = 1;
		if (!SafeLoadString(info.key, args, nIndex++))		// nIndex + 0
		{
			return false;
		}
		if (!SafeLoadString(info.account, args, nIndex++))
		{
			return false;
		}
		if (!SafeLoadString(info.roleid, args, nIndex++))
		{
			return false;
		}

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(info.key.c_str());
		args.AddString(info.account.c_str());
		args.AddString(info.roleid.c_str());

		return true;
	}
};

class NetExtraAction_Exch_Gift : public NetVarListAction<PROTOCOL_ID_GIFT_EXCH, NetExtra_Exch_Gift>
{

};

//--------------------------------------------------------------------
// 兑换礼包结果
//--------------------------------------------------------------------
class NetExtra_Gift_Result : public NetWorldRequest
{
public:
	ExchGiftResult data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 5);

		size_t nIndex = 1;
		if (!SafeLoadString(data.roleId, args, nIndex++))	// nIndex + 0
		{
			return false;
		}
		if (!SafeLoadString(data.key, args, nIndex++))		// nIndex + 1
		{
			return false;
		}
		data.result = args.IntVal(nIndex++);				// nIndex + 2
		if (!SafeLoadString(data.capitals, args, nIndex++))	// nIndex + 3
		{
			return false;
		}
		if (!SafeLoadString(data.items, args, nIndex++))	// nIndex + 4
		{
			return false;
		}
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(data.roleId.c_str());
		args.AddString(data.key.c_str());
		args.AddInt(data.result);
		args.AddString(data.capitals.c_str());
		args.AddString(data.items.c_str());
		return true;
	}
};

class NetExtraAction_Gift_Result : public NetVarListAction<PROTOCOL_ID_GIFT_RESULT, NetExtra_Gift_Result>
{

};

//--------------------------------------------------------------------
// 属性查询
//--------------------------------------------------------------------
class NetExtra_Role_Prop_Query : public NetWorldRequest
{
public:
	CmdPropQuery data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);

		size_t nIndex = 1;
		data.queryId = args.Int64Val(nIndex++);				// nIndex + 0
		if (!SafeLoadString(data.roleId, args, nIndex++))	// nIndex + 1
		{
			return false;
		}
		size_t nCount = args.GetCount();
		while ( nIndex < nCount )
		{
			std::string strProp;
			if (!SafeLoadString(strProp, args, nIndex++))
			{
				return false;
			}
			data.props.push_back(strProp);
		}
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt64( data.queryId );
		args.AddString(data.roleId.c_str());
		size_t nCount = data.props.size();
		for (size_t i = 0; i < nCount; ++i)
		{
			args.AddString(data.props[i].c_str());
		}
		return true;
	}
};

class NetExtraAction_Role_Prop_Query : public NetVarListAction<PROTOCOL_ID_ROLE_PROP_QUERY, NetExtra_Role_Prop_Query>
{

};

//--------------------------------------------------------------------
// 属性查询返回
//--------------------------------------------------------------------
class NetExtra_Role_Prop_Data : public NetWorldRequest
{
public:
	CmdPropData data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 4);

		size_t nIndex = 1;
		data.queryId = args.Int64Val(nIndex++);				// nIndex + 0
		data.result = args.IntVal(nIndex++);				// nIndex + 1
		if (!SafeLoadString(data.roleId, args, nIndex++))	// nIndex + 2
		{
			return false;
		}

		size_t nCount = args.GetCount();
		assert((nCount - nIndex) % 2 == 0);
		data.props.clear();
		while (nIndex < nCount)
		{
			std::string key;
			if (!SafeLoadString(key, args, nIndex++))
			{
				return false;
			}

			if (nIndex >= nCount)
			{
				assert(false);
				break;
			}
			std::string value = StringUtil::ConvertUtf8String(args, nIndex++);
			data.props[key] = value;
		}
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt64(data.queryId);
		args.AddInt(data.result);
		args.AddString(data.roleId.c_str());
		for (std::map<std::string, std::string>::iterator itr = data.props.begin(); itr != data.props.end(); ++itr)
		{
			args.AddString(itr->first.c_str());
			args.AddString(itr->second.c_str());
		}
		return true;
	}
};

class NetExtraAction_Role_Prop_Data : public NetVarListAction<PROTOCOL_ID_ROLE_PROP_DATA, NetExtra_Role_Prop_Data>
{
};

//--------------------------------------------------------------------
// 表数据查询
//--------------------------------------------------------------------
class NetExtra_Role_Record_Query : public NetWorldRequest
{
public:
	CmdRecordQuery data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 5);

		size_t nIndex = 1;
		data.queryId = args.Int64Val(nIndex++);				// nIndex + 0
		if (!SafeLoadString(data.roleId, args, nIndex++))	// nIndex + 1
		{
			return false;
		}
		if (!SafeLoadString(data.recordName, args, nIndex++))// nIndex + 2
		{
			return false;
		}
		data.offset = args.IntVal(nIndex++);				// nIndex + 3
		data.count = args.IntVal(nIndex++);				// nIndex + 4
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt64(data.queryId);
		args.AddString(data.roleId.c_str());
		args.AddString(data.recordName.c_str());
		args.AddInt(data.offset);
		args.AddInt(data.count);
		return true;
	}
};

class NetExtraAction_Role_Record_Query : public NetVarListAction<PROTOCOL_ID_ROLE_RECORD_QUERY, NetExtra_Role_Record_Query>
{

};

//--------------------------------------------------------------------
// 表数据查询返回
//--------------------------------------------------------------------
class NetExtra_Role_Record_Data : public NetWorldRequest
{
public:
	CmdRecordData data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 5);

		size_t nIndex = 1;
		data.queryId = args.Int64Val(nIndex++);				// nIndex + 0
		if (!SafeLoadString(data.roleId, args, nIndex++))	// nIndex + 1
		{
			return false;
		}
		if (!SafeLoadString(data.recordName, args, nIndex++))// nIndex + 2
		{
			return false;
		}
		data.result = args.IntVal(nIndex++);			// nIndex + 3
		data.offset = args.IntVal(nIndex++);			// nIndex + 4
		data.count = args.IntVal(nIndex++);				// nIndex + 5
		data.data.clear();
		if (data.result == DATA_QUERY_SUCCESS)
		{
			data.cols = args.IntVal(nIndex++);			// nIndex + 6
			size_t nCount = args.GetCount();
			while ( nIndex < nCount )
			{
				std::string v;
				if (!SafeLoadString(v, args, nIndex++))
				{
					return false;
				}
				data.data.push_back(v);
			}
			assert(data.cols > 0);
			assert(data.data.size() % data.cols == 0);
		}
		else
		{
			data.cols = 0;
		}
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt64(data.queryId);
		args.AddString(data.roleId.c_str());
		args.AddString(data.recordName.c_str());
		args.AddInt(data.result);
		args.AddInt(data.offset);
		args.AddInt(data.count);
		args.AddInt(data.cols);
		for (std::vector<std::string>::iterator itr = data.data.begin(); itr != data.data.end(); ++itr)
		{
			args.AddString( itr->c_str());
		}
		return true;
	}
};

class NetExtraAction_Role_Record_Data : public NetVarListAction<PROTOCOL_ID_ROLE_RECORD_DATA, NetExtra_Role_Record_Data>
{

};

//--------------------------------------------------------------------
// 表数据查询返回
//--------------------------------------------------------------------
class NetExtra_MemberInfo : public NetWorldRequest
{
public:
	int					online_count;
	int					memberId;			// 成员id
	EmMemberWorkState	workState;			// 运行状态
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() >= 4);
		if (args.GetCount() < 4)
		{
			return false;
		}

		size_t nIndex = 1;
		online_count = args.IntVal(nIndex++);						// nIndex + 0
		memberId = args.IntVal(nIndex++);							// nIndex + 1
		workState = (EmMemberWorkState)args.IntVal(nIndex++);		// nIndex + 2
		return true;
	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(online_count);
		args.AddInt(memberId);
		args.AddInt(workState);
		return true;
	}
};

class NetExtraAction_ReportOnlineCount : public NetVarListAction<PROTOCOL_ID_REPORT_MEMBER_INFO, NetExtra_MemberInfo>
{

};
#endif 