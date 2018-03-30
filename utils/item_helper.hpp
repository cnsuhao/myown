//---------------------------------------------------------
//文件名:       item_helper.cpp
//内  容:       物品助手
//说  明:       
//
//创建日期:     2017年08月07日
//创建人:       kevin 
// 版权所有:	WhalesGame Technology co.Ltd  
//---------------------------------------------------------
#ifndef __HEPLER_ITEM_HPP__
#define __HEPLER_ITEM_HPP__

#include <vector>
#include "cache/CacheString.hpp"
#include "string_util.h"
#include "util_func.h"
//#include <sstream>

// item node
struct ItemNode
{
public:
	int64_t item_num_;
	CacheString<64> item_id_;
public:
	ItemNode() :item_num_(0){}
	~ItemNode(){}
public:
	void Clear()
	{
		item_num_ = 0;
		item_id_.Clear();
	}
	bool IsValid() const
	{
		if (item_id_.IsEmpty() || item_num_<0)
		{
			return false;
		}
		return true;
	}
};

// for item list
struct ItemNodeList
{
private:
	std::vector<ItemNode> list_;
public:
	ItemNodeList(){}
	~ItemNodeList(){}
public:
	void Clear()
	{
		if (GetCount()>0)
		{
			list_.clear();
		}
	}
public:
	int GetCount() const
	{
		return (int)list_.size();
	}
public:
	//decode: "item-d:item-num,item-d:item-num" = > list
	bool ItemDeCode(const char* item_list_string, const char* exp_equa = C_KERNEL_STRIMG_COLON, const char* exp_split = C_KERNEL_STRIMG_COMMA)
	{
		if (item_list_string == NULL || strcmp(item_list_string,C_KERNEL_STRIMG_EMPTY) == 0)
		{
			return false;
		}
		ItemNode p_node;
		CVarList parse_result;
		util_split_express_string(parse_result, item_list_string,exp_equa,exp_split);

		for (int index = 0; index < (int)parse_result.GetCount(); index = index + 2)
		{
			p_node.Clear();
			p_node.item_id_.SetString(parse_result.StringVal(index));
			p_node.item_num_ = Port_StringToInt64( parse_result.StringVal(index + 1));
			if (p_node.IsValid())
			{
				list_.push_back(p_node);
			}
		}
		return true;
	}
	//eccode: list = > "item-d:item-num,item-d:item-num"
	bool ItemEnCode(ICacheString& dst_buff, const char* exp_equa = C_KERNEL_STRIMG_COLON, const char* exp_split = C_KERNEL_STRIMG_COMMA)
	{
		dst_buff.Clear();
		int n_count = GetCount();
		if (n_count>0)
		{
			for (int index = 0; index < n_count;index++)
			{
				ItemNode& p_node = list_[index];
				if (!p_node.IsValid())
				{
					continue;
				}
				dst_buff.AppendSplitFormat(exp_split, "%s%s%lld", p_node.item_id_.GetString(), exp_equa, p_node.item_num_);
			}
		}
		return true;
	}
public:
	//add item
	bool AddItem(const char* item_id, int64_t item_num)
	{
		ItemNode p_node;
		p_node.item_id_.SetString(item_id);
		p_node.item_num_ = item_num;
		return AddItem(p_node);
	}
	bool AddItem(const ItemNode& p_node)
	{
		if (!p_node.IsValid())
		{
			return false;
		}
		list_.push_back(p_node);
		return true;
	}
	bool GetItem(const char* item_id, ItemNode*& p_node)
	{
		p_node = NULL;
		if (item_id == NULL || strcmp(item_id, C_KERNEL_STRIMG_EMPTY) == 0)
		{
			Assert(0);
			return false;
		}
		int n_count = GetCount();
		if (n_count > 0)
		{
			for (int index = 0; index < n_count; index++)
			{
				if (list_[index].item_id_.IsEqual(item_id))
				{
					p_node = &list_[index];
					break;
				}
			}
		}
		if (p_node == NULL)
		{
			return false;
		}
		return true;
	}
	int64_t GetItemValue(const char* item_id,int64_t def_value=0)
	{
		if (item_id == NULL || strcmp(item_id, C_KERNEL_STRIMG_EMPTY) == 0)
		{
			Assert(0);
			return def_value;
		}
		int n_count = GetCount();
		if (n_count > 0)
		{
			for (int index = 0; index < n_count; index++)
			{
				if (list_[index].item_id_.IsEqual(item_id))
				{
					return list_[index].item_num_;
				}
			}
		}
		return def_value;
	}
	bool SetItemValue(const char* item_id, int64_t i_value = 0)
	{
		if (item_id == NULL || strcmp(item_id, C_KERNEL_STRIMG_EMPTY) == 0)
		{
			Assert(0);
			return false;
		}
		int n_count = GetCount();
		if (n_count > 0)
		{
			for (int index = 0; index < n_count; index++)
			{
				if (list_[index].item_id_.IsEqual(item_id))
				{
					list_[index].item_num_ = i_value;
					return true;
				}
			}
		}
		return false;
	}
	bool GetItemByIndex(int index, ItemNode*& p_node)
	{
		p_node = NULL;
		if (index < 0 || index >= GetCount())
		{
			return false;
		}
		p_node = &list_[index];
		if (p_node == NULL)
		{
			return false;
		}
		return true;
	}
public:
	// for is contain ItemNodeList
	bool IsContain(const ItemNodeList& ref)
	{
		int n_count = ref.GetCount();
		if (GetCount() < n_count)
		{
			return false;
		}
		if (n_count ==0)
		{
			return true;
		}
		ItemNode* p_node = NULL;
		for (int index = 0; index < n_count; index++)
		{
			if (!GetItem(ref.list_[index].item_id_.GetString(),p_node))
			{
				return false;
			}
		}
		return true;
	}
	// for contain dec ItemNodeList
	bool DecContain(const ItemNodeList& ref)
	{
		int n_count = ref.GetCount();
		if (GetCount() < n_count)
		{
			return false;
		}
		if (n_count == 0)
		{
			return true;
		}
		ItemNode* p_node = NULL;
		for (int index = 0; index < n_count; index++)
		{
			if (!GetItem(ref.list_[index].item_id_.GetString(), p_node))
			{
				Assert(0);
				continue;
			}
			p_node->item_num_ -= ref.list_[index].item_num_;
			if (p_node->item_num_<0)
			{
				Assert(0);
				p_node->item_num_ = 0;
			}
		}
		return true;
	}
};


#endif // _CAU_H__
