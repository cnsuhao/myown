//--------------------------------------------------------------------
// 文件名:		NetWorldPush.h
// 内  容:		推送服务器通信协议
// 说  明:		
// 创建日期:		2016年10月19日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_WORLD_PUSH_NOTIFICATION_H__
#define __NET_WORLD_PUSH_NOTIFICATION_H__

#include "NetVarListAction.h"
#include "ProtocolsID.h"
#include <vector>
#include <string>
#include "../model/PushData.h"

//--------------------------------------------------------------------
// 推送通知请求
//--------------------------------------------------------------------
class NetExtra_PushRequest : public NetVarListRequest
{
public:
	PushData data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() >= 10);

		size_t nIndex = 1;
		data.msgType = args.IntVal(nIndex++);							// index + 0
		data.recvierPlatform = args.IntVal(nIndex++);					// index + 1

		// 接收类型种类
		int _nKinds = args.IntVal(nIndex++);							// index + 2
		for (int i = 0; i < _nKinds; ++i)
		{
			int _nType = args.IntVal(nIndex++);
			int _nCount = args.IntVal(nIndex++);
			for (int j = 0; j < _nCount; ++j)
			{
				std::string strIdent;
				if (!SafeLoadString(strIdent, args, nIndex++))		// nIndex + 0
				{
					return false;
				}
				data.recivers[_nType].push_back(strIdent);
			}
		}

		// 没有接收者目标则为广播
		//if (data.recivers.size() == 0)
		//{
		//	return false;
		//}
		
		// 最后必须还有5个字段
		if (nIndex + 5 > args.GetCount())
		{
			return false;
		}

		if (!SafeLoadString(data.title, args, nIndex++))
		{
			return false;
		}

		if (!SafeLoadString(data.content, args, nIndex++))
		{
			return false;
		}

		if (!SafeLoadString(data.sound, args, nIndex++))
		{
			return false;
		}
		data.builderid = args.IntVal(nIndex++);
		data.badge = args.IntVal(nIndex++);
		data.content_available = args.IntVal(nIndex++);

		if (nIndex < args.GetCount())
		{
			int _nCount = args.IntVal(nIndex++);
			assert(_nCount * 2 + nIndex <= args.GetCount());

			// 读取扩展数据
			for (int i = 0; i < _nCount; ++i)
			{
				std::string key;
				if (!SafeLoadString(key, args, nIndex))
				{
					break;
				}

				std::string value;
				if (!SafeLoadString(value, args, nIndex))
				{
					break;
				}
				data.extra[key] = value;
			}
		}
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt(data.msgType);
		args.AddInt(data.recvierPlatform);

		args.AddInt((int)data.recivers.size());
		for (std::map<int, std::vector<std::string> >::iterator itr = data.recivers.begin(); itr != data.recivers.end(); ++itr)
		{
			int _nType = itr->first;
			args.AddInt(_nType);

			const std::vector<std::string>& _recivers = itr->second;
			args.AddInt( (int)_recivers.size() );
			for (std::vector<std::string>::const_iterator vit = _recivers.begin(); vit != _recivers.end(); ++vit )
			{
				args.AddString(vit->c_str());
			}
		}

		args.AddString(data.title.c_str());
		args.AddString(data.content.c_str());
		args.AddString(data.sound.c_str());
		args.AddInt(data.builderid);
		args.AddInt(data.badge);
		args.AddInt(data.content_available);

		args.AddInt((int)data.extra.size());
		for (std::map<std::string, std::string>::iterator itr = data.extra.begin(); itr != data.extra.end(); ++itr)
		{
			args.AddString(itr->first.c_str());
			args.AddString(itr->second.c_str());
		}
		
		return true;
	}
};

class NetExtraAction_PushNotification : public NetVarListAction<PROTOCOL_ID_PUSH_NOTIFICATION, NetExtra_PushRequest>
{

};
#endif 