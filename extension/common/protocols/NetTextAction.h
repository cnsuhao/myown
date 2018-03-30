//--------------------------------------------------------------------
// 文件名:		NetTextMessage.h
// 内  容:		文本格式网络消息
// 说  明:		
// 创建日期:		2016年5月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_TEXT_MESSAGE_H__
#define __NET_TEXT_MESSAGE_H__

#include "action/INetAction.h"
#include "public/FastStr.h"
#include "MsgEncoder.h"
#include "public/VarList.h"

class NetTextMessage : public INetRequest
{
public:

	virtual const char* GetMsgBody()
	{
		return m_strData.c_str();
	}

	virtual bool LoadFrom(const FsIStream& stream)
	{
		char* szData = new char[stream.GetReadLeft() + 1];
		memset(szData, 0, stream.GetReadLeft() + 1);

		stream.GetStream(szData, stream.GetReadLeft());
		m_strData = szData;

		delete[] szData;
		return true;
	}

	virtual bool WriteTo(FsIStream& stream) const
	{
		stream.AddStream(m_strData.c_str(), m_strData.length());
		return true;
	}

	TFastStr<char, 1024>& GetMutableBody()
	{
		return m_strData;
	}

private:
	TFastStr<char, 1024>	m_strData;
};


class TextAction : public NetAction<0, NetTextMessage>
{
public:

	virtual const char* ActionKey() const
	{
		return "TEXT";
	}
};

class NetTextMessagePacker : public INetActionPacker
{
public:
	virtual EMPackResult Pack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const
	{
		INetMessage* pMsg = action->Request();
		if (NULL == pMsg) return EM_PACK_ERROR;

		if (NULL != nOutLen)
		{
			size_t nPos = stream.GetWritePos();
			bool bOK = pMsg->WriteTo(stream);
			*nOutLen = stream.GetWritePos() - nPos;
			return bOK ? EM_PACK_SUCCESS : EM_PACK_ERROR;
		}
		return pMsg->WriteTo(stream) ? EM_PACK_SUCCESS : EM_PACK_ERROR;
	}

	virtual EMPackResult UnPack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const
	{
		const char* pBuffer = stream.GetData();
		size_t nLen = stream.GetReadLeft();
		for (size_t i = 0; i < nLen; ++i)
		{
			if (i + 1 < nLen)
			{
				if (pBuffer[i] == '\r' && pBuffer[i + 1] == '\n')
				{
					std::shared_ptr<INetAction> pa(new TextAction());
					action.swap(pa);

					size_t nMsgLen = i;
					if (NULL != nOutLen)
					{
						*nOutLen = nMsgLen + 2;
					}

					INetMessage* pMsg = action->Request();
					if (NULL == pMsg)
					{
						return EM_PACK_UNREGISTER;
					}

					FsRefStream ref((char*)pBuffer, nMsgLen);
					ref.SeekWritePos((int)nMsgLen);
					return pMsg->LoadFrom(ref) ? EM_PACK_SUCCESS : EM_PACK_ERROR;
				}
			}
			else
			{
				return EM_PACK_BROKEN;
			}
		}
		return EM_PACK_BROKEN;
	}
};




class NetTextMessageEx : public INetRequest
{
public:

	virtual const char* GetMsgBody()
	{
		return NULL;
	}

	virtual bool LoadFrom(const FsIStream& stream)
	{
		m_varMsg.Clear();
		if (text_decode_msg(stream.GetData(), stream.GetReadLeft(), m_varMsg))
		{
			return true;
		}
		return false;
	}

	virtual bool WriteTo(FsIStream& stream) const
	{
		int nNeedSize = text_encode_msg(m_varMsg, NULL, 0);
		if (nNeedSize > 0)
		{
			char* buf = new char[nNeedSize << 1];
			int nSize = text_encode_msg(m_varMsg, buf, nNeedSize);
			if (nSize > 0)
			{
				stream.AddStream(buf, nSize);
				delete[] buf;
				return true;
			}
		}
		return false;
	}

	const IVarList& GetBody() const
	{
		return m_varMsg;
	}

	IVarList& GetMutableBody()
	{
		return m_varMsg;
	}

private:
	CVarList			m_varMsg;
};


class TextActionEx : public NetAction<0, NetTextMessageEx>
{
public:

	virtual const char* ActionKey() const
	{
		/*
		if (m_req.GetBody().GetCount() > 0)
		{
			return m_req.GetBody().StringVal(0);
		}
		*/
		return "TEXT_EX";
	}
};

class NetTextMessagePackerEx : INetActionPacker
{
public:
	virtual EMPackResult Pack(INetConnection* conn, FsIStream& stream, std::shared_ptr<INetAction>& action, size_t* nOutLen = NULL) const
	{
		INetRequest* pMsg = action->Request();
		if (NULL == pMsg) return EM_PACK_ERROR;

			size_t nPos = stream.GetWritePos();
			bool bOK = pMsg->WriteTo(stream);
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
		for (size_t i = 0; i < nLen; ++i)
		{
			if (i + 1 < nLen)
			{
				if (pBuffer[i] == '\r' && pBuffer[i + 1] == '\n')
				{
					std::shared_ptr<INetAction> p(new TextActionEx());
					action.swap(p);

					size_t nMsgLen = i;
					FsRefStream refStream((char*)pBuffer, nMsgLen);
					refStream.SeekWritePos((int)nMsgLen);
					if (NULL != nOutLen)
					{
						*nOutLen = nMsgLen + 2;
					}

					INetRequest* pMsg = action->Request();
					if (NULL == pMsg)
					{
						return EM_PACK_UNREGISTER;
					}

					return pMsg->LoadFrom(refStream) ? EM_PACK_SUCCESS : EM_PACK_ERROR;
				}
			}
			else
			{
				return EM_PACK_BROKEN;
			}
		}
		return EM_PACK_BROKEN;
	}
};

#endif // END __NET_TEXT_MESSAGE_H__