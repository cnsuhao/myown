#if !defined __ViewDefine_h__
#define __ViewDefine_h__

//视图编号,后面开发填充
enum   EViewDef
{
	VIEWPORT_EQUIP          = 1,    //已装备栏
	VIEWPORT_SKILL			= 7,	//技能
	VIEWPORT_BUFFER         = 8,    //BUFFER容器
	VIEWPORT_STRENGTHEN     = 9,	//装备强化栏
    VIEWPORT_TOOL_BOX       = 10,	//道具栏
    VIEWPORT_RIDE           = 11,   //坐骑
	VIEWPORT_JADE_BOX		= 12,   //玉珏容器
//     VIEWPORT_PET            = 12,   //已得宠物
// 	VIEWPORT_TEMP_BAG       = 13,   //临时背包
//     VIEWPORT_PET_SKILL      = 14,   //宠物技能
 	
// 	VIEWPORT_WING_BAG       = 16,   //翅膀背包
    VIEWPORT_ITEM_BOX       = 17,   //消耗类道具背包
	//VIEWPORT_MARKER		= 21,	//印记背包
};
#endif
