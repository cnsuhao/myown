//--------------------------------------------------------------------
// 文件名:		IAction.h
// 内  容:		动作定义
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __I_ACTION_H__
#define __I_ACTION_H__

#include "utils/FsStream.h"

#define DEF_ACTION_REQUEST_TIMEOUT	30000

class IMessage
{
public:
	virtual ~IMessage() = 0;

	virtual bool LoadFrom(const FsIStream& stream)
	{
		return true;
	}

	virtual bool WriteTo(FsIStream& stream) const
	{
		return true;
	}
};
inline IMessage::~IMessage(){};

class IRequest : public IMessage
{
public:
	virtual ~IRequest() = 0;

	virtual unsigned GetTimeOut() const
	{
		return DEF_ACTION_REQUEST_TIMEOUT;
	};
};
inline IRequest::~IRequest(){};

typedef IMessage IResponse;

enum EmActionState
{
	EM_ACTION_REQUEST,			// 请求
	EM_ACTION_REPLY,			// 回应
	EM_ACTION_RESULT,			// 结果
};
class IAction
{
public:
	virtual ~IAction() = 0;
	virtual unsigned short ActionId() const = 0;
	virtual const char* ActionKey() const = 0;

	virtual IRequest* Request() = 0;
	virtual IResponse* Response() = 0;

	virtual bool LoadFrom(const FsIStream& stream) = 0;
	virtual bool WriteTo(FsIStream& stream) const = 0;
	virtual bool IsAsync() const = 0;

public:
	virtual void SetIndex(unsigned short index)
	{
		m_nIndex = index;
	}
	virtual unsigned short GetIndex() const
	{
		return m_nIndex;
	}

	virtual void SetState(EmActionState eState)
	{
		m_eState = eState;
	}

	virtual EmActionState GetState() const
	{
		return m_eState;
	}

	virtual bool IsRequest() const
	{
		return m_eState == EM_ACTION_REQUEST;
	}

	virtual bool IsReply() const
	{
		return m_eState == EM_ACTION_REPLY;
	}

	virtual bool IsResult() const
	{
		return m_eState == EM_ACTION_RESULT;
	}
	virtual int GetTag() const
	{
		return m_nTag;
	}

	virtual void SetTag(int nTag)
	{
		m_nTag = nTag;
	}

protected:
	unsigned short m_nIndex;
	EmActionState m_eState;
	int m_nTag;
	IAction() : m_nIndex(0), m_eState(EM_ACTION_REQUEST), m_nTag( 0 )
	{
	}
};
inline IAction::~IAction(){}

template<unsigned short ID, bool ASYNC = true>
class EmptyAction : public IAction
{
public:
	virtual ~EmptyAction(){};
	virtual unsigned short ActionId() const
	{
		return ID;
	}
	virtual const char* ActionKey() const
	{
		return "";
	}

	virtual IRequest* Request()
	{
		return NULL;
	}

	virtual IResponse* Response()
	{
		return NULL;
	}

	virtual bool LoadFrom(const FsIStream& stream)
	{
		return true;
	}

	virtual bool WriteTo(FsIStream& stream) const
	{
		return true;
	}

	virtual bool IsAsync() const
	{
		return ASYNC;
	}
};

template<unsigned short ID, typename Req, bool ASYNC = true>
class Action : public IAction
{
public:
	virtual ~Action(){};
	virtual unsigned short ActionId() const
	{
		return ID;
	}
	virtual const char* ActionKey() const
	{
		return NULL;
	}

	virtual IRequest* Request()
	{
		return &m_req;
	}
	virtual IResponse* Response()
	{
		return NULL;
	}

	virtual bool LoadFrom(const FsIStream& stream)
	{
		return m_req.LoadFrom(stream);
	}

	virtual bool WriteTo(FsIStream& stream) const
	{
		return m_req.WriteTo(stream);
	}

	virtual bool IsAsync() const
	{
		return ASYNC;
	}
protected:
	Req m_req;
};


template<unsigned short ID, typename Req, typename Res, bool ASYNC = true>
class ReqAction : public Action<ID, Req, ASYNC>
{
public:
	ReqAction()
		: m_bRequest(true)
	{
	}
	virtual ~ReqAction(){};

	virtual IResponse* Response()
	{
		return &m_res;
	}

	virtual bool LoadFrom(const FsIStream& stream)
	{
		if (IsRequest())
		{
			return Action<ID, Req>::LoadFrom(stream);
		}

		return m_res.LoadFrom(stream);
	}

	virtual bool WriteTo(FsIStream& stream) const
	{
		if (IsRequest())
		{
			return Action<ID, Req>::WriteTo(stream);
		}

		return m_res.WriteTo(stream);
	}

	virtual void SetState(EmActionState eState)
	{
		m_eState = eState;
	}
protected:
	Res				m_res;
	bool			m_bRequest;
};


class BinaryMessage : public IRequest
{
	FsStream m_binStream;
public:
	const char* GetMsgBody() const
	{
		return m_binStream.GetBuffer();
	}

	size_t GetMsgLen() const
	{
		return m_binStream.GetWritePos();
	}

	virtual bool LoadFrom(const FsIStream& stream)
	{
		m_binStream.Reset();
		m_binStream.AddStream(stream.GetData() + stream.GetReadPos(), stream.GetReadLeft());
		return true;
	}

	virtual bool WriteTo(FsIStream& stream) const
	{
		stream.AddStream(GetMsgBody(), GetMsgLen());
		return true;
	}

	void SetMsgBody(const char* pData, size_t nLen)
	{
		if (NULL == pData || 0 == nLen)
		{
			ResetMsgBody();
		}
		else
		{
			FsRefStream stream((char*)pData, nLen);
			LoadFrom(stream);
		}
	}

	void ResetMsgBody()
	{
		m_binStream.Reset();
	}
};

#endif // END __I_ACTION_H__