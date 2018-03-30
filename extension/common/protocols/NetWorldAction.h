//--------------------------------------------------------------------
// 文件名:		NetWorldAction.h
// 内  容:		与世界服务器通信的协议
// 说  明:		
// 创建日期:		2016年06月08日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_WORLD_AND_CHARGE_ACTION_H__
#define __NET_WORLD_AND_CHARGE_ACTION_H__

#include "action/INetAction.h"
#include "NetVarListAction.h"
#include <string>
#include "public/VarList.h"
#include "MsgEncoder.h"
#include "NetChargeCustom.h"

// 计费登陆 文本格式: 服务器信息($s_sceneid_connid_serial_自增值_uid) + 协议名("register" or "login"....) + 参数
// 服务器信息第一个参数带回
template<typename T>
class NetWorldAction : public NetVarListAction<0, T>
{
public:
	std::string coninfo;	// 连接服务器信息( $s_sceneid_connid_serial_自增值_uid )
	std::string protocol;

	virtual bool LoadFromVarList(const IVarList& args)
	{
		if (args.GetCount() < 2)
		{
			return false;
		}

		if (!SafeLoadString(coninfo, args, 0))
		{
			return false;
		}

		if (!SafeLoadString(protocol, args, 1))
		{
			return false;
		}
		return m_req.LoadFromVarList( args );
	}

	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(coninfo.c_str());
		const char* key = ActionKey();
		if (NULL != key)
		{
			args.AddString( key );
		}
		else
		{
			args.AddString(protocol.c_str());
		}
		return m_req.WriteToVarList( args );
	}
};

template<typename REQ, typename ACK>
class NetWorldReqAction : public NetWorldAction<REQ>
{
public:
	NetWorldReqAction() 
	{
	}

	virtual IResponse* Response()
	{
		return &m_ack;
	}

	virtual bool LoadFromVarList( const IVarList& args )
	{
		if (args.GetCount() < 2)
		{
			return false;
		}

		if (!SafeLoadString(coninfo, args, 0))
		{
			return false;
		}

		if (!SafeLoadString(protocol, args, 1))
		{
			return false;
		}

		if (IsRequest())
		{
			return m_req.LoadFromVarList(args);
		}

		return m_ack.LoadFromVarList(args);
	}

	virtual bool WriteToVarList( IVarList& args )
	{
		args.AddString(coninfo.c_str());
		args.AddString(protocol.c_str());

		if (IsRequest())
		{
			return m_req.WriteToVarList(args);
		}

		return m_ack.WriteToVarList(args);
	}

protected:
	ACK		m_ack;
};

typedef NetVarListRequest NetWorldRequest;
typedef NetVarListResponse NetWorldResponse;

//--------------------------------------------------------------------
// 服务器注册
//--------------------------------------------------------------------
class NetWorldMessage_ServerRgister : public NetWorldRequest
{
public:
	int  value_of_2;		// 未知属性 1
	int  value_of_3;		// 未知属性 7440005

	std::string srvinfo;	// 服务器信息 server_7440005
	std::string enc;		// md5串		cb10de5f8d730ec666dde710665cf07

	int	value_of_6;			// 未知属性

	virtual bool LoadFromVarList(const IVarList& args)
	{
		if (args.GetCount() < 7)
		{
			return false;
		}

		value_of_2 = args.IntVal(2);
		value_of_3 = args.IntVal(3);
		value_of_6 = args.IntVal(6);

		if (!SafeLoadString(srvinfo, args, 4))
		{
			return false;
		}

		if (!SafeLoadString(enc, args, 5))
		{
			return false;
		}
		return true;
	}

	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(value_of_2);
		args.AddInt(value_of_3);
		args.AddString(srvinfo.c_str());
		args.AddString(enc.c_str());
		args.AddInt(value_of_6);
		return true;
	}
};
class NetWorldAction_ServerRegister : public NetWorldAction<NetWorldMessage_ServerRgister>
{
public:

	virtual const char* ActionKey() const
	{
		return "register";
	}
};



//--------------------------------------------------------------------
// 登陆
//--------------------------------------------------------------------
class NetWorldMessage_Login_Req : public NetWorldRequest
{
public:
	int			loginType;			// 登陆类型 index:2
	std::string name;				// 用户名 index:3
	std::string token;				// token串 index:4
	std::string ip;					// 客户端ip index:5
	int			port;				// 客户端连接端口 index:6
	std::string loginString;		// 登陆串(需要返回) index:7
	int			clientType;			// 客户端类型 index:8

	virtual bool LoadFromVarList(const IVarList& args)
	{
		if (args.GetCount() < 9)
		{
			return false;
		}

		loginType = args.IntVal(2);

		if (!SafeLoadString(name, args, 3))
		{
			return false;
		}

		if (!SafeLoadString(token, args, 4))
		{
			return false;
		}

		if (!SafeLoadString(ip, args, 5))
		{
			return false;
		}

		port = args.IntVal(6);
		if (!SafeLoadString(loginString, args, 7))
		{
			return false;
		}
		clientType = args.IntVal(8);
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(loginType);
		args.AddString(name.c_str());
		args.AddString(token.c_str());
		args.AddString(ip.c_str());
		args.AddInt(port);
		args.AddString(loginString.c_str());
		args.AddInt(clientType);
		return true;
	}
};

class NetWorldMessage_Login_Ack: public NetWorldResponse
{
public:
	int			acc_type;			// 账号类型 index:2
	std::string name;				// 帐号 index:3
	int			result;				// 结果 index:4
	int			acc_id;				// 计费id index:5
	std::string logon_id;			// 此次登录的序列号，用于logout消息时返回给计费
	std::string user_name;			// 暂时没用
	int			gm_level;			// gm等级
	std::string passport;			// 暂时没用
	int			points;				// 账号对应的计费点数
	double		acc_limit;			// 账号有效时间，月卡收费模式使用
	int			is_free;			// 是否免费用户，月卡收费模式使用
	std::string acc_info;			// 账号相关扩展信息（防沉迷时间、VIP）等
	int			login_type;			// 登录类型（和计费特殊约定字段1，万能密码登录，5，VIP登录等）
	std::string	vaild_string;		// 一次性身份验证串，用于无密码登录、跨服、断线重连、logincache服务器等
	int			issur_id;			// 联运商编号

	NetWorldMessage_Login_Ack()
	{
		acc_type = 0;
		result = 0;
		acc_id = 0;
		gm_level = 0;
		points = 0;
		acc_limit = 0.0f;
		is_free = 0;
		login_type = 0;
		issur_id = 0;
	}

	virtual bool LoadFromVarList(const IVarList& args)
	{
		if (args.GetCount() < 17)
		{
			return false;
		}

		int nIndex = 2;
		acc_type = args.IntVal( nIndex++ );
		if (!SafeLoadString(name, args, nIndex++))
		{
			return false;
		}

		result = args.IntVal(nIndex++);
		acc_id = args.IntVal(nIndex++);
		if (!SafeLoadString(logon_id, args, nIndex++))
		{
			return false;
		}		
		if (!SafeLoadString(user_name, args, nIndex++))
		{
			return false;
		}
		gm_level = args.IntVal(nIndex++);
		if (!SafeLoadString(passport, args, nIndex++))
		{
			return false;
		}
		points = args.IntVal(nIndex++);
		acc_limit = args.DoubleVal(nIndex++);
		is_free = args.IntVal(nIndex++);
		if (!SafeLoadString(acc_info, args, nIndex++))
		{
			return false;
		}
		login_type = args.IntVal(nIndex++);
		if (!SafeLoadString(vaild_string, args, nIndex++))
		{
			return false;
		}
		issur_id = args.IntVal(nIndex++);

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(acc_type);
		args.AddString(name.c_str());
		args.AddInt(result);
		args.AddInt(acc_id);
		args.AddString(logon_id.c_str());
		args.AddString(user_name.c_str());
		args.AddInt(gm_level);
		args.AddString(passport.c_str());
		args.AddInt(points);
		args.AddDouble(acc_limit);
		args.AddInt(is_free);
		args.AddString(acc_info.c_str());
		args.AddInt(login_type);
		args.AddString(vaild_string.c_str());
		args.AddInt(issur_id);
		return true;
	}
};

class NetWorldAction_Login : public NetWorldReqAction<NetWorldMessage_Login_Req, NetWorldMessage_Login_Ack>
{
public:

	virtual const char* ActionKey() const
	{
		return "login";
	}
};


//--------------------------------------------------------------------
// 退出
//--------------------------------------------------------------------
class NetWorldMessage_Logout : public NetWorldRequest
{
public:
	int			value_of_1;			// 未知属性1 index:2
	std::string value_of_2;			// 未知属性2 index:3
	int			value_of_3;			// 未知属性3 index:4
	std::string name;				// 账号 index:5

	virtual bool LoadFromVarList(const IVarList& args)
	{
		if (args.GetCount() < 6)
		{
			return false;
		}

		value_of_1 = args.IntVal(2);

		if (!SafeLoadString(value_of_2, args, 3))
		{
			return false;
		}

		value_of_3 = args.IntVal(4);

		if (!SafeLoadString(name, args, 5))
		{
			return false;
		}
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(value_of_1);
		args.AddString(value_of_2.c_str());
		args.AddInt(value_of_3);
		args.AddString(name.c_str());
		return true;
	}
};

class NetWorldAction_Logout : public NetWorldAction<NetWorldMessage_Logout>
{
public:

	virtual const char* ActionKey() const
	{
		return "logout";
	}
};

//--------------------------------------------------------------------
// 心跳保持
//--------------------------------------------------------------------
class NetWorldMessage_Keep : public NetWorldRequest
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
class NetWorldAction_Keep : public NetWorldAction<NetWorldMessage_Keep>
{
public:

	virtual const char* ActionKey() const
	{
		return "keep";
	}
};

//--------------------------------------------------------------------
// 自定义协议
//--------------------------------------------------------------------
class NetWorldMessage_Custom : public NetWorldRequest
{
public:
	virtual bool LoadFromVarList(const IVarList& args)
	{
		cargs.Clear();
		if (args.GetCount() < 2)
			return false;

		cargs.Append(args, 2, args.GetCount() - 2);
		return true;
	}

	virtual bool WriteToVarList(IVarList& args)
	{
		args.Append(cargs, 0, cargs.GetCount());
		return true;
	}

	CVarList cargs;
};
class NetWorldAction_Custom : public NetWorldAction<NetWorldMessage_Custom>
{
public:
	virtual const char* ActionKey() const
	{
		return "custom";
	}
};


/*
// 游戏准备好了
class NetWorldMessage_GameReady : public NetWorldRequest
{
public:
	bool IsReady;
	NetWorldMessage_GameReady() : IsReady(0)
	{

	}
	virtual bool LoadFromVarList(const IVarList& args)
	{
		if (args.GetCount() < 3)
		{
			return false;
		}

		IsReady = args.IntVal(2) != 0;
		return true;
	}

	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(IsReady ? 1 : 0);
		return true;
	}
};
class NetWorldAction_GameReady : public NetWorldAction<NetWorldMessage_GameReady>
{
public:

	virtual const char* ActionKey() const
	{
		return "game_ready";
	}
};

// 请求游戏状态LOGIN->GAME
class NetWorldMessage_GameState : public NetWorldRequest
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
class NetWorldAction_GameState : public NetWorldAction<NetWorldMessage_GameState>
{
public:

	virtual const char* ActionKey() const
	{
		return "game_state";
	}
};
*/
//--------------------------------------------------------------------
// 未知协议
//--------------------------------------------------------------------
typedef NetVarListMessage_Unkown NetWorldMessage_Unkown;
typedef NetVarListAction_Unknown NetWorldAction_Unknown;


// 消息打包器
class NetWorldMessagePacker : public NetVarListPacker
{
public:
	NetWorldMessagePacker() : NetVarListPacker(true, true)
	{
	}

protected:
	virtual INetVarListActionProtocol* CreateAction(const char* key) const
	{
		INetVarListActionProtocol* pAction = NULL;
		if (strcmp(key, "login") == 0)
		{
			pAction = new NetWorldAction_Login();
		}
		else if (strcmp(key, "keep") == 0)
		{
			pAction = new NetWorldAction_Keep();
		}
		else if (strcmp(key, "logout") == 0)
		{
			pAction = new NetWorldAction_Logout();
		}
		else if (strcmp(key, "register") == 0)
		{
			pAction = new NetWorldAction_ServerRegister();
		}
		else if (strcmp(key, "custom") == 0)
		{
			pAction = new NetWorldAction_Custom();
		}

		return pAction;
	}
};

#endif // END __NET_WORLD_AND_CHARGE_ACTION_H__