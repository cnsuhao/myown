//--------------------------------------------------------------------
// 文件名:		NetVarListAction.h
// 内  容:		与游戏服务器通信的扩展服协议定义
// 说  明:		
// 创建日期:		2016年9月0819日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_VARLIST_ACTION_H__
#define __NET_VARLIST_ACTION_H__

#include "action/INetAction.h"
#include <string>
#include "public/VarList.h"
#include "MsgEncoder.h"
#include "protocols/SysAction.h"

#define NET_EXTRA_MAX_MSG_ID	0
class INetVarListActionProtocol
{
public:
	virtual ~INetVarListActionProtocol() {};
	virtual bool LoadFromVarList(const IVarList& args) = 0;
	virtual bool WriteToVarList(IVarList& args) = 0;
};

// 服务器信息第一个参数带回
template< int ID, typename T >
class NetVarListAction : public NetAction<ID, T>, public INetVarListActionProtocol
{
public:
	virtual bool LoadFromVarList(const IVarList& args)
	{
		return m_req.LoadFromVarList( args );
	}

	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt((int)ActionId());
		return m_req.WriteToVarList( args );
	}

protected:
	bool SafeLoadString(std::string& v, const IVarList& args, int nIndex)
	{
		const char* p = args.StringVal(nIndex);
		if (NULL == p)
		{
			return false;
		}

		v = p;
		return true;
	}
};

template<int ID, typename REQ, typename ACK>
class NetVarListReqAction : public NetVarListAction<ID, REQ>
{
public:
	virtual IResponse* Response()
	{
		return &m_ack;
	}

	virtual bool LoadFromVarList( const IVarList& args )
	{
		if (IsRequest())
		{
			return m_req.LoadFromVarList(args);
		}

		return m_ack.LoadFromVarList(args);
	}

	virtual bool WriteToVarList( IVarList& args )
	{
		args.AddInt((int)ActionId());
		if (IsRequest())
		{
			return m_req.WriteToVarList(args);
		}

		return m_ack.WriteToVarList(args);
	}

public:
	ACK		m_ack;
};

class NetVarListRequest : public INetRequest
{
public:
	virtual ~NetVarListRequest() = 0;
	virtual bool LoadFromVarList(const IVarList& args) = 0;
	virtual bool WriteToVarList(IVarList& args) = 0;

protected:
	bool SafeLoadString(std::string& v, const IVarList& args, size_t nIndex)
	{
		const char* p = args.StringVal(nIndex);
		if (NULL == p)
		{
			return false;
		}

		v = p;
		return true;
	}
};
inline NetVarListRequest::~NetVarListRequest(){};

class NetVarListResponse : public INetResponse
{
public:
	virtual ~NetVarListResponse() = 0;
	virtual bool LoadFromVarList(const IVarList& args) = 0;
	virtual bool WriteToVarList(IVarList& args) = 0;

protected:
	bool SafeLoadString(std::string& v, const IVarList& args, int nIndex)
	{
		const char* p = args.StringVal(nIndex);
		if (NULL == p)
		{
			return false;
		}

		v = p;
		return true;
	}
};
inline NetVarListResponse::~NetVarListResponse(){};

//--------------------------------------------------------------------
// 未知协议
//--------------------------------------------------------------------
class NetVarListMessage_Unkown : public NetVarListRequest
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
class NetVarListAction_Unknown : public NetVarListAction<NET_EXTRA_MAX_MSG_ID, NetVarListMessage_Unkown>
{
public:
	virtual const char* ActionKey() const
	{
		return "unknown";
	}

	virtual bool LoadFromVarList(const IVarList& args)
	{
		cargs.Clear();
		cargs.Append(args, 0, args.GetCount());
		return true;
	}

	virtual bool WriteToVarList(IVarList& args)
	{
		args.Append(cargs, 0, cargs.GetCount());
		return true;
	}

	CVarList cargs;
};


// 消息打包器
class NetVarListPacker : public INetActionPacker
{
protected:
	bool m_bCopyEmptyPackPostData;		// 空包是否复制队列数据
	bool m_bPostEmtpyPack;				// 是否抛出空消息
public:
	NetVarListPacker(bool bPostEmpty = false, bool bCopyEmptyData = false)
		: m_bCopyEmptyPackPostData(bCopyEmptyData), m_bPostEmtpyPack(bPostEmpty)
	{

	}
	virtual EMPackResult Pack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const
	{
		bool bOK = false;
		size_t nPos = stream.GetWritePos();

		INetVarListActionProtocol* pAction = dynamic_cast<INetVarListActionProtocol*>(action.get());
		if (NULL != pAction)
		{
			CVarList args;
			if (pAction->WriteToVarList(args))
			{
				int nNeedSize = text_encode_msg(args, NULL, 0);
				if (nNeedSize > 0)
				{
					char* buf = new char[nNeedSize << 1];
					int nSize = text_encode_msg(args, buf, nNeedSize);
					if (nSize > 0)
					{
						stream.AddStream(buf, nSize);
						bOK = true;
					}
					delete[] buf;
				}
			}
		}
		else if ( action.get() )
		{
			bOK = action->WriteTo( stream );
		}

		if (bOK)
		{
			stream.write_int8('\r');
			stream.write_int8('\n');
		}
		if (nOutLen != NULL)
		{
			*nOutLen = stream.GetWritePos() - nPos;
		}
		return bOK ? EM_PACK_SUCCESS : EM_PACK_ERROR;
	}

	virtual EMPackResult UnPack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const
	{
		const char* pBuffer = stream.GetData();
		size_t nLen = stream.GetReadLeft();
		for (size_t i = 0; i + 1 < nLen; ++i)
		{
			if (pBuffer[i] == '\r' && pBuffer[i + 1] == '\n')
			{
				size_t nMsgLen = i++;
				FsRefStream refStream((char*)pBuffer, nMsgLen);
				refStream.SeekWritePos((int)nMsgLen);
				if (NULL != nOutLen)
				{
					*nOutLen = nMsgLen + 2;
				}

				if (nMsgLen > 0)
				{
					CVarList args;
					if (!text_decode_msg(refStream.GetData(), refStream.GetReadLeft(), args) || args.GetCount() < 1)
					{
						return EM_PACK_ERROR;
					}

					INetVarListActionProtocol* protocol = NULL;
					if (args.GetType(0) == VTYPE_INT)
					{
						protocol = CreateAction(args.IntVal(0));
					}
					else
					{
						const char* key = args.StringVal(1);
						if (NULL == key) return EM_PACK_ERROR;
						protocol = CreateAction(key);
					}

					if (NULL != protocol)
					{
						INetAction* pAction = dynamic_cast<INetAction*>(protocol);
						assert(NULL != pAction);
						action.reset(pAction);
					}
					else
					{
						NetVarListAction_Unknown* p = new NetVarListAction_Unknown();
						protocol = p;
						action.reset(p);
					}

					return protocol->LoadFromVarList(args) ? EM_PACK_SUCCESS : EM_PACK_ERROR;
				}
				else 
				{
					if (m_bPostEmtpyPack)
					{
						// 空包
						SYSActionEmtpy* p = new SYSActionEmtpy();
						if (m_bCopyEmptyPackPostData)
						{
							p->LoadFrom(stream);
						}					
					}
					return EM_PACK_EMPTY;
				}
			}
		}
		return EM_PACK_BROKEN;
	}

protected:
	virtual INetVarListActionProtocol* CreateAction(const char* key) const
	{
		return NULL;
	}
	virtual INetVarListActionProtocol* CreateAction(int nKey) const
	{
		return NULL;
	}
};
#endif // END __NET_WORLD_AND_CHARGE_ACTION_H__