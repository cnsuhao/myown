//--------------------------------------------------------------------
// 文件名:		ssl_env.h
// 内  容:		ssl_env环境配置
// 说  明:		
// 创建日期:		2016年10月29日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#include <stdio.h>
#include "ssl_env.h"
#include <string>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

	static ssl_verfiy_cb ms_verify_cb = NULL;
	static EmSSLVerifyMode ms_verfiy_mode = EM_ENV_SSL_VERIFY_NONE;
	static EmSSLvMethod ms_ssl_method = EM_ENV_SSL_METHOD_V23;
	static std::string ms_str_ca = "certs/ca.crt";
	static std::string ms_str_trust = "certs/trust";
	static std::string ms_str_cerficate;
	static std::string ms_str_key;
	static std::string ms_str_pass;

	void ssl_env_set_verify(EmSSLVerifyMode mode, ssl_verfiy_cb cb)
	{
		ms_verify_cb = cb;
		ms_verfiy_mode = mode;
	}

	EmSSLVerifyMode ssl_env_get_verfiy_mode()
	{
		return ms_verfiy_mode;
	}

	ssl_verfiy_cb ssl_env_get_verfiy_cb()
	{
		return ms_verify_cb;
	}

	void ssl_env_set_method(EmSSLvMethod method)
	{
		ms_ssl_method = method;
	}

	EmSSLvMethod ssl_env_get_metod()
	{
		return ms_ssl_method;
	}

	void ssl_env_set_ca( const char* szCA )
	{
		if (NULL == szCA)
		{
			ms_str_ca = "certs/ca.cert";
		}
		else
		{
			ms_str_ca = szCA;
		}
	}

	const char* ssl_env_get_ca()
	{
		return ms_str_ca.c_str();
	}

	void ssl_env_set_trust(const char* szTrustPath)
	{
		if (NULL == szTrustPath)
		{
			ms_str_trust = "certs/trust";
		}
		else
		{
			ms_str_trust = szTrustPath;
		}
	}

	const char* ssl_env_get_trust()
	{
		return ms_str_trust.c_str();
	}

	void ssl_env_set_certificate_file(const char* cert)
	{
		if (NULL == cert)
		{
			ms_str_cerficate.clear();
		}
		else
		{
			ms_str_cerficate = cert;
		}
	}

	const char* ssl_env_get_certificate_file()
	{
		return ms_str_cerficate.empty() ? NULL : ms_str_cerficate.c_str();
	}


	void ssl_env_set_private_key(const char* key)
	{
		if (NULL == key)
		{
			ms_str_key.clear();
		}
		else
		{
			ms_str_key = key;
		}
	}

	const char* ssl_env_get_private_key()
	{
		return ms_str_key.empty() ? NULL : ms_str_key.c_str();
	}

	void ssl_env_set_private_pass(const char* pass)
	{
		if (NULL == pass)
		{
			ms_str_pass.clear();
		}
		else
		{
			ms_str_pass = pass;
		}
	}

	const char* ssl_env_get_private_pass()
	{
		return ms_str_pass.empty() ? NULL : ms_str_pass.c_str();
	}

#ifdef __cplusplus
}

#endif