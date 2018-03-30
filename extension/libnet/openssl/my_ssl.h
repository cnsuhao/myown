//--------------------------------------------------------------------
// 文件名:		my_ssl.h
// 内  容:		ssl初始化单例实现
// 说  明:		
// 创建日期:		2016年10月29日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __SSL_SINGTON_IMP_H__
#define __SSL_SINGTON_IMP_H__
#include <openssl/ssl.h>

class MySSL
{
private:
	MySSL(MySSL&) = delete;
	MySSL(MySSL&&) = delete;
	MySSL& operator=(MySSL&) = delete;
	MySSL& operator=(MySSL&&) = delete;

	SSL_CTX* m_sslCtx;
public:
	MySSL();

	bool Init();
	void Release();

	SSL_CTX* Context() { return m_sslCtx; }
	SSL* CreateSSL();
	void FreeSSL(SSL* ssl);
	void SetFd(SSL* ssl, size_t fd);
	void SetRFd(SSL* ssl, size_t fd);
	void SetWFd(SSL* ssl, size_t fd);
};

#endif // END __SSL_SINGTON_IMP_H__