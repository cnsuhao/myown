//--------------------------------------------------------------------
// 文件名:      StaticDataDefine.h
// 内  容:      静态数据类型定义
// 说  明:      本文件修改请同步修改 客户端lua文件Client\lua\share\static_data_type.lua
// 创建日期:    
// 创建人:      
//    :       
//--------------------------------------------------------------------
#ifndef _StaticDataDefine_H
#define _StaticDataDefine_H

enum eStaticDataType
{
    // 角色属性相关
	STATIC_DATA_LEVELUP_ADDPROP = 1,			/**< 等级成长属性 */
	STATIC_DATA_NPC_BASE_PACK,					/**< npc基础实力配置包 */
	STATIC_DATA_BUFF_BASE_PACK,					/**< BUFF属性包 */
	STATIC_DATA_EQUIP_ADD_PACK,					/**< 装备附加属性包 */
	STATIC_DATA_EQUIP_RANDINTERVAL_PACK,		/**< 装备附加随机数区间包 */
	STATIC_DATA_EQUIP_MODIFYADD_PACK,			/**< 装备附加属性修正包 */
	STATIC_DATA_EQUIP_BASE_PACK,				/**< 装备基础属性包 */
	STATIC_DATA_JEWEL_BASE_PACK,				/**< 宝石属性包 */
	STATIC_DATA_STRENGTHEN_PACK,				/**< 强化属性包 */
	STATIC_DATA_STRENGTHEN_SUIT_PACK,			/**< 强化套装属性包 */
	STATIC_DATA_RIDE_ATTRIBUTE_PACK,			/**< 坐骑属性包 */
	STATIC_DATA_WING_ATTRIBUTE_PACK,			/**< 翅膀属性包 */
	STATIC_DATA_JADE_ATTRIBUTE_PACK,			/**< 玉珏属性包 */
	STATIC_DATA_APPERANCE_PACK,					/**< 外观穿戴属性包 */

	//其他配置100-199

    STATIC_DATA_MAX,
};

// 全局常量段名
const char* const GLOBAL_CONST_SEC = "ConstVal";

#endif // _StaticDataDefine_H
