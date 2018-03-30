//--------------------------------------------------------------------
// 文件名:	    SceneListDefine.h
// 内  容:		场景列表相关定义
// 说  明:		
// 创建日期:	2015年7月28日
// 创建人:		 
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __SCENE_LIST_H__
#define __SCENE_LIST_H__

// 记录场景服务器上第一个加载的场景
const char* const SERVER_MEMBER_FIRST_LOAD_SCENE_REC = "member_scene_record";
enum
{
	SERVER_MEMBER_FIRST_LOAD_SCENE_COL_member_id,		// 场景服务器member ID
	SERVER_MEMBER_FIRST_LOAD_SCENE_COL_scene_id,		// 场景ID
	SERVER_MEMBER_FIRST_LOAD_SCENE_COL_TOTAL,
};

// 最大行数
const int SERVER_MEMBER_FIRST_LOAD_SCENE_REC_MAX_ROW = 32;

#endif // __SCENE_LIST_H__