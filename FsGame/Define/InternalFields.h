//-------------------------------------------------------------------- 
// 文件名:		InternalFields.h 
// 内  容:		引挚内部字段名定义 
// 说  明:		字段名称定义 
// 创建者:		lihl 
// 日  期:      2016/03/31		 
//-------------------------------------------------------------------- 
#ifndef _INTERNAL_FIELDS_DEFINE_H_ 
#define _INTERNAL_FIELDS_DEFINE_H_ 
 
// 场景的内部属性定义：
#ifndef FIELD_PROP_NAME
#define	FIELD_PROP_NAME				"Name"				// VTYPE_WIDESTR		// 名字
#endif 

#ifndef FIELD_PROP_MAXPLAYERS
#define	FIELD_PROP_MAXPLAYERS		"MaxPlayers"		// VTYPE_INT		// 最大玩家数量参考值
#endif 

#ifndef FIELD_PROP_CURPLAYERS
#define	FIELD_PROP_CURPLAYERS		"CurPlayers"		// VTYPE_INT		// 当前玩家数量
#endif 

#ifndef FIELD_PROP_MAXVISUALS
#define	FIELD_PROP_MAXVISUALS		"MaxVisuals"		// VTYPE_INT		// 最大视野内移动物体数量
#endif 

#ifndef FIELD_PROP_MAXLOOKERS
#define	FIELD_PROP_MAXLOOKERS		"MaxLookers"		// VTYPE_INT		// 0号分组中可视对象的最大观察者数量
#endif 

#ifndef FIELD_PROP_MAXLOOKERS
#define	FIELD_PROP_COLLIDEPATH		"CollidePath"		// VTYPE_STRING		// 场景碰撞信息文件路径
#endif

#ifndef FIELD_PROP_MAPEVENT
#define	FIELD_PROP_MAPEVENT			"MapEvent"			// VTYPE_INT		// 是否产生地图区域变化事件OnMapChanged
#endif

#ifndef FIELD_PROP_BORN
#define	FIELD_PROP_BORN				"Born"				// VTYPE_STRING		// 出生点名
#endif

#ifndef FIELD_PROP_GROUPING
#define	FIELD_PROP_GROUPING			"Grouping"			// VTYPE_INT		// 是否分组场景
#endif


#ifndef FIELD_PROP_WORLDINFO
#define	FIELD_PROP_WORLDINFO			"WorldInfo"			// VTYPE_WIDESTR		// 游戏世界信息
#endif

#ifndef FIELD_PROP_NOTSWITCHSTORE
#define	FIELD_PROP_NOTSWITCHSTORE	"NotSwitchStore"	// VTYPE_INT		// 不在切场景前保存玩家数据
#endif

#ifndef FIELD_PROP_NOTTIMINGSTORE
#define	FIELD_PROP_NOTTIMINGSTORE	"NotTimingStore"	// VTYPE_INT		// 不需要定时保存玩家数据
#endif

#ifndef FIELD_PROP_ISNIGHT
#define	FIELD_PROP_ISNIGHT			"IsNight"		   	// VTYPE_INT		// 
#endif

//玩家角色的内部属性定义：
#ifndef FIELD_PROP_NAME
#define	FIELD_PROP_NAME				"Name"				// VTYPE_WIDESTR		// 玩家名字
#endif

#ifndef FIELD_PROP_UID
#define	FIELD_PROP_UID				"Uid"				// VTYPE_STRING		// 角色唯一标识
#endif

#ifndef FIELD_PROP_ROLEINFO
#define	FIELD_PROP_ROLEINFO			"RoleInfo"			// VTYPE_WIDESTR		// 角色登录信息（角色数据表里的save_para1字段）
#endif

#ifndef FIELD_PROP_ACCOUNT
#define	FIELD_PROP_ACCOUNT			"Account"			// VTYPE_STRING		// 帐号
#endif

#ifndef FIELD_PROP_ACCOUNTID
#define	FIELD_PROP_ACCOUNTID			"AccountID"			// VTYPE_INT		// 帐号编号
#endif

#ifndef FIELD_PROP_ISSUERID
#define	FIELD_PROP_ISSUERID			"IssuerID"			// VTYPE_INT		// 运营商编号
#endif

#ifndef FIELD_PROP_LOGONID
#define	FIELD_PROP_LOGONID			"LogonID"			// VTYPE_STRING		// 本次登录标识符
#endif

#ifndef FIELD_PROP_CONNID
#define	FIELD_PROP_CONNID			"ConnId"			// VTYPE_INT		// 本次连接编号
#endif

#ifndef FIELD_PROP_TOTALSEC
#define	FIELD_PROP_TOTALSEC			"TotalSec"			// VTYPE_INT		// 角色总在线时间（秒）
#endif

#ifndef FIELD_PROP_ILLEGALS
#define	FIELD_PROP_ILLEGALS			"Illegals"			// VTYPE_INT		// 客户端作弊次数
#endif

#ifndef FIELD_PROP_ADDRESS
#define	FIELD_PROP_ADDRESS			"Address"			// VTYPE_STRING		// 本次连接IP地址
#endif

#ifndef FIELD_PROP_GAMEMASTER
#define	FIELD_PROP_GAMEMASTER		"GameMaster"		// VTYPE_INT		// GM权限（0为普通玩家）
#endif

#ifndef FIELD_PROP_VISUALRANGE
#define	FIELD_PROP_VISUALRANGE		"VisualRange"		// VTYPE_FLOAT		// 视野范围	
#endif

#ifndef FIELD_PROP_SPRINGRANGE
#define	FIELD_PROP_SPRINGRANGE		"SpringRange"		// VTYPE_FLOAT		// 触发范围
#endif

#ifndef FIELD_PROP_INVISIBLE
#define	FIELD_PROP_INVISIBLE			"Invisible"			// VTYPE_INT		// 是否不可见
#endif

#ifndef FIELD_PROP_NOCOLLIDE
#define	FIELD_PROP_NOCOLLIDE			"NoCollide"			// VTYPE_INT		// 是否无碰撞
#endif

#ifndef FIELD_PROP_THROUGH
#define	FIELD_PROP_THROUGH			"Through"			// VTYPE_INT		// 是否可以穿透其他物体
#endif

#ifndef FIELD_PROP_MAPFREE
#define	FIELD_PROP_MAPFREE			"MapFree"			// VTYPE_INT		// 是否不受可行走范围的约束
#endif

#ifndef FIELD_PROP_WALKCAP
#define	FIELD_PROP_WALKCAP			"WalkCap"			// VTYPE_INT		// 行走能力掩码
#endif

#ifndef FIELD_PROP_WALKSTEP
#define	FIELD_PROP_WALKSTEP			"WalkStep"			// VTYPE_FLOAT		// 行走高度差
#endif

#ifndef FIELD_PROP_DUMMY
#define	FIELD_PROP_DUMMY				"Dummy"				// VTYPE_INT		// 是否还不在场景容器中
#endif

#ifndef FIELD_PROP_COLLIDESHAPE
#define	FIELD_PROP_COLLIDESHAPE		"CollideShape"		// VTYPE_INT		// 碰撞外形
#endif

#ifndef FIELD_PROP_COLLIDEHEIGHT
#define	FIELD_PROP_COLLIDEHEIGHT		"CollideHeight"		// VTYPE_FLOAT		// 碰撞高度
#endif

#ifndef FIELD_PROP_COLLIDERADIUS
#define	FIELD_PROP_COLLIDERADIUS		"CollideRadius"		// VTYPE_FLOAT		// 碰撞半径
#endif

#ifndef FIELD_PROP_COLLIDEVERTEX
#define	FIELD_PROP_COLLIDEVERTEX		"CollideVertex"		// VTYPE_STRING		// 碰撞多边形顶点
#endif

#ifndef FIELD_PROP_GROUPID
#define	FIELD_PROP_GROUPID			"GroupID"			// VTYPE_INT		// 分组编号
#endif

#ifndef FIELD_PROP_NOSPEECH
#define	FIELD_PROP_NOSPEECH			"NoSpeech"			// VTYPE_INT		// 是否禁止普通聊天
#endif

#ifndef FIELD_PROP_ONLINE
#define	FIELD_PROP_ONLINE			"Online"			// VTYPE_INT		// 是否在线
#endif

#ifndef FIELD_PROP_ACCOUNTINFO
#define	FIELD_PROP_ACCOUNTINFO		"AccountInfo"		// VTYPE_STRING		// 帐号信息
#endif

#ifndef FIELD_PROP_CHARGEINFO
#define	FIELD_PROP_CHARGEINFO		"ChargeInfo"		// VTYPE_STRING		// 计费信息
#endif

#ifndef FIELD_PROP_LETTERMAX
#define	FIELD_PROP_LETTERMAX			"LetterMax"			// VTYPE_INT		// 邮箱容量
#endif

#ifndef FIELD_PROP_CREATETIME
#define	FIELD_PROP_CREATETIME		"CreateTime"		// VTYPE_DOUBLE		// 角色创建时间
#endif

#ifndef FIELD_PROP_LIMITTIME
#define	FIELD_PROP_LIMITTIME			"LimitTime"			// VTYPE_DOUBLE		// 月卡截止时间
#endif

#ifndef FIELD_PROP_VISUALPLAYERS
#define	FIELD_PROP_VISUALPLAYERS		"VisualPlayers"		// VTYPE_INT		// 视野内可见其他玩家数量上限
#endif

#ifndef FIELD_PROP_VISUALEVENT
#define	FIELD_PROP_VISUALEVENT		"VisualEvent"		// VTYPE_INT		// 对象进出视野时是否触发回调
#endif

#ifndef FIELD_PROP_LANDSCENE
#define	FIELD_PROP_LANDSCENE			"LandScene"			// VTYPE_INT		// 下次登录时上线的场景编号
#endif

#ifndef FIELD_PROP_LANDPOSIX
#define	FIELD_PROP_LANDPOSIX			"LandPosiX"			// VTYPE_DOUBLE		// 下次登录时上线的位置
#endif

#ifndef FIELD_PROP_LANDPOSIY
#define	FIELD_PROP_LANDPOSIY			"LandPosiY"			// VTYPE_DOUBLE		//
#endif

#ifndef FIELD_PROP_LANDPOSIZ
#define	FIELD_PROP_LANDPOSIZ			"LandPosiZ"			// VTYPE_DOUBLE		//
#endif

#ifndef FIELD_PROP_LANDORIENT
#define	FIELD_PROP_LANDORIENT		"LandOrient"		// VTYPE_DOUBLE		//
#endif

#ifndef FIELD_PROP_CHECKSYNCRANGE
#define	FIELD_PROP_CHECKSYNCRANGE	"CheckSyncRange"	// VTYPE_FLOAT		// 位置校验修正最小距离范围
#endif

#ifndef FIELD_PROP_CHECKSYNCBIAS
#define	FIELD_PROP_CHECKSYNCBIAS		"CheckSyncBias"		// VTYPE_FLOAT		// 位置校验修正偏移百分比
#endif

#ifndef FIELD_PROP_SERVERILLEGAL
#define	FIELD_PROP_SERVERILLEGAL		"ServerIllegal"		// VTYPE_INT		// 是否在属于非法服务器的用户
#endif

#ifndef FIELD_PROP_CHECKDISTTOTAL
#define	FIELD_PROP_CHECKDISTTOTAL	"CheckDistTotal"	// VTYPE_DOUBLE		// 统计总共的修正偏移
#endif

#ifndef FIELD_PROP_SERVERMONITOR
#define	FIELD_PROP_SERVERMONITOR		"ServerMonitor"		// VTYPE_INT64		// 服务内部监控值
#endif

#ifndef FIELD_PROP_TYPE
#define	FIELD_PROP_TYPE				"Type"				// VTYPE_INT		//玩家类型
#endif

#ifndef FIELD_PROP_CAPACITY
#define	FIELD_PROP_CAPACITY			"Capacity"			// VTYPE_INT		//玩家容量
#endif

#ifndef FIELD_PROP_CONTAINERPOS
#define	FIELD_PROP_CONTAINERPOS		"ContainerPos"		// VTYPE_INT		//玩家位置
#endif

#ifndef FIELD_PROP_SCRIPT
#define	FIELD_PROP_SCRIPT			"Script"			// VTYPE_STRING		//玩家脚本
#endif

#ifndef FIELD_PROP_CONFIG
#define	FIELD_PROP_CONFIG			"Config"			// VTYPE_STRING		//玩家的配置
#endif

#ifndef FIELD_PROP_RANDSEED
#define	FIELD_PROP_RANDSEED			"RandSeed"			// VTYPE_INT		// 自定义 数据验证使用
#endif

#ifndef FIELD_PROP_PATHGRID
#define	FIELD_PROP_PATHGRID			"PathGrid"			// VTYPE_INT		// 占用路径格子数量
#endif

#ifndef FIELD_PROP_PATHTHROUGH
#define	FIELD_PROP_PATHTHROUGH		"PathThrough"		// VTYPE_INT		// 可穿越格子上对象
#endif

//NPC的内部属性定义：
#ifndef FIELD_PROP_NAME
#define	FIELD_PROP_NAME				"Name"					// VTYPE_WIDESTR		// 名字
#endif

#ifndef FIELD_PROP_VISUALRANGE
#define	FIELD_PROP_VISUALRANGE		"VisualRange"			// VTYPE_FLOAT		// 视野范围
#endif

#ifndef FIELD_PROP_SPRINGRANGE
#define	FIELD_PROP_SPRINGRANGE		"SpringRange"			// VTYPE_FLOAT		// 触发范围
#endif

#ifndef FIELD_PROP_INVISIBLE
#define	FIELD_PROP_INVISIBLE			"Invisible"				// VTYPE_INT		// 是否不可见
#endif

#ifndef FIELD_PROP_NOCOLLIDE
#define	FIELD_PROP_NOCOLLIDE			"NoCollide"				// VTYPE_INT		// 是否无碰撞
#endif

#ifndef FIELD_PROP_NOSPRING
#define	FIELD_PROP_NOSPRING			"NoSpring"				// VTYPE_INT		// 是否无触发
#endif

#ifndef FIELD_PROP_SPRINGER
#define	FIELD_PROP_SPRINGER			"Springer"				// VTYPE_INT		// 是否可以触发其他物体
#endif

#ifndef FIELD_PROP_THROUGH
#define	FIELD_PROP_THROUGH			"Through"				// VTYPE_INT		// 是否可以穿透其他物体
#endif

#ifndef FIELD_PROP_MAPFREE
#define	FIELD_PROP_MAPFREE			"MapFree"				// VTYPE_INT		// 是否不受地图可行走范围的约束
#endif

#ifndef FIELD_PROP_WALKCAP
#define	FIELD_PROP_WALKCAP			"WalkCap"				// VTYPE_INT		// 行走能力掩码
#endif

#ifndef FIELD_PROP_WALKSTEP
#define	FIELD_PROP_WALKSTEP			"WalkStep"				// VTYPE_FLOAT		// 行走高度差
#endif

#ifndef FIELD_PROP_LIFETIME
#define	FIELD_PROP_LIFETIME			"LifeTime"				// VTYPE_INT		// 在场景里的生存时间（为0标识无限制）
#endif

#ifndef FIELD_PROP_COLLIDESHAPE
#define	FIELD_PROP_COLLIDESHAPE		"CollideShape"			// VTYPE_INT		// 碰撞外形
#endif

#ifndef FIELD_PROP_COLLIDEHEIGHT
#define	FIELD_PROP_COLLIDEHEIGHT		"CollideHeight"			// VTYPE_FLOAT		// 碰撞高度
#endif

#ifndef FIELD_PROP_COLLIDERADIUS
#define	FIELD_PROP_COLLIDERADIUS		"CollideRadius"			// VTYPE_FLOAT		// 碰撞半径
#endif

#ifndef FIELD_PROP_COLLIDEVERTEX
#define	FIELD_PROP_COLLIDEVERTEX		"CollideVertex"			// VTYPE_STRING		// 碰撞多边形顶点
#endif

#ifndef FIELD_PROP_FIXED
#define	FIELD_PROP_FIXED				"Fixed"					// VTYPE_INT		// 是否固定物体（可以存在于地图的不可行走区域）
#endif

#ifndef FIELD_PROP_GROUPID
#define	FIELD_PROP_GROUPID			"GroupID"				// VTYPE_INT		// 分组编号 
#endif

#ifndef FIELD_PROP_TYPE
#define	FIELD_PROP_TYPE				"Type"			// VTYPE_INT		//npc类型
#endif

#ifndef FIELD_PROP_CAPACITY
#define	FIELD_PROP_CAPACITY			"Capacity"		// VTYPE_INT		//npc容量
#endif

#ifndef FIELD_PROP_CONTAINERPOS
#define	FIELD_PROP_CONTAINERPOS		"ContainerPos	// VTYPE_INT		//npc位置
#endif

#ifndef FIELD_PROP_SCRIPT
#define	FIELD_PROP_SCRIPT			"Script"		// VTYPE_STRING		//npc脚本
#endif

#ifndef FIELD_PROP_CONFIG
#define	FIELD_PROP_CONFIG			"Config"		// VTYPE_STRING		//npc的配置
#endif

#ifndef FIELD_PROP_PATHGRID
#define	FIELD_PROP_PATHGRID			"PathGrid"		// VTYPE_INT		// 占用路径格子数量
#endif

#ifndef FIELD_PROP_PATHTHROUGH
#define	FIELD_PROP_PATHTHROUGH		"PathThrough"	// VTYPE_INT		// 可穿越格子上对象
#endif

// 物品的内部属性定义：
#ifndef FIELD_PROP_NAME
#define	FIELD_PROP_NAME				"Name"				//VTYPE_WIDESTR		// 名字
#endif

#ifndef FIELD_PROP_VISUALRANGE
#define	FIELD_PROP_VISUALRANGE		"VisualRange"		//VTYPE_FLOAT		// 视野范围
#endif

#ifndef FIELD_PROP_SPRINGRANGE
#define	FIELD_PROP_SPRINGRANGE		"SpringRange"		//VTYPE_FLOAT		// 触发范围
#endif

#ifndef FIELD_PROP_INVISIBLE
#define	FIELD_PROP_INVISIBLE			"Invisible"			//VTYPE_INT		// 是否不可见
#endif


#ifndef FIELD_PROP_NOCOLLIDE
#define	FIELD_PROP_NOCOLLIDE			"NoCollide"			//VTYPE_INT		// 是否无碰撞
#endif

#ifndef FIELD_PROP_NOSPRING
#define	FIELD_PROP_NOSPRING			"NoSpring"			//VTYPE_INT		// 是否无触发
#endif

#ifndef FIELD_PROP_LIFETIME
#define	FIELD_PROP_LIFETIME			"LifeTime"			//VTYPE_INT		// 在场景里的生存时间（为0标识无限制）
#endif

#ifndef FIELD_PROP_COLLIDEHEIGHT
#define	FIELD_PROP_COLLIDEHEIGHT		"CollideHeight"		//VTYPE_FLOAT		// 碰撞高度
#endif

#ifndef FIELD_PROP_COLLIDEVERTEX
#define	FIELD_PROP_COLLIDEVERTEX		"CollideVertex"		//VTYPE_STRING		// 碰撞多边形顶点
#endif

#ifndef FIELD_PROP_FIXED
#define	FIELD_PROP_FIXED				"Fixed"				//VTYPE_INT		// 是否固定物体（可以存在于地图的不可行走区域）
#endif

#ifndef FIELD_PROP_GROUPID
#define	FIELD_PROP_GROUPID			"GroupID"			//VTYPE_INT		// 分组编号
#endif

#ifndef FIELD_PROP_TYPE
#define	FIELD_PROP_TYPE				"Type"				// VTYPE_INT		//物品类型
#endif

#ifndef FIELD_PROP_CONTAINERPOS
#define	FIELD_PROP_CONTAINERPOS		"ContainerPos"		// VTYPE_INT		//物品位置
#endif

#ifndef FIELD_PROP_SCRIPT
#define	FIELD_PROP_SCRIPT			"Script"			// VTYPE_STRING		//物品脚本
#endif

#ifndef FIELD_PROP_CONFIG
#define	FIELD_PROP_CONFIG			"Config"			// VTYPE_STRING		//物品的配置
#endif

#ifndef FIELD_PROP_PATHGRID
#define	FIELD_PROP_PATHGRID			"PathGrid"			// VTYPE_INT		// 占用路径格子数量
#endif

#ifndef FIELD_PROP_PATHTHROUGH
#define	FIELD_PROP_PATHTHROUGH		"PathThrough"		// VTYPE_INT		// 可穿越格子上对象
#endif


//游戏辅助对象的内部属性定义：
#ifndef FIELD_PROP_NAME
#define	FIELD_PROP_NAME				"Name"		// VTYPE_WIDESTR	        // 名字
#endif

#ifndef FIELD_PROP_GROUPID
#define	FIELD_PROP_GROUPID			"GroupID"	// VTYPE_INT		// 分组编号
#endif

//弱关联容器的内部属性定义：
#ifndef FIELD_PROP_NAME
#define	FIELD_PROP_NAME				"Name"		// VTYPE_WIDESTR		// 名字
#endif

#ifndef FIELD_PROP_GROUPID
#define	FIELD_PROP_GROUPID			"GroupID"	// VTYPE_INT		// 分组编号
#endif

#ifndef FIELD_PROP_TYPE
#define	FIELD_PROP_TYPE				"Type"				// VTYPE_INT		//npc类型
#endif

#ifndef FIELD_PROP_CAPACITY
#define	FIELD_PROP_CAPACITY			"Capacity"			// VTYPE_INT		//npc容量
#endif

#ifndef FIELD_PROP_CONTAINERPOS
#define	FIELD_PROP_CONTAINERPOS		"ContainerPos"		// VTYPE_INT		//npc位置
#endif

#ifndef FIELD_PROP_SCRIPT
#define	FIELD_PROP_SCRIPT			"Script"			// VTYPE_STRING		//npc脚本
#endif

#ifndef FIELD_PROP_CONFIG
#define	FIELD_PROP_CONFIG			"Config"			// VTYPE_STRING		//npc的配置
#endif

 
#endif // _FIELDS_DEFINE_H_