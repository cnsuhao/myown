//--------------------------------------------------------------------
// 文件名:		HttpJsonFormater.h
// 内  容:		http数据格式化成json
// 说  明:		
// 创建日期:		2016年10月21日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __HTTP_DATA_JSON_FORMATER_H__
#define __HTTP_DATA_JSON_FORMATER_H__

#include "HttpData.h"
#include "utils/json.h"
#include <assert.h>

class HttpJsonFormater : IHttpFormater
{
public:
	virtual size_t Formater(const IHttpNode* pNode, std::string& outStr)
	{
		Json::Value _root;
		PostTrans2Json(_root, pNode);
		outStr.clear();
		const char* _szStr = _root.asCString();
		if (NULL != _szStr)
		{
			outStr = _szStr;
		}
		return outStr.size();
	}

	//virtual bool Parse(IHttpPostNode* pNode, const char* pszData)
	//{
	//	//if (NULL == pNode) return false;
	//	//if (NULL == pNode->SimpleNode()) return false;
	//	//if (NULL == pszData) return false;

	//	//HttpPostData* pData = dynamic_cast<HttpPostData*>(pNode);
	//	//if (NULL == pszData) return false;
	//	//Json::Reader reader;
	//	//Json::Value root;

	//	//std::string strData = pszData;
	//	//bool _bOK = reader.parse(strData, root);
	//	//if (!_bOK)
	//	//{
	//	//	return false;
	//	//}
	//}
	virtual bool Parse(IHttpNode* pNode, const char* pszData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

private:
	void PostTrans2Json( Json::Value& root, const IHttpNode* pNode )
	{
		if (NULL == pNode) return;
		if (pNode->SimpleNode())
		{
			std::string _strValue;
			pNode->toString(_strValue);
			root = _strValue;
			return;
		}

		const HttpData* pPostData = dynamic_cast<const HttpData*>(pNode);
		assert(NULL != pPostData);
		if (NULL == pPostData)
		{
			return;
		}

		bool bAppendConnector = false;
		HttpData::const_iterator itr = pPostData->Begin();
		for (; itr != pPostData->End(); ++itr)
		{
			Json::Value& _value = root[itr->first];
			PostTrans2Json(_value, itr->second.get());
		}
	}
};



#endif // END __HTTP_POST_JSON_FORMATER_H__