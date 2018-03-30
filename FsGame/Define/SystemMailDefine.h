//--------------------------------------------------------------------
// 文件名:		SystemMailDefine.h
// 内  容:		系统邮件定义
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __SystemMailDefine_H__
#define __SystemMailDefine_H__

#define SYSTEM_MAIL_MAX 100

#define SYSTEM_MAIL_REC "system_mail_rec"
#define GLOBAL_MAIL_REC "global_mail_rec"
//全局邮件索引
#define GLOBAL_MAIL_UID "global_mail_uid"
enum SystemMailRecordCol
{
    MAIL_REC_COL_SERIAL_NO=0,	//流水号 string
    MAIL_REC_COL_SENDER_NAME,	//发送者名字 widestr
    MAIL_REC_COL_LIFE_TIME,		//存在时间 int64
    MAIL_REC_COL_POST_TIME,		//发送时间 int64
    MAIL_REC_COL_MAIL_TYPE,		//邮件类型 byte
    MAIL_REC_COL_TITLE,			//邮件标题 widestr
    MAIL_REC_COL_CONTENT,		//邮件内容 widestr
    MAIL_REC_COL_MONEY,			//附件：金钱 string
    MAIL_REC_COL_ITEM,			//附件：物品 string
    MAIL_REC_COL_READ_FLAG,		//已读标志 byte
    MAIL_REC_COL_PICK_FLAG,		//附件提取标志 byte
	MAIL_REC_COL_PROPERTY,		//附件：属性 string
};

enum
{
	DOMAIN_MAIL_REC_MAIL_ID,			//邮件id
	DOMAIN_MAIL_REC_MAIL_NAME,          //邮件名
	DOMAIN_MAIL_REC_MAIL_OVERTIMER,     //超时时间
	DOMAIN_MAIL_REC_MAIL_CONTEXT,		//内容
	DOMAIN_MAIL_REC_REV_NATION,			//国籍限制(0没有限制 1江湖2朝廷 3有国籍即可)
};

enum 
{
	S2S_COMMAND_MAIL_ADD_MAIL, //添加邮件

};


enum SystemMailSubMsg 
{

    MAIL_SUB_MSG_MAIL_COUNT     = 1, // 获取邮件的数量
    MAIL_SUB_MSG_RETRIEVE       = 2, // 获取邮件列表
    MAIL_SUB_MSG_DELETE         = 3, // 删除一封邮件
    MAIL_SUB_MSG_DELETE_ALL     = 4, // 删除全部邮件
    MAIL_SUB_MSG_PICK           = 5, // 提取一封邮件
    MAIL_SUB_MSG_PICK_ALL       = 6, // 提取全部邮件
    MAIL_SUB_MSG_READ           = 7, // 读取邮件
    MAIL_SUB_MSG_UNREAD_MAIL    = 8, // 有未读邮件
	
};

enum SystemMailResultCode
{
    MAIL_CODE_FAILED                    = 0, // 失败
    MAIL_CODE_SUCCESS                   = 1, // 成功
    MAIL_CODE_ATTACHEMENT_UNPICKED      = 2, // 附件未提取
    MAIL_CODE_INVALID_MAIL              = 3, // 无效邮件
    MAIL_CODE_TOOL_BOX_INSUFFICIENCY    = 4, // 背包空间不足
    MAIL_CODE_INVALID_ATTACHMENT        = 5, // 无效的附件
    MAIL_CODE_MAIL_NOT_FOUND            = 6, // 邮件不存在
    MAIL_CODE_SUCCESS_WITH_INFO         = 7, // 成功但有额外信息
};

enum SystemMailReadFlag
{
    MAIL_FLAG_UNREAD = 0, // 未读邮件
    MAIL_FLAG_READ   = 1, // 已读邮件
};

enum SystemMailPickFlag
{
    MAIL_FLAG_UNPICKED = 0, // 附件未提取
    MAIL_FLAG_PICKED   = 1, // 附件已提取
};

enum SystemMailAttachmentFlag
{
    MAIL_FLAG_NO_ATTACHMENT     = 0, // 没有附件
    MAIL_FLAG_HAS_ATTACHMENT    = 1, // 有附件
};

enum SystemMailInnerCode
{

};

#endif //__SystemMailDefine_H__
