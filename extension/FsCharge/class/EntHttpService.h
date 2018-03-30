//--------------------------------------------------------------------
// 文件名:		EntHttpServer.h
// 内  容:		http服务器实现
// 说  明:		
// 创建日期:		2016年5月18日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __ENT_HTTP_SERVER_H__
#define __ENT_HTTP_SERVER_H__

#include "EntNetBase.h"

class SoloCharge;
class EntHttpService : public EntNetBase
{
public:
	EntHttpService();
	virtual bool Init(const IVarList& args) override;
	virtual bool Shut() override;

	virtual bool Startup() override;
	virtual bool Stop() override;

	void SetLoginVerifyUrl( const char* url )
	{
		m_strLoginVerifyUrl = url;
	};
	const char* GetLoginVerifyUrl() const
	{
		return m_strLoginVerifyUrl.c_str();
	}

	// 请求重试次数
	void SetReqResueNum(int nNum);
	int GetReqResueNum();
	// 请求重用数
	void SetReqRetries(int nNum);
	int GetReqRetries();

	// 默认请求头(启用服务后调用有效)
	void AddReqDefHeader(const char* pszKey, const char* pszValue);
	void RemoveReqDefHeader(const char* pszKey);
	void ClearReqDefHeaders();

	void AddPostUser(const char* pszUserPost);
	void RemovePostUser(const char* pszUserPost);
	void ClearPostUser();
	std::string DumpPostUser();
public:
	virtual void OnAction(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);
	virtual void OnReply(const SERID& serid, const SESSID& sid, std::shared_ptr<INetAction>& action);

	virtual int GetNetKey() const;

protected:
	virtual INetIoDesc* GetIoDesc();

	virtual void OnInit();
	void DumpAction(std::shared_ptr<INetAction>& action);
public:
	//登陆
	void OnGameLogin(const char*token, const char*name, const char*verifyStr, int64_t gameId, int64_t nAreaId, const char* pszIP, const char* pszAppid);
private:
	// 登陆返回
	void OnRspGameLogin( std::shared_ptr<INetAction>& action );
private:
	SoloCharge* m_pMainEntity;
	std::string m_strLoginVerifyUrl;

	std::vector<std::string> m_vecAllowPostUser;

	int m_nReqReuseNum;
	int m_nReqRetries;
};

#endif // END __INT_HTTP_SERVER_H__