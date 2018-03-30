//--------------------------------------------------------------------
// 文件名:		GameInfo.h
// 内  容:		游以服务器状态信息
// 说  明:		
// 创建日期:		2016年12月03日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MODEL_GAME_SERVER_INFO_H__
#define __MODEL_GAME_SERVER_INFO_H__
#include <string>
#include <map>
#include <time.h>
#include "utils\FsSpinLock.h"
#include <vector>

enum EmMemberState
{
	EM_MEMBER_STATE_CLOSING = 1,			// 关闭中
	EM_MEMBER_STATE_CLOSED,					// 关闭了
	EM_MEMBER_STATE_OPENING,				// 开启中
	EM_MEMBER_STATE_OPENED,					// 已开启
	EM_MEMBER_STATE_DEAD,					// 断线
};

enum EmMemberWorkState
{
	EM_MEMBER_WORK_STATE_IDLE,			// 空闲
	EM_MEMBER_WORK_STATE_NORMAL,		// 一般
	EM_MEMBER_WORK_STATE_BUSY,			// 忙
	EM_MEMBER_WORK_STATE_FULL,			// 满员
};

struct MemberInfo
{
	std::string		name;				// 名称
	int				memberId;			// 成员id
	EmMemberState	state;				// 状态
	EmMemberWorkState workState;		// 运行状态
	time_t			reportTime;			// 最后上报时间
};

class GameInfo
{
	std::string m_strName;		// 游戏服务名称
	long long m_nAreaId;		// 区服id
	int m_nOnline;				// 当前在线人数
	EmMemberState m_eState;		// 世界服状态
	int m_nCheckInterVal;		// 断线检测时间
	// 成员服务器列表
	std::map<int, MemberInfo> m_mapMembers;

	mutable FsLocker	m_gameLock;
public:
	GameInfo() : m_nAreaId(0), m_nOnline(0), m_nCheckInterVal(60), m_eState(EM_MEMBER_STATE_CLOSED)
	{

	}

public:
	const std::string& GetName() const;
	void SetName(const char* pszName);
	void SetName(const std::string& strName);

	long long GetAreaId() const;
	void SetAreaId(long long nAreaId);

	int GetOnlineCount() const;
	void SetOnlineCount(int nCount);

	int GetCheckVal() const;
	void SetCheckVal(int nValue);

	int GetMemberIds( std::vector<int>& refIds ) const;
	std::string GetMemberName(int nMemberId) const;
	EmMemberState GetMemberState(int nMemberId) const;
	EmMemberWorkState GetMemberWorkState(int nMemberId) const;

	void SetMember(const char* pszName, int nMemberId, EmMemberState nState, EmMemberWorkState workState);
	void DeadAll();

	void SetState(EmMemberState state);
	EmMemberState GetState() const;

	// 更新服务状态
	void Update();
};
#endif 