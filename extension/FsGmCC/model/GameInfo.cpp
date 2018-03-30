#include "GameInfo.h"

const std::string& GameInfo::GetName() const
{
	return m_strName;
}

void GameInfo::SetName(const char* pszName)
{
	if (NULL == pszName)return;
	m_strName = pszName;
}

void GameInfo::SetName(const std::string& strName)
{
	m_strName = strName;
}

long long GameInfo::GetAreaId() const
{
	return m_nAreaId;
}

void GameInfo::SetAreaId(long long nAreaId)
{
	m_nAreaId = nAreaId;
}

int GameInfo::GetOnlineCount() const
{
	LockGuard<FsLocker> guard(m_gameLock);
	return m_nOnline;
}

void GameInfo::SetOnlineCount(int nCount)
{
	LockGuard<FsLocker> guard( m_gameLock);
	m_nOnline = nCount;
}

int GameInfo::GetCheckVal() const
{
	return m_nCheckInterVal;
}

void GameInfo::SetCheckVal(int nValue)
{
	m_nCheckInterVal = nValue;
}

int GameInfo::GetMemberIds( std::vector<int>& refIds ) const
{
	LockGuard<FsLocker> guard(m_gameLock);
	for (std::map<int, MemberInfo>::const_iterator itr = m_mapMembers.begin(); itr != m_mapMembers.end(); ++itr)
	{
		refIds.push_back(itr->first);
	}

	return refIds.size();
}

std::string GameInfo::GetMemberName(int nMemberId) const
{
	LockGuard<FsLocker> guard(m_gameLock);
	std::map<int, MemberInfo>::const_iterator itr = m_mapMembers.find(nMemberId);
	if (itr != m_mapMembers.end())
	{
		return itr->second.name;
	}

	return std::string();
}
 
EmMemberState GameInfo::GetMemberState(int nMemberId) const
{
	LockGuard<FsLocker> guard(m_gameLock);
	std::map<int, MemberInfo>::const_iterator itr = m_mapMembers.find(nMemberId);
	if (itr != m_mapMembers.end())
	{
		return itr->second.state;
	}
	
	return EM_MEMBER_STATE_DEAD;
}

EmMemberWorkState GameInfo::GetMemberWorkState(int nMemberId) const
{
	LockGuard<FsLocker> guard(m_gameLock);
	std::map<int, MemberInfo>::const_iterator itr = m_mapMembers.find(nMemberId);
	if (itr != m_mapMembers.end())
	{
		return itr->second.workState;
	}

	return EM_MEMBER_WORK_STATE_IDLE;
}

void GameInfo::SetMember(const char* pszName, int nMemberId, EmMemberState nState, EmMemberWorkState workState)
{
	LockGuard<FsLocker> guard(m_gameLock);
	time_t tNow = time(NULL);
	MemberInfo& memInfo = m_mapMembers[nMemberId];
	memInfo.memberId = nMemberId;
	memInfo.name = pszName;
	memInfo.state = nState;
	memInfo.workState = workState;
	memInfo.reportTime = tNow;
}

void GameInfo::DeadAll()
{
	LockGuard<FsLocker> guard(m_gameLock);
	for (std::map<int, MemberInfo>::iterator itr = m_mapMembers.begin();
		itr != m_mapMembers.end(); ++itr)
	{
		if (itr->second.state == EM_MEMBER_STATE_CLOSING)
		{
			itr->second.state = EM_MEMBER_STATE_CLOSED;
		}
		else if (itr->second.state != EM_MEMBER_STATE_CLOSED)
		{
			itr->second.state = EM_MEMBER_STATE_DEAD;
		}
	}
}

void GameInfo::SetState(EmMemberState state)
{
	{
		LockGuard<FsLocker> guard(m_gameLock);
		m_eState = state;
	}
	if (state == EM_MEMBER_STATE_DEAD || state == EM_MEMBER_STATE_CLOSED )
	{
		DeadAll();
	}
}

EmMemberState GameInfo::GetState() const
{
	LockGuard<FsLocker> guard(m_gameLock);
	return m_eState;
}

void GameInfo::Update()
{
	LockGuard<FsLocker> guard(m_gameLock);
	time_t tNow = time(NULL);
	for (std::map<int, MemberInfo>::iterator itr = m_mapMembers.begin(); itr != m_mapMembers.end(); ++itr )
	{
		if (itr->second.state == EM_MEMBER_STATE_OPENED)
		{
			if (tNow - itr->second.reportTime >= m_nCheckInterVal)
			{
				itr->second.state = EM_MEMBER_STATE_OPENED;
			}
		}
	}
}

