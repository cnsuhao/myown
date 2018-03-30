//--------------------------------------------------------------------
// 文件名:		NetWorldActivity.h
// 内  容:		运营活动服务器通信协议
// 说  明:		
// 创建日期:		2016年11月27日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_WORLD_ACTIVITY_ACTION_H__
#define __NET_WORLD_ACTIVITY_ACTION_H__

#include "NetVarListAction.h"
#include "ProtocolsID.h"
#include <vector>
#include <string>
#include "../model/ShopItem.h"
#include "utils/string_util.h"
#include <map>

//--------------------------------------------------------------------
// 推送商品 EXT->GAME
//--------------------------------------------------------------------
class NetExtra_ShopItemList : public NetVarListRequest
{
public:
	std::wstring	name;				// 名字
	std::wstring	desc;				// 介绍
	time_t			startTime;			// 开始时间
	time_t			endTime;			// 结束时间
	std::vector<ShopTag> catalogs;
	std::vector<ShopItem> items;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 6);
		if (args.GetCount() < 7)
		{
			return false;
		}

		size_t nIndex = 1;
		name = args.WideStrVal(nIndex++);							// index + 0
		desc = args.WideStrVal(nIndex++);
		startTime = args.Int64Val(nIndex++);
		endTime = args.Int64Val(nIndex++);

		// catalog
		int nCount = args.IntVal(nIndex++);							// index + 4
		for (int i = 0; i < nCount; ++i)
		{
			ShopTag tag_;
			tag_.id = args.IntVal(nIndex++);
			tag_.name = args.WideStrVal(nIndex++);
			catalogs.push_back(tag_);
		}

		// items
		nCount = args.IntVal(nIndex++);							// index + nCount * 2 + 5
		for (int i = 0; i < nCount; ++i)
		{
			ShopItem item;

			item.id = args.StringVal(nIndex++);
			item.name = args.WideStrVal(nIndex++);
			item.desc = args.WideStrVal(nIndex++);
			item.goods = args.StringVal(nIndex++);
			item.price = args.IntVal(nIndex++);
			item.color = args.IntVal(nIndex++);
			item.count = args.IntVal(nIndex++);
			item.level = args.IntVal(nIndex++);
			item.vip = args.IntVal(nIndex++);
			item.purchaseNum = args.IntVal(nIndex++);
			item.refreshType = args.IntVal(nIndex++);
			item.discount = args.IntVal(nIndex++);
			item.catalog = args.IntVal(nIndex++);

			item.onShowTime = args.Int64Val(nIndex++);
			item.offShowTime = args.Int64Val(nIndex++);
			item.onSellTime = args.Int64Val(nIndex++);
			item.offSellTime = args.Int64Val(nIndex++);
			item.newGoodTime = args.Int64Val(nIndex++);

			items.push_back(item);
		}

		return true;
	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddWideStr(name.c_str());
		args.AddWideStr(desc.c_str());
		args.AddInt64(startTime);
		args.AddInt64(endTime);


		args.AddInt((int)catalogs.size());
		for (std::vector<ShopTag>::iterator itr = catalogs.begin(); itr != catalogs.end(); ++itr)
		{
			args.AddInt(itr->id);
			args.AddWideStr( itr->name.c_str() );
		}

		args.AddInt( (int)items.size() );
		for (std::vector<ShopItem>::iterator itr = items.begin(); itr != items.end(); ++itr)
		{
			ShopItem& item = *itr;
			args.AddString(item.id.c_str());
			args.AddWideStr(item.name.c_str());
			args.AddWideStr(item.desc.c_str());
			args.AddString(item.goods.c_str());

			args.AddInt(item.price);
			args.AddInt(item.color);
			args.AddInt(item.count);
			args.AddInt(item.level);
			args.AddInt(item.vip);
			args.AddInt(item.purchaseNum);
			args.AddInt(item.refreshType);
			args.AddInt(item.discount);
			args.AddInt(item.catalog);

			args.AddInt64(item.onShowTime);
			args.AddInt64(item.offShowTime);
			args.AddInt64(item.onSellTime);
			args.AddInt64(item.offSellTime);
			args.AddInt64(item.newGoodTime);
		}
		
		return true;
	}
};

class NetExtraAction_ShopPushItem : public NetVarListAction<PROTOCOL_ID_PUSH_SHOP_ITEM, NetExtra_ShopItemList>
{

};

//--------------------------------------------------------------------
// 删除商品 EXT->GAME
//--------------------------------------------------------------------
class NetExtra_ShopRemoveList : public NetVarListRequest
{
public:
	std::vector<std::string> removes;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 1);

		size_t nIndex = 1;
		int nCount = args.IntVal(nIndex++);							// index + 0
		for (int i = 0; i < nCount; ++i)
		{
			const char* pszId = args.StringVal(nIndex++);
			if (StringUtil::CharIsNull(pszId))
			{
				continue;
			}
			removes.push_back(pszId);
		}
		return true;
	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt((int)removes.size());
		for (std::vector<std::string>::iterator itr = removes.begin(); itr != removes.end(); ++itr)
		{
			args.AddString(itr->c_str());
		}

		return true;
	}
};

class NetExtraAction_ShopRemoveItem : public NetVarListAction<PROTOCOL_ID_DEL_SHOP_ITEM, NetExtra_ShopRemoveList>
{

};

//--------------------------------------------------------------------
// 查询商品 EXT->GAME
//--------------------------------------------------------------------
class NetExtra_ShopItemQuery : public NetVarListRequest
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

class NetExtraAction_ShopQueryItem : public NetVarListAction<PROTOCOL_ID_QUERY_SHOP_ITEM, NetExtra_ShopItemQuery>
{

};


//--------------------------------------------------------------------
// 商品列表返加 GAME->EXT
//--------------------------------------------------------------------
class NetExtraAction_ShopItemList : public NetVarListAction<PROTOCOL_ID_SHOP_ITEM_LIST, NetExtra_ShopItemList>
{

};


//--------------------------------------------------------------------
// 通用活动配置 EXT->GAME
//--------------------------------------------------------------------
class NetExtra_ActiityConfig : public NetVarListRequest
{
public:
	ActivityInfo data;				// 活动信息
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() >= 7);
		assert((args.GetCount() - 7) % 4 == 0);
		if (args.GetCount() < 7)
		{
			return false;
		}

		size_t nIndex = 1;
		data.name = args.WideStrVal(nIndex++);						// index + 0
		data.desc = args.WideStrVal(nIndex++);						// index + 1
		data.actType = args.IntVal(nIndex++);						// index + 2
		data.startTime = args.Int64Val(nIndex++);					// index + 3
		data.endTime = args.Int64Val(nIndex++);						// index + 4
		int nCount = args.IntVal(nIndex++);							// index + 5
		for (int i = 0; i < nCount; ++i)
		{
			if (nIndex + 4 >= args.GetCount())
			{
				return false;
			}

			ActivityItem item;
			item.value = args.IntVal(nIndex++);
			item.name = args.WideStrVal(nIndex++);
			item.goods = args.StringVal(nIndex++);
			item.count = args.IntVal(nIndex++);

			data.items.push_back(item);
		}

		return true;
	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddWideStr(data.name.c_str());
		args.AddWideStr(data.desc.c_str());
		args.AddInt(data.actType);
		args.AddInt64(data.startTime);
		args.AddInt64(data.endTime);
		args.AddInt((int)data.items.size());
		for (std::vector<ActivityItem>::iterator itr = data.items.begin(); itr != data.items.end(); ++itr)
		{
			ActivityItem& item = *itr;
			args.AddInt(item.value);
			args.AddWideStr(item.name.c_str());
			args.AddString(item.goods.c_str());
			args.AddInt(item.count);
		}

		return true;
	}
};

class NetExtraAction_ActiityConfig : public NetVarListAction<PROTOCOL_ID_SET_ACTIVITY_CONFIG, NetExtra_ActiityConfig>
{

};


//--------------------------------------------------------------------
// 加载配置 EXT->GAME
//--------------------------------------------------------------------
class NetExtra_LoadConfig : public NetExtraGameAck
{
public:
	std::vector<ConfigLoadInfo> data;				// 活动信息
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() >= 3);
		assert((args.GetCount() - 3) % 4 == 0);
		if (args.GetCount() < 3)
		{
			return false;
		}

		if (!NetExtraGameAck::LoadFromVarList(args))
		{
			return false;
		}

		size_t nIndex = 2;
		int32_t nCount = args.IntVal(nIndex++);						// index + 0
		for (int i = 0; i < nCount; ++i)
		{
			if (nIndex + 4 >= args.GetCount())
			{
				return false;
			}

			ConfigLoadInfo cfg;
			cfg.key = args.StringVal(nIndex++);
			cfg.path = args.StringVal(nIndex++);
			cfg.catalog = args.IntVal(nIndex++);
			cfg.cfgid = args.IntVal(nIndex++);

			data.push_back(cfg);
		}

		return true;
	}
	virtual bool WriteToVarList(IVarList& args)
	{
		NetExtraGameAck::WriteToVarList(args);

		int32_t nCount = (int32_t)data.size();
		args.AddInt(nCount);
		for (std::vector<ConfigLoadInfo>::iterator itr = data.begin(); itr != data.end(); ++itr)
		{
			ConfigLoadInfo& cfg = *itr;
			args.AddString(cfg.key.c_str());
			args.AddString(cfg.path.c_str());
			args.AddInt(cfg.catalog);
			args.AddInt(cfg.cfgid);
		}

		return true;
	}
};

class NetExtraAction_LoadConfig : public NetVarListAction<PROTOCOL_ID_LOAD_CONFIG, NetExtra_LoadConfig>
{

};
#endif 