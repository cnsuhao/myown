//---------------------------------------------------------
//文件名:       MapDefine.h
//内  容:       地图模块类型定义
//说  明:       
//
//创建日期:      2015年4月21日
//创建人:         
//修改人:
//   :         
//---------------------------------------------------------

#ifndef __MAP_DEFINE_H__
#define __MAP_DEFINE_H__


// 客户端发送服务器二级消息
enum 
{
	CS_REQUEST_MAP_SWITCH_SCENE = 1,	// 通过地图切场景 int msgid, int submsgid, int sceneid 场景id 
};

#endif // __MAP_SYSINFO_DEFINE_H__