//--------------------------------------------------------------------
// 文件名:		HttpFormater.h
// 内  容:		http数据格式化
// 说  明:		
// 创建日期:		2016年10月21日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __HTTP_DATA_FORMATER_H__
#define __HTTP_DATA_FORMATER_H__

#include "HttpData.h"
#include <assert.h>

class HttpDataFormater : public IHttpFormater
{
public:
	virtual size_t Formater(const IHttpNode* pNode, std::string& outStr)
	{
		if ( pNode->SimpleNode() )
		{
			std::string _formaterString;
			pNode->toString(_formaterString);
			outStr.append( _formaterString );
			return _formaterString.size();
		}

		const HttpData* pPostData = dynamic_cast<const HttpData*>(pNode);
		assert(NULL != pPostData);
		if (NULL == pPostData)
		{
			return 0;
		}

		bool bAppendConnector = false;
		HttpData::const_iterator itr = pPostData->Begin();
		for (; itr != pPostData->End(); ++itr )
		{
			outStr.append(itr->first);
			outStr.append("=");
			Formater(itr->second.get(), outStr);
			outStr.append("&");
			bAppendConnector = true;
		}

		size_t _szLen = outStr.size();
		if ( bAppendConnector )
		{
			--_szLen;
			outStr.erase(_szLen);
		}
		return _szLen;
	}

	virtual bool Parse(IHttpNode* pNode, const char* pszData)
	{
		if (NULL == pNode) return false;
		if ( NULL == pNode->SimpleNode() ) return false;

		if (NULL != pszData)
		{
			HttpData* pData = dynamic_cast<HttpData*>(pNode);
			if (NULL == pszData) return false;

			const char* _szTemp = strchr(pszData, '=');
			// 单节点
			if (NULL == _szTemp)
			{
				pData->AddValue("__data", pszData);
			}
			else
			{
				const size_t _szLen = strlen(pszData);
				char* _pTmp = new char[_szLen + 1];
				strncpy(_pTmp, pszData, _szLen);
				_pTmp[_szLen] = 0;

				char* _pData = _pTmp;
				while (NULL != _pData && *_pData != '\0')
				{
					char* _sp1 = strchr(_pData, '&');
					if (NULL != _sp1)
					{
						*_sp1 = '\0';
						++_sp1;
					}

					// kv
					char* k = _pData;
					char* v = strchr(k, '=');
					if (NULL != v)
					{
						*v = 0;
						++v;
						pData->AddValue(k, v);
					}

					_pData = _sp1;
				};
				delete[] _pTmp;
			}

			return true;
		}

		return false;
	}
};



#endif // END __HTTP_POST_DATA_FORMATER_H__