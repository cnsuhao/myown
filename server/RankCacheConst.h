/********************************************
* File  :		RankCacheConst.h
* Author:		Eddie LIU
* Date  :		2015-7-27
* 
* Copyright:	Snail Game (ltao@snail.com)
********************************************/

#ifndef _RANK_CACHE_CONST_H
#define _RANK_CACHE_CONST_H

// 请求类型
enum{
	REQ_SET_SCORE = 0,		// 设置排行榜用户积分
	REQ_SET_EXTRA,			// 设置排行榜用户附加数据
	REQ_GET_SCORE,			// 获取排行榜用户积分
	REQ_GET_EXTRA,			// 获取排行榜用户附加数据
	REQ_GET_RANK,			// 获取排行榜用户排名
	REQ_GET_RANGE,			// 获取排行榜某个范围的名次信息
	REQ_DEL,				// 删除排行榜某个用户
	REQ_SET_SCORE_EXTRA,	// 设置排行榜用户积分和附加数据
	REQ_SET_GLOBAL_DATA,	// 设置全局数据表的某一列的某个值
	REQ_GET_GLOBAL_DATA,	// 获取全局数据表的某一列的多个值
	REQ_GET_RANGE_BY_SCORE,
	REQ_REBUILD,			// 重建排行榜（测试用）
	REQ_DESTROY,			// 删除某个排行榜
	REQ_DESTROY_ALL,		// 删除所有排行榜
	REQ_DESTROY_GLOBAL_DATA,// 删除全局数据表
	REQ_SAVE,
	REQ_SHUTDOWN,
	REQ_REWRITE,
	REQ_LIST,
	REQ_DUMP,
	REQ_UNKNOWN
};

#endif //_RANK_CACHE_CONST_H