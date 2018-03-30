//--------------------------------------------------------------------
// 文件名:		NetWorkPayed.h
// 内  容:		支付相关协议
// 说  明:		
// 创建日期:		2016年11月28日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __NET_WORLD_PAYED_ACTION_H__
#define __NET_WORLD_PAYED_ACTION_H__

#include "NetVarListAction.h"
#include "ProtocolsID.h"
#include <vector>
#include <string>
#include "NetWorldAction.h"
#include "PayedOrder.h"
#include <sstream>

//--------------------------------------------------------------------
// 商品列表
//--------------------------------------------------------------------
class NetExtra_Products_Push : public NetWorldRequest
{
	bool LoadItems(const IVarList& args, size_t& nOffset, std::vector<GoodsItem>& items)
	{
		const char* pszItems = args.StringVal(nOffset++);
		CVarList arr;
		if (StringUtil::ParseToVector(pszItems, ',', arr) > 0)
		{
			for (size_t i = 0; i < arr.GetCount(); ++i)
			{
				const char* pkv = arr.StringVal(i);
				if (pkv[0] == '\0') continue;

				CVarList kvList;
				if (StringUtil::ParseToVector(pkv, ':', kvList) < 2) continue;

				try
				{
					const char* k = kvList.StringVal(0);
					if (k[0] == 0) continue;

					GoodsItem item_;
					item_.ItemId = k;
					item_.Number = kvList.IntVal(1);
					items.push_back(item_);
				}
				catch (...)
				{
					return false;
				}
			}
		}

		//int nCount_ = args.IntVal(nOffset++);
		//for (int i = 0; i < nCount_; ++i)
		//{
		//	if (args.GetCount() < nOffset + 2)
		//	{
		//		return false;
		//	}
		//	GoodsItem item_;
		//	item_.ItemId = args.StringVal(nOffset++);
		//	item_.Number = args.IntVal(nOffset++);
		//	items.push_back(item_);
		//}
		return true;
	}

	bool WriteItems(IVarList& args, const std::vector<GoodsItem>& items)
	{
		std::stringstream stream_;
		if (items.size() > 0)
		{
			const GoodsItem& item_ = items[0];
			stream_ << item_.ItemId << ":" << item_.Number;

			for (size_t i = 1; i < items.size(); ++i)
			{
				const GoodsItem& item_ = items[i];
				stream_ << "," << item_.ItemId << ":" << item_.Number;
			}
		}

		args.AddString( stream_.str().c_str() );
		return true;
	}
public:
	std::vector<Product> data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);

		size_t nIndex_ = 1;
		int nCount_ = args.IntVal(nIndex_++);
		for (int i = 0; i < nCount_; ++i)
		{
			Product product_;
			if (!SafeLoadString(product_.ProductId, args, nIndex_++))	// nIndex + 0
			{
				return false;
			}
			product_.Price = args.FloatVal(nIndex_++);
			product_.Rule = args.IntVal(nIndex_++);
			product_.Times = args.IntVal(nIndex_++);
			product_.State = args.IntVal(nIndex_++);

			// load items
			if (!LoadItems(args, nIndex_, product_.Items))
			{
				return false;
			}

			// load rewards
			if (!LoadItems(args, nIndex_, product_.Rewards))
			{
				return false;
			}

			// load gifts
			if (!LoadItems(args, nIndex_, product_.Gifts))
			{
				return false;
			}

			// load extra
			if (!LoadItems(args, nIndex_, product_.Extra))
			{
				return false;
			}

			data.push_back(product_);
		}

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddInt((int)data.size());
		for (std::vector<Product>::iterator itr = data.begin(); itr != data.end(); ++itr )
		{
			Product& product_ = *itr;
			args.AddString( product_.ProductId.c_str() );
			args.AddFloat(product_.Price);
			args.AddInt(product_.Rule);
			args.AddInt(product_.Times);
			args.AddInt(product_.State);

			WriteItems(args, product_.Items);
			WriteItems(args, product_.Rewards);
			WriteItems(args, product_.Gifts);
			WriteItems(args, product_.Extra);
		}

		return true;
	}
};

class NetExtraAction_Products_Push : public NetVarListAction<PROTOCOL_ID_PRODUCTS_PSH, NetExtra_Products_Push>
{

};

//--------------------------------------------------------------------
// 支付信息
//--------------------------------------------------------------------
class NetExtra_PaymentInfo : public NetWorldRequest
{
public:
	Payment data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 8);

		size_t nIndex_ = 1;
		data.RoleId = args.StringVal(nIndex_++);
		data.OrderId = args.StringVal(nIndex_++);
		data.ProductId = args.StringVal(nIndex_++);
		data.Amount = args.FloatVal(nIndex_++);
		data.State = args.IntVal(nIndex_++);
		data.PayType = args.IntVal(nIndex_++);
		data.OrderTime = args.Int64Val(nIndex_++);
		data.PayedTime = args.Int64Val(nIndex_++);

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(data.RoleId.c_str());
		args.AddString(data.OrderId.c_str());
		args.AddString(data.ProductId.c_str());
		args.AddFloat(data.Amount);
		args.AddInt(data.State);
		args.AddInt(data.PayType);
		args.AddInt64(data.OrderTime);
		args.AddInt64(data.PayedTime);

		return true;
	}
};

class NetExtraAction_Payment : public NetVarListAction<PROTOCOL_ID_PAYMENT_PSH, NetExtra_PaymentInfo>
{

};

//--------------------------------------------------------------------
// 支付下单
//--------------------------------------------------------------------
class NetExtra_Product_Order : public NetWorldRequest
{
public:
	ProductOrder data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 4);

		size_t nIndex_ = 1;
		data.RoleId = args.StringVal(nIndex_++);
		data.RoleName = StringUtil::WideStrAsUTF8String(args.WideStrVal(nIndex_++));
		data.Token = args.StringVal(nIndex_++);
		data.ProductId = args.StringVal(nIndex_++);
		
		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(data.RoleId.c_str());
		args.AddWideStr( StringUtil::UTF8StringAsWideStr( data.RoleName.c_str() ).c_str() );
		args.AddString(data.Token.c_str());
		args.AddString(data.ProductId.c_str());

		return true;
	}
};

class NetExtraAction_Product_Order : public NetVarListAction<PROTOCOL_ID_ORDER_REQ, NetExtra_Product_Order>
{

};

//--------------------------------------------------------------------
// 支付下单返回
//--------------------------------------------------------------------
class NetExtra_Product_Order_Info : public NetWorldRequest
{
public:
	ProductOderInfo data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 5);

		size_t nIndex_ = 1;
		data.RoleId = args.StringVal(nIndex_++);
		data.ProductId = args.StringVal(nIndex_++);
		data.OrderId = args.StringVal(nIndex_++);
		data.State = args.IntVal(nIndex_++);
		data.PayType = args.IntVal(nIndex_++);
		data.Extra = args.StringVal(nIndex_++);
		data.Price = args.FloatVal(nIndex_++);

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(data.RoleId.c_str());
		args.AddString(data.ProductId.c_str());
		args.AddString(data.OrderId.c_str());
		args.AddInt(data.State);
		args.AddInt(data.PayType);
		args.AddString(data.Extra.c_str());
		args.AddFloat(data.Price);

		return true;
	}
};

class NetExtraAction_Product_Order_Info : public NetVarListAction<PROTOCOL_ID_ORDER_INFO, NetExtra_Product_Order_Info>
{

};

//--------------------------------------------------------------------
// 订单状态改变
//--------------------------------------------------------------------
class NetExtra_Product_Order_Notify : public NetWorldRequest
{
public:
	OrderNotify data;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);

		size_t nIndex_ = 1;
		data.RoleId = args.StringVal(nIndex_++);
		data.OrderId = args.StringVal(nIndex_++);
		data.State = args.IntVal(nIndex_++);

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(data.RoleId.c_str());
		args.AddString(data.OrderId.c_str());
		args.AddInt(data.State);

		return true;
	}
};

class NetExtraAction_Product_Order_Notify : public NetVarListAction<PROTOCOL_ID_ORDER_NOTIFY, NetExtra_Product_Order_Notify>
{

};

//--------------------------------------------------------------------
// 查询测试充值返利
//--------------------------------------------------------------------
class NetExtra_Payed_Rebate_Query : public NetWorldRequest
{
public:
	std::string uid;
	std::string rid;

	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 2);

		size_t nIndex_ = 1;
		uid = args.StringVal(nIndex_++);
		rid = args.StringVal(nIndex_++);

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(uid.c_str());
		args.AddString(rid.c_str());

		return true;
	}
};

class NetExtraAction_PayedRebateQuery : public NetVarListAction<PROTOCOL_ID_REBATE_QUERY, NetExtra_Payed_Rebate_Query>
{

};

//--------------------------------------------------------------------
// 查询测试充值返利返回
//--------------------------------------------------------------------
class NetExtra_Payed_Rebate_Info : public NetWorldRequest
{
public:
	std::string rid;
	int			type;
	int			rebate;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);

		size_t nIndex_ = 1;
		rid = args.StringVal(nIndex_++);
		rebate = args.IntVal(nIndex_++);
		type = args.IntVal(nIndex_++);

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(rid.c_str());
		args.AddInt(rebate);
		args.AddInt(type);

		return true;
	}
};

class NetExtraAction_PayedRebateInfo : public NetVarListAction<PROTOCOL_ID_REBATE_INFO, NetExtra_Payed_Rebate_Info>
{

};

//--------------------------------------------------------------------
// 测试充值返利领取通知
//--------------------------------------------------------------------
class NetExtra_Payed_Rebate_Pick : public NetWorldRequest
{
public:
	std::string uid;
	std::string rid;
	std::wstring name;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 3);

		size_t nIndex_ = 1;
		uid = args.StringVal(nIndex_++);
		rid = args.StringVal(nIndex_++);
		name = args.WideStrVal(nIndex_++);

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(uid.c_str());
		args.AddString(rid.c_str());
		args.AddWideStr(name.c_str());

		return true;
	}
};

class NetExtraAction_PayedRebatePick : public NetVarListAction<PROTOCOL_ID_REBATE_PICK, NetExtra_Payed_Rebate_Pick>
{

};

//--------------------------------------------------------------------
// 测试充值返利领取通知
//--------------------------------------------------------------------
class NetExtra_Payed_Rebate_Result : public NetWorldRequest
{
public:
	std::string rid;
	int result;
	int amount;
	virtual bool LoadFromVarList(const IVarList& args)
	{
		assert(args.GetCount() > 4);

		size_t nIndex_ = 1;
		rid = args.StringVal(nIndex_++);
		result = args.IntVal(nIndex_++);
		amount = args.IntVal(nIndex_++);

		return true;

	}
	virtual bool WriteToVarList(IVarList& args)
	{
		args.AddString(rid.c_str());
		args.AddInt(result);
		args.AddInt(amount);

		return true;
	}
};

class NetExtraAction_PayedRebateResult : public NetVarListAction<PROTOCOL_ID_REBATE_RESULT, NetExtra_Payed_Rebate_Result>
{

};
#endif 