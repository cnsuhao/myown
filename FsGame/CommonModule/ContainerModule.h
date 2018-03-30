//---------------------------------------------------------
//文件名:       ContainerModule.h
//内  容:       容器
//说  明:       
//          
//创建日期:      2014年10月31日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------
#ifndef FSGAME_COMMON_CONTAINER_MODULE_H_
#define FSGAME_COMMON_CONTAINER_MODULE_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/ContainerDefine.h"

#include <vector>
#include <list>
#include <map>
#include "FsGame/Define/FunctionEventDefine.h"

class LogModule;
class AsynCtrlModule;
class CoolDownModule;
class ItemBaseModule;
//class EquipmentModule;

class ContainerModule : public ILogicModule
{
public:

    //初始化
    bool Init(IKernel* pKernel);
    //释放
    bool Shut(IKernel* pKernel);


public:
	//////////////////////////////////////////////////////////////////////////
	// 容器维护接口
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 查询接口
	//////////////////////////////////////////////////////////////////////////

	// 查询物品在容器中的位置，返回0表示不存在
	static int GetItemPos(IKernel* pKernel, const PERSISTID& container,
		const PERSISTID& request_item);

	//获得指定物品的数量
	static int GetItemCount(IKernel* pKernel, const PERSISTID& container,
		const char* configid, int match = ITEM_MATCH_ALL);

	// 从容器中查找指定的物品, 返回找到的第一个物品
	// 找不到的时候返回一个无效的PERSISTID
	static PERSISTID FindItem(IKernel* pKernel, const PERSISTID& container,
		const char* configid, int match = ITEM_MATCH_ALL);
	// 从容器中查找指定的物品, 优先返回找到绑定的第一个物品
	// 找不到的时候返回一个无效的PERSISTID
	static PERSISTID FindItemBindPrior(IKernel* pKernel, const PERSISTID& container,
		const char* configid, int match = ITEM_MATCH_ALL);

	// 从容器中查找指定属性的物品, 通过UID找
	// 找不到的时候返回一个无效的PERSISTID,必须指定到分类背包，不能使用ToolBox
	static PERSISTID FindItemByUid(IKernel* pKernel, const PERSISTID& container,
		const char * uid);

	// 测试@self是否可以操作指定容器container
	static bool CanOperateContainer(IKernel* pKernel, const PERSISTID& self,
		const PERSISTID& container, int optype, int pos);

	//获得容器的有效容量(排除锁定的)，必须指定到分类背包，不能使用ToolBox
	static int GetAvailableCapacity(IKernel* pKernel, const PERSISTID& container);

	//获得容器中的所有物品，必须指定到分类背包，不能使用ToolBox
	static int GetAllItems(IKernel* pKernel, const PERSISTID& container, IVarList& res);

	//获得一个空闲的位置, 返回0表示无空闲位置，必须指定到分类背包，不能使用ToolBox
	static int GetFreePos(IKernel* pKernel, const PERSISTID& container);

	// 获取容器中空闲位置的数量,必须指定到分类背包，不能使用ToolBox
	static int GetFreePosCount(IKernel* pKernel, const PERSISTID& container);

	//////////////////////////////////////////////////////////////////////////
	// 放入物品接口
	//////////////////////////////////////////////////////////////////////////

	// 尝试把@item放入容器@container中, 返回可以放入的数量
	// 此操作不会真正放入物品, 仅仅是计算可放入的数量
	// @destpos = 0 表示不指定放入位置
	static int TryAddItem(IKernel* pKernel, const PERSISTID& container,
		const PERSISTID& item, int destpos = 0);

	// 尝试把指定数量@item的物品放入容器@container中, 返回可以放入的数量
	// 此操作不会真正放入物品, 仅仅是计算可放入的数量
	static int TryAddItem(IKernel* pKernel, const PERSISTID& container, SBoxTestItem* request);

	// 尝试把指定数量@count的物品@config放入容器@container中, 返回可以放入的数量
	// 此操作不会真正放入物品, 仅仅是计算可放入的数量 frozenTime: 冻结时间
	static int TryAddItem(IKernel* pKernel, const PERSISTID& container,
		const char* config, int count, int bound = ITEM_NOT_BOUND, int64_t frozenTime = 0);

	// 尝试把多个物品放入容器中, 全部可以放入返回true
	static bool TryAddItems(IKernel* pKernel, const PERSISTID& container,VectorBoxTestItem *item_list);
	// 尝试把多个物品放入容器中, 全部可以放入返回true
	static bool TryAddItems(IKernel* pKernel, BoxItemData& item_datas);

	// 将多件物品放入容器,, 失败返回false
	// 如果想确保全部放入，请先调用TryAddItems()进行判断
	static bool PlaceItems( IKernel* pKernel, BoxItemData& item_datas, EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true );
	static bool PlaceItems(IKernel* pKernel, const PERSISTID& container, VectorBoxTestItem *item_list,
							EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true);

	// 将物品存放到容器中(destpos==0,不指定位置), 返回放入的数量
	// 使用TryAddItem() 可以测试物品能否全部放入
	static int PlaceItem(IKernel* pKernel, const PERSISTID& container, const PERSISTID& item,
				EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true, int destpos = 0);

	//将物品存放到容器中, 返回放入的数量 frozenTime: 冻结时间
	static int PlaceItem(IKernel* pKernel, const PERSISTID& container,
				const char *config_id, int request_amount,
				EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true,
				int bound = ITEM_NOT_BOUND, int64_t frozenTime = 0);

	// 把物品放入背包，返回放入的数量.  frozenTime: 冻结时间
	// @saved_items 里返回实际保存的对象和数量, 格式为:
	//      [CONFIG_ID][UNIQUE_ID][SAVED_AMOUNT]...[CONFIG_ID][UNIQUE_ID][SAVED_AMOUNT]
	static int PlaceItem(IKernel* pKernel, const PERSISTID& container,
		const char *config_id, int request_amount, CVarList &saved_items, 
		EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true,
		int bound = ITEM_NOT_BOUND, int64_t frozenTime = 0);

	//物品放入背包，背包空间不足放入临时背包, 临时背包空间不足返回false
	static bool PlaceItemsToBagOrTemp(IKernel* pKernel, const PERSISTID& self,
					const char *config_id, int request_amount, 
					EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true,
					int bound = ITEM_NOT_BOUND, int64_t frozenTime = 0);
	static bool PlaceItemsToBagOrTemp(IKernel* pKernel, const PERSISTID& self, VectorBoxTestItem *item_list,
					EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true);
public:
	// 尝试从容器中删除指定物品的数量, 可以删除指定的数量返回true
	// 此操作不会真正删除物品
	static bool TryRemoveItems(IKernel* pKernel, const PERSISTID& container,
		const char* configid, int count, int match = ITEM_MATCH_ALL);
	static bool TryRemoveItems( IKernel* pKernel, const PERSISTID& container, 
		const std::map<std::string, int>& items, int match = ITEM_MATCH_ALL);

	// 从容器中删除指定物品的数量, 返回删除的数量, 清空后销毁
	static int RemoveItem(IKernel* pKernel, const PERSISTID& container,
		const PERSISTID& item, int count,
		EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsConsume = true );
	// 从容器中删除指定物品的数量, 返回删除的数量
	static int RemoveItems(IKernel* pKernel, const PERSISTID& container,
		const char* configid, int count,
		EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsConsume = true, 
		int match = ITEM_MATCH_ALL);
	// 从容器中删除指定物品的数量, 全部删除返回true, 否则返回false result返回删除数量
	static bool RemoveItems( IKernel* pKernel, const PERSISTID& container, 
		const std::map<std::string, int>& items, std::map<std::string, int>* pResult /*= NULL*/, 
		EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsConsume = true,
		int match = ITEM_MATCH_ALL);

	// 从容器中删除指定物品的数量, 返回删除的数量 优先移除绑定的，性能比RemoveItems要差
	static int RemoveItemsBindPrior(IKernel* pKernel, const PERSISTID& container,
		const char* configid, int count, 
		EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsConsume = true, 
		int match = ITEM_MATCH_ALL);
	// 从容器中删除指定物品的数量, 全部删除返回true, 否则返回false result返回删除数量 优先移除绑定的，性能比RemoveItems要差
	static bool RemoveItemsBindPrior(IKernel* pKernel, const PERSISTID& container,
		const std::map<std::string, int>& items, std::map<std::string, int>* pResult = NULL, 
		EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsConsume = true,
		int match = ITEM_MATCH_ALL);
public:
	// 清空容器，必须指定到分类背包，不能使用ToolBox
	static bool ClearAllItems(IKernel* pKernel, const PERSISTID& self, const PERSISTID& container);

	//加载物品所属背包规则
	bool LoadBoxRule(IKernel* pKernel);

	//物品所属背包的Script
	static const char* GetBoxScript(IKernel* pKernel, const char* itemConfig);

	//物品所属背包
	static PERSISTID GetBoxContainer(IKernel* pKernel, const PERSISTID& self, const char* itemConfig);

	//////////////////////////////////////////////////////////////////////////
	// 排序整理接口
	//////////////////////////////////////////////////////////////////////////

	//整理容器中的物品, @cooldown_ms 冷却时间(毫秒)
	static bool ArrangeItem(IKernel* pKernel, const PERSISTID& container, int cooldown_ms = 0);

	// 解析字符串型物品 已冒号和逗号分隔 例如:"物品id:数量,物品id:数量"
	static bool ParseItems(IKernel* pKernel, const char* strItems, VectorBoxTestItem& itemList, int num = 1, int nBindState = ITEM_NOT_BOUND);

	// 将物品列表按背包合并, 返回合并后的数量
	static int MergeItemData(IKernel* pKernel, const PERSISTID& defBox, const VectorBoxTestItem& itemList, BoxItemData& boxItemData);

	// 是否需要记录物品日志
	static bool IsNeedSaveItemLog(IKernel* pKernel, const char* szItemId, int nColorLv);
private:
    //回调函数
    static int OnNoAdd(IKernel* pKernel, const PERSISTID& container,
        const PERSISTID& sender, const IVarList& args);

    static int OnNoRemove(IKernel * pKernel, const PERSISTID & container, 
        const PERSISTID & sender, const IVarList & args);

    static int OnAfterAdd(IKernel * pKernel, const PERSISTID & container, 
        const PERSISTID & sender, const IVarList & args);

    static int OnRemove(IKernel * pKernel, const PERSISTID & container, 
        const PERSISTID & sender, const IVarList & args);

	//////////////////////////////////////////////////////////////////////////
	// 用于回调的接口
	//////////////////////////////////////////////////////////////////////////

	/// 来自玩家的消息
	static int OnClientMsg(IKernel* pKernel, const PERSISTID& owener, 
		const PERSISTID& sender, const IVarList& args);

	// 移动容器物品
	static int OnMoveItem(IKernel* pKernel, const PERSISTID& owener, 
		const PERSISTID& sender, const IVarList& args);

private:
    //将物品存放到容器中, 返回放入的数量
    static int PlaceItem(IKernel* pKernel, const PERSISTID& container, SBoxTestItem *request,
		EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true);

    // 把物品放入背包，返回放入的数量. 
    // @saved_items 里返回实际保存的对象和数量, 格式为:
    //      [CONFIG_ID][UNIQUE_ID][SAVED_AMOUNT]...[CONFIG_ID][UNIQUE_ID][SAVED_AMOUNT]
	static int PlaceItem(IKernel* pKernel, const PERSISTID& container,
						SBoxTestItem *request, CVarList &saved_items,
						EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true);


    //////////////////////////////////////////////////////////////////////////
    // 修改物品接口
    //////////////////////////////////////////////////////////////////////////       

private:
    // 增加物品的数量, 返回实际增加的数量
    static int IncItemAmount(IKernel* pKernel, const PERSISTID &item, int amount,
        bool trigger_event = true);

    // 删除物品的数量, 返回实际删除的数量
    static int DecItemAmount(IKernel* pKernel, const PERSISTID &item, int amount,
        bool trigger_event = true);

private:
    static int GetAllItems(IKernel* pKernel, const PERSISTID& container, VectorBoxTestItem *item_list);

    // 是否可以合并物品
    static bool CanMergeItem(IKernel* pKernel, const PERSISTID& srcitem, const PERSISTID& destitem);

    // 是否可以把@request 的物品合并到@dest_item 上
    static bool CanMergeItem(IKernel* pKernel, const PERSISTID& dest_item, const SBoxTestItem *request);

    // 是否可以合并两个物品
    static bool CanMergeItem(IKernel* pKernel, const SBoxTestItem *dest_item, const SBoxTestItem *request_item);

	// 判断self是否是容器的拥有者
	static bool SelfIsOwner(IKernel* pKernel, const PERSISTID& self, const PERSISTID& container);

private:
    // 尝试把@item放入容器@container中, 返回可以放入的数量
    // 此操作不会真正放入物品, 仅仅是计算可放入的数量
    static int TryAddItemAuto(IKernel* pKernel, const PERSISTID& container,
                               const PERSISTID& srcitem);

    // 尝试把@item放入容器@container中的指定位置@pos中, 返回可以放入的数量
    // 此操作不会真正放入物品, 仅仅是计算可放入的数量
    static int TryAddItemToPosition(IKernel* pKernel, const PERSISTID& container,
                                     const PERSISTID& srcitem, int pos);

    //自动存放到容器中, 返回放入的数量
    static int PlaceItemAuto(IKernel* pKernel, const PERSISTID& owener, const PERSISTID& container,
					const PERSISTID& item, EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true);

    //存在到容器的指定位置, 返回放入的数量
	static int PlaceItemToPosition(IKernel* pKernel, const PERSISTID& owener, const PERSISTID& container,
		const PERSISTID& item, int destpos, EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS, bool bIsGain = true);		

	// 从容器中删除可穿戴物品(装备、砭石)
	static int RemoveWearItem(IKernel* pKernel, const PERSISTID& player, const PERSISTID& item, EmFunctionEventId srcFunctionId = FUNCTION_EVENT_ID_SYS);
private:
	// 获得物品处理  (nColorLv 只有装备时才用)
	static void OnGainItem(IKernel* pKernel, const PERSISTID& player, const char* szItemId, int nNum, int nTatalNum, EmFunctionEventId src, int nColorLv);

	// 消耗物品处理	(nColorLv 只有装备时才用)
	static void OnLoseItem(IKernel* pKernel, const PERSISTID& player, const char* szItemId, int nNum, int nTatalNum, EmFunctionEventId src);

private:
    // 检查容器的操作标志位
    static bool CheckContainerOperFlag(IKernel* pKernel, const PERSISTID& container, int opType);

    // 判断是否是一个有效的位置(锁定的格子不是有效的位置)
    static bool IsValidPos(IKernel* pKernel, const PERSISTID& container, int pos);
       
    // 获取可用位置中，最大的那个
    static int GetMaxUsablePos(IKernel* pKernel, const PERSISTID& container);

	static void ReloadConfig(IKernel* pKernel);
public:

    static ContainerModule* m_pContainerModule;

public:
	static AsynCtrlModule* m_pAsynCtrlModule;
    static CoolDownModule* m_pCoolDownModule;
    static ItemBaseModule* m_pItemBaseModule;
    //static EquipmentModule* m_pEquipmentModule;

private:
	// itemtype值的区间
	struct ItemTypeLim 
	{
		int		nMinVal;		// 最小值
		int		nMaxVal;		// 最大值
		ItemTypeLim() : nMinVal(0),
						nMaxVal(0)
		{

		}
	};

    std::map<std::string, ItemTypeLim> m_mapBoxRule;

};

#endif // FSGAME_COMMON_CONTAINER_MODULE_H_
