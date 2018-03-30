//---------------------------------------------------------
//文件名:       LookDefine.h
//内  容:       信息查看定义
//说  明:       
//创建日期:   
//创建人:                
//---------------------------------------------------------
#ifndef _FSGAME_LookDefine_H_
#define _FSGAME_LookDefine_H_

#include <string>

#define SHOWOFF_ITEM_PUB_RECORD_NAME     "ShowoffItemPubRec"
#define SHOWOFF_ITEM_PUB_RECORD_MAX_ROW  500

#define SHOWOFF_ITEM_PUB_RECORD_COL_COUNT        4
#define SHOWOFF_ITEM_PUB_RECORD_ROLE_UID_COL     0
#define SHOWOFF_ITEM_PUB_RECORD_ROLE_NAME_COL    1
#define SHOWOFF_ITEM_PUB_RECORD_UID_COL          2
#define SHOWOFF_ITEM_PUB_RECORD_PROP_COL         3

// 子消息定义
enum
{
    SHOWOFF_PUB_SAVE_ITEM   = 1, // 通知pub服务器,保存炫耀的物品信息
    SHOWOFF_PUB_REMOVE_ITEM = 2,
    SHOWOFF_PUB_CLEAR_ALL   = 3,
};


#endif
