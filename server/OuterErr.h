//--------------------------------------------------------------------
// 文件名:		OuterErr.h
// 内  容:		通用服务器引擎，客户端和服务器通讯的错误代码
// 说  明:		
// 创建日期:	2008年9月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_OUTERERR_H
#define _SERVER_OUTERERR_H

// 错误码信息格式 [5位整型数]
// 第1位是子系统代码
// 第2位是子模块代码
// 第3位是类型(0-存储过程、1-程序错误、2-网络通信)
// 后两位为顺序号编码
// 1000为不可预知的错误

#define OUTER_RESULT_SUCCEED		1		// 结果成功
#define OUTER_RESULT_FAILED			0		// 结果失败

// 1000为不可预知的错误
#define OUTER_ERR_UNEXPECT			1000	// 非预期错误

// 游戏服务器相关错误码描述[2xxxx]

// 网络接口相关错误[20xxx]

#define MGS_ERR_VERSION				            20101	// 网络通讯协议版本错误
#define MGS_ERR_REPEAT_LOGIN		          20102	// 同一个帐号不能重复登陆
#define MGS_ERR_LAND_SCENE			          20103	// 要进入的场景未开放或已关闭
#define MGS_ERR_NO_CONNECT			          20104	// 连接数满
#define MGS_ERR_BLOCK_MOMENT		          20105	// 在一段时间内禁止登陆
#define MGS_ERR_TIME_OUT			            20106	// 登录超时
#define MGS_ERR_QUEUE_FULL			          20107	// 登录队列满
#define MGS_ERR_STORAGE_FAILED		        20108	// 服务器与角色数据库的连接故障
#define MGS_ERR_CHAREGE_FAILED		        20109	// 服务器与计费服务器的连接故障
#define MGS_ERR_SWITCH_SCENE		          20110	// 要切换的场景不存在
#define MGS_ERR_ACCOUNT_LOGGED		        20111	// 此帐号已经登陆此游戏服务器
#define MGS_ERR_ACCOUNT_INVALID		        20112	// 帐号密码中包含非法字符
#define MGS_ERR_CHARGE_BREAK		          20113	// 因费用不足已经被系统踢出
#define MGS_ERR_PLAYER_FULL			          20114	// 承载人数已经达到服务器上限
#define MGS_ERR_NO_ANY_SCENE		          20115	// 没有可使用的场景
#define MGS_ERR_KICK_OUT			            20116	// 被系统踢下线,被其他玩家踢下线
#define MGS_ERR_IN_PROCESSING		          20117	// 正在进行处理中
#define MGS_ERR_CANT_PROCESS		          20118	// 无法处理请求
#define MGS_ERR_MESSAGE_ILLEGAL		        20119	// 涉嫌使用作弊程序向服务器发送非法消息
#define MGS_ERR_ACCOUNT_IN_GAME		        20120	// 此帐号已经进入游戏
#define MGS_ERR_CANT_REPLACE		          20121	// 使用密保卡登录的不能被其他不同IP顶替
#define MGS_ERR_SUPERVISOR_KICK_OUT	      20122	// 被管理员踢下线
#define MGS_ERR_SUPERVISOR_LOGGED	        20123	// 登录类型为管理员，不能被踢下线
#define MGS_ERR_DENY_ENTRY_SCENE	        20124	// 当前服务器不允许进入场景
#define MGS_ERR_CAN_NOT_RECONNECT         20125   // 该账号无法使用计费验证串自动重连
#define MSG_ERR_ROOM				              20126	// 房间异常
#define MSG_ERR_TRY_LOGIN			            20127	// 请稍等重新登录服务器（等待战斗结果）



// 存储接口相关错误[21xxx]

#define MGS_ERR_ROLE_DUPLICATE		        21101	// 人物重名
#define MGS_ERR_ROLE_PROTECT		          21102	// 人物在保护期内，不可删除
#define MGS_ERR_ROLE_SAVE_FAIL		        21103	// 人物存盘失败
#define MGS_ERR_ROLE_DATA			            21104	// 人物数据错误
#define MGS_ERR_ROLE_TOO_MANY		          21105	// 超过总的人物数量限制
#define MGS_ERR_ROLE_INDEX			          21106	// 人物位置错误
#define MGS_ERR_ROLE_NOT_FOUND		        21107	// 人物没找到
#define MGS_ERR_NAME_REPEAT			          21108	// 名字重复
#define MGS_ERR_NAME_INVALID		          21109	// 名字包含非法字符
#define MGS_ERR_NAME_EXISTS			          21110	// 此名称已存在
#define MGS_ERR_STORAGE_BUSY		          21111	// 数据库忙，无法处理请求
#define MGS_ERR_CHARGE_BUSY			          21112	// 计费系统忙，无法处理请求
#define MGS_ERR_LETTER_TOO_MANY		        21113	// 信件数量超过上限
#define MGS_ERR_NO_ROLE_UID			          21114	// 创建人物时编号资源不足
#define MGS_ERR_SWITCH_NO_SERVER	        21115	// 人物转服时服务器不存在
#define MGS_ERR_SWITCH_CAN_NOT		        21116	// 此服务器不能执行人物转服
#define MGS_ERR_ROLE_TOO_BIG		          21117	// 人物存盘数据超过允许的上限
#define MGS_ERR_STATUS_CHANGED		        21118	// 转服时目标服务器的状态发生改变
#define MGS_ERR_SET_LAND_SCENE		        21119	// 新建人物时未设置登陆场景
#define MGS_ERR_NEW_ROLE_FAILED		        21120	// 新建人物时保存数据失败
#define MGS_ERR_ROLE_BLOCKED		          21121	// 人物数据在一段时间内被封闭
#define MGS_ERR_LOGON_FAILED		          21122	// 计费登陆失败
#define MGS_ERR_LOAD_ROLE_FAILED	        21123	// 加载人物数据失败
#define MGS_ERR_GET_ROLES_FAILED	        21124	// 获取角色列表失败
#define MGS_ERR_CREATE_ROLE_FAILED	      21125	// 创建人物数据失败
#define MGS_ERR_DELETE_ROLE_FAILED	      21126	// 删除人物数据失败
#define MGS_ERR_RECEIVER_NOT_FOUND	      21127	// 收信人不存在
#define MGS_ERR_ROLE_IN_USE			          21128	// 角色正在使用中
#define MGS_ERR_VALIDATE_FAILED		        21129	// 验证帐号失败
#define MGS_ERR_CONTINUE_FAILED		        21130	// 顶替登录失败
#define MGS_ERR_ROLE_LOCKED			          21131	// 角色被暂时锁定
#define MGS_ERR_DENY_CREATE_ROLE	        21132	// 禁止创建新人物
#define MGS_ERR_ACC_NOT_IS_OWNER	        21133	// 角色不属于该账号
#define MGS_ERR_CROSS_STATUS_ERROR	      21134	// 角色跨区状态错误
#define MGS_ERR_CROSSING			            21135	// 角色数据跨服中不能登录
#define MGS_ERR_CROSS_NO_SERVER		        21136	// 角色跨服时目标服务器不存在
#define MGS_ERR_ROLE_DATA_ERROR		21137	// 角色数据异常
#define MSG_ERR_CHOOSE_OTHER_OS_ROLE			 21138	// 该角色绑定的平台错误，无法登陆！
#define MGS_ERR_CONTINUE_OTHER_OS_ROLE		        21139	// 其他平台角色不可顶替

// 战斗房间相关错误[22xxx]

#define MGS_ERR_ROOM_INNER					      22000	// 服务器内部错误
#define MGS_ERR_ROOM_CLOSE					      22001	// 房间服务器已经关闭
#define MSG_ERR_PLAYER_NOT_FOUNND			    22002	// 玩家不存在
#define MSG_ERR_PLATFORM_CLOSE				    22003	// 平台服务器已经关闭
#define MSG_ERR_BATTLE_OVER					      22004	// 战场已经结束
#define MSG_ERR_WORLD_REGISTED				    22005	// world id 已经被注册

// 计费应用相关错误码描述

#define CAS_ERR_CONNECT_TIMEOUT						1000	 //连接超时,请稍后再试
#define CAS_ERR_OPERATE_ERR								10002	 //操作异常
#define CAS_ERR_PARAM_NOT_ENOUGH					1001	 //缺少参数或参数不正确
#define CAS_ERR_TABLE_NOT_EXIST						1002	 //表或视图不存在
#define CAS_ERR_JOINT_OPERATION_LOGIN_URL_FAILURE						1003	 //联合运营串登录 URL串时间超期失效
#define CAS_ERR_JOINT_OPERATION_LOGIN_STRING_FAILURE        1004	 //联合运营串登录 校验串不正确
#define CAS_ERR_SQL_PROCEDURE_ERR					1005	 //数据库过程调用失败

// 服务器注册、注销 50xxx
#define CAS_ERR_VALIDATE_FAILED           50000	//帐号验证失败，请稍后再试 (需要登陆器主动调用注册接口完成注册过程)
#define CAS_ERR_SERVER_ID			            50001	// 游戏服务器ID不可识别
#define CAS_ERR_SERVER_NOT_EXIST          50002	// 游戏服务器不存在或已禁用
#define CAS_ERR_SERVER_FORBID		          50003	// 游戏服务器状态为已禁用
#define CAS_ERR_SERVER_IP			            50004	// 游戏服务器注册IP不正确
//#define CAS_ERR_GAME_ID				            50004	// 游戏类型ID不正确

// 玩家帐号登录、扣点、退出 51xxx
#define CAS_ERR_NO_ACCOUNT			          51001	// 蜗牛通行证不存在
#define CAS_ERR_ACCOUNT_PSWD_NOT_MATCH    51002 //您的帐号与此服务器运营商不匹配，请到会员中心查询或咨询客服
#define CAS_ERR_ACCOUNT_BLOCK		          51003	// 此帐号已被冻结
#define CAS_ERR_ACCOUNT_ONLY		          51004	// 帐号为锁定状态，请联系会员中心解锁后再试
#define	CAS_ERR_ILLEGAL_GM			          51005	// 非法GM帐号登录
#define CAS_ERR_ACCOUNT_PSWD		          51006	// 登录帐号或密码不正确

#define CAS_ERR_DYNAMIC_PSWD_PROTECT      51007	// 此帐号受动态口令服务保护，请选择动态口令登录方式
#define	CAS_ERR_ACCOUNT_NOT_DYNAMIC_PSWD	51008	// 您的帐号并未绑定动态口令卡或尚未开通服务，请选择普通方式登录
#define CAS_ERR_GAME_DISTRICT_NOT_EXIST   51009	// 此游戏分区不存在
#define CAS_ERR_GAME_NOT_EXIST            51010	// 此游戏不存在或已禁用

//#define CAS_ERR_GAME_DISTRICT_NOT_ACTiVATE	51011	// 此游戏分区未被激活
#define CAS_ERR_ACCOUNT_LOGGED		51011	// 此帐号已在分区内其他服务器登录

#define CAS_ERR_GAME_NOT_ACTIVATE		      51012	// 此游戏未被激活
#define CAS_ERR_ACCOUNT_DISTRICT_NOT_ACTiVATE   51013	// 此游戏分区帐户尚未激活(此用户在这个分区没有充过值)配),请到会员中心查看自己能登录哪些游戏服务器.
#define CAS_ERR_ACCOUNT_DISTRICT_LOGGED   51014 //此帐号已在游戏分区内其他服务器登录
#define CAS_ERR_DYNAMIC_PSWD_ERROR				51015	//动态口令错误，禁止登录
#define CAS_ERR_ACCOUNT_DYNAMIC_PSWD_OVERDUE	51016	//您的帐号绑定的动态口令卡已过期，请解除绑定或换卡后再登录
#define CAS_ERR_GAME_DISTRICT_NO_POINTS 	51017	//此游戏分区帐户余额不足
#define CAS_ERR_ACCOUNT_UPDATE_ERROR			51018	//更新分区账户信息失败
#define CAS_ERR_ADD_SUBSITE_LOGIN_STAT_ERR  		51019	//新增游戏分站帐号登录统计信息失败
#define CAS_ERR_SUBSITE_LOGIN_STAT_ERR 		51020	//记录游戏分站帐号登录统计信息失败
#define CAS_ERR_CREATE_LOGIN_LOG_ERR  	  51021	//创建登录日志失败
#define CAS_ERR_QUERY_ITEM_LOG_ERR			  51022	//查询游戏道具信息失败[虚拟道具编码不存在]
#define CAS_ERR_ITEM_PRICE_NOT_MATCH			51023	//游戏道具价格不匹配[物品点数单价发生变化,请刷新]
#define CAS_ERR_BUY_NUM_ERROR        			51024	//购买数量或单价有误
#define CAS_ERR_ITEM_POINT_RATE_NOT_MATCH	51025	//游戏道具代点券使用比率不匹配
#define CAS_ERR_ACCOUNT_NOT_POINT		      51026	//账户余额不足[门户帐户余额不足]
#define CAS_ERR_ACCOUNT_GIVE_NOT_EXIST		51027	//所赠送的帐号不存在[未知帐号请求]
#define CAS_ERR_BUY_ITEM_LOG_ERROR				51028	//商城内购买道具日志记录失败
#define CAS_ERR_DEDUCT_POINT_ERROR				51029	//扣除账户余额失败[点卡可能之前已售出或已撤消]
#define CAS_ERR_CARD_LOG_INSERT_ERROR			51030	//寄售点卡记录插入失败
#define CAS_ERR_DISTRICT_ACCOUNT_RECEIPTS_ERR		51031	//分区账户进账失败
#define CAS_ERR_BUY_CARD_ERR		          51032	//购买寄售点卡失败
#define CAS_ERR_BUY_CARD_LOG_INSERT_ERR		51033	//插入游戏中点卡寄售购买错误日志记录失败
#define CAS_ERR_QUERY_LAST_LOGIN_ERR		  51034	//查询用户最后登录信息失败
#define CAS_ERR_QUERY_ACCOUNT_SECURITY_ERR	    51035	//查询帐号安全信息失败
#define CAS_ERR_QUERY_SERVICE_SUBMIT_ERR  51036	//查询客服提交问题失败
#define CAS_ERR_QUERY_ACCOUNT_BASE_INFO_ERR	    51037	//查询通行证基本信息失败
#define CAS_ERR_QUERY_ELEC_CARD_INFO_ERR	51038	//查询最近获取电子口令卡信息失败
#define CAS_ERR_WEEL_GETED_ELEC_CARD			51039	//一周内已取过电子口令卡
#define CAS_ERR_NO_AVAILABLE_ELEC_CARD		51040	//没有可用的电子口令卡
#define CAS_ERR_GET_ELEC_CARD_ERR     		51041	//获取电子口令卡失败
#define CAS_ERR_QUERY_VIP_LEVEL_ERR				51042	//查询VIP级别失败,无记录
#define CAS_ERR_RECORD_SUBSITE_LOGIN_LOG_ERR	  51043	//记录游戏分站帐号登录日志失败
#define CAS_ERR_UPDATE_SUBSITE_LOGIN_LOG_ERR 		51044	//更新游戏分站帐号登录日志失败
#define CAS_ERR_CARD_NOT_FILL							51045	//口令卡号未填写
#define CAS_ERR_QUERY_ACCOUNT_DISTRICT_ACTIVATE_ERR 51046	//查询此用户分区激活信息失败
#define CAS_ERR_QUERY_ACCOUNT_ACTIVATE_ERR			51047	//查询此用户游戏激活信息失败
#define CAS_ERR_QUERY_ACCOUNT_DISTRICT_ERR			51048	//分区账户查询失败
#define CAS_ERR_QUERY_UNREAD_PEPLY_ERR	  51049	//查询用户未读回复信息失败
#define CAS_ERR_BUY_ITEM_GET_EXECUTE_ERR	51050	//用户游戏内购买物品获取回执执行失败
#define CAS_ERR_QUERY_BUY_ITEM_INFO_ERR		51051	//查询用户网上商城购买物品或赠品信息失败
#define CAS_ERR_GET_NUM_LESSTHAN_ZERO		  51052	//用户获取数量小于0
#define CAS_ERR_GET_BUY_ITEM_INFO_ERR			51053	//获取玩家网上商城购买物品或赠品信息失败
#define CAS_ERR_RECV_ITEM_UPDATE_ERR			51054	//用户收到物品的回执更新失败
#define CAS_ERR_SUPER_PWSD_LOGIN_LON_ERR	51055	//记录超级密码登录日志失败
#define CAS_ERR_GAME_SERVER_HISTTORY_ONLINE_NUM_ERR	51056	//记录游戏服务器历史在线人数失败
#define CAS_ERR_CONSIGNMENT_CARD_UNDO    	51057	//寄售点卡已撤销
#define CAS_ERR_CONSIGNMENT_CARD_SOLD			51058	//寄售点卡已售出
#define CAS_ERR_CONSIGNMENT_CARD_INFO_ERR	51059	//寄售点卡信息有误
#define CAS_ERR_CONSIGNMENT_CARD_UNDO_2 	51060	//点卡寄售两小时内不可撤消
#define CAS_ERR_NOT_BUY_SELF_CARD		      51061	//寄售人不能购买自己寄售的点卡
#define CAS_ERR_ONLINE_NUM_STAT_ERR	      51062	//游戏服务器在线人数统计失败
#define CAS_ERR_CONSIGNMENT_CARD_POINT_NOT_100 			51063	//寄售点数不能少于100点
#define CAS_ERR_CONSIGNMENT_ACCOUNT_NOT_EXIT				51064	//寄售人帐号不存在
#define CAS_ERR_CONSIGNMENT_ITEM_NOT_EXIT	51065	//寄售的道具不存在
#define CAS_ERR_CONSIGNMENT_ITEM_SOLD   	51066	//寄售的道具已售出
#define CAS_ERR_UPDATE_LOGIN_LOG_MAC_ERR	51067	//更新游戏登录日志MAC地址失败
#define CAS_ERR_CURR_ONLINE_NOT_EXIST   	51068	//当前在线记录不存在
#define CAS_ERR_ANTI_ADDICTION_UNABLE_GAME          51069	//尊敬的用户，您属于防沉迷帐号，现在是不健康游戏时间，请您休息满5小时后方可登陆游戏！
#define CAS_ERR_EXCHANGE_RATE_ERROR				51070	//兑换比例不正确(1点只能兑换4文金子或银票)
#define CAS_ERR_EXCHANGE_POINT_TYPE_ERROR	51071	//兑换游戏点数类型不正确
#define CAS_ERR_ACOUNT_IN_SIM_PROTECT			51072	//此帐号受SIM卡密宝服务保护，请选择SIM卡方式登录
#define CAS_ERR_ACOUNT_AND_SIM_NOT_BINDE	51073	//您的帐号并未绑定SIM卡或尚未开通服务，请选择普通方式登录
#define CAS_ERR_SIM_DYNAMIC_PSWD_ERROR  	51074	 //SIM卡动态口令不正确
#define CAS_ERR_ITEM_SOLD_OUT             51075	//柜面道具已下架
#define CAS_ERR_ACTIVITY_NOT_STARTED  		51076	//活动未开始
#define CAS_ERR_ACTIVITY_END							51077	//活动已结束
#define CAS_ERR_TOTAL_NUM_UPPER_LIMIT			51078	//总数量限制上限
#define CAS_ERR_SINGLE_ACCOUNT_UPPER_LIMIT	  	51079	//单帐号限量上限
#define CAS_ERR_CREATE_ACCOUNT_ERROR			51080	//创建失败, 游戏角色名/军团名已被使用
#define CAS_ERR_EDITE_ACCOUNT_ERROR			  51081	//修改失败, 游戏角色名/军团名不存在
#define CAS_ERR_DELETE_ACCOUNT_ERROR			51082	//删除失败, 游戏角色名/军团名不存在
#define CAS_ERR_ORDER_NUMBER_DISPOSED   	51083	//此订单号已处理过(订单号:引擎生成的唯一标识)
#define CAS_ERR_INTERFACE_STOP_USED 			51084	//此接口已停止使用，请更新到新版本
#define CAS_ERR_EXCHANGE_POINT_ERROR			51085	//兑换游戏点数失败
#define CAS_ERR_DISTRICT_TEST_NOT_BELONG	51086	//对不起,您不属于本次的测试区域,无法进行游戏登陆
#define CAS_ERR_LOGIN_LIMITED 						51087	//登录受限,请联系客服处理
#define CAS_ERR_ACCOUNT_TOKEN_PROTECT_OVERDUE			51088	//您的帐号绑定的令牌密保已过期，请解除绑定或更换后再登录
#define CAS_ERR_ACCOUNT_TOKEN_NEED_VERIFY	51089	//您的帐号绑定的令牌密保需要效验，请到会员中心操作
#define CAS_ERR_TOKEN_PWSD_ERROR					51090	//您的令牌密保错误，禁止登录
#define CAS_ERR_ACCOUNT_TOKEN_PROTECT			51091	//此帐号受令牌密保服务保护，请选择令牌密保方式登录
#define CAS_ERR_ACCOUNT_TRANSFER_LOCK			51092	//帐号为转服锁定状态，请稍后再试
#define CAS_ERR_GAME_PERMISSION_NOT_ACTIVATE			51093	//您在此服务器的游戏权限尚未激活,请至jhm页面进行激活
#define CAS_ERR_ACCOUNT_PHONE_TOKEN_PROTECT			  51094	//此帐号受手机令牌密保服务保护，请选择手机令牌密保方式登录
#define CAS_ERR_ACCOUNT_PHONE_TOKEN_PROTECT_OVERDUE			51095	//您的帐号绑定的手机令牌密保已过期，请解除绑定或更换后再登录
#define CAS_ERR_ACCOUNT_PHONE_TOKEN_NEED_VERIFY   51096	//您的帐号绑定的手机令牌密保需要效验，请到会员中心操作
#define CAS_ERR_PHONE_TOKEN_PWSD_ERROR		51097	//您的手机令牌密保错误，禁止登录
#define CAS_ERR_MONTH_CARD_OPERATE_ERROR  51098	//操作失败，精修明俊时间大于江湖明俊时间
#define CAS_ERR_SSO_VERIFY_ERROR        	51101	//SSO 验证失败，请重新登录(串失效，重新刷新)
#define CAS_ERR_ACCOUNT_NOT_SECURITY			51990	//您的账户不安全， 建议去XXXX修改密码
#define CAS_ERR_ACCOUNT_PARENTS_MONITOR		51999	//您的帐号已纳入家长监控，账号被封停

#define APP_CLIENT_VERSION_TOOLOWER			90001	// 客户端版本过低
#endif // _SERVER_OUTERERR_H

