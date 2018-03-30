#ifndef FSGAME_DEFINE_TOOL_BOX_DEFINE_H
#define FSGAME_DEFINE_TOOL_BOX_DEFINE_H

// 背包操作
#define SYS_INFO_TOOL_BOX_ITEM_SOLD              "sys_info_tool_box_003" // 成功出售, 获得{@n}个铜币
#define SYS_INFO_TOOL_BOX_UNLOCK_SUCCESS         "sys_info_tool_box_004" // 成功解锁{@n}个个格子, 花费{@n}个元宝
#define SYS_INFO_TOOL_BOX_UNLOCK_FAILED_GOLD     "sys_info_tool_box_005" // 资金不足,解锁失败
#define SYS_INFO_TOOL_BOX_EXPAND_SUCCESS         "sys_info_tool_box_007" // 背包扩充成功, 获得{@n}个格子
#define SYS_INFO_TOOL_BOX_ARRANGE_FAILED         "sys_info_tool_box_008" // 背包整理失败
#define SYS_INFO_TOOL_BOX_ARRANGE_CD             "sys_info_tool_box_009" // 背包整理太频繁(整理冷却中)
#define SYS_INFO_TOOL_BOX_INSUFFICIENT           "sys_info_tool_box_010" // 背包空间不足
#define SYS_INFO_TOOL_BOX_SPACE_TIP              "sys_info_tool_box_012" // 背包低容量提醒，不足{@n}个格子
#define SYS_INFO_TOOL_BOX_CLEAR_TEMP              "sys_info_tool_box_013" // 请先清理临时背包
#define SYS_INFO_TOOL_BOX_FULL_AND_SEND_BYMAIL    "sys_info_tool_box_014"  // 背包空间不足, 物品已通过邮件发送

// 背包物品使用
#define SYS_INFO_TOOL_BOX_ITEM_CD                "sys_info_tool_box_020" // 物品冷却中
#define SYS_INFO_TOOL_BOX_COMBINE_FAILED         "sys_info_tool_box_021" // 物品合成失败
#define SYS_INFO_TOOL_BOX_COMBINE_SUCCESS        "sys_info_tool_box_022" // 物品合并成功
#define SYS_INFO_TOOL_BOX_INC_HP                 "sys_info_tool_box_023" // 获得{@n} HP
#define SYS_INFO_TOOL_BOX_KEY_NEED               "sys_info_tool_box_026" // 需要{@钥匙} {@n}个
#define SYS_INFO_TOOL_BOX_LEVEL_REQUIRED         "sys_info_tool_box_027" // 等级不足, 需要等级{@n}
#define SYS_INFO_TOOL_BOX_ITEM_GET               "sys_info_tool_box_028" // 获得{@物品}{@n}个
#define SYS_INFO_TOOL_BOX_TOTAL_PAY              "sys_info_tool_box_032" // 获得充值金额{@n}
#define SYS_INFO_TOOL_BOX_DEAD_NOT_USE           "sys_info_tool_box_034" // 死亡不可以使用道具

#define  STR_BAG_LESSEN_INFO                        "sys_bag_122010"//包裹拆卸成功，您的背包空间变小
#define  STR_ORG_ITEM_NOT_A_BAG                     "sys_bag_30036"//原物品不是一个包裹
#define  STR_CANT_REMOVE_FROM_BAG                   "sys_bag_30037"//小包裹中还有物品，不能移除
#define  STR_SPACE_INSUFFICIENT_UNINSTALL_FAIL      "sys_bag_123009"//空间不足，拆卸失败
#define  STR_DEPORTSPACE_INSUFFICENT_REPLACE_FAIL   "sys_depot_123007"//仓库空间不足，替换失败
#define  STR_TOOLSPACE_INSUFFICENT_REPLACE_FAIL     "sys_bag_122009"//空间不足，替换失败

#define  STR_CURCONTAINER_CANT_OPERATE          "sys_cont_125000" //当前容器不可操作
#define  STR_TARGETCONTAINER_CANT_OPERATE       "sys_cont_125001" //目标容器不可操作

#define  STR_ITEM_BINDED                        "sys_item_134013" //物品已经绑定
#define  STR_ITEM_CANT_MOVE                     "sys_item_125002" //物品不可移动
#define  STR_ITEM_LOCKED                        "sys_item_125003" //物品已经锁定
#define  STR_REPLACEDITEM_CANT_MOVE             "sys_item_125004" //被替换的物品不能被移动
#define  STR_SAME_EQUIP_CANT_MOVE               "sys_item_125005" //装备栏中已有的物品无法移入祭坛

#define  STR_DEPORT_FULL                        "sys_depot_123001" //仓库空间已满，无法存储更多物品


#define  STR_BAG_SPACE_INSUFFICIENT             "sys_bag_122006" //背包空间不足，无法获得更多物品


#define  STR_CONTAINER_SPACE_INSUFFICIENT       "sys_cont_125005" //容器空间不足
#define  STR_CONTAINER_NOT_REPLACE		        "sys_cont_125008" //该物品不能被替换


#define  STR_CANTSPLIT                          "sys_cont_123012" //不能拆分
#define  STR_ITEM_CANT_DELETE                   "sys_cont_125006" //物品不可以删除
#define  STR_ITEM_ARRANGE_TOO_FAST              "sys_cont_125007" //整理物品的频率太快，请稍等片刻
#define  STR_MAIL_ITEM_LEVEL_UNSUIT             "sys_bag_0013" //此物品不能邮寄
#define  STR_NEED_UNLOCK                        "sys_lock_0009" //请先解除安全锁
#define  STR_TOOL_CANT_MOVE_DEPOT               "sys_item_0078" //任务道具不能移入仓库
#define  STR_PART_ITEM_SHORT_CAPITAL			"sys_item_0079" // 资金不足,碎片合成失败
#define  STR_ITEM_VIP_LEVEL_LIMIT				"sys_item_0080" // VIP等级需要{@vip等级}级
#define  STR_PART_ITEM_SHORT					"sys_item_0081" // 碎片不足合成失败

#define STR_SYSINFO_ADD_SILVER					"sys_add_silver"			// 银元
#define STR_SYSINFO_ADD_COPPER					"sys_add_copper"			// 铜币
#define STR_SYSINFO_ADD_SMELT					"sys_add_smelt"				// 熔炼值
#define STR_SYSINFO_ADD_EXPLOIT					"sys_add_public_office"     // 功勋
#define STR_SYSINFO_ADD_SKILL_POINT				"sys_add_skill_point"       // 技能点
#define STR_SYSINFO_ADD_GUILD					"sys_add_guild"				// 帮会贡献
#define STR_SYSINFO_ADD_EXP						"sys_add_exp"				// 经验
#define STR_SYSINFO_ADD_BATTLE_SOUL				"sys_add_battle_soul"		// 战魂
#define STR_SYSINFO_ADD_HONOR					"sys_add_honor"				// 荣誉值
#define STR_SYSINFO_ADD_FEEDING					"sys_add_feeding"			// 饲养值
#define STR_SYSINFO_ADD_CHIVALRY				"sys_add_chivalry"			// 侠义值
#define STR_SYSINFO_ADD_TRADE_SILVER           "sys_add_trade_silver"		// 交易银元
#define STR_SYSINFO_ADD_CAMPVALUE				"sys_add_campvalue"			// 阵营荣誉值
#define STR_SYSINFO_ADD_GUILD_CAPITAL           "sys_add_guild_capital"     // 组织资金
#define STR_SYSINFO_ADD_FRIENDSHIP				"sys_add_friendship"		// 侠客情义值
#define STR_SYSINFO_ADD_WISH_SCORE				"sys_add_wish_score"		// 祝福积分



#define  SYS_REMOVE_BUFF_FAIL					"sys_remove_buff_fail" // 移除指定buff失败

#define  SYS_USE_MULTI_EXP_RATE_FAIL_EXISTS		"sys_use_multi_exp_rate_fail1" // 已存在多倍经验效果
#define  SYS_USE_MULTI_EXP_RATE_FAIL_TIME		"sys_use_multi_exp_rate_fail2" // 剩余时间过长

#define	 SYS_ADD_SKILL_FAILED_EXISTS			"sys_add_skill_failed"		// 已经学习此技能

#endif