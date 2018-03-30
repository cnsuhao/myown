//--------------------------------------------------------------------
// 文件名:		PlayerData.h
// 内  容:		玩家数据查询定义
// 说  明:		
// 创建日期:		2016年9月27日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_PLAYER_DATA_H__
#define __MODEL_PLAYER_DATA_H__
#include <string>
#include <vector>

// 查询在线玩家属性结构
struct CmdPropQuery
{
	int64_t     queryId;				// 查询id
	std::string roleId;					// 角色id    
	std::vector<std::string>  props;	// 属性列表	
};

// 查询属性数据结果
struct CmdPropData
{
	int64_t     queryId;						// 查询id
	int			result;							// 结果（EmDataQueryResult）
	std::string roleId;							// 角色id
	std::map<std::string, std::string>  props;	// 属性值列表
};

// 查询在线玩家表结构数据
struct CmdRecordQuery
{
	int64_t     queryId;				// 查询id
	std::string roleId;					// 角色id
	std::string recordName;				// 表名
	int offset;							// 偏移值
	int count;							// 查询条数
};

// 查询表结构数据结果
struct CmdRecordData
{
	int64_t     queryId;				// 查询id
	std::string roleId;					// 角色id
	std::string recordName;				// 表名
	int	result;							// 结果（EmDataQueryResult）
	int offset;							// 偏移值
	int count;							// 查询条数
	int cols;							// 返回表行
	std::vector<std::string> data;		// 返回的值( data.size() % cols == 0 )
};
#endif 