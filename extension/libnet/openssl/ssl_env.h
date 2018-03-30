//--------------------------------------------------------------------
// 文件名:		ssl_env.h
// 内  容:		ssl_env环境配置
// 说  明:		
// 创建日期:		2016年10月29日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __MY_SSL_ENV_H__
#define __MY_SSL_ENV_H__

enum EmSSLvMethod
{
	EM_ENV_SSL_METHOD_V1,			/* TLSv1.0 */
	EM_ENV_SSL_METHOD_V1_1,			/* TLSv1.1 */
	EM_ENV_SSL_METHOD_V1_2,			/* TLSv1.2 */
	EM_ENV_SSL_METHOD_V2,			/* TLSv2.0 */
	EM_ENV_SSL_METHOD_V3,			/* TLSv3.0 */
	EM_ENV_SSL_METHOD_V23,			/* TLSv2.0 & 3.0 */
	EM_ENV_SSL_METHOD_DV1,			/* DTLSv1.0 */
	EM_ENV_SSL_METHOD_DV1_2,		/* DTLSv1.2 */
	EM_ENV_SSL_METHOD_DV1_12,		/* DTLS 1.0 and 1.2 */
};

enum EmSSLVerifyMode
{
	EM_ENV_SSL_VERIFY_NONE                 = 0x00,
	EM_ENV_SSL_VERIFY_PEER                 = 0x01,
	EM_ENV_SSL_VERIFY_FAIL_IF_NO_PEER_CERT = 0x02,
	EM_ENV_SSL_VERIFY_CLIENT_ONCE          = 0x04,
};

#ifdef __cplusplus
extern "C" {
#else
#define const	// c not have const
#endif
	// ctx是X509_STORE_CTX
	typedef int(*ssl_verfiy_cb)(int, void* ctx);

	// 设置证书验证模式及自定义验证回调(默认为EM_ENV_SSL_VERIFY_NONE, 回调为NULL)
	void ssl_env_set_verify(EmSSLVerifyMode mode, ssl_verfiy_cb cb);
	// 获取证书验证模式
	EmSSLVerifyMode ssl_env_get_verfiy_mode();
	// 获取回调
	ssl_verfiy_cb ssl_env_get_verfiy_cb();

	// 设置创建方法(默认为EM_ENV_SSL_METHOD_V23)
	void ssl_env_set_method(EmSSLvMethod method);
	EmSSLvMethod ssl_env_get_metod();

	// 设置根证书(默认为certs/ca.cert)
	void ssl_env_set_ca(const char* szCA);
	const char* ssl_env_get_ca();

	// 设置信任证书路径(默认certs/trust)
	void ssl_env_set_trust(const char* szTrustPath);
	const char* ssl_env_get_trust();

	// 设置自己的证书(默认为NULL,不使用)
	void ssl_env_set_certificate_file( const char* cert );
	const char* ssl_env_get_certificate_file();

	// 设置自己的私钥
	void ssl_env_set_private_key(const char* key);
	const char* ssl_env_get_private_key();

	// 设置自己的私钥密码
	void ssl_env_set_private_pass(const char* key);
	const char* ssl_env_get_private_pass();
#ifdef __cplusplus
}
#endif

#endif // END __OPEN_SSL_ENV_H__