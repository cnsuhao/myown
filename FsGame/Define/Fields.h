//-------------------------------------------------------------------- 
// ÎÄ¼þÃû:		Fields.h 
// ÄÚ  ÈÝ:		×Ö¶ÎÃû¶¨Òå 
// Ëµ  Ã÷:		×Ö¶ÎÃû³Æ¶¨Òå 
// ´´½¨Õß:		 
// ÈÕ  ÆÚ:       
//-------------------------------------------------------------------- 
#ifndef _FIELDS_DEFINE_H_ 
#define _FIELDS_DEFINE_H_ 
 
// ×Ö¶Î¶¨Òå=============================================================== 
 
//<property name="PathFinding"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÇ·ñÑ°Â·ÖÐ"/> 
#define FIELD_PROP_PATH_FINDING			"PathFinding" 
 
//<property name="FindPathX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°Ä¿±êµãX"/> 
#define FIELD_PROP_FIND_PATH_X			"FindPathX" 
 
//<property name="FindPathZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°Ä¿±êµãZ"/> 
#define FIELD_PROP_FIND_PATH_Z			"FindPathZ" 
 
//<property name="DestPathX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×îÖÕÄ¿±êX"/> 
#define FIELD_PROP_DEST_PATH_X			"DestPathX" 
 
//<property name="DestPathZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×îÖÕÄ¿±êZ"/> 
#define FIELD_PROP_DEST_PATH_Z			"DestPathZ" 
 
//<property name="LittleStepX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ëé²½ÒÆ¶¯Ä¿±êX"/> 
#define FIELD_PROP_LITTLE_STEP_X			"LittleStepX" 
 
//<property name="LittleStepZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ëé²½ÒÆ¶¯Ä¿±êZ"/> 
#define FIELD_PROP_LITTLE_STEP_Z			"LittleStepZ" 
 
//<property name="LastFindPath"  
// 	type="Int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×îÖÕÑ°Â·"/> 
#define FIELD_PROP_LAST_FIND_PATH			"LastFindPath" 
 
//<property name="RetryPrePath"  
// 	type="Int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ö¸¶¨´ÎÊýÇ°ÖØÊÔ£¬Ö®ºóÆ«ÒÆ¶¯Ì¬ÕÒÂ·"/> 
#define FIELD_PROP_RETRY_PRE_PATH			"RetryPrePath" 
 
//<property name="PathFindStep"  
// 	type="Int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc=""/> 
#define FIELD_PROP_PATH_FIND_STEP			"PathFindStep" 
 
//<property name="ContinueMove"  
// 	type="Int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Á¬ÐøËé²½±êÖ¾,0·ñ1ÊÇ"/> 
#define FIELD_PROP_CONTINUE_MOVE			"ContinueMove" 
 
//<property name="TargetX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ä¿±êµãX"/> 
#define FIELD_PROP_TARGET_X			"TargetX" 
 
//<property name="TargetZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ä¿±êµãZ"/> 
#define FIELD_PROP_TARGET_Z			"TargetZ" 
 
//<property name="NearTargetGrid"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ä¿±ê¸½½üµÄµã"/> 
#define FIELD_PROP_NEAR_TARGET_GRID			"NearTargetGrid" 
 
//<property name="MotionState"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ñ°Â·ÒÆ¶¯×´Ì¬"/> 
#define FIELD_PROP_MOTION_STATE			"MotionState" 
 
//<property name="RoundGrid"  
// 	type="Int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="±ÜÈÃÊ±ËùÔÚ¸ñ×Ó"/> 
#define FIELD_PROP_ROUND_GRID			"RoundGrid" 
 
//<property name="PathDistanceMin"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÀëÄ¿±ê×îÐ¡¾àÀë"/> 
#define FIELD_PROP_PATH_DISTANCE_MIN			"PathDistanceMin" 
 
//<property name="PathDistanceMax"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÀëÄ¿±ê×î´ó¾àÀë"/> 
#define FIELD_PROP_PATH_DISTANCE_MAX			"PathDistanceMax" 
 
//<property name="MotionResult"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÒÆ¶¯½á¹û"/> 
#define FIELD_PROP_MOTION_RESULT			"MotionResult" 
 
//<property name="WaitTarget"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µÈ´ý¶ÔÏó×ß¿ª"/> 
#define FIELD_PROP_WAIT_TARGET			"WaitTarget" 
 
//<property name="WaitTargetX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µÈ´ý¶ÔÏó×ß¿ªµÄ×ø±êµãX"/> 
#define FIELD_PROP_WAIT_TARGET_X			"WaitTargetX" 
 
//<property name="WaitTargetZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µÈ´ý¶ÔÏó×ß¿ªµÄ×ø±êµãZ"/> 
#define FIELD_PROP_WAIT_TARGET_Z			"WaitTargetZ" 
 
//<property name="RoundTotalCount"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="±ÜÈÃµÄ×Ü´ÎÊý"/> 
#define FIELD_PROP_ROUND_TOTAL_COUNT			"RoundTotalCount" 
 
//<property name="SpaceGrid"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼ä¸ô¸ñÊý"/> 
#define FIELD_PROP_SPACE_GRID			"SpaceGrid" 
 
//<property name="CallBackFunction"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="»Øµ÷º¯Êý"/> 
#define FIELD_PROP_CALL_BACK_FUNCTION			"CallBackFunction" 
 
//<property name="MotionMethod"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ñ°Â··½Ê½"/> 
#define FIELD_PROP_MOTION_METHOD			"MotionMethod" 
 
//<property name="SelfLastX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×ÔÉíÉÏ´Î×ø±êµãX"/> 
#define FIELD_PROP_SELF_LAST_X			"SelfLastX" 
 
//<property name="SelfLastZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×ÔÉíÉÏ´Î×ø±êµãZ"/> 
#define FIELD_PROP_SELF_LAST_Z			"SelfLastZ" 
 
//<property name="Name"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="Ãû×Ö"/> 
#define FIELD_PROP_NAME			"Name" 
 
//<property name="BaseCap"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="»ù´¡ÈÝÁ¿, Îª0Ê±Ê¹ÓÃ×î´óÈÝÁ¿"/> 
#define FIELD_PROP_BASE_CAP			"BaseCap" 
 
//<property name="CantAdd"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÇ·ñ²»ÄÜÌí¼ÓÎïÆ·µ½ÈÝÆ÷ÖÐ"/> 
#define FIELD_PROP_CANT_ADD			"CantAdd" 
 
//<property name="CantRemove"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÇ·ñ²»ÄÜ½«ÎïÆ·´ÓÈÝÆ÷ÖÐÒÆ×ß"/> 
#define FIELD_PROP_CANT_REMOVE			"CantRemove" 
 
//<property name="CantArrange"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÇ·ñ²»ÄÜÖ´ÐÐÕûÀí²Ù×÷"/> 
#define FIELD_PROP_CANT_ARRANGE			"CantArrange" 
 
//<property name="CanOP"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Íæ¼Ò¿É²Ù×÷ÈÝÆ÷µÄ±êÖ¾"/> 
#define FIELD_PROP_CAN_OP			"CanOP" 
 
//<property name="LockedSize"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Ëø¶¨µÄ¸ñ×ÓÊý"/> 
#define FIELD_PROP_LOCKED_SIZE			"LockedSize" 
 
//<property name="EquipType"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×°±¸À¸Î»ÀàÐÍ, equipment_config.xmlÎÄ¼þÖÐµÄ¶¨Òå"/> 
#define FIELD_PROP_EQUIP_TYPE			"EquipType" 
 
//<property name="BasePackage"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="»ù´¡ÊôÐÔ°ü"/> 
#define FIELD_PROP_BASE_PACKAGE			"BasePackage" 
 
//<property name="StrPackage"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ç¿»¯»ù´¡°ü"/> 
#define FIELD_PROP_STR_PACKAGE			"StrPackage" 
 
//<property name="AddPackage"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¸½¼ÓÊôÐÔ°ü"/> 
#define FIELD_PROP_ADD_PACKAGE			"AddPackage" 
 
//<property name="ClassLimit"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ö°ÒµÏÞÖÆ"/> 
#define FIELD_PROP_CLASS_LIMIT			"ClassLimit" 
 
//<property name="ModifyLevel"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ëæ»úÊýµÈ¼¶Çø¼ä"/> 
#define FIELD_PROP_MODIFY_LEVEL			"ModifyLevel" 
 
//<property name="ModifyAddPackage"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×°±¸¸½¼ÓÊôÐÔËæ»úÇø¼äÖµ"/> 
#define FIELD_PROP_MODIFY_ADD_PACKAGE			"ModifyAddPackage" 
 
//<property name="EquipNotifyFlag"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="»ñµÃ×°±¸ºóµÄÍ¨ÖªÉèÖÃ(°´Î»ÆÁ±Î)"/> 
#define FIELD_PROP_EQUIP_NOTIFY_FLAG			"EquipNotifyFlag" 
 
//<property name="ApperanceM"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÄÐ×°±¸Íâ¹Ûid"/> 
#define FIELD_PROP_APPERANCE_M			"ApperanceM" 
 
//<property name="ApperanceW"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="Å®×°±¸Íâ¹Ûid"/> 
#define FIELD_PROP_APPERANCE_W			"ApperanceW" 
 
//<property name="FashionM"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÄÐÊ±×°Íâ¹Ûid"/> 
#define FIELD_PROP_FASHION_M			"FashionM" 
 
//<property name="FashionW"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="Å®Ê±×°Íâ¹Ûid"/> 
#define FIELD_PROP_FASHION_W			"FashionW" 
 
//<property name="Config"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="±àºÅ"/> 
#define FIELD_PROP_CONFIG			"Config" 
 
//<property name="Script"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="±àºÅ"/> 
#define FIELD_PROP_SCRIPT			"Script" 
 
//<property name="ColorLevel"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÑÕÉ«µÈ¼¶"/> 
#define FIELD_PROP_COLOR_LEVEL			"ColorLevel" 
 
//<property name="UniqueID"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Î¨Ò»±àºÅ"/> 
#define FIELD_PROP_UNIQUE_ID			"UniqueID" 
 
//<property name="Amount"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="µ±Ç°ÊýÁ¿"/> 
#define FIELD_PROP_AMOUNT			"Amount" 
 
//<property name="ItemType"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÎïÆ·×ÓÀàÐÍ"/> 
#define FIELD_PROP_ITEM_TYPE			"ItemType" 
 
//<property name="MaxAmount"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×î´óÊýÁ¿"/> 
#define FIELD_PROP_MAX_AMOUNT			"MaxAmount" 
 
//<property name="LimitLevel"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µÈ¼¶ÏÞÖÆ"/> 
#define FIELD_PROP_LIMIT_LEVEL			"LimitLevel" 
 
//<property name="SellPrice"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÛ¼ÛÍ­±Ò"/> 
#define FIELD_PROP_SELL_PRICE			"SellPrice" 
 
//<property name="SpecialItem"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÇ·ñÏ¡ÓÐÎïÆ·"/> 
#define FIELD_PROP_SPECIAL_ITEM			"SpecialItem" 
 
//<property name="BindStatus"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÎïÆ·µÄ°ó¶¨×´Ì¬"/> 
#define FIELD_PROP_BIND_STATUS			"BindStatus" 
 
//<property name="Use"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÎïÆ·µÄÊ¹ÓÃÉèÖÃ"/> 
#define FIELD_PROP_USE			"Use" 
 
//<property name="CoolDownID"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÎïÆ·µÄÀäÈ´ID"/> 
#define FIELD_PROP_COOL_DOWN_ID			"CoolDownID" 
 
//<property name="CoolDownTime"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÎïÆ·µÄÀäÈ´Ê±³¤(µ¥Î»Ãë)"/> 
#define FIELD_PROP_COOL_DOWN_TIME			"CoolDownTime" 
 
//<property name="Purchase"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÂòÈë¼Û¸ñ"/> 
#define FIELD_PROP_PURCHASE			"Purchase" 
 
//<property name="RemindAmount"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÒÑ¾­ÌáÐÑÊ¹ÓÃµÄÊýÁ¿"/> 
#define FIELD_PROP_REMIND_AMOUNT			"RemindAmount" 
 
//<property name="IsNoNeedLog"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÊÇ·ñ²»ÐèÒª¼ÇÂ¼ÈÕÖ¾"/> 
#define FIELD_PROP_IS_NO_NEED_LOG			"IsNoNeedLog" 
 
//<property name="ItemFrozenTime"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¶³½áÊ±¼ä(ÔÚÕâÊ±¼äÖ®ºó¿ÉÊ¹ÓÃ)"/> 
#define FIELD_PROP_ITEM_FROZEN_TIME			"ItemFrozenTime" 
 
//<property name="BindAble"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÎïÆ·ÊÇ·ñÖ§³Ö°ó¶¨"/> 
#define FIELD_PROP_BIND_ABLE			"BindAble" 
 
//<property name="Level"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÓñçåµÈ¼¶"/> 
#define FIELD_PROP_LEVEL			"Level" 
 
//<property name="Step"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Óñçå½×¼¶"/> 
#define FIELD_PROP_STEP			"Step" 
 
//<property name="PointValue"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼ÛÖµµãÊý"/> 
#define FIELD_PROP_POINT_VALUE			"PointValue" 
 
//<property name="UpMoney"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Éý¼¶ÐèÒªÍ­±Ò"/> 
#define FIELD_PROP_UP_MONEY			"UpMoney" 
 
//<property name="UpLevel"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Éý¼¶ÐèÒªµÄÍæ¼Ò×îµÍµÈ¼¶"/> 
#define FIELD_PROP_UP_LEVEL			"UpLevel" 
 
//<property name="MountCost"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÏâÇ¶´Ë±¦Ê¯µÄ·ÑÓÃ(Í­±Ò)"/> 
#define FIELD_PROP_MOUNT_COST			"MountCost" 
 
//<property name="ExtirpateCost"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Õª³ý´Ë±¦Ê¯µÄ·ÑÓÃ(Í­±Ò)"/> 
#define FIELD_PROP_EXTIRPATE_COST			"ExtirpateCost" 
 
//<property name="Index"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="±¦Ê¯Ë÷Òý"/> 
#define FIELD_PROP_INDEX			"Index" 
 
//<property name="BaseAddPkg"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="»ù´¡ÐÞÕý°üid"/> 
#define FIELD_PROP_BASE_ADD_PKG			"BaseAddPkg" 
 
//<property name="PreRideAddPkg"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÉÏÒ»¸öÐÞÕý°üid"/> 
#define FIELD_PROP_PRE_RIDE_ADD_PKG			"PreRideAddPkg" 
 
//<property name="RideAddPkg"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°ÐÞÕý°üid"/> 
#define FIELD_PROP_RIDE_ADD_PKG			"RideAddPkg" 
 
//<property name="RideActived"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÊÇ·ñ¼¤»î"/> 
#define FIELD_PROP_RIDE_ACTIVED			"RideActived" 
 
//<property name="RideStar"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×øÆïÐÇ¼¶"/> 
#define FIELD_PROP_RIDE_STAR			"RideStar" 
 
//<property name="RideStep"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×øÆï½×¼¶"/> 
#define FIELD_PROP_RIDE_STEP			"RideStep" 
 
//<property name="DefaultSkin"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="×øÆïÄ¬ÈÏÆ¤·ô"/> 
#define FIELD_PROP_DEFAULT_SKIN			"DefaultSkin" 
 
//<property name="CurState"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°×´Ì¬"/> 
#define FIELD_PROP_CUR_STATE			"CurState" 
 
//<property name="SubState"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°×´Ì¬µÄ×Ó×´Ì¬"/> 
#define FIELD_PROP_SUB_STATE			"SubState" 
 
//<property name="AITargetObejct"  
// 	type="object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°AIÄ¿±ê¶ÔÏó"/> 
#define FIELD_PROP_AITARGET_OBEJCT			"AITargetObejct" 
 
//<property name="AITemplate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="AIÄ£¿é"/> 
#define FIELD_PROP_AITEMPLATE			"AITemplate" 
 
//<property name="Initiative"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÇ·ñÖ÷¶¯"/> 
#define FIELD_PROP_INITIATIVE			"Initiative" 
 
//<property name="FightAttackFreq"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¹¥»÷ÆµÂÊ"/> 
#define FIELD_PROP_FIGHT_ATTACK_FREQ			"FightAttackFreq" 
 
//<property name="LastAttackTime"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´ÓÉÏ´Î¹¥»÷µ½ÏÖÔÚ¹ýÈ¥µÄ¹¥»÷ÐÄÌø¼ì²âÊ±¼ä(¹¥»÷ÐÄ¼ä¸ô»Øµ÷ÀÛ¼ÓÖµ)"/> 
#define FIELD_PROP_LAST_ATTACK_TIME			"LastAttackTime" 
 
//<property name="PatrolMode"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ñ²ÂßÄ£Ê½"/> 
#define FIELD_PROP_PATROL_MODE			"PatrolMode" 
 
//<property name="BornX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="³öÉúµã"/> 
#define FIELD_PROP_BORN_X			"BornX" 
 
//<property name="BornZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="³öÉúµã"/> 
#define FIELD_PROP_BORN_Z			"BornZ" 
 
//<property name="BornOrient"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="³öÉúµã³¯Ïò"/> 
#define FIELD_PROP_BORN_ORIENT			"BornOrient" 
 
//<property name="MinAttackDist"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×îÐ¡¹¥»÷¾àÀë"/> 
#define FIELD_PROP_MIN_ATTACK_DIST			"MinAttackDist" 
 
//<property name="MaxAttackDist"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×î´ó¹¥»÷¾àÀë"/> 
#define FIELD_PROP_MAX_ATTACK_DIST			"MaxAttackDist" 
 
//<property name="PathID"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ñ°Â·Â·¾¶ID"/> 
#define FIELD_PROP_PATH_ID			"PathID" 
 
//<property name="Recuperate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÐÝÑøÊ±¼ä"/> 
#define FIELD_PROP_RECUPERATE			"Recuperate" 
 
//<property name="EscapeWarningHPRate"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÌÓÅÜ¾¯½äHP±ÈÀý"/> 
#define FIELD_PROP_ESCAPE_WARNING_HPRATE			"EscapeWarningHPRate" 
 
//<property name="EscapeLastTime"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÌÓÅÜÐÐÎªµÄ³ÖÐøÊ±¼ä"/> 
#define FIELD_PROP_ESCAPE_LAST_TIME			"EscapeLastTime" 
 
//<property name="EscapeCD"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢ÌÓÅÜµÄCD"/> 
#define FIELD_PROP_ESCAPE_CD			"EscapeCD" 
 
//<property name="GlobalStep"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°´ó²¨´Î½øÐÐµ½ÄÄ²¨"/> 
#define FIELD_PROP_GLOBAL_STEP			"GlobalStep" 
 
//<property name="LittleStepMax"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°²¨Ð¡²¨´Î´ÎÊý"/> 
#define FIELD_PROP_LITTLE_STEP_MAX			"LittleStepMax" 
 
//<property name="LittlestepNow"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°²¨Ð¡²¨´Î´ÎÊý"/> 
#define FIELD_PROP_LITTLESTEP_NOW			"LittlestepNow" 
 
//<property name="SceneId"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="³¡¾°id"/> 
#define FIELD_PROP_SCENE_ID			"SceneId" 
 
//<property name="MaxScope"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="»·ÐÎÍâ»·°ë¾¶"/> 
#define FIELD_PROP_MAX_SCOPE			"MaxScope" 
 
//<property name="MinScope"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="»·ÐÎÄÚ»·°ë¾¶"/> 
#define FIELD_PROP_MIN_SCOPE			"MinScope" 
 
//<property name="SpringCount"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="½øÈë´¥·¢·¶Î§µÄÊýÁ¿"/> 
#define FIELD_PROP_SPRING_COUNT			"SpringCount" 
 
//<property name="CreateType"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="0-´¥·¢ºó´´½¨NPC£¬1-´´½¨Ëæ»úÎ»ÖÃ¾«Ó¢NPC£¬2-´´½¨Ëæ»úÎ»ÖÃ¾«Ó¢´ø²¨´Î¹ÖÎï"/> 
#define FIELD_PROP_CREATE_TYPE			"CreateType" 
 
//<property name="TotalStep"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¹ÖÎïµÄ²¨Êý"/> 
#define FIELD_PROP_TOTAL_STEP			"TotalStep" 
 
//<property name="CurStep"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°²¨Êý"/> 
#define FIELD_PROP_CUR_STEP			"CurStep" 
 
//<property name="RandRow"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ëæ»úµÄÐÐÊý"/> 
#define FIELD_PROP_RAND_ROW			"RandRow" 
 
//<property name="RandHelper"  
// 	type="object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ëæ»úµÄ¸¨Öú´´½¨Æ÷"/> 
#define FIELD_PROP_RAND_HELPER			"RandHelper" 
 
//<property name="MonsterAmount"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¹ÖÎï×ÜÊý"/> 
#define FIELD_PROP_MONSTER_AMOUNT			"MonsterAmount" 
 
//<property name="IsDestroy"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢½áÊø¹ÖÎïÊÇ·ñÏú»Ù"/> 
#define FIELD_PROP_IS_DESTROY			"IsDestroy" 
 
//<property name="SceneAction"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¹Ø¿¨ÃØ¾³ÐÐ½ø²½Öè"/> 
#define FIELD_PROP_SCENE_ACTION			"SceneAction" 
 
//<property name="NpcID"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Éú³ÉµÄNPCµÄId"/> 
#define FIELD_PROP_NPC_ID			"NpcID" 
 
//<property name="CurAmount"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°ÊýÁ¿"/> 
#define FIELD_PROP_CUR_AMOUNT			"CurAmount" 
 
//<property name="RefreshPeriod"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ë¢ÐÂÊ±¼äÖÜÆÚ,µ¥Î»ÎªÃë"/> 
#define FIELD_PROP_REFRESH_PERIOD			"RefreshPeriod" 
 
//<property name="Limit"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="É±¶àÉÙ¹ÖË¢ÐÂÏÂÒ»²¨"/> 
#define FIELD_PROP_LIMIT			"Limit" 
 
//<property name="TranslateSceneID"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´«ËÍ³¡¾°±àºÅ£¨¹«Ê½£©"/> 
#define FIELD_PROP_TRANSLATE_SCENE_ID			"TranslateSceneID" 
 
//<property name="TranslatePosX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×ø±êX"/> 
#define FIELD_PROP_TRANSLATE_POS_X			"TranslatePosX" 
 
//<property name="TranslatePosY"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×ø±êY"/> 
#define FIELD_PROP_TRANSLATE_POS_Y			"TranslatePosY" 
 
//<property name="TranslatePosZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×ø±êZ"/> 
#define FIELD_PROP_TRANSLATE_POS_Z			"TranslatePosZ" 
 
//<property name="TranslateAy"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´«ËÍ³¯Ïò"/> 
#define FIELD_PROP_TRANSLATE_AY			"TranslateAy" 
 
//<property name="OwnSkills"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÓµÓÐµÄ¼¼ÄÜ£¨ÆÕÍ¨¼¼ÄÜ³ýÍâ£©"/> 
#define FIELD_PROP_OWN_SKILLS			"OwnSkills" 
 
//<property name="ChaseRange"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×·Öð°ë¾¶"/> 
#define FIELD_PROP_CHASE_RANGE			"ChaseRange" 
 
//<property name="PatrolRange"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ñ²Âß°ë¾¶"/> 
#define FIELD_PROP_PATROL_RANGE			"PatrolRange" 
 
//<property name="DropType"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µôÂä·ÖÀà"/> 
#define FIELD_PROP_DROP_TYPE			"DropType" 
 
//<property name="DropToKillerCnt"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¸ßÉËº¦·ÖÅäÈËÊý"/> 
#define FIELD_PROP_DROP_TO_KILLER_CNT			"DropToKillerCnt" 
 
//<property name="ExpAward"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¾­Ñé"/> 
#define FIELD_PROP_EXP_AWARD			"ExpAward" 
 
//<property name="SkillStr"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼¼ÄÜ"/> 
#define FIELD_PROP_SKILL_STR			"SkillStr" 
 
//<property name="NpcBasePackage"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="»ù´¡ÊôÐÔ°ü"/> 
#define FIELD_PROP_NPC_BASE_PACKAGE			"NpcBasePackage" 
 
//<property name="DropID"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µôÂäID, Ö§³ÖÓÃ¶ººÅ·Ö¸ôµÄ¶à¸öID"/> 
#define FIELD_PROP_DROP_ID			"DropID" 
 
//<property name="MinCapitalAward"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×Ê½ð½±ÀøÏÂÏÞ"/> 
#define FIELD_PROP_MIN_CAPITAL_AWARD			"MinCapitalAward" 
 
//<property name="MaxCapitalAward"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×Ê½ð½±ÀøÉÏÏÞ"/> 
#define FIELD_PROP_MAX_CAPITAL_AWARD			"MaxCapitalAward" 
 
//<property name="BornState"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="³öÉú×´Ì¬,1ÔÚ³öÉú×´Ì¬ÖÐ"/> 
#define FIELD_PROP_BORN_STATE			"BornState" 
 
//<property name="BornTime"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="³öÉú³ÖÐøÊ±¼ä"/> 
#define FIELD_PROP_BORN_TIME			"BornTime" 
 
//<property name="BornStand"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="³öÉú²»Ñ²Âß"/> 
#define FIELD_PROP_BORN_STAND			"BornStand" 
 
//<property name="TowerRatio"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÉËº¦Öµ¼Ó³É(Óë¸ÃÊýÖµÏà³Ë)"/> 
#define FIELD_PROP_TOWER_RATIO			"TowerRatio" 
 
//<property name="DropMeetLevel"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Èç¹ûºÍ¹ÖÎïµÈ¼¶Ïà²î´óÓÚµÈÓÚ¾Í²»µôÂä"/> 
#define FIELD_PROP_DROP_MEET_LEVEL			"DropMeetLevel" 
 
//<property name="FightMoveRate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Õ½¶·ÒÆ¶¯µ÷Õû¼¸ÂÊ"/> 
#define FIELD_PROP_FIGHT_MOVE_RATE			"FightMoveRate" 
 
//<property name="FightMoveDis"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Õ½¶·ÒÆ¶¯µÄ·¶Î§"/> 
#define FIELD_PROP_FIGHT_MOVE_DIS			"FightMoveDis" 
 
//<property name="CantRestoreHP"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÍÑÕ½ºó²»ÄÜ»Ö¸´ÂúÑª"/> 
#define FIELD_PROP_CANT_RESTORE_HP			"CantRestoreHP" 
 
//<property name="NpcDeadTime"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="ËÀÍöÂý¶¯×÷Ê±¼ä"/> 
#define FIELD_PROP_NPC_DEAD_TIME			"NpcDeadTime" 
 
//<property name="NpcPackageLevel"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊôÐÔ°üµÈ¼¶"/> 
#define FIELD_PROP_NPC_PACKAGE_LEVEL			"NpcPackageLevel" 
 
//<property name="NpcType"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="NPCÀàÐÍ 0-200NormalNpc,201-400StaticNpc"/> 
#define FIELD_PROP_NPC_TYPE			"NpcType" 
 
//<property name="SafeRule"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="±£»¤¹æÔò"/> 
#define FIELD_PROP_SAFE_RULE			"SafeRule" 
 
//<property name="Master"  
// 	type="object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ö÷ÈË"/> 
#define FIELD_PROP_MASTER			"Master" 
 
//<property name="Skill"  
// 	type="object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼¼ÄÜ"/> 
#define FIELD_PROP_SKILL			"Skill" 
 
//<property name="SpringDestroy"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢ÊÇ·ñ¾Í´Ý»Ù"/> 
#define FIELD_PROP_SPRING_DESTROY			"SpringDestroy" 
 
//<property name="SpringType"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢¶ÔÏóÀàÐÍ"/> 
#define FIELD_PROP_SPRING_TYPE			"SpringType" 
 
//<property name="SpringRelation"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢¹ØÏµÀàÐÍ"/> 
#define FIELD_PROP_SPRING_RELATION			"SpringRelation" 
 
//<property name="SpringEvent"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢ÊÂ¼þ"/> 
#define FIELD_PROP_SPRING_EVENT			"SpringEvent" 
 
//<property name="EventValue"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÉËº¦ÊýÖµ"/> 
#define FIELD_PROP_EVENT_VALUE			"EventValue" 
 
//<property name="EventRange"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´Ý»Ù¾àÀë"/> 
#define FIELD_PROP_EVENT_RANGE			"EventRange" 
 
//<property name="EventCount"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÈºÌå´¥·¢×î´ó¸öÊý"/> 
#define FIELD_PROP_EVENT_COUNT			"EventCount" 
 
//<property name="EndSpringEvent"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÍÑÀëÊÂ¼þ"/> 
#define FIELD_PROP_END_SPRING_EVENT			"EndSpringEvent" 
 
//<property name="DestroyEvent"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´Ý»ÙÊÂ¼þ"/> 
#define FIELD_PROP_DESTROY_EVENT			"DestroyEvent" 
 
//<property name="SpringEffect"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢ÌØÐ§"/> 
#define FIELD_PROP_SPRING_EFFECT			"SpringEffect" 
 
//<property name="Resource"  
// 	type="string"  
//	public="true"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢Ð§¹û"/> 
#define FIELD_PROP_RESOURCE			"Resource" 
 
//<property name="ResourceB"  
// 	type="string"  
//	public="true"  
//	private="false"  
//	save="false"  
//	desc="´¥·¢ºóÐ§¹û"/> 
#define FIELD_PROP_RESOURCE_B			"ResourceB" 
 
//<property name="EffectTime"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÌØÐ§³ÖÐøÊ±¼ä"/> 
#define FIELD_PROP_EFFECT_TIME			"EffectTime" 
 
//<property name="TrapRatio"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÉËº¦Öµ¼Ó³É(Óë¸ÃÊýÖµÏà³Ë)"/> 
#define FIELD_PROP_TRAP_RATIO			"TrapRatio" 
 
//<property name="DelayTime"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÑÓ³Ù´¥·¢"/> 
#define FIELD_PROP_DELAY_TIME			"DelayTime" 
 
//<property name="ArenaFlag"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¾º¼¼³¡±êÊ¶"/> 
#define FIELD_PROP_ARENA_FLAG			"ArenaFlag" 
 
//<property name="ArenaChanceResetDate"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="¾º¼¼³¡´ÎÊýµÄÖØÖÃÈÕÆÚ(YYYY-MM-DD)"/> 
#define FIELD_PROP_ARENA_CHANCE_RESET_DATE			"ArenaChanceResetDate" 
 
//<property name="ArenaChance"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="µ±ÈÕ¾º¼¼³¡´ÎÊý"/> 
#define FIELD_PROP_ARENA_CHANCE			"ArenaChance" 
 
//<property name="ArenaChanceUsed"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="µ±ÈÕ¾º¼¼³¡ÒÑ¾­ÌôÕ½´ÎÊý"/> 
#define FIELD_PROP_ARENA_CHANCE_USED			"ArenaChanceUsed" 
 
//<property name="ArenaChallengeeObj"  
// 	type="Object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÌôÕ½¶ÔÏó"/> 
#define FIELD_PROP_ARENA_CHALLENGEE_OBJ			"ArenaChallengeeObj" 
 
//<property name="ArenaChallengeeInfo"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÌôÕ½¶ÔÏóÐÅÏ¢"/> 
#define FIELD_PROP_ARENA_CHALLENGEE_INFO			"ArenaChallengeeInfo" 
 
//<property name="ArenaCurvRankList"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÉÏÒ»´ÎµÄ¾º¼¼³¡°ñ"/> 
#define FIELD_PROP_ARENA_CURV_RANK_LIST			"ArenaCurvRankList" 
 
//<property name="ArenaSN"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°¾º¼¼³¡µÄÁ÷Ë®ºÅ£¬Ã¿³¡Õ½¶·¶¼»áÓÐÐÂµÄ±àºÅÉú³É"/> 
#define FIELD_PROP_ARENA_SN			"ArenaSN" 
 
//<property name="HighestArenaRank"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="»ñµÃµÄ×î¸ßÅÅÃû"/> 
#define FIELD_PROP_HIGHEST_ARENA_RANK			"HighestArenaRank" 
 
//<property name="BattleAbility"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="Õ½¶·Á¦"/> 
#define FIELD_PROP_BATTLE_ABILITY			"BattleAbility" 
 
//<property name="HighestBattleAbility"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÀúÊ·×î¸ßÕ½¶·Á¦"/> 
#define FIELD_PROP_HIGHEST_BATTLE_ABILITY			"HighestBattleAbility" 
 
//<property name="BeforeGroup"  
// 	type="word"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="½øÈë·Ö×éÇ°µÄ·Ö×éºÅ"/> 
#define FIELD_PROP_BEFORE_GROUP			"BeforeGroup" 
 
//<property name="SumFlower"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÀúÊ·ÊÕ»¨"/> 
#define FIELD_PROP_SUM_FLOWER			"SumFlower" 
 
//<property name="WeekFlower"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="±¾ÖÜÊÕ»¨"/> 
#define FIELD_PROP_WEEK_FLOWER			"WeekFlower" 
 
//<property name="DailyIntimacyAward"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Ç×ÃÜ¶ÈÁìÈ¡×´Ì¬"/> 
#define FIELD_PROP_DAILY_INTIMACY_AWARD			"DailyIntimacyAward" 
 
//<property name="GuildName"  
// 	type="widestr"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="¹«»áÃû³Æ"/> 
#define FIELD_PROP_GUILD_NAME			"GuildName" 
 
//<property name="GuildPosition"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="°ï»áÖÐµÄÖ°Î»"/> 
#define FIELD_PROP_GUILD_POSITION			"GuildPosition" 
 
//<property name="QuitGuildDate"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÍË³ö¹«»áÈÕÆÚ"/> 
#define FIELD_PROP_QUIT_GUILD_DATE			"QuitGuildDate" 
 
//<property name="GuildListBeginRow"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="»ñÈ¡¹«»áÁÐ±íµÄÆðÊ¼ÐÐ"/> 
#define FIELD_PROP_GUILD_LIST_BEGIN_ROW			"GuildListBeginRow" 
 
//<property name="GuildLevel"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="°ï»áµÈ¼¶"/> 
#define FIELD_PROP_GUILD_LEVEL			"GuildLevel" 
 
//<property name="OneKeyJoinTime"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Ò»¼üÉêÇëÊ±¼ä"/> 
#define FIELD_PROP_ONE_KEY_JOIN_TIME			"OneKeyJoinTime" 
 
//<property name="GuildMutualTimes"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×éÖ¯»¥Öú´ÎÊý"/> 
#define FIELD_PROP_GUILD_MUTUAL_TIMES			"GuildMutualTimes" 
 
//<property name="GuildLastIssueMutualTimes"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÉÏÒ»´Î·¢²¼×éÖ¯»¥ÖúÊ±¼ä"/> 
#define FIELD_PROP_GUILD_LAST_ISSUE_MUTUAL_TIMES			"GuildLastIssueMutualTimes" 
 
//<property name="GuildShortName"  
// 	type="widestr"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="×éÖ¯¶ÌÃû"/> 
#define FIELD_PROP_GUILD_SHORT_NAME			"GuildShortName" 
 
//<property name="GuildIdentifying"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="×éÖ¯±êÊ¶"/> 
#define FIELD_PROP_GUILD_IDENTIFYING			"GuildIdentifying" 
 
//<property name="GuildDonateTimes"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="×éÖ¯¾èÏ×´ÎÊý"/> 
#define FIELD_PROP_GUILD_DONATE_TIMES			"GuildDonateTimes" 
 
//<property name="GuildDailyGif"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="ÁìÈ¡×éÖ¯Àñ°üÊý"/> 
#define FIELD_PROP_GUILD_DAILY_GIF			"GuildDailyGif" 
 
//<property name="ActiveJade"  
// 	type="object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°¼¤»îµÄÓñçå"/> 
#define FIELD_PROP_ACTIVE_JADE			"ActiveJade" 
 
//<property name="ActiveJadeId"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="µ±Ç°¼¤»îµÄÓñçåid"/> 
#define FIELD_PROP_ACTIVE_JADE_ID			"ActiveJadeId" 
 
//<property name="PlayerExp"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¾­Ñé"/> 
#define FIELD_PROP_PLAYER_EXP			"PlayerExp" 
 
//<property name="PlayerUpgradeExp"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="Éý¼¶ÐèÒª¾­Ñé"/> 
#define FIELD_PROP_PLAYER_UPGRADE_EXP			"PlayerUpgradeExp" 
 
//<property name="ItemKNExpRate"  
// 	type="byte"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÎïÆ·¸½¼ÓµÄÉ±¹Ö¶à±¶¾­Ñé"/> 
#define FIELD_PROP_ITEM_KNEXP_RATE			"ItemKNExpRate" 
 
//<property name="KNExpRate"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="É±¹Ö¾­Ñé±ÈÂÊ"/> 
#define FIELD_PROP_KNEXP_RATE			"KNExpRate" 
 
//<property name="WorldLevelAddExp"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÊÀ½çµÈ¼¶µ±Ç°¸½¼Ó¾­ÑéÖµ"/> 
#define FIELD_PROP_WORLD_LEVEL_ADD_EXP			"WorldLevelAddExp" 
 
//<property name="WorldLevelTotalExp"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÊÀ½çµÈ¼¶¸½¼Ó×Ü¾­ÑéÖµ"/> 
#define FIELD_PROP_WORLD_LEVEL_TOTAL_EXP			"WorldLevelTotalExp" 
 
//<property name="AsynWait"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´«ËÍÃÅÒì²½µÈ´ýÀàÐÍ"/> 
#define FIELD_PROP_ASYN_WAIT			"AsynWait" 
 
//<property name="AsynWaitBeginTime"  
// 	type="int"  
//	public="false"  
//	private=""  
//	save="false"  
//	desc="´«ËÍÃÅÒì²½µÈ´ýÊ±¼ä"/> 
#define FIELD_PROP_ASYN_WAIT_BEGIN_TIME			"AsynWaitBeginTime" 
 
//<property name="SkillPoint"  
// 	type="DWORD"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¼¼ÄÜµã"/> 
#define FIELD_PROP_SKILL_POINT			"SkillPoint" 
 
//<property name="VipLevel"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="VIPµÈ¼¶"/> 
#define FIELD_PROP_VIP_LEVEL			"VipLevel" 
 
//<property name="VipExp"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÀÛ¼Æ³äÖµ ¿ÉÊÜµÀ¾ßÓ°Ïì"/> 
#define FIELD_PROP_VIP_EXP			"VipExp" 
 
//<property name="TotalPay"  
// 	type="float"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÀÛ¼Æ³äÖµ ¿ÉÊÜµÀ¾ßÓ°Ïì"/> 
#define FIELD_PROP_TOTAL_PAY			"TotalPay" 
 
//<property name="TotalPayReal"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÀÛ¼Æ³äÖµ ÕæÊµ³äÖµ¶î¶È"/> 
#define FIELD_PROP_TOTAL_PAY_REAL			"TotalPayReal" 
 
//<property name="PayTimes"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="×Ü³äÖµ´ÎÊý"/> 
#define FIELD_PROP_PAY_TIMES			"PayTimes" 
 
//<property name="BoughtVipItem"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÒÑ¾­¹ºÂòµÄVIPÀñ°ü"/> 
#define FIELD_PROP_BOUGHT_VIP_ITEM			"BoughtVipItem" 
 
//<property name="PickedVipItem"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÒÑ¾­ÁìÈ¡µÄVIPÀñ°ü"/> 
#define FIELD_PROP_PICKED_VIP_ITEM			"PickedVipItem" 
 
//<property name="WeekCardEndDate"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÖÜ¿¨½ØÖ¹ÈÕÆÚ"/> 
#define FIELD_PROP_WEEK_CARD_END_DATE			"WeekCardEndDate" 
 
//<property name="MonthCardEndDate"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÔÂ¿¨½ØÖ¹ÈÕÆÚ"/> 
#define FIELD_PROP_MONTH_CARD_END_DATE			"MonthCardEndDate" 
 
//<property name="QuarterCardEndDate"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¼¾¿¨½ØÖ¹ÈÕÆÚ"/> 
#define FIELD_PROP_QUARTER_CARD_END_DATE			"QuarterCardEndDate" 
 
//<property name="YearCardEndDate"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Äê¿¨½ØÖ¹ÈÕÆÚ"/> 
#define FIELD_PROP_YEAR_CARD_END_DATE			"YearCardEndDate" 
 
//<property name="ForeverCardState"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÊÇ·ñ¹ºÂò¸£Àû¿¨"/> 
#define FIELD_PROP_FOREVER_CARD_STATE			"ForeverCardState" 
 
//<property name="CardPickedFlag"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Ã¿ÈÕÁìÈ¡±êÖ¾(Äê¿¨£¬ÔÂ¿¨£¬ÖÜ¿¨...., Ã¿Ò»Î»±íÊ¾Ò»ÖÖ¿¨, 1ÎªÁìÈ¡×´Ì¬)"/> 
#define FIELD_PROP_CARD_PICKED_FLAG			"CardPickedFlag" 
 
//<property name="VipPickedFlag"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="VipÃ¿ÈÕÀñ°üÁìÈ¡±êÊ¶"/> 
#define FIELD_PROP_VIP_PICKED_FLAG			"VipPickedFlag" 
 
//<property name="PayedRebate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="É¾µµ²âÊÔ³äÖµ·µÀû"/> 
#define FIELD_PROP_PAYED_REBATE			"PayedRebate" 
 
//<property name="PayedRebateType"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="É¾µµ²âÊÔ³äÖµ·µÀûÀàÐÍ£¬webºÍ¿Í»§¶Ë¶¨Òå"/> 
#define FIELD_PROP_PAYED_REBATE_TYPE			"PayedRebateType" 
 
//<property name="Invisible"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc=""/> 
#define FIELD_PROP_INVISIBLE			"Invisible" 
 
//<property name="Uid"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="½ÇÉ«Î¨Ò»±êÊ¶"/> 
#define FIELD_PROP_UID			"Uid" 
 
//<property name="GroupID"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="·Ö×é³¡¾°×éºÅ"/> 
#define FIELD_PROP_GROUP_ID			"GroupID" 
 
//<property name="AccountID"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÕÊºÅ±àºÅ"/> 
#define FIELD_PROP_ACCOUNT_ID			"AccountID" 
 
//<property name="IssuerID"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÔËÓªÉÌ±àºÅ"/> 
#define FIELD_PROP_ISSUER_ID			"IssuerID" 
 
//<property name="Account"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÕÊºÅ"/> 
#define FIELD_PROP_ACCOUNT			"Account" 
 
//<property name="ServerID"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="·þÎñÆ÷±àºÅ"/> 
#define FIELD_PROP_SERVER_ID			"ServerID" 
 
//<property name="Sex"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="ÐÔ±ð 0:ÄÐÐÔ,1:Å®ÐÔ"/> 
#define FIELD_PROP_SEX			"Sex" 
 
//<property name="Job"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="Ö°Òµ"/> 
#define FIELD_PROP_JOB			"Job" 
 
//<property name="LastObject"  
// 	type="Object"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="Ñ¡ÖÐÍæ¼Ò"/> 
#define FIELD_PROP_LAST_OBJECT			"LastObject" 
 
//<property name="RoleCreateTime"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="´´½¨Ê±¼ä"/> 
#define FIELD_PROP_ROLE_CREATE_TIME			"RoleCreateTime" 
 
//<property name="RoleLoginTime"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="µÇÂ½Ê±¼ä"/> 
#define FIELD_PROP_ROLE_LOGIN_TIME			"RoleLoginTime" 
 
//<property name="RoleLogoutTime"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="µÇ³öÊ±¼ä"/> 
#define FIELD_PROP_ROLE_LOGOUT_TIME			"RoleLogoutTime" 
 
//<property name="LoginDays"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="µÇÂ½ÌìÊý"/> 
#define FIELD_PROP_LOGIN_DAYS			"LoginDays" 
 
//<property name="ClientType"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="¿Í»§¶ËÀàÐÍ"/> 
#define FIELD_PROP_CLIENT_TYPE			"ClientType" 
 
//<property name="Token"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="macµØÖ·"/> 
#define FIELD_PROP_TOKEN			"Token" 
 
//<property name="ServerName"  
// 	type="widestr"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="µ±Ç°·þÎñÆ÷Ãû×Ö"/> 
#define FIELD_PROP_SERVER_NAME			"ServerName" 
 
//<property name="AppPackage"  
// 	type="string"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="°üÃû"/> 
#define FIELD_PROP_APP_PACKAGE			"AppPackage" 
 
//<property name="PrepareRole"  
// 	type="byte"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="Ô¤´´½¨½ÇÉ«0:ÆÕÍ¨ 1:»úÆ÷ÈË"/> 
#define FIELD_PROP_PREPARE_ROLE			"PrepareRole" 
 
//<property name="AccountUID"  
// 	type="string"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="Ä³Ð©µÚÈý·½ÇþµÀUID"/> 
#define FIELD_PROP_ACCOUNT_UID			"AccountUID" 
 
//<property name="PlayerVersion"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="Íæ¼Ò°æ±¾ºÅ"/> 
#define FIELD_PROP_PLAYER_VERSION			"PlayerVersion" 
 
//<property name="CurAction"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Íæ¼Òµ±Ç°ÐÐÎª"/> 
#define FIELD_PROP_CUR_ACTION			"CurAction" 
 
//<property name="StrenSuitPkg"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Ç¿»¯Ì××°ÊôÐÔ°ü"/> 
#define FIELD_PROP_STREN_SUIT_PKG			"StrenSuitPkg" 
 
//<property name="SilenceTick"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="½ûÑÔÊ±¼ä£¨ºÁÃë£©"/> 
#define FIELD_PROP_SILENCE_TICK			"SilenceTick" 
 
//<property name="PKModel"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="PKÄ£Ê½"/> 
#define FIELD_PROP_PKMODEL			"PKModel" 
 
//<property name="CriminalValue"  
// 	type="dword"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×ï¶ñÖµ"/> 
#define FIELD_PROP_CRIMINAL_VALUE			"CriminalValue" 
 
//<property name="CrimeState"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="×ï¶ñ×´Ì¬"/> 
#define FIELD_PROP_CRIME_STATE			"CrimeState" 
 
//<property name="CrimieOnlineTime"  
// 	type="dword"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×ï¶ñ×´Ì¬ÔÚÏßÊ±¼ä"/> 
#define FIELD_PROP_CRIMIE_ONLINE_TIME			"CrimieOnlineTime" 
 
//<property name="DecCrimeScore"  
// 	type="dword"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¼õÉÙ×ï¶ñÖµµÄ»ý·Ö"/> 
#define FIELD_PROP_DEC_CRIME_SCORE			"DecCrimeScore" 
 
//<property name="ActivatedRide"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="µ±Ç°Æï³ËµÄÆ¤·ôID"/> 
#define FIELD_PROP_ACTIVATED_RIDE			"ActivatedRide" 
 
//<property name="CurMainTask"  
// 	type="dword"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="µ±Ç°ÕýÔÚ½øÐÐµÄÖ÷ÏßÈÎÎñID"/> 
#define FIELD_PROP_CUR_MAIN_TASK			"CurMainTask" 
 
//<property name="TeamID"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¶ÓÎé±àºÅ"/> 
#define FIELD_PROP_TEAM_ID			"TeamID" 
 
//<property name="TeamCaptain"  
// 	type="widestr"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="¶Ó³¤"/> 
#define FIELD_PROP_TEAM_CAPTAIN			"TeamCaptain" 
 
//<property name="AutoMatch"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="×Ô¶¯Æ¥Åä"/> 
#define FIELD_PROP_AUTO_MATCH			"AutoMatch" 
 
//<property name="WingLevel"  
// 	type="byte"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="³á°òµÈ¼¶"/> 
#define FIELD_PROP_WING_LEVEL			"WingLevel" 
 
//<property name="WingStep"  
// 	type="byte"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="³á°ò½×¼¶"/> 
#define FIELD_PROP_WING_STEP			"WingStep" 
 
//<property name="WingStepBless"  
// 	type="byte"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="³á°òÉý½××£¸£Öµ"/> 
#define FIELD_PROP_WING_STEP_BLESS			"WingStepBless" 
 
//<property name="CantMove"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="ÊÇ·ñÄÜ¹»ÒÆ¶¯ 0:¿ÉÒÆ¶¯ 1:²»ÄÜÒÆ¶¯"/> 
#define FIELD_PROP_CANT_MOVE			"CantMove" 
 
//<property name="CantRotate"  
// 	type="byte"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÇ·ñÄÜ¹»×ªÏò 0:¿É×ªÏò 1:²»ÄÜ×ªÏò"/> 
#define FIELD_PROP_CANT_ROTATE			"CantRotate" 
 
//<property name="CantAttack"  
// 	type="byte"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÊÇ·ñÄÜ¹»¹¥»÷"/> 
#define FIELD_PROP_CANT_ATTACK			"CantAttack" 
 
//<property name="CantBeAttack"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="ÊÇ·ñÄÜ¹»±»¹¥»÷ 0:¿É±»¹¥»÷ 1£º²»¿É±»¹¥»÷"/> 
#define FIELD_PROP_CANT_BE_ATTACK			"CantBeAttack" 
 
//<property name="CantUseSkill"  
// 	type="byte"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÊÇ·ñÄÜ¹»Ê¹ÓÃ¼¼ÄÜ"/> 
#define FIELD_PROP_CANT_USE_SKILL			"CantUseSkill" 
 
//<property name="ImmunoControlBuff"  
// 	type="byte"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÃâÒß¿ØÖÆÀàbuff"/> 
#define FIELD_PROP_IMMUNO_CONTROL_BUFF			"ImmunoControlBuff" 
 
//<property name="Silent"  
// 	type="byte"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="³ÁÄ¬(²»ÄÜÊÍ·Å¼¼ÄÜ,²»¹ý¿ÉÒÔÆÕÍ¨¹¥»÷)"/> 
#define FIELD_PROP_SILENT			"Silent" 
 
//<property name="FixBeDamaged"  
// 	type="byte"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="±»¹Ì¶¨ÊýÖµÉËº¦"/> 
#define FIELD_PROP_FIX_BE_DAMAGED			"FixBeDamaged" 
 
//<property name="GaintState"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="°ÔÌå×´Ì¬"/> 
#define FIELD_PROP_GAINT_STATE			"GaintState" 
 
//<property name="StiffState"  
// 	type="byte"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÊÜ»÷Ó²Ö±×´Ì¬"/> 
#define FIELD_PROP_STIFF_STATE			"StiffState" 
 
//<property name="Dead"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="ËÀÍö×´Ì¬0:Î´ËÀÍö,1:ÒÑËÀÍö"/> 
#define FIELD_PROP_DEAD			"Dead" 
 
//<property name="BuffDisplayID"  
// 	type="string"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="buffÏÔÊ¾µÄid"/> 
#define FIELD_PROP_BUFF_DISPLAY_ID			"BuffDisplayID" 
 
//<property name="FightState"  
// 	type="byte"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="Õ½¶·×´Ì¬ 0·ñ1ÊÇ"/> 
#define FIELD_PROP_FIGHT_STATE			"FightState" 
 
//<property name="FightTime"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¿ªÊ¼Õ½¶·Ê±¼ä"/> 
#define FIELD_PROP_FIGHT_TIME			"FightTime" 
 
//<property name="FightActionState"  
// 	type="byte"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Õ½¶·ÐÐÎª×´Ì¬"/> 
#define FIELD_PROP_FIGHT_ACTION_STATE			"FightActionState" 
 
//<property name="Camp"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="ÕóÓª"/> 
#define FIELD_PROP_CAMP			"Camp" 
 
//<property name="HP"  
// 	type="int64"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="µ±Ç°HPÖµ"/> 
#define FIELD_PROP_HP			"HP" 
 
//<property name="MaxHP"  
// 	type="int64"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="×î´óÆøÑªÖµ"/> 
#define FIELD_PROP_MAX_HP			"MaxHP" 
 
//<property name="MP"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="µ±Ç°MPÖµ"/> 
#define FIELD_PROP_MP			"MP" 
 
//<property name="MaxMP"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="×î´óMPÖµ"/> 
#define FIELD_PROP_MAX_MP			"MaxMP" 
 
//<property name="PhysAttack"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÎïÀí¹¥»÷"/> 
#define FIELD_PROP_PHYS_ATTACK			"PhysAttack" 
 
//<property name="MagicAttack"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="·¨Êõ¹¥»÷"/> 
#define FIELD_PROP_MAGIC_ATTACK			"MagicAttack" 
 
//<property name="PhysDefend"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÎïÀí·ÀÓù"/> 
#define FIELD_PROP_PHYS_DEFEND			"PhysDefend" 
 
//<property name="MagicDefend"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="·¨Êõ·ÀÓù"/> 
#define FIELD_PROP_MAGIC_DEFEND			"MagicDefend" 
 
//<property name="Hit"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÃüÖÐ"/> 
#define FIELD_PROP_HIT			"Hit" 
 
//<property name="Miss"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÉÁ±Ü"/> 
#define FIELD_PROP_MISS			"Miss" 
 
//<property name="Crit"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="»áÐÄ(±©»÷)"/> 
#define FIELD_PROP_CRIT			"Crit" 
 
//<property name="CritRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="»áÐÄÂÊ"/> 
#define FIELD_PROP_CRIT_RATE			"CritRate" 
 
//<property name="Toug"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="×îÖÕ »áÐÄµÖ¿¹"/> 
#define FIELD_PROP_TOUG			"Toug" 
 
//<property name="TougRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="»áÐÄµÖ¿¹ÂÊ"/> 
#define FIELD_PROP_TOUG_RATE			"TougRate" 
 
//<property name="PhysPenetrate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÎïÀí´©Í¸"/> 
#define FIELD_PROP_PHYS_PENETRATE			"PhysPenetrate" 
 
//<property name="MagicPenetrate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="·¨Êõ´©Í¸"/> 
#define FIELD_PROP_MAGIC_PENETRATE			"MagicPenetrate" 
 
//<property name="AddHurtRate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÔöÉË"/> 
#define FIELD_PROP_ADD_HURT_RATE			"AddHurtRate" 
 
//<property name="DecHurtRate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼õÉË"/> 
#define FIELD_PROP_DEC_HURT_RATE			"DecHurtRate" 
 
//<property name="FlowPrepareTime"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="×¼±¸Ê±¼ä"/> 
#define FIELD_PROP_FLOW_PREPARE_TIME			"FlowPrepareTime" 
 
//<property name="FlowHitTimes"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´ò»÷´ÎÊý"/> 
#define FIELD_PROP_FLOW_HIT_TIMES			"FlowHitTimes" 
 
//<property name="FlowLeadTime"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Òýµ¼³ÖÐøÊ±¼ä"/> 
#define FIELD_PROP_FLOW_LEAD_TIME			"FlowLeadTime" 
 
//<property name="FlowLeadSepTime"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Òýµ¼¼ä¸ôÊ±¼ä"/> 
#define FIELD_PROP_FLOW_LEAD_SEP_TIME			"FlowLeadSepTime" 
 
//<property name="FlowHits"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°ÃüÖÐ´ÎÊý(×¼±¸¿ªÊ¼ÃüÖÐÐÄÌøÊ±£¬È¡flow_hittime_rec±íÖÐÄÄÒ»ÐÐµÄÊ±¼äÊýÖµ)"/> 
#define FIELD_PROP_FLOW_HITS			"FlowHits" 
 
//<property name="FlowCategory"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°Ê¹ÓÃµÄÁ÷³ÌµÄ·ÖÀàÃû³Æ"/> 
#define FIELD_PROP_FLOW_CATEGORY			"FlowCategory" 
 
//<property name="FlowType"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Á÷³ÌÀàÐÍ£º0£ºÆÕÍ¨¼¼ÄÜÁ÷³Ì£¬1£ºÒýµ¼¼¼ÄÜÁ÷³Ì"/> 
#define FIELD_PROP_FLOW_TYPE			"FlowType" 
 
//<property name="FlowBeginTime"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Á÷³Ì¿ªÊ¼µÄÊ±¼ä"/> 
#define FIELD_PROP_FLOW_BEGIN_TIME			"FlowBeginTime" 
 
//<property name="Flowing"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊÇ·ñÔÚÁ÷³ÌÖÐ"/> 
#define FIELD_PROP_FLOWING			"Flowing" 
 
//<property name="GaintVal"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="µ±Ç°°ÔÌåÖµ"/> 
#define FIELD_PROP_GAINT_VAL			"GaintVal" 
 
//<property name="MaxGaintVal"  
// 	type="dword"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="×î´ó°ÔÌåÖµ"/> 
#define FIELD_PROP_MAX_GAINT_VAL			"MaxGaintVal" 
 
//<property name="GaintValAdd"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="°ÔÌå¸½¼ÓÖµ"/> 
#define FIELD_PROP_GAINT_VAL_ADD			"GaintValAdd" 
 
//<property name="GaintDecHurtRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="°ÔÌå¼õÉË"/> 
#define FIELD_PROP_GAINT_DEC_HURT_RATE			"GaintDecHurtRate" 
 
//<property name="Str"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="Á¦Á¿"/> 
#define FIELD_PROP_STR			"Str" 
 
//<property name="Dex"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="Ãô½Ý"/> 
#define FIELD_PROP_DEX			"Dex" 
 
//<property name="Sta"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÌåÁ¦"/> 
#define FIELD_PROP_STA			"Sta" 
 
//<property name="Ing"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÖÇÁ¦"/> 
#define FIELD_PROP_ING			"Ing" 
 
//<property name="Spi"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¾«Éñ"/> 
#define FIELD_PROP_SPI			"Spi" 
 
//<property name="TempHP"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÁÙÊ±±£´æHPµ±Ç°Öµ"/> 
#define FIELD_PROP_TEMP_HP			"TempHP" 
 
//<property name="TempMP"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÁÙÊ±±£´æMPµ±Ç°Öµ"/> 
#define FIELD_PROP_TEMP_MP			"TempMP" 
 
//<property name="HPAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼ÓÆøÑª"/> 
#define FIELD_PROP_HPADD			"HPAdd" 
 
//<property name="HPRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÆøÑªÔö·ù 2¼¶ÊôÐÔ"/> 
#define FIELD_PROP_HPRATE			"HPRate" 
 
//<property name="MPAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼ÓMP"/> 
#define FIELD_PROP_MPADD			"MPAdd" 
 
//<property name="MPRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼ÓMP°Ù·Ö±È"/> 
#define FIELD_PROP_MPRATE			"MPRate" 
 
//<property name="PhysAttackAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼Ó ÎïÀí¹¥»÷"/> 
#define FIELD_PROP_PHYS_ATTACK_ADD			"PhysAttackAdd" 
 
//<property name="MagicttackAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼Ó ·¨Êõ¹¥»÷"/> 
#define FIELD_PROP_MAGICTTACK_ADD			"MagicttackAdd" 
 
//<property name="PhysAttackRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÎïÀí¹¥»÷Ç¿¶È 2¼¶ÊôÐÔ"/> 
#define FIELD_PROP_PHYS_ATTACK_RATE			"PhysAttackRate" 
 
//<property name="MagicAttackRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="·¨Êõ¹¥»÷Ç¿¶È 2¼¶ÊôÐÔ"/> 
#define FIELD_PROP_MAGIC_ATTACK_RATE			"MagicAttackRate" 
 
//<property name="PhysDefendAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼Ó ÎïÀí·ÀÓù"/> 
#define FIELD_PROP_PHYS_DEFEND_ADD			"PhysDefendAdd" 
 
//<property name="MagicDefendAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼Ó ·¨Êõ·ÀÓù"/> 
#define FIELD_PROP_MAGIC_DEFEND_ADD			"MagicDefendAdd" 
 
//<property name="PhysDefendRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÎïÀí·ÀÓùÇ¿¶È 2¼¶ÊôÐÔ"/> 
#define FIELD_PROP_PHYS_DEFEND_RATE			"PhysDefendRate" 
 
//<property name="MagicDefendRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="·¨Êõ·ÀÓùÇ¿¶È 2¼¶ÊôÐÔ"/> 
#define FIELD_PROP_MAGIC_DEFEND_RATE			"MagicDefendRate" 
 
//<property name="PhysPenetrateAdd"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÎïÀí´©Í¸Öµ"/> 
#define FIELD_PROP_PHYS_PENETRATE_ADD			"PhysPenetrateAdd" 
 
//<property name="PhysPenetrateRate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÎïÀí´©Í¸ÂÊ"/> 
#define FIELD_PROP_PHYS_PENETRATE_RATE			"PhysPenetrateRate" 
 
//<property name="MagicPenetrateAdd"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="·¨Êõ´©Í¸Öµ"/> 
#define FIELD_PROP_MAGIC_PENETRATE_ADD			"MagicPenetrateAdd" 
 
//<property name="MagicPenetrateRate"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="·¨Êõ´©Í¸ÂÊ"/> 
#define FIELD_PROP_MAGIC_PENETRATE_RATE			"MagicPenetrateRate" 
 
//<property name="HitAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼Ó ÃüÖÐ"/> 
#define FIELD_PROP_HIT_ADD			"HitAdd" 
 
//<property name="HitRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÃüÖÐÂÊ"/> 
#define FIELD_PROP_HIT_RATE			"HitRate" 
 
//<property name="MissAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼Ó »Ø±Ü"/> 
#define FIELD_PROP_MISS_ADD			"MissAdd" 
 
//<property name="MissRate"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="»Ø±ÜÂÊ"/> 
#define FIELD_PROP_MISS_RATE			"MissRate" 
 
//<property name="CritAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼Ó »áÐÄ(±©»÷)"/> 
#define FIELD_PROP_CRIT_ADD			"CritAdd" 
 
//<property name="TougAdd"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½¼Ó »áÐÄµÖ¿¹"/> 
#define FIELD_PROP_TOUG_ADD			"TougAdd" 
 
//<property name="CurSkill"  
// 	type="object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°ÕýÔÚÊ¹ÓÃµÄ¼¼ÄÜ"/> 
#define FIELD_PROP_CUR_SKILL			"CurSkill" 
 
//<property name="CurSkillUUID"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°ÕýÔÚÊ¹ÓÃ¼¼ÄÜµÄ64Î»Î¨Ò»±êÖ¾"/> 
#define FIELD_PROP_CUR_SKILL_UUID			"CurSkillUUID" 
 
//<property name="PreSkillUUID"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ç°Ò»¸öÊ¹ÓÃ¼¼ÄÜµÄ64Î»Î¨Ò»±êÖ¾"/> 
#define FIELD_PROP_PRE_SKILL_UUID			"PreSkillUUID" 
 
//<property name="CurSkillTarget"  
// 	type="object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°µÄ¼¼ÄÜÄ¿±ê"/> 
#define FIELD_PROP_CUR_SKILL_TARGET			"CurSkillTarget" 
 
//<property name="CurSkillTargetX"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°µÄ¼¼ÄÜ×ø±ê"/> 
#define FIELD_PROP_CUR_SKILL_TARGET_X			"CurSkillTargetX" 
 
//<property name="CurSkillTargetY"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°µÄ¼¼ÄÜ×ø±ê"/> 
#define FIELD_PROP_CUR_SKILL_TARGET_Y			"CurSkillTargetY" 
 
//<property name="CurSkillTargetZ"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°µÄ¼¼ÄÜ×ø±ê"/> 
#define FIELD_PROP_CUR_SKILL_TARGET_Z			"CurSkillTargetZ" 
 
//<property name="CurSkillStartTime"  
// 	type="int64"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="µ±Ç°¼¼ÄÜµÄ¿ªÊ¼Ê±¼ä"/> 
#define FIELD_PROP_CUR_SKILL_START_TIME			"CurSkillStartTime" 
 
//<property name="MaxFlyEnergy"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="æœ€å¤§è½»åŠŸç²¾åŠ›å€¼"/> 
#define FIELD_PROP_MAX_FLY_ENERGY			"MaxFlyEnergy" 
 
//<property name="FlyEnergyRestore"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="è½»åŠŸç²¾åŠ›å€¼å›žå¤é€Ÿåº¦(/s)"/> 
#define FIELD_PROP_FLY_ENERGY_RESTORE			"FlyEnergyRestore" 
 
//<property name="FlyEnergy"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="è½»åŠŸç²¾åŠ›å€¼"/> 
#define FIELD_PROP_FLY_ENERGY			"FlyEnergy" 
 
//<property name="JumpSec"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="å½“å‰è½»åŠŸæ®µæ•°"/> 
#define FIELD_PROP_JUMP_SEC			"JumpSec" 
 
//<property name="JumpHeight"  
// 	type="float"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="è·³è·ƒé«˜åº¦"/> 
#define FIELD_PROP_JUMP_HEIGHT			"JumpHeight" 
 
//<property name="CantUseFlySkill"  
// 	type="byte"  
//	public="flase"  
//	private="true"  
//	save="false"  
//	desc="æ˜¯å¦å¯ä»¥ä½¿ç”¨è½»åŠŸ"/> 
#define FIELD_PROP_CANT_USE_FLY_SKILL			"CantUseFlySkill" 
 
//<property name="MoveSpeed"  
// 	type="float"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="ÒÆ¶¯ËÙ¶È"/> 
#define FIELD_PROP_MOVE_SPEED			"MoveSpeed" 
 
//<property name="RunSpeed"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÅÜ¶¯ËÙ¶È"/> 
#define FIELD_PROP_RUN_SPEED			"RunSpeed" 
 
//<property name="RunSpeedAdd"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÒÆ¶¯ËÙ¶ÈµÄ¸Ä±ä"/> 
#define FIELD_PROP_RUN_SPEED_ADD			"RunSpeedAdd" 
 
//<property name="RollSpeed"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ðý×ªËÙ¶È"/> 
#define FIELD_PROP_ROLL_SPEED			"RollSpeed" 
 
//<property name="Weapon"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="ÎäÆ÷"/> 
#define FIELD_PROP_WEAPON			"Weapon" 
 
//<property name="Cloth"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc=""/> 
#define FIELD_PROP_CLOTH			"Cloth" 
 
//<property name="Hair"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc=""/> 
#define FIELD_PROP_HAIR			"Hair" 
 
//<property name="Wing"  
// 	type="int"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc=""/> 
#define FIELD_PROP_WING			"Wing" 
 
//<property name="ID"  
// 	type="int"  
//	public="true"  
//	private="false"  
//	save="false"  
//	desc="³¡¾°»ù´¡×ÊÔ´Ãû³Æ"/> 
#define FIELD_PROP_ID			"ID" 
 
//<property name="VisualRange"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="³¡¾°ÄÚÍæ¼ÒÖ®¼äµÄ¿ÉÊÓ·¶Î§"/> 
#define FIELD_PROP_VISUAL_RANGE			"VisualRange" 
 
//<property name="LocallLevelLim"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="±¾¹ú½ø³¡¾°ÏÞÖÆ"/> 
#define FIELD_PROP_LOCALL_LEVEL_LIM			"LocallLevelLim" 
 
//<property name="AttackType"  
// 	type="byte"  
//	public="true"  
//	private="false"  
//	save="false"  
//	desc="³¡¾°¹¥»÷ÀàÐÍ"/> 
#define FIELD_PROP_ATTACK_TYPE			"AttackType" 
 
//<property name="ProtoSceneID"  
// 	type="int"  
//	public="true"  
//	private="false"  
//	save="false"  
//	desc="Ô­ÐÍ³¡¾°ID"/> 
#define FIELD_PROP_PROTO_SCENE_ID			"ProtoSceneID" 
 
//<property name="CanShowRide"  
// 	type="byte"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¿É·ñÏÔÊ¾×øÆï 0£º²»ÏÔÊ¾ 1£ºÏÔÊ¾"/> 
#define FIELD_PROP_CAN_SHOW_RIDE			"CanShowRide" 
 
//<property name="CanFly"  
// 	type="byte"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¿É·ñ·ÉÐÐ 0²»¿É·ÉÐÐ 1¿É·ÉÐÐ"/> 
#define FIELD_PROP_CAN_FLY			"CanFly" 
 
//<property name="LifeTime"  
// 	type="int"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="³ÖÐøÊ±¼ä(ms) Èç¹ûÎª0,Ôò³Ö¾ÃÓÐÐ§"/> 
#define FIELD_PROP_LIFE_TIME			"LifeTime" 
 
//<property name="Sender"  
// 	type="object"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="BUFFERµÄÊ©¼ÓÕß"/> 
#define FIELD_PROP_SENDER			"Sender" 
 
//<property name="AddTime"  
// 	type="int64"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="BUFFERÌí¼ÓµÄÊ±¼ä µ¥Î»s(time_t)"/> 
#define FIELD_PROP_ADD_TIME			"AddTime" 
 
//<property name="Active"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="BUFFERÊÇ·ñÓÐÐ§"/> 
#define FIELD_PROP_ACTIVE			"Active" 
 
//<property name="Counter"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="BUFFERÒÑ¼Æ´ÎµÄ´ÎÊý"/> 
#define FIELD_PROP_COUNTER			"Counter" 
 
//<property name="Timer"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="BUFFERÒÑ´æÔÚµÄÊ±¼ä µ¥Î»ms"/> 
#define FIELD_PROP_TIMER			"Timer" 
 
//<property name="HurtCount"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼¼ÄÜ×ÜÃüÖÐ´ò»÷´ÎÊý,ÓÃÓÚ¼ÆËãÉËº¦"/> 
#define FIELD_PROP_HURT_COUNT			"HurtCount" 
 
//<property name="CurHits"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼¼ÄÜµ±Ç°´ò»÷´ÎÊý"/> 
#define FIELD_PROP_CUR_HITS			"CurHits" 
 
//<property name="SkillAttackAdd"  
// 	type="int"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼¼ÄÜ¹¥»÷Á¦"/> 
#define FIELD_PROP_SKILL_ATTACK_ADD			"SkillAttackAdd" 
 
//<property name="SkillDamageRate"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼¼ÄÜÉËº¦±¶ÂÊ"/> 
#define FIELD_PROP_SKILL_DAMAGE_RATE			"SkillDamageRate" 
 
//<property name="SkillFormula"  
// 	type="float"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼¼ÄÜ¹«Ê½½áËãµÄÖµ"/> 
#define FIELD_PROP_SKILL_FORMULA			"SkillFormula" 
 
//<property name="Fixed"  
// 	type="byte"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc=""/> 
#define FIELD_PROP_FIXED			"Fixed" 
 
//<property name="Photo"  
// 	type="string"  
//	public="true"  
//	private="true"  
//	save="true"  
//	desc="Í¼Ïñ"/> 
#define FIELD_PROP_PHOTO			"Photo" 
 
//<property name="State"  
// 	type="string"  
//	public="true"  
//	private="true"  
//	save="false"  
//	desc="×´Ì¬"/> 
#define FIELD_PROP_STATE			"State" 
 
 
// ±í¶¨Òå============================================================== 
 
 
//<record name="AIFindPathRec" 
//	cols="2"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ñ°Â·±í"/> 
 
#define FIELD_RECORD_AIFIND_PATH_REC			"AIFindPathRec" 
// AIFindPathRec column index define 
enum COLUMNS_OF_AIFIND_PATH_REC 
{ 
 
	// <column type="float" desc="x"/> 
	COLUMN_AIFIND_PATH_REC_0000, 
 
	// <column type="float" desc="z"/> 
	COLUMN_AIFIND_PATH_REC_0001, 
 
}; 
 
 
//<record name="RoundPathListRec" 
//	cols="2"  
//	maxrows="128"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÈÆ¿ªÕÏ°­ÎïËù×ß¹ýµÄ¸ñ×Ó"/> 
 
#define FIELD_RECORD_ROUND_PATH_LIST_REC			"RoundPathListRec" 
// RoundPathListRec column index define 
enum COLUMNS_OF_ROUND_PATH_LIST_REC 
{ 
 
	// <column type="dword" desc="¸ñ×ÓË÷Òý"/> 
	COLUMN_ROUND_PATH_LIST_REC_0000, 
 
	// <column type="dword" desc="ÖØ¸´×ß¹ýµÄ´ÎÊý"/> 
	COLUMN_ROUND_PATH_LIST_REC_0001, 
 
}; 
 
 
//<record name="FindRoundPathRec" 
//	cols="2"  
//	maxrows="128"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¶¯Ì¬Ñ°Â·Â·¾¶"/> 
 
#define FIELD_RECORD_FIND_ROUND_PATH_REC			"FindRoundPathRec" 
// FindRoundPathRec column index define 
enum COLUMNS_OF_FIND_ROUND_PATH_REC 
{ 
 
	// <column type="dword" desc="¸ñ×ÓË÷Òý"/> 
	COLUMN_FIND_ROUND_PATH_REC_0000, 
 
	// <column type="dword" desc="±»·ÃÎÊ´ÎÊý"/> 
	COLUMN_FIND_ROUND_PATH_REC_0001, 
 
}; 
 
 
//<record name="CantArriveGridListRec" 
//	cols="1"  
//	maxrows="128"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÎÞ·¨µ½´ïµÄ¸ñ×ÓÁÐ±í"/> 
 
#define FIELD_RECORD_CANT_ARRIVE_GRID_LIST_REC			"CantArriveGridListRec" 
// CantArriveGridListRec column index define 
enum COLUMNS_OF_CANT_ARRIVE_GRID_LIST_REC 
{ 
 
	// <column type="dword" desc="¸ñ×ÓË÷Òý"/> 
	COLUMN_CANT_ARRIVE_GRID_LIST_REC_0000, 
 
}; 
 
 
//<record name="ride_skin_rec" 
//	cols="3"  
//	maxrows="100"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×øÆïÆ¤·ô±í"/> 
 
#define FIELD_RECORD_RIDE_SKIN_REC			"ride_skin_rec" 
// ride_skin_rec column index define 
enum COLUMNS_OF_RIDE_SKIN_REC 
{ 
 
	// <column type="int" desc="Æ¤·ô±àºÅ"/> 
	COLUMN_RIDE_SKIN_REC_ID, 
 
	// <column type="byte" desc="×´Ì¬"/> 
	COLUMN_RIDE_SKIN_REC_STATE, 
 
	// <column type="byte" desc="ÐÂÆ¤·ô±êÊ¶"/> 
	COLUMN_RIDE_SKIN_REC_FLAG, 
 
}; 
 
 
//<record name="PatrolPointRec" 
//	cols="3"  
//	maxrows="64"  
//	public=""  
//	private=""  
//	save="false"  
//	desc="¶¨µãÑ²Âß±í"/> 
 
#define FIELD_RECORD_PATROL_POINT_REC			"PatrolPointRec" 
// PatrolPointRec column index define 
enum COLUMNS_OF_PATROL_POINT_REC 
{ 
 
	// <column type="float" desc=""/> 
	COLUMN_PATROL_POINT_REC_X, 
 
	// <column type="float" desc=""/> 
	COLUMN_PATROL_POINT_REC_Z, 
 
	// <column type="dword" desc="Í£ÁôÊ±¼ä£¬µ¥Î»ms"/> 
	COLUMN_PATROL_POINT_REC_STAY_TIME, 
 
}; 
 
 
//<record name="AttackerList" 
//	cols="4"  
//	maxrows="2000"  
//	public=""  
//	private=""  
//	save="false"  
//	desc="¹¥»÷¶ÔÏó±í"/> 
 
#define FIELD_RECORD_ATTACKER_LIST			"AttackerList" 
// AttackerList column index define 
enum COLUMNS_OF_ATTACKER_LIST 
{ 
 
	// <column type="object" desc="¶ÔÏó"/> 
	COLUMN_ATTACKER_LIST_0000, 
 
	// <column type="dword" desc="³ðºÞÖµ"/> 
	COLUMN_ATTACKER_LIST_0001, 
 
	// <column type="int64" desc="×î½ü¹¥»÷Ê±¼ä"/> 
	COLUMN_ATTACKER_LIST_0002, 
 
	// <column type="int64" desc="µÚÒ»´Î¹¥»÷Ê±¼ä"/> 
	COLUMN_ATTACKER_LIST_0003, 
 
}; 
 
 
//<record name="FindPathPointList" 
//	cols="2"  
//	maxrows="64"  
//	public=""  
//	private=""  
//	save="false"  
//	desc="Ñ°Â·Â·¾¶µã±í"/> 
 
#define FIELD_RECORD_FIND_PATH_POINT_LIST			"FindPathPointList" 
// FindPathPointList column index define 
enum COLUMNS_OF_FIND_PATH_POINT_LIST 
{ 
 
	// <column type="float" desc="X×ø±ê"/> 
	COLUMN_FIND_PATH_POINT_LIST_X, 
 
	// <column type="float" desc="Y×ø±ê"/> 
	COLUMN_FIND_PATH_POINT_LIST_Y, 
 
}; 
 
 
//<record name="boss_skill_rec" 
//	cols="2"  
//	maxrows="10"  
//	public=""  
//	private=""  
//	save="false"  
//	desc="NPC¼¼ÄÜ±í"/> 
 
#define FIELD_RECORD_BOSS_SKILL_REC			"boss_skill_rec" 
// boss_skill_rec column index define 
enum COLUMNS_OF_BOSS_SKILL_REC 
{ 
 
	// <column type="string" desc="¼¼ÄÜID"/> 
	COLUMN_BOSS_SKILL_REC_0000, 
 
	// <column type="int" desc="µ±Ç°È¨Öµ"/> 
	COLUMN_BOSS_SKILL_REC_0001, 
 
}; 
 
 
//<record name="object_list_rec" 
//	cols="1"  
//	maxrows="256"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="±¾Éú³ÉÆ÷Éú³ÉµÄ¶ÔÏó"/> 
 
#define FIELD_RECORD_OBJECT_LIST_REC			"object_list_rec" 
// object_list_rec column index define 
enum COLUMNS_OF_OBJECT_LIST_REC 
{ 
 
	// <column type="object" desc=""/> 
	COLUMN_OBJECT_LIST_REC_0000, 
 
}; 
 
 
//<record name="damage_rec" 
//	cols="3"  
//	maxrows="100"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÉËº¦±í"/> 
 
#define FIELD_RECORD_DAMAGE_REC			"damage_rec" 
// damage_rec column index define 
enum COLUMNS_OF_DAMAGE_REC 
{ 
 
	// <column type="widestr" desc="Íæ¼ÒÃû×Ö"/> 
	COLUMN_DAMAGE_REC_PLAYER_NAME, 
 
	// <column type="byte" desc="Ö°Òµ"/> 
	COLUMN_DAMAGE_REC_JOB, 
 
	// <column type="dword" desc="×ÜÉËº¦"/> 
	COLUMN_DAMAGE_REC_DAMAGE, 
 
}; 
 
 
//<record name="position_info_rec" 
//	cols="7"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¶ÔÏóËæ»úÎ»ÖÃÐÅÏ¢"/> 
 
#define FIELD_RECORD_POSITION_INFO_REC			"position_info_rec" 
// position_info_rec column index define 
enum COLUMNS_OF_POSITION_INFO_REC 
{ 
 
	// <column type="float" desc="X"/> 
	COLUMN_POSITION_INFO_REC_0000, 
 
	// <column type="float" desc="Y"/> 
	COLUMN_POSITION_INFO_REC_0001, 
 
	// <column type="float" desc="Z"/> 
	COLUMN_POSITION_INFO_REC_0002, 
 
	// <column type="float" desc="Orient"/> 
	COLUMN_POSITION_INFO_REC_0003, 
 
	// <column type="int" desc="used:ÊÇ·ñÒÑ±»Ê¹ÓÃ"/> 
	COLUMN_POSITION_INFO_REC_0004, 
 
	// <column type="int64" desc="ÉÏ´ÎÏú»ÙÊ±¼ä"/> 
	COLUMN_POSITION_INFO_REC_0005, 
 
	// <column type="float" desc="Ëæ»úË¢ÐÂ°ë¾¶"/> 
	COLUMN_POSITION_INFO_REC_0006, 
 
}; 
 
 
//<record name="SpringInfoRec" 
//	cols="2"  
//	maxrows="64"  
//	public=""  
//	private=""  
//	save="false"  
//	desc="´¥·¢Ä¿±êÐÅÏ¢±í"/> 
 
#define FIELD_RECORD_SPRING_INFO_REC			"SpringInfoRec" 
// SpringInfoRec column index define 
enum COLUMNS_OF_SPRING_INFO_REC 
{ 
 
	// <column type="object" desc="¶ÔÏóºÅ"/> 
	COLUMN_SPRING_INFO_REC_0000, 
 
	// <column type="string" desc="Ôö¼ÓµÄBUFF"/> 
	COLUMN_SPRING_INFO_REC_0001, 
 
}; 
 
 
//<record name="activate_function_rec" 
//	cols="1"  
//	maxrows="500"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¹¦ÄÜ¼¤»î±í"/> 
 
#define FIELD_RECORD_ACTIVATE_FUNCTION_REC			"activate_function_rec" 
// activate_function_rec column index define 
enum COLUMNS_OF_ACTIVATE_FUNCTION_REC 
{ 
 
	// <column type="byte" desc="¹¦ÄÜÀàÐÍ"/> 
	COLUMN_ACTIVATE_FUNCTION_REC_TYPE, 
 
}; 
 
 
//<record name="arena_fight_result" 
//	cols="5"  
//	maxrows="20"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="¾º¼¼³¡Õ½¶·½á¹û"/> 
 
#define FIELD_RECORD_ARENA_FIGHT_RESULT			"arena_fight_result" 
// arena_fight_result column index define 
enum COLUMNS_OF_ARENA_FIGHT_RESULT 
{ 
 
	// <column type="int" desc="Õ½¶·½á¹û"/> 
	COLUMN_ARENA_FIGHT_RESULT_RESULT, 
 
	// <column type="widestr" desc="µÐ·½Ãû×Ö"/> 
	COLUMN_ARENA_FIGHT_RESULT_NAME, 
 
	// <column type="int" desc="Õ½¶·Ç°ÅÅÃû"/> 
	COLUMN_ARENA_FIGHT_RESULT_OLDRANK, 
 
	// <column type="int" desc="Õ½¶·ºóÅÅÃû"/> 
	COLUMN_ARENA_FIGHT_RESULT_CURRANK, 
 
	// <column type="int64" desc="Õ½¶·Ê±¼ä"/> 
	COLUMN_ARENA_FIGHT_RESULT_TIME, 
 
}; 
 
 
//<record name="battlle_ability_rec" 
//	cols="2"  
//	maxrows="100"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Õ½¶·Á¦Í³¼Æ±í"/> 
 
#define FIELD_RECORD_BATTLLE_ABILITY_REC			"battlle_ability_rec" 
// battlle_ability_rec column index define 
enum COLUMNS_OF_BATTLLE_ABILITY_REC 
{ 
 
	// <column type="byte" desc="Õ½¶·Á¦ÀàÐÍ"/> 
	COLUMN_BATTLLE_ABILITY_REC_BATYPE, 
 
	// <column type="float" desc="Õ½¶·Á¦ÊýÖµ"/> 
	COLUMN_BATTLLE_ABILITY_REC_BABVALUE, 
 
}; 
 
 
//<record name="capital_rec" 
//	cols="3"  
//	maxrows="30"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×Ê²ú±í"/> 
 
#define FIELD_RECORD_CAPITAL_REC			"capital_rec" 
// capital_rec column index define 
enum COLUMNS_OF_CAPITAL_REC 
{ 
 
	// <column type="string" desc="×Ê²úÃû³Æ"/> 
	COLUMN_CAPITAL_REC_0000, 
 
	// <column type="byte" desc="×Ê²úÀàÐÍ"/> 
	COLUMN_CAPITAL_REC_0001, 
 
	// <column type="int64" desc="×Ê²úÊýÁ¿"/> 
	COLUMN_CAPITAL_REC_0002, 
 
}; 
 
 
//<record name="dayToplimitCapital" 
//	cols="3"  
//	maxrows="30"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="½ñÈÕ×Ê²úÔö¼Ó¼ÇÂ¼±í"/> 
 
#define FIELD_RECORD_DAY_TOPLIMIT_CAPITAL			"dayToplimitCapital" 
// dayToplimitCapital column index define 
enum COLUMNS_OF_DAY_TOPLIMIT_CAPITAL 
{ 
 
	// <column type="string" desc="×Ê²úÃû³Æ"/> 
	COLUMN_DAY_TOPLIMIT_CAPITAL_0000, 
 
	// <column type="byte" desc="×Ê²úÀàÐÍ"/> 
	COLUMN_DAY_TOPLIMIT_CAPITAL_0001, 
 
	// <column type="int64" desc="Ôö¼Ó×Ê²úÊýÁ¿"/> 
	COLUMN_DAY_TOPLIMIT_CAPITAL_0002, 
 
}; 
 
 
//<record name="valid_damage_rec" 
//	cols="2"  
//	maxrows="1000"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÓÐÐ§ÉËº¦¼ÇÂ¼±í"/> 
 
#define FIELD_RECORD_VALID_DAMAGE_REC			"valid_damage_rec" 
// valid_damage_rec column index define 
enum COLUMNS_OF_VALID_DAMAGE_REC 
{ 
 
	// <column type="string" desc="Íæ¼Òuid"/> 
	COLUMN_VALID_DAMAGE_REC_UID, 
 
	// <column type="int64" desc="ÉËº¦Ê±¼ä"/> 
	COLUMN_VALID_DAMAGE_REC_TIME, 
 
}; 
 
 
//<record name="valid_kill_rec" 
//	cols="2"  
//	maxrows="1000"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÓÐÐ§»÷É±Íæ¼Ò¼ÇÂ¼±í"/> 
 
#define FIELD_RECORD_VALID_KILL_REC			"valid_kill_rec" 
// valid_kill_rec column index define 
enum COLUMNS_OF_VALID_KILL_REC 
{ 
 
	// <column type="string" desc="Íæ¼Òuid"/> 
	COLUMN_VALID_KILL_REC_UID, 
 
	// <column type="int64" desc="»÷É±Ê±¼ä"/> 
	COLUMN_VALID_KILL_REC_TIME, 
 
}; 
 
 
//<record name="EquStrengthenRec" 
//	cols="8"  
//	maxrows="6"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×°±¸¸ñ×ÓÇ¿»¯µÈ¼¶"/> 
 
#define FIELD_RECORD_EQU_STRENGTHEN_REC			"EquStrengthenRec" 
// EquStrengthenRec column index define 
enum COLUMNS_OF_EQU_STRENGTHEN_REC 
{ 
 
	// <column type="BYTE" desc="×°±¸¸ñ×ÓË÷Òý(1-6)"/> 
	COLUMN_EQU_STRENGTHEN_REC_0000, 
 
	// <column type="WORD" desc="Ç¿»¯µÈ¼¶"/> 
	COLUMN_EQU_STRENGTHEN_REC_0001, 
 
	// <column type="STRING" desc="Ã¿¼þ×°±¸ÏâÇ¶µÄÁé»ê,Ã»ÓÐÓÃ''Õ¼Î»±íÊ¾"/> 
	COLUMN_EQU_STRENGTHEN_REC_0002, 
 
	// <column type="WORD" desc="Áé»êÍÌÊÉÊýÁ¿"/> 
	COLUMN_EQU_STRENGTHEN_REC_0003, 
 
	// <column type="STRING" desc="Ã¿¼þ×°±¸ÏâÇ¶µÄ±¦Ê¯1,Ã»ÓÐÓÃ''Õ¼Î»±íÊ¾"/> 
	COLUMN_EQU_STRENGTHEN_REC_0004, 
 
	// <column type="STRING" desc="Ã¿¼þ×°±¸ÏâÇ¶µÄ±¦Ê¯2,Ã»ÓÐÓÃ''Õ¼Î»±íÊ¾"/> 
	COLUMN_EQU_STRENGTHEN_REC_0005, 
 
	// <column type="STRING" desc="Ã¿¼þ×°±¸ÏâÇ¶µÄ±¦Ê¯3,Ã»ÓÐÓÃ''Õ¼Î»±íÊ¾"/> 
	COLUMN_EQU_STRENGTHEN_REC_0006, 
 
	// <column type="STRING" desc="Ã¿¼þ×°±¸ÏâÇ¶µÄ±¦Ê¯4,Ã»ÓÐÓÃ''Õ¼Î»±íÊ¾"/> 
	COLUMN_EQU_STRENGTHEN_REC_0007, 
 
}; 
 
 
//<record name="BaptiseRec" 
//	cols="3"  
//	maxrows="6"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="Ï´Á¶±í"/> 
 
#define FIELD_RECORD_BAPTISE_REC			"BaptiseRec" 
// BaptiseRec column index define 
enum COLUMNS_OF_BAPTISE_REC 
{ 
 
	// <column type="BYTE" desc="×°±¸¸ñ×ÓË÷Òý(1-6)"/> 
	COLUMN_BAPTISE_REC_POS, 
 
	// <column type="STRING" desc="ÊôÐÔÏ´Á·Öµ£¨json×ª»¯µÄ×Ö·û´®£©"/> 
	COLUMN_BAPTISE_REC_VALUE_INFO, 
 
	// <column type="STRING" desc="ÊôÐÔËø¶¨×´Ì¬£¨json×ª»¯µÄ×Ö·û´®£©"/> 
	COLUMN_BAPTISE_REC_LOCK_INFO, 
 
}; 
 
 
//<record name="BaptiseResultRec" 
//	cols="2"  
//	maxrows="6"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×°±¸Ï´Á¶½á¹û±í"/> 
 
#define FIELD_RECORD_BAPTISE_RESULT_REC			"BaptiseResultRec" 
// BaptiseResultRec column index define 
enum COLUMNS_OF_BAPTISE_RESULT_REC 
{ 
 
	// <column type="BYTE" desc="×°±¸¸ñ×ÓË÷Òý(1-6)"/> 
	COLUMN_BAPTISE_RESULT_REC_POS, 
 
	// <column type="STRING" desc="Ï´Á·½á¹û£¨json×ª»¯µÄ×Ö·û´®£©"/> 
	COLUMN_BAPTISE_RESULT_REC_RESULT_INFO, 
 
}; 
 
 
//<record name="friend_rec" 
//	cols="21"  
//	maxrows="50"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ºÃÓÑÁÐ±í"/> 
 
#define FIELD_RECORD_FRIEND_REC			"friend_rec" 
// friend_rec column index define 
enum COLUMNS_OF_FRIEND_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_FRIEND_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_FRIEND_REC_NAME, 
 
	// <column type="word" desc="¶Ô·½µÈ¼¶"/> 
	COLUMN_FRIEND_REC_LEVEL, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_FRIEND_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_FRIEND_REC_SEX, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_FRIEND_REC_POWER, 
 
	// <column type="widestr" desc="°ï»áÃû³Æ"/> 
	COLUMN_FRIEND_REC_GUILD_NAME, 
 
	// <column type="byte" desc="¶Ô·½ÔÚÏß×´Ì¬"/> 
	COLUMN_FRIEND_REC_ONLINE, 
 
	// <column type="int64" desc="×îºóÒ»´ÎÀëÏßÊ±¼ä"/> 
	COLUMN_FRIEND_REC_LAST_TIME, 
 
	// <column type="word" desc="Ç×ÃÜ¶ÈµÈ¼¶"/> 
	COLUMN_FRIEND_REC_INTIMACY_LEVEL, 
 
	// <column type="dword" desc="Ç×ÃÜ¶È"/> 
	COLUMN_FRIEND_REC_INTIMACY_EXP, 
 
	// <column type="dword" desc="ÀúÊ·ÊÕ»¨"/> 
	COLUMN_FRIEND_REC_SUM_FLOWER, 
 
	// <column type="dword" desc="±¾ÖÜÊÕ»¨"/> 
	COLUMN_FRIEND_REC_WEEK_FLOWER, 
 
	// <column type="dword" desc="ÎÒµÄËÍ»¨Êý"/> 
	COLUMN_FRIEND_REC_SEND_FLOWER, 
 
	// <column type="dword" desc="ÎÒµÄÊÕ»¨Êý"/> 
	COLUMN_FRIEND_REC_RECEIVE_FLOWER, 
 
	// <column type="dword" desc="ºÃÓÑteamID"/> 
	COLUMN_FRIEND_REC_TEAM_ID, 
 
	// <column type="byte" desc="¹ØÏµ"/> 
	COLUMN_FRIEND_REC_RELATION, 
 
	// <column type="widestr" desc="³ÆÎ½"/> 
	COLUMN_FRIEND_REC_TITLE, 
 
	// <column type="int64" desc="Ñ°ÇóÔ®ÖúÊ±¼ä"/> 
	COLUMN_FRIEND_REC_SEEK_SWORN_AID_TIME, 
 
	// <column type="dword" desc="vipµÈ¼¶"/> 
	COLUMN_FRIEND_REC_VIP_LEVEL, 
 
	// <column type="dword" desc="Ã¿ÈÕÔö¼ÓÇ×ÃÜ¶ÈÖµ"/> 
	COLUMN_FRIEND_REC_DAILY_INTIMACY, 
 
}; 
 
 
//<record name="blacklist_rec" 
//	cols="4"  
//	maxrows="50"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ºÚÃûµ¥ÁÐ±í"/> 
 
#define FIELD_RECORD_BLACKLIST_REC			"blacklist_rec" 
// blacklist_rec column index define 
enum COLUMNS_OF_BLACKLIST_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_BLACKLIST_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_BLACKLIST_REC_NAME, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_BLACKLIST_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_BLACKLIST_REC_SEX, 
 
}; 
 
 
//<record name="enemy_rec" 
//	cols="12"  
//	maxrows="50"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="³ðÈËÁÐ±í"/> 
 
#define FIELD_RECORD_ENEMY_REC			"enemy_rec" 
// enemy_rec column index define 
enum COLUMNS_OF_ENEMY_REC 
{ 
 
	// <column type="string" desc="³ðÈËUID"/> 
	COLUMN_ENEMY_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_ENEMY_REC_NAME, 
 
	// <column type="word" desc="¶Ô·½µÈ¼¶"/> 
	COLUMN_ENEMY_REC_LEVEL, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_ENEMY_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_ENEMY_REC_SEX, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_ENEMY_REC_POWER, 
 
	// <column type="widestr" desc="°ï»áÃû³Æ"/> 
	COLUMN_ENEMY_REC_GUILD_NAME, 
 
	// <column type="string" desc="Íæ¼ÒÕËºÅ"/> 
	COLUMN_ENEMY_REC_ACCOUNT, 
 
	// <column type="byte" desc="¶Ô·½ÔÚÏß×´Ì¬"/> 
	COLUMN_ENEMY_REC_ONLINE, 
 
	// <column type="dword" desc="³ðºÞÖµ"/> 
	COLUMN_ENEMY_REC_HATRED, 
 
	// <column type="int64" desc="±»É±Ê±¼ä"/> 
	COLUMN_ENEMY_REC_BE_KILL_TIME, 
 
	// <column type="dword" desc="vipµÈ¼¶"/> 
	COLUMN_ENEMY_REC_VIP_LEVEL, 
 
}; 
 
 
//<record name="friend_apply_rec" 
//	cols="9"  
//	maxrows="20"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ºÃÓÑÉêÇëÎ´´¦Àí±í"/> 
 
#define FIELD_RECORD_FRIEND_APPLY_REC			"friend_apply_rec" 
// friend_apply_rec column index define 
enum COLUMNS_OF_FRIEND_APPLY_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_FRIEND_APPLY_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_FRIEND_APPLY_REC_NAME, 
 
	// <column type="word" desc="¶Ô·½µÈ¼¶"/> 
	COLUMN_FRIEND_APPLY_REC_LEVEL, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_FRIEND_APPLY_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_FRIEND_APPLY_REC_SEX, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_FRIEND_APPLY_REC_POWER, 
 
	// <column type="widestr" desc="°ï»áÃû³Æ"/> 
	COLUMN_FRIEND_APPLY_REC_GUILD_NAME, 
 
	// <column type="byte" desc="0-ºÃÓÑÉêÇë£¬1-»ï°éÉêÇë"/> 
	COLUMN_FRIEND_APPLY_REC_APPLY_TYPE, 
 
	// <column type="dword" desc="vipµÈ¼¶"/> 
	COLUMN_FRIEND_APPLY_REC_VIP_LEVEL, 
 
}; 
 
 
//<record name="friend_recommend_mid_rec" 
//	cols="8"  
//	maxrows="100"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="·ûºÏºÃÓÑÍÆ¼öÌõ¼þµÄ±í(²»±£´æ)"/> 
 
#define FIELD_RECORD_FRIEND_RECOMMEND_MID_REC			"friend_recommend_mid_rec" 
// friend_recommend_mid_rec column index define 
enum COLUMNS_OF_FRIEND_RECOMMEND_MID_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_FRIEND_RECOMMEND_MID_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_FRIEND_RECOMMEND_MID_REC_NAME, 
 
	// <column type="word" desc="¶Ô·½µÈ¼¶"/> 
	COLUMN_FRIEND_RECOMMEND_MID_REC_LEVEL, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_FRIEND_RECOMMEND_MID_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_FRIEND_RECOMMEND_MID_REC_SEX, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_FRIEND_RECOMMEND_MID_REC_POWER, 
 
	// <column type="widestr" desc="°ï»áÃû³Æ"/> 
	COLUMN_FRIEND_RECOMMEND_MID_REC_GUILD_NAME, 
 
	// <column type="dword" desc="vipµÈ¼¶"/> 
	COLUMN_FRIEND_RECOMMEND_MID_REC_VIP_LEVEL, 
 
}; 
 
 
//<record name="friend_invitation_send_rec" 
//	cols="1"  
//	maxrows="100"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÒÑ¾­·¢³öµÄÑûÇë(²»±£´æ)"/> 
 
#define FIELD_RECORD_FRIEND_INVITATION_SEND_REC			"friend_invitation_send_rec" 
// friend_invitation_send_rec column index define 
enum COLUMNS_OF_FRIEND_INVITATION_SEND_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_FRIEND_INVITATION_SEND_REC_UID, 
 
}; 
 
 
//<record name="friend_recommend_rec" 
//	cols="8"  
//	maxrows="5"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ºÃÓÑÍÆ¼ö±í(²»±£´æ)"/> 
 
#define FIELD_RECORD_FRIEND_RECOMMEND_REC			"friend_recommend_rec" 
// friend_recommend_rec column index define 
enum COLUMNS_OF_FRIEND_RECOMMEND_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_FRIEND_RECOMMEND_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_FRIEND_RECOMMEND_REC_NAME, 
 
	// <column type="word" desc="¶Ô·½µÈ¼¶"/> 
	COLUMN_FRIEND_RECOMMEND_REC_LEVEL, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_FRIEND_RECOMMEND_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_FRIEND_RECOMMEND_REC_SEX, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_FRIEND_RECOMMEND_REC_POWER, 
 
	// <column type="widestr" desc="°ï»áÃû³Æ"/> 
	COLUMN_FRIEND_RECOMMEND_REC_GUILD_NAME, 
 
	// <column type="dword" desc="vipµÈ¼¶"/> 
	COLUMN_FRIEND_RECOMMEND_REC_VIP_LEVEL, 
 
}; 
 
 
//<record name="nearby_recommend_rec" 
//	cols="9"  
//	maxrows="20"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½½üÍÆ¼ö±í(²»±£´æ)"/> 
 
#define FIELD_RECORD_NEARBY_RECOMMEND_REC			"nearby_recommend_rec" 
// nearby_recommend_rec column index define 
enum COLUMNS_OF_NEARBY_RECOMMEND_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_NEARBY_RECOMMEND_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_NEARBY_RECOMMEND_REC_NAME, 
 
	// <column type="word" desc="¶Ô·½µÈ¼¶"/> 
	COLUMN_NEARBY_RECOMMEND_REC_LEVEL, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_NEARBY_RECOMMEND_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_NEARBY_RECOMMEND_REC_SEX, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_NEARBY_RECOMMEND_REC_POWER, 
 
	// <column type="widestr" desc="°ï»áÃû³Æ"/> 
	COLUMN_NEARBY_RECOMMEND_REC_GUILD_NAME, 
 
	// <column type="dword" desc="ºÃÓÑteamID"/> 
	COLUMN_NEARBY_RECOMMEND_REC_TEAM_ID, 
 
	// <column type="dword" desc="vipµÈ¼¶"/> 
	COLUMN_NEARBY_RECOMMEND_REC_VIP_LEVEL, 
 
}; 
 
 
//<record name="system_recommend_rec" 
//	cols="9"  
//	maxrows="20"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="ÏµÍ³ÍÆ¼öºÃÓÑ(²»±£´æ)"/> 
 
#define FIELD_RECORD_SYSTEM_RECOMMEND_REC			"system_recommend_rec" 
// system_recommend_rec column index define 
enum COLUMNS_OF_SYSTEM_RECOMMEND_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_NAME, 
 
	// <column type="word" desc="¶Ô·½µÈ¼¶"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_LEVEL, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_SEX, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_POWER, 
 
	// <column type="widestr" desc="°ï»áÃû³Æ"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_GUILD_NAME, 
 
	// <column type="dword" desc="ºÃÓÑteamID"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_TEAM_ID, 
 
	// <column type="dword" desc="vipµÈ¼¶"/> 
	COLUMN_SYSTEM_RECOMMEND_REC_VIP_LEVEL, 
 
}; 
 
 
//<record name="friend_receive_flower_rec" 
//	cols="5"  
//	maxrows="20"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÊÕ»¨¼ÇÂ¼±í"/> 
 
#define FIELD_RECORD_FRIEND_RECEIVE_FLOWER_REC			"friend_receive_flower_rec" 
// friend_receive_flower_rec column index define 
enum COLUMNS_OF_FRIEND_RECEIVE_FLOWER_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_FRIEND_RECEIVE_FLOWER_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_FRIEND_RECEIVE_FLOWER_REC_NAME, 
 
	// <column type="word" desc="ËÍ»¨Êý"/> 
	COLUMN_FRIEND_RECEIVE_FLOWER_REC_COUNT, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_FRIEND_RECEIVE_FLOWER_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_FRIEND_RECEIVE_FLOWER_REC_SEX, 
 
}; 
 
 
//<record name="friend_send_flower_rec" 
//	cols="5"  
//	maxrows="20"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ËÍ»¨¼ÇÂ¼±í"/> 
 
#define FIELD_RECORD_FRIEND_SEND_FLOWER_REC			"friend_send_flower_rec" 
// friend_send_flower_rec column index define 
enum COLUMNS_OF_FRIEND_SEND_FLOWER_REC 
{ 
 
	// <column type="string" desc="¶Ô·½UID"/> 
	COLUMN_FRIEND_SEND_FLOWER_REC_UID, 
 
	// <column type="widestr" desc="¶Ô·½Ãû³Æ"/> 
	COLUMN_FRIEND_SEND_FLOWER_REC_NAME, 
 
	// <column type="word" desc="ËÍ»¨Êý"/> 
	COLUMN_FRIEND_SEND_FLOWER_REC_COUNT, 
 
	// <column type="byte" desc="¶Ô·½Ö°Òµ"/> 
	COLUMN_FRIEND_SEND_FLOWER_REC_JOB, 
 
	// <column type="byte" desc="¶Ô·½ÐÔ±ð"/> 
	COLUMN_FRIEND_SEND_FLOWER_REC_SEX, 
 
}; 
 
 
//<record name="friend_applay_sworn_rec" 
//	cols="1"  
//	maxrows="50"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÇëÇó½áÒå"/> 
 
#define FIELD_RECORD_FRIEND_APPLAY_SWORN_REC			"friend_applay_sworn_rec" 
// friend_applay_sworn_rec column index define 
enum COLUMNS_OF_FRIEND_APPLAY_SWORN_REC 
{ 
 
	// <column type="widestr" desc="¶Ô·½Name"/> 
	COLUMN_FRIEND_APPLAY_SWORN_REC_NAME, 
 
}; 
 
 
//<record name="guild_skill_rec" 
//	cols="1"  
//	maxrows="100"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="×éÖ¯¼¼ÄÜ"/> 
 
#define FIELD_RECORD_GUILD_SKILL_REC			"guild_skill_rec" 
// guild_skill_rec column index define 
enum COLUMNS_OF_GUILD_SKILL_REC 
{ 
 
	// <column type="dword" desc="»¥ÖúË÷Òý"/> 
	COLUMN_GUILD_SKILL_REC_0000, 
 
}; 
 
 
//<record name="guild_teach_req_rec" 
//	cols="3"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="´«¹¦ÇëÇó±í"/> 
 
#define FIELD_RECORD_GUILD_TEACH_REQ_REC			"guild_teach_req_rec" 
// guild_teach_req_rec column index define 
enum COLUMNS_OF_GUILD_TEACH_REQ_REC 
{ 
 
	// <column type="widestr" desc="¶Ô·½êÇ³Æ"/> 
	COLUMN_GUILD_TEACH_REQ_REC_NAME, 
 
	// <column type="dword" desc="ÇëÇóÀàÐÍ"/> 
	COLUMN_GUILD_TEACH_REQ_REC_TYPE, 
 
	// <column type="int64" desc="ÇëÇóÊ±¼ä"/> 
	COLUMN_GUILD_TEACH_REQ_REC_TIME, 
 
}; 
 
 
//<record name="shop_manager_rec" 
//	cols="7"  
//	maxrows="5"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÉÌµê¹ÜÀí±í"/> 
 
#define FIELD_RECORD_SHOP_MANAGER_REC			"shop_manager_rec" 
// shop_manager_rec column index define 
enum COLUMNS_OF_SHOP_MANAGER_REC 
{ 
 
	// <column type="byte" desc="ÉÌµêÀàÐÍ"/> 
	COLUMN_SHOP_MANAGER_REC_0000, 
 
	// <column type="byte" desc="µ±Ç°Ë¢ÐÂ´ÎÊý"/> 
	COLUMN_SHOP_MANAGER_REC_0001, 
 
	// <column type="byte" desc="µ±Ç°¸¶·ÑË¢ÐÂ´ÎÊý"/> 
	COLUMN_SHOP_MANAGER_REC_0002, 
 
	// <column type="byte" desc="Ãâ·ÑË¢ÐÂ×î´ó´ÎÊý"/> 
	COLUMN_SHOP_MANAGER_REC_0003, 
 
	// <column type="byte" desc="ÉÌµêÊ¹ÓÃµÄ»õ±ÒÀàÐÍ"/> 
	COLUMN_SHOP_MANAGER_REC_0004, 
 
	// <column type="dword" desc="Ë¢ÐÂ¼Û¸ñ"/> 
	COLUMN_SHOP_MANAGER_REC_0005, 
 
	// <column type="byte" desc="ÊÇ·ñµÚÒ»´ÎÊÖ¶¯Ë¢ÐÂ"/> 
	COLUMN_SHOP_MANAGER_REC_0006, 
 
}; 
 
 
//<record name="shop_gold_rec" 
//	cols="6"  
//	maxrows="12"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="Ôª±¦ÉÌµêË¢ÐÂÎïÆ·±í"/> 
 
#define FIELD_RECORD_SHOP_GOLD_REC			"shop_gold_rec" 
// shop_gold_rec column index define 
enum COLUMNS_OF_SHOP_GOLD_REC 
{ 
 
	// <column type="string" desc="ÎïÆ·ID"/> 
	COLUMN_SHOP_GOLD_REC_0000, 
 
	// <column type="dword" desc="ÎïÆ·Æ·ÖÊ"/> 
	COLUMN_SHOP_GOLD_REC_0001, 
 
	// <column type="dword" desc="ÎïÆ·ÊýÄ¿"/> 
	COLUMN_SHOP_GOLD_REC_0002, 
 
	// <column type="dword" desc="¼Û¸ñ"/> 
	COLUMN_SHOP_GOLD_REC_0003, 
 
	// <column type="dword" desc="Ôª±¦¼Û¸ñ"/> 
	COLUMN_SHOP_GOLD_REC_0004, 
 
	// <column type="float" desc="ÕÛ¿Û"/> 
	COLUMN_SHOP_GOLD_REC_0005, 
 
}; 
 
 
//<record name="shop_commerce_purchase_record" 
//	cols="3"  
//	maxrows="1024"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¹ºÂòÉÌ»áÎïÆ·¼ÇÂ¼"/> 
 
#define FIELD_RECORD_SHOP_COMMERCE_PURCHASE_RECORD			"shop_commerce_purchase_record" 
// shop_commerce_purchase_record column index define 
enum COLUMNS_OF_SHOP_COMMERCE_PURCHASE_RECORD 
{ 
 
	// <column type="string" desc="ÎïÆ·Ë÷Òý"/> 
	COLUMN_SHOP_COMMERCE_PURCHASE_RECORD_ITEM_ID, 
 
	// <column type="dword" desc="ÒÑ¾­¹ºÂòµÄ¸öÊý"/> 
	COLUMN_SHOP_COMMERCE_PURCHASE_RECORD_PUSRCHASE_NUM, 
 
	// <column type="dword" desc="ÒÑ¾­³öÊÛµÄ¸öÊý"/> 
	COLUMN_SHOP_COMMERCE_PURCHASE_RECORD_SALED_NUM, 
 
}; 
 
 
//<record name="shop_buy_plus_item_record" 
//	cols="4"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="¹ºÂòÔËÓªÀñ°ü¼ÇÂ¼"/> 
 
#define FIELD_RECORD_SHOP_BUY_PLUS_ITEM_RECORD			"shop_buy_plus_item_record" 
// shop_buy_plus_item_record column index define 
enum COLUMNS_OF_SHOP_BUY_PLUS_ITEM_RECORD 
{ 
 
	// <column type="string" desc="ÎïÆ·Ë÷Òý"/> 
	COLUMN_SHOP_BUY_PLUS_ITEM_RECORD_0000, 
 
	// <column type="dword" desc="ÒÑ¾­¹ºÂòµÄ¸öÊý"/> 
	COLUMN_SHOP_BUY_PLUS_ITEM_RECORD_0001, 
 
	// <column type="int64" desc="Ê±¼ä´Á"/> 
	COLUMN_SHOP_BUY_PLUS_ITEM_RECORD_0002, 
 
	// <column type="dword" desc="Ë¢ÐÂÀàÐÍ"/> 
	COLUMN_SHOP_BUY_PLUS_ITEM_RECORD_0003, 
 
}; 
 
 
//<record name="payed_order" 
//	cols="8"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="¹ºÂòÖ§¸¶¼ÇÂ¼(¼ÇÂ¼×î½üµÄ1024Ìõ)"/> 
 
#define FIELD_RECORD_PAYED_ORDER			"payed_order" 
// payed_order column index define 
enum COLUMNS_OF_PAYED_ORDER 
{ 
 
	// <column type="string" desc="¶©µ¥id"/> 
	COLUMN_PAYED_ORDER_ORDER_ID, 
 
	// <column type="string" desc="ÉÌÆ·id"/> 
	COLUMN_PAYED_ORDER_PRODUCT_ID, 
 
	// <column type="string" desc="ÉÌÆ·Êý¾Ý"/> 
	COLUMN_PAYED_ORDER_ITEMS, 
 
	// <column type="string" desc="ÔùËÍÎïÆ·"/> 
	COLUMN_PAYED_ORDER_REWARDS, 
 
	// <column type="string" desc="Ê×³äÀñ°ü"/> 
	COLUMN_PAYED_ORDER_GIFTS, 
 
	// <column type="float" desc="Ö§¸¶µÄ½ð¶î"/> 
	COLUMN_PAYED_ORDER_AMOUNT, 
 
	// <column type="int64" desc="¶©µ¥´´½¨Ê±¼ä"/> 
	COLUMN_PAYED_ORDER_CREATE_TIME, 
 
	// <column type="int64" desc="¶©µ¥Íê³ÉÊ±¼ä"/> 
	COLUMN_PAYED_ORDER_PAYED_TIME, 
 
}; 
 
 
//<record name="payed_products" 
//	cols="4"  
//	maxrows="256"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="¹ºÂò¹ýµÄÎïÆ·"/> 
 
#define FIELD_RECORD_PAYED_PRODUCTS			"payed_products" 
// payed_products column index define 
enum COLUMNS_OF_PAYED_PRODUCTS 
{ 
 
	// <column type="string" desc="ÉÌÆ·id"/> 
	COLUMN_PAYED_PRODUCTS_PRODUCT_ID, 
 
	// <column type="int" desc="×Ü¹ºÂò´ÎÊý"/> 
	COLUMN_PAYED_PRODUCTS_TATAL_TIMES, 
 
	// <column type="int" desc="¹ºÂò´ÎÊý"/> 
	COLUMN_PAYED_PRODUCTS_PURCHASE_TIMES, 
 
	// <column type="int64" desc="×îºóÒ»´ÎÖ§¸¶Íê³ÉµÄÏÂµ¥Ê±¼ä"/> 
	COLUMN_PAYED_PRODUCTS_ORDER_TIME, 
 
}; 
 
 
//<record name="item_drop_record" 
//	cols="3"  
//	maxrows="30"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="±£´æÍæ¼ÒµôÂäÎïÆ·"/> 
 
#define FIELD_RECORD_ITEM_DROP_RECORD			"item_drop_record" 
// item_drop_record column index define 
enum COLUMNS_OF_ITEM_DROP_RECORD 
{ 
 
	// <column type="string" desc="µôÂäÎïÆ·id"/> 
	COLUMN_ITEM_DROP_RECORD_0000, 
 
	// <column type="int64" desc="µôÂäÎïÆ·ÊýÁ¿"/> 
	COLUMN_ITEM_DROP_RECORD_0001, 
 
	// <column type="WORD" desc="µôÂäÎïÆ·cdÊ±¼ä"/> 
	COLUMN_ITEM_DROP_RECORD_0002, 
 
}; 
 
 
//<record name="system_mail_rec" 
//	cols="12"  
//	maxrows="256"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÓÊ¼þ±í"/> 
 
#define FIELD_RECORD_SYSTEM_MAIL_REC			"system_mail_rec" 
// system_mail_rec column index define 
enum COLUMNS_OF_SYSTEM_MAIL_REC 
{ 
 
	// <column type="string" desc="ÐÅ¼þÁ÷Ë®ºÅ"/> 
	COLUMN_SYSTEM_MAIL_REC_0000, 
 
	// <column type="widestr" desc="·¢¼þÈË"/> 
	COLUMN_SYSTEM_MAIL_REC_0001, 
 
	// <column type="int64" desc="´æÔÚÊ±¼ä"/> 
	COLUMN_SYSTEM_MAIL_REC_0002, 
 
	// <column type="int64" desc="ÓÊ¼ÄÊ±¼ä"/> 
	COLUMN_SYSTEM_MAIL_REC_0003, 
 
	// <column type="byte" desc="ÐÅ¼þÀàÐÍ"/> 
	COLUMN_SYSTEM_MAIL_REC_0004, 
 
	// <column type="widestr" desc="ÐÅ¼þÌâÄ¿"/> 
	COLUMN_SYSTEM_MAIL_REC_0005, 
 
	// <column type="widestr" desc="ÐÅ¼þÄÚÈÝ"/> 
	COLUMN_SYSTEM_MAIL_REC_0006, 
 
	// <column type="string" desc="½ðÇ®¸½¼þ"/> 
	COLUMN_SYSTEM_MAIL_REC_0007, 
 
	// <column type="string" desc="ÎïÆ·¸½¼þ"/> 
	COLUMN_SYSTEM_MAIL_REC_0008, 
 
	// <column type="byte" desc="ÒÑ¶Á±ê¼Ç"/> 
	COLUMN_SYSTEM_MAIL_REC_0009, 
 
	// <column type="byte" desc="¸½¼þÌáÈ¡±ê¼Ç"/> 
	COLUMN_SYSTEM_MAIL_REC_0010, 
 
	// <column type="string" desc="ÊôÐÔ¸½¼þ"/> 
	COLUMN_SYSTEM_MAIL_REC_0011, 
 
}; 
 
 
//<record name="reset_timer_rec" 
//	cols="4"  
//	maxrows="250"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="¶¨Ê±Ë¢ÐÂ±í"/> 
 
#define FIELD_RECORD_RESET_TIMER_REC			"reset_timer_rec" 
// reset_timer_rec column index define 
enum COLUMNS_OF_RESET_TIMER_REC 
{ 
 
	// <column type="dword" desc="Ë¢ÐÂÀàÐÍ"/> 
	COLUMN_RESET_TIMER_REC_TYPE, 
 
	// <column type="word" desc="ÉÏ´ÎË¢ÐÂweekday"/> 
	COLUMN_RESET_TIMER_REC_WEEKDAY, 
 
	// <column type="word" desc="ÉÏ´ÎË¢ÐÂHour"/> 
	COLUMN_RESET_TIMER_REC_HOUR, 
 
	// <column type="double" desc="ÉÏ´ÎË¢ÐÂDate"/> 
	COLUMN_RESET_TIMER_REC_DATE, 
 
}; 
 
 
//<record name="TaskProgressRec" 
//	cols="3"  
//	maxrows="512"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÈÎÎñ½ø¶È±í"/> 
 
#define FIELD_RECORD_TASK_PROGRESS_REC			"TaskProgressRec" 
// TaskProgressRec column index define 
enum COLUMNS_OF_TASK_PROGRESS_REC 
{ 
 
	// <column type="dword" desc="ÈÎÎñ±àºÅ"/> 
	COLUMN_TASK_PROGRESS_REC_ID, 
 
	// <column type="word" desc="ÈÎÎñ×´Ì¬"/> 
	COLUMN_TASK_PROGRESS_REC_STATUS, 
 
	// <column type="string" desc="µ±Ç°½ø¶È"/> 
	COLUMN_TASK_PROGRESS_REC_PROGRESS, 
 
}; 
 
 
//<record name="TaskSubmitRec" 
//	cols="1"  
//	maxrows="5000"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÒÑÌá½»ÈÎÎñ±í"/> 
 
#define FIELD_RECORD_TASK_SUBMIT_REC			"TaskSubmitRec" 
// TaskSubmitRec column index define 
enum COLUMNS_OF_TASK_SUBMIT_REC 
{ 
 
	// <column type="dword" desc="ÈÎÎñ±àºÅ"/> 
	COLUMN_TASK_SUBMIT_REC_ID, 
 
}; 
 
 
//<record name="TaskNpcRec" 
//	cols="1"  
//	maxrows="512"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="ÈÎÎñNPC¼ÇÂ¼±í"/> 
 
#define FIELD_RECORD_TASK_NPC_REC			"TaskNpcRec" 
// TaskNpcRec column index define 
enum COLUMNS_OF_TASK_NPC_REC 
{ 
 
	// <column type="object" desc="ÈÎÎñNPC¶ÔÏó"/> 
	COLUMN_TASK_NPC_REC_OBJECT, 
 
}; 
 
 
//<record name="TaskConfigRec" 
//	cols="2"  
//	maxrows="50"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÈÎÎñÅäÖÃ±í"/> 
 
#define FIELD_RECORD_TASK_CONFIG_REC			"TaskConfigRec" 
// TaskConfigRec column index define 
enum COLUMNS_OF_TASK_CONFIG_REC 
{ 
 
	// <column type="word" desc="ÈÎÎñÀàÐÍ"/> 
	COLUMN_TASK_CONFIG_REC_TYPE, 
 
	// <column type="word" desc="ÒÑÍê³É´ÎÊý"/> 
	COLUMN_TASK_CONFIG_REC_COUNT, 
 
}; 
 
 
//<record name="team_rec" 
//	cols="22"  
//	maxrows="4"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="¶ÓÎé³ÉÔ±ÁÐ±í"/> 
 
#define FIELD_RECORD_TEAM_REC			"team_rec" 
// team_rec column index define 
enum COLUMNS_OF_TEAM_REC 
{ 
 
	// <column type="string" desc="UID"/> 
	COLUMN_TEAM_REC_0000, 
 
	// <column type="widestr" desc="¶ÓÔ±Ãû"/> 
	COLUMN_TEAM_REC_0001, 
 
	// <column type="dword" desc="µÈ¼¶"/> 
	COLUMN_TEAM_REC_0002, 
 
	// <column type="byte" desc="Ö°Òµ"/> 
	COLUMN_TEAM_REC_0003, 
 
	// <column type="byte" desc="ÐÔ±ð"/> 
	COLUMN_TEAM_REC_0004, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_TEAM_REC_0005, 
 
	// <column type="int64" desc="ÉÏ´ÎÀëÏßÊ±¼ä"/> 
	COLUMN_TEAM_REC_0006, 
 
	// <column type="byte" desc="¶ÓÎéµÄÖ°Î»"/> 
	COLUMN_TEAM_REC_0007, 
 
	// <column type="dword" desc="³¡¾°±àºÅ"/> 
	COLUMN_TEAM_REC_0008, 
 
	// <column type="string" desc="Buffers"/> 
	COLUMN_TEAM_REC_0009, 
 
	// <column type="int64" desc="HP"/> 
	COLUMN_TEAM_REC_0010, 
 
	// <column type="widestr" desc="¹«»áÃû³Æ"/> 
	COLUMN_TEAM_REC_0011, 
 
	// <column type="int64" desc="MaxHP"/> 
	COLUMN_TEAM_REC_0012, 
 
	// <column type="byte" desc="¸úËæ×´Ì¬"/> 
	COLUMN_TEAM_REC_0013, 
 
	// <column type="byte" desc="×¼±¸×´Ì¬"/> 
	COLUMN_TEAM_REC_0014, 
 
	// <column type="dword" desc="¸±±¾ID"/> 
	COLUMN_TEAM_REC_0015, 
 
	// <column type="byte" desc="Online"/> 
	COLUMN_TEAM_REC_0016, 
 
	// <column type="float" desc="x×ø±ê"/> 
	COLUMN_TEAM_REC_0017, 
 
	// <column type="float" desc="z×ø±ê"/> 
	COLUMN_TEAM_REC_0018, 
 
	// <column type="byte" desc="ÄÜ·ñ±»Ìß³ö¶ÓÎé"/> 
	COLUMN_TEAM_REC_0019, 
 
	// <column type="byte" desc="Õ½¶·×´Ì¬"/> 
	COLUMN_TEAM_REC_0020, 
 
	// <column type="word" desc="vip"/> 
	COLUMN_TEAM_REC_0021, 
 
}; 
 
 
//<record name="nearby_team_rec" 
//	cols="8"  
//	maxrows="10"  
//	public="false"  
//	private="true"  
//	save="false"  
//	desc="¸½½ü¶ÓÎé±í"/> 
 
#define FIELD_RECORD_NEARBY_TEAM_REC			"nearby_team_rec" 
// nearby_team_rec column index define 
enum COLUMNS_OF_NEARBY_TEAM_REC 
{ 
 
	// <column type="byte" desc="¶ÓÎé±àºÅ"/> 
	COLUMN_NEARBY_TEAM_REC_0000, 
 
	// <column type="widestr" desc="¶Ó³¤Ãû×Ö"/> 
	COLUMN_NEARBY_TEAM_REC_0001, 
 
	// <column type="dword" desc="¶Ó³¤µÈ¼¶"/> 
	COLUMN_NEARBY_TEAM_REC_0002, 
 
	// <column type="byte" desc="¶Ó³¤Ö°Òµ"/> 
	COLUMN_NEARBY_TEAM_REC_0003, 
 
	// <column type="byte" desc="¶Ó³¤ÐÔ±ð"/> 
	COLUMN_NEARBY_TEAM_REC_0004, 
 
	// <column type="dword" desc="¶Ó³¤Õ½¶·Á¦"/> 
	COLUMN_NEARBY_TEAM_REC_0005, 
 
	// <column type="dword" desc="¶ÓÎéÄ¿±ê"/> 
	COLUMN_NEARBY_TEAM_REC_0006, 
 
	// <column type="byte" desc="¶ÓÎéÈËÊý"/> 
	COLUMN_NEARBY_TEAM_REC_0007, 
 
}; 
 
 
//<record name="request_rec" 
//	cols="9"  
//	maxrows="30"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Ìí¼ÓÇëÇóÕß±í(¼ÇÂ¼µ±Ç°Íæ¼ÒËù·¢ËÍ¹ýµÄËùÓÐÇëÇó)"/> 
 
#define FIELD_RECORD_REQUEST_REC			"request_rec" 
// request_rec column index define 
enum COLUMNS_OF_REQUEST_REC 
{ 
 
	// <column type="byte" desc="ÇëÇóË÷Òý"/> 
	COLUMN_REQUEST_REC_0000, 
 
	// <column type="widestr" desc="ÇëÇóÕßÐÕÃû"/> 
	COLUMN_REQUEST_REC_0001, 
 
	// <column type="byte" desc="ÇëÇóÀàÐÍ"/> 
	COLUMN_REQUEST_REC_0002, 
 
	// <column type="byte" desc="ÇëÇóÊ±¼ä"/> 
	COLUMN_REQUEST_REC_0003, 
 
	// <column type="byte" desc="ÇëÇó×´Ì¬"/> 
	COLUMN_REQUEST_REC_0004, 
 
	// <column type="byte" desc="Ö°Òµ"/> 
	COLUMN_REQUEST_REC_0005, 
 
	// <column type="dword" desc="Õ½¶·Á¦"/> 
	COLUMN_REQUEST_REC_0006, 
 
	// <column type="word" desc="µÈ¼¶"/> 
	COLUMN_REQUEST_REC_0007, 
 
	// <column type="word" desc="ÐÔ±ð"/> 
	COLUMN_REQUEST_REC_0008, 
 
}; 
 
 
//<record name="buff_add_rec" 
//	cols="2"  
//	maxrows="256"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¼ÓbuffÍ³¼Æ±í"/> 
 
#define FIELD_RECORD_BUFF_ADD_REC			"buff_add_rec" 
// buff_add_rec column index define 
enum COLUMNS_OF_BUFF_ADD_REC 
{ 
 
	// <column type="int64" desc="¼ÓbuffµÄÊ±¼ä"/> 
	COLUMN_BUFF_ADD_REC_0000, 
 
	// <column type="dword" desc="debuffµÄÀàÐÍ"/> 
	COLUMN_BUFF_ADD_REC_0001, 
 
}; 
 
 
//<record name="flow_hittime_rec" 
//	cols="2"  
//	maxrows="64"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc=""/> 
 
#define FIELD_RECORD_FLOW_HITTIME_REC			"flow_hittime_rec" 
// flow_hittime_rec column index define 
enum COLUMNS_OF_FLOW_HITTIME_REC 
{ 
 
	// <column type="word" desc=""/> 
	COLUMN_FLOW_HITTIME_REC_0000, 
 
	// <column type="word" desc=""/> 
	COLUMN_FLOW_HITTIME_REC_0001, 
 
}; 
 
 
//<record name="flow_mutli_hit_timer_rec" 
//	cols="2"  
//	maxrows="64"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc=""/> 
 
#define FIELD_RECORD_FLOW_MUTLI_HIT_TIMER_REC			"flow_mutli_hit_timer_rec" 
// flow_mutli_hit_timer_rec column index define 
enum COLUMNS_OF_FLOW_MUTLI_HIT_TIMER_REC 
{ 
 
	// <column type="int64" desc="¿ªÊ¼Ê±¼ä"/> 
	COLUMN_FLOW_MUTLI_HIT_TIMER_REC_0000, 
 
	// <column type="int64" desc="¶¨Ê±Ê±¼ä"/> 
	COLUMN_FLOW_MUTLI_HIT_TIMER_REC_0001, 
 
}; 
 
 
//<record name="cooldown_rec" 
//	cols="3"  
//	maxrows="36"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÀäÈ´±í"/> 
 
#define FIELD_RECORD_COOLDOWN_REC			"cooldown_rec" 
// cooldown_rec column index define 
enum COLUMNS_OF_COOLDOWN_REC 
{ 
 
	// <column type="dword" desc="ÀäÈ´·ÖÀàID"/> 
	COLUMN_COOLDOWN_REC_0000, 
 
	// <column type="int64" desc="ÀäÈ´¿ªÊ¼Ê±¼ä£¬ÏÖÊµ64Î»Ê±¼äÊý¾Ý"/> 
	COLUMN_COOLDOWN_REC_0001, 
 
	// <column type="int64" desc="ÀäÈ´½áÊøÊ±¼ä£¬ÏÖÊµ64Î»Ê±¼äÊý¾Ý"/> 
	COLUMN_COOLDOWN_REC_0002, 
 
}; 
 
 
//<record name="PropModifyRec" 
//	cols="3"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="¶ÔÏóÊôÐÔÐÞÕý±í"/> 
 
#define FIELD_RECORD_PROP_MODIFY_REC			"PropModifyRec" 
// PropModifyRec column index define 
enum COLUMNS_OF_PROP_MODIFY_REC 
{ 
 
	// <column type="string" desc="ÐÞÕýÊôÐÔÃû"/> 
	COLUMN_PROP_MODIFY_REC_0000, 
 
	// <column type="float" desc="ÐÞÕýÊôÐÔÖµ"/> 
	COLUMN_PROP_MODIFY_REC_0001, 
 
	// <column type="string" desc="ÐÞÕýÀ´Ô´"/> 
	COLUMN_PROP_MODIFY_REC_0002, 
 
}; 
 
 
//<record name="passive_skill_rec" 
//	cols="3"  
//	maxrows="1000"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc=""/> 
 
#define FIELD_RECORD_PASSIVE_SKILL_REC			"passive_skill_rec" 
// passive_skill_rec column index define 
enum COLUMNS_OF_PASSIVE_SKILL_REC 
{ 
 
	// <column type="DWORD" desc="¼¼ÄÜid"/> 
	COLUMN_PASSIVE_SKILL_REC_0000, 
 
	// <column type="DWORD" desc="¼¼ÄÜµÈ¼¶"/> 
	COLUMN_PASSIVE_SKILL_REC_0001, 
 
	// <column type="DWORD" desc="±»¶¯¼¼ÄÜÀ´Ô´"/> 
	COLUMN_PASSIVE_SKILL_REC_0002, 
 
}; 
 
 
//<record name="PropModifyByEquip" 
//	cols="3"  
//	maxrows="300"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="ÊôÐÔË¢ÐÂ±í"/> 
 
#define FIELD_RECORD_PROP_MODIFY_BY_EQUIP			"PropModifyByEquip" 
// PropModifyByEquip column index define 
enum COLUMNS_OF_PROP_MODIFY_BY_EQUIP 
{ 
 
	// <column type="string" desc="ÊôÐÔÃû"/> 
	COLUMN_PROP_MODIFY_BY_EQUIP_0000, 
 
	// <column type="float" desc="ÊôÐÔÖµ"/> 
	COLUMN_PROP_MODIFY_BY_EQUIP_0001, 
 
	// <column type="string" desc="À´Ô´"/> 
	COLUMN_PROP_MODIFY_BY_EQUIP_0002, 
 
}; 
 
 
//<record name="fashion_unlock_rec" 
//	cols="4"  
//	maxrows="100"  
//	public="false"  
//	private="true"  
//	save="true"  
//	desc="ÒÑ½âËøÊ±×°±í"/> 
 
#define FIELD_RECORD_FASHION_UNLOCK_REC			"fashion_unlock_rec" 
// fashion_unlock_rec column index define 
enum COLUMNS_OF_FASHION_UNLOCK_REC 
{ 
 
	// <column type="word" desc="Ê±×°id"/> 
	COLUMN_FASHION_UNLOCK_REC_ID, 
 
	// <column type="word" desc="Ê±×°ÀàÐÍ"/> 
	COLUMN_FASHION_UNLOCK_REC_TYPE, 
 
	// <column type="int64" desc="½âËøÊ±¼ä"/> 
	COLUMN_FASHION_UNLOCK_REC_UNLOCK_TIME, 
 
	// <column type="int64" desc="Ê±Ð§Ê±¼ä"/> 
	COLUMN_FASHION_UNLOCK_REC_VALID_TIME, 
 
}; 
 
 
//<record name="ApperanceRec" 
//	cols="3"  
//	maxrows="20"  
//	public="false"  
//	private="false"  
//	save="true"  
//	desc="Íâ¹Û¹ÜÀí"/> 
 
#define FIELD_RECORD_APPERANCE_REC			"ApperanceRec" 
// ApperanceRec column index define 
enum COLUMNS_OF_APPERANCE_REC 
{ 
 
	// <column type="WORD" desc="Íâ¹Ûid"/> 
	COLUMN_APPERANCE_REC_ID, 
 
	// <column type="BYTE" desc="Íâ¹ÛÀ´Ô´"/> 
	COLUMN_APPERANCE_REC_SOURCE, 
 
	// <column type="WORD" desc="Íâ¹ÛÀàÐÍ£¨×°±¸²¿Î»£©"/> 
	COLUMN_APPERANCE_REC_TYPE, 
 
}; 
 
 
//<record name="creator_id_rec" 
//	cols="2"  
//	maxrows="4096"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="Éú³ÉÆ÷ºÍid¶ÔÓ¦±í"/> 
 
#define FIELD_RECORD_CREATOR_ID_REC			"creator_id_rec" 
// creator_id_rec column index define 
enum COLUMNS_OF_CREATOR_ID_REC 
{ 
 
	// <column type="string" desc="Éú³ÉÆ÷id"/> 
	COLUMN_CREATOR_ID_REC_0000, 
 
	// <column type="object" desc="Éú³ÉÆ÷¶ÔÏó"/> 
	COLUMN_CREATOR_ID_REC_0001, 
 
}; 
 
 
//<record name="create_group_static_flow" 
//	cols="4"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="·Ö×é³¡¾°¾²Ì¬npc´´½¨»º´æ"/> 
 
#define FIELD_RECORD_CREATE_GROUP_STATIC_FLOW			"create_group_static_flow" 
// create_group_static_flow column index define 
enum COLUMNS_OF_CREATE_GROUP_STATIC_FLOW 
{ 
 
	// <column type="word" desc="SubSceneNo"/> 
	COLUMN_CREATE_GROUP_STATIC_FLOW_0000, 
 
	// <column type="word" desc="·Ö×éºÅ"/> 
	COLUMN_CREATE_GROUP_STATIC_FLOW_0001, 
 
	// <column type="word" desc="ÕýÔÚ´´½¨µÄÎÄ¼þ±àºÅ"/> 
	COLUMN_CREATE_GROUP_STATIC_FLOW_0002, 
 
	// <column type="word" desc="ÕýÔÚ´´½¨µÄnpcË÷Òý"/> 
	COLUMN_CREATE_GROUP_STATIC_FLOW_0003, 
 
}; 
 
 
//<record name="create_group_random_flow" 
//	cols="4"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="·Ö×é³¡¾°Ëæ»úÉú³ÉÆ÷´´½¨»º´æ"/> 
 
#define FIELD_RECORD_CREATE_GROUP_RANDOM_FLOW			"create_group_random_flow" 
// create_group_random_flow column index define 
enum COLUMNS_OF_CREATE_GROUP_RANDOM_FLOW 
{ 
 
	// <column type="word" desc="SubSceneNo"/> 
	COLUMN_CREATE_GROUP_RANDOM_FLOW_0000, 
 
	// <column type="word" desc="·Ö×éºÅ"/> 
	COLUMN_CREATE_GROUP_RANDOM_FLOW_0001, 
 
	// <column type="word" desc="ÕýÔÚ´´½¨µÄÎÄ¼þ±àºÅ"/> 
	COLUMN_CREATE_GROUP_RANDOM_FLOW_0002, 
 
	// <column type="word" desc="ÕýÔÚ´´½¨µÄnpcË÷Òý"/> 
	COLUMN_CREATE_GROUP_RANDOM_FLOW_0003, 
 
}; 
 
 
//<record name="kiki_rec" 
//	cols="1"  
//	maxrows="1024"  
//	public="false"  
//	private="false"  
//	save="false"  
//	desc="²âÊÔ±í¸ñ"/> 
 
#define FIELD_RECORD_KIKI_REC			"kiki_rec" 
// kiki_rec column index define 
enum COLUMNS_OF_KIKI_REC 
{ 
 
	// <column type="int" desc="²âÊÔÁÐ"/> 
	COLUMN_KIKI_REC_0000, 
 
}; 
 
 
#endif // _FIELDS_DEFINE_H_