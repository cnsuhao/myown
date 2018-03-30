//--------------------------------------------------------------------
// 文件名:		INetType.h
// 内  容:		网络类型定义
// 说  明:		
// 创建日期:		2016年5月28日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __INET_TYPE_DEFINE_H__
#define __INET_TYPE_DEFINE_H__

#include <stdlib.h>

#if defined( WIN32) || defined( WIN64 )
typedef intptr_t NETFD;
#else
#define NETFD int
#endif

typedef unsigned char	SERID;
typedef long long		SESSID;
typedef SESSID NID;

#define  NULL_NID		0

#endif	// END __INET_TYPE_DEFINE_H__