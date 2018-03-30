//--------------------------------------------------------------------
// 文件名:		my_ssl.h
// 内  容:		ssl初始化单例实现
// 说  明:		
// 创建日期:		2016年10月29日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#include "my_ssl.h"
#include "ssl_env.h"
#include "openssl\err.h"

void on_ssl_error( const char* msg )
{
	unsigned long ulErr = ERR_get_error(); // 获取错误号

	char szErrMsg[1024] = { 0 };
	char *pTmp = NULL;
	pTmp = ERR_error_string(ulErr, szErrMsg);

	printf("%s(%s).\n", msg, pTmp);
}

int on_ssl_verify_cb( int method, X509_STORE_CTX* ctx)
{
	ssl_verfiy_cb cb = ssl_env_get_verfiy_cb();
	if (cb)
	{
		return cb(method, ctx);
	}
	return 1;
}

const SSL_METHOD* ssl_get_method()
{
	int nMethod = ssl_env_get_metod();
	switch (nMethod)
	{
	case EM_ENV_SSL_METHOD_V1:			/* TLSv1.0 */
		return TLSv1_method();
	case EM_ENV_SSL_METHOD_V1_1:			/* TLSv1.1 */
		return TLSv1_1_method();
	case EM_ENV_SSL_METHOD_V1_2:			/* TLSv1.2 */
		return TLSv1_2_method();
	case EM_ENV_SSL_METHOD_V2:			/* TLSv2.0 */
		return	SSLv23_method();
	case EM_ENV_SSL_METHOD_V3:			/* TLSv3.0 */
		return SSLv3_method();
	case EM_ENV_SSL_METHOD_V23:			/* TLSv2.0 & 3.0 */
		return	SSLv23_method();
	case EM_ENV_SSL_METHOD_DV1:			/* DTLSv1.0 */
		return DTLSv1_method();
	case EM_ENV_SSL_METHOD_DV1_2:		/* DTLSv1.2 */
		return DTLSv1_2_method();
	case EM_ENV_SSL_METHOD_DV1_12:		/* DTLS 1.0 and 1.2 */
		return DTLS_method();
	default:
		break;
	}
	return NULL;
}

MySSL::MySSL() : m_sslCtx( NULL )
{

}

bool MySSL::Init()
{
	if (NULL != m_sslCtx)
	{
		return true;
	}

#if OPENSSL_VERSION_NUMBER < 0x10100000L
	// Initialize OpenSSL
	SSL_library_init();
	ERR_load_crypto_strings();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	ERR_load_PEM_strings();
	ERR_load_ERR_strings();
	OpenSSL_add_all_algorithms();
#endif

	ERR_clear_error();
	const SSL_METHOD* metod = ssl_get_method();
	if (NULL != metod)
	{
		m_sslCtx = SSL_CTX_new( metod );
	}
	else
	{
		on_ssl_error("Err: don't support ssl method");
		goto _LOCAL_FAILED;
	}

	if (NULL == m_sslCtx)
	{
		on_ssl_error("Err: SSL_CTX_new failed");
		goto _LOCAL_FAILED;
	}

	//SSL_CTX_set_options(m_sslCtx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
	//SSL_CTX_set_cipher_list(m_sslCtx, "TLSv1.2:TLSv1:SSLv3:!SSLv2:HIGH:!MEDIUM:!LOW");
	//SSL_CTX_set_cipher_list(m_sslCtx, "ECDHE-RSA-AES128-GCM-SHA256:!RC4");	
	//SSL_CTX_set_options(m_sslCtx, SSL_OP_CIPHER_SERVER_PREFERENCE);
	// 设置证书验证模式
	SSL_CTX_set_verify(m_sslCtx, ssl_env_get_verfiy_mode(), on_ssl_verify_cb);
	// 加载信任证书
	if (!SSL_CTX_load_verify_locations(m_sslCtx, ssl_env_get_ca(), ssl_env_get_trust() ) )
	{
		on_ssl_error("Err: SSL_CTX_load_verify_locations failed");
		goto _LOCAL_FAILED;
	}

	{
		const char* pszCerificate = ssl_env_get_certificate_file();
		if (NULL != pszCerificate)
		{
			// 加载自己的证书
			if (SSL_CTX_use_certificate_file(m_sslCtx, pszCerificate, SSL_FILETYPE_PEM) <= 0)
			{
				on_ssl_error("Err: SSL_CTX_use_certificate_file failed");
				goto _LOCAL_FAILED;
			}

			// 设置密码
			SSL_CTX_set_default_passwd_cb_userdata(m_sslCtx, (void*)ssl_env_get_private_pass() );

			// 加载自己的私钥
			if (SSL_CTX_use_PrivateKey_file(m_sslCtx, ssl_env_get_private_key(), SSL_FILETYPE_PEM) <= 0)
			{
				on_ssl_error("Err: SSL_CTX_use_PrivateKey_file failed");
				goto _LOCAL_FAILED;
			}

			// 判断私钥是否正确
			if ( !SSL_CTX_check_private_key(m_sslCtx) )
			{
				on_ssl_error("Err: SSL_CTX_check_private_key failed");
				goto _LOCAL_FAILED;
			}
		}
	}

	return true;

_LOCAL_FAILED:
	Release();
	return false;
}

void MySSL::Release()
{
	if (NULL != m_sslCtx)
	{
		SSL_CTX_free(m_sslCtx);
		m_sslCtx = NULL;
	}

#if OPENSSL_VERSION_NUMBER < 0x10100000L
	EVP_cleanup();
	ERR_free_strings();

#ifdef EVENT__HAVE_ERR_REMOVE_THREAD_STATE
	ERR_remove_thread_state(NULL);
#else
	ERR_remove_state(0);
#endif
	CRYPTO_cleanup_all_ex_data();

	sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
#endif /*OPENSSL_VERSION_NUMBER < 0x10100000L */
}

SSL* MySSL::CreateSSL()
{
	SSL* pSSL = NULL;
	if (NULL != m_sslCtx)
	{
		pSSL = SSL_new(m_sslCtx);
	}

	return pSSL;
}

void MySSL::FreeSSL(SSL* ssl)
{
	if (NULL != ssl)
	{
		SSL_free(ssl);
	}
}

void MySSL::SetFd(SSL* ssl, size_t fd)
{
	SSL_set_fd(ssl, (int)fd);
}

void MySSL::SetRFd(SSL* ssl, size_t fd)
{
	SSL_set_rfd(ssl, (int)fd);
}

void MySSL::SetWFd(SSL* ssl, size_t fd)
{
	SSL_set_wfd(ssl, (int)fd);
}
