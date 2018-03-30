//--------------------------------------------------------------------
// 文件名:		HttpPost.h
// 内  容:		http post传输数据
// 说  明:		
// 创建日期:		2016年10月20日
// 创建人:		李海罗
// 版权所有:		苏州鲸宝互动有限公司
//--------------------------------------------------------------------
#ifndef __HTTP_POST_DATA_H__
#define __HTTP_POST_DATA_H__

#include <memory>
#include <string>
#include "public/Var.h"
#include <sstream>
#include <iosfwd>
#include <map>

class IHttpNode
{
public:
	virtual ~IHttpNode() {};
	virtual bool SimpleNode() const = 0;
	virtual bool toString(std::string& strOut) const = 0;
};

class IHttpFormater
{
public:
	virtual ~IHttpFormater() {};
	virtual size_t Formater(const IHttpNode* pNode, std::string& outStr) = 0;
	virtual bool Parse(IHttpNode* pNode, const char* pszData) = 0;
};

class HttpNode : public IHttpNode
{
private:
	CVar m_data;

public:
	explicit HttpNode()
	{

	}

	explicit HttpNode(bool value) 
		: m_data(VTYPE_BOOL, value)
	{
	}

	explicit HttpNode(int value)
		: m_data(VTYPE_INT, value)
	{
	}

	explicit HttpNode(int64_t value)
		: m_data(VTYPE_INT64, value)
	{
	}

	explicit HttpNode(float value)
		: m_data(VTYPE_FLOAT, value)
	{
	}

	explicit HttpNode(double value)
		: m_data(VTYPE_DOUBLE, value)
	{
	}

	explicit HttpNode(const char* value)
		: m_data(VTYPE_STRING, value)
	{
	}

	explicit HttpNode( const PERSISTID& value )
		: m_data(VTYPE_OBJECT, value)
	{

	}

	virtual bool SimpleNode() const
	{
		return true;
	}

	virtual bool toString(std::string& strOut) const
	{
		std::stringstream _str;
		switch ( m_data.GetType() )
		{
		case VTYPE_BOOL:
			_str << m_data.BoolVal();
			break;
		case VTYPE_INT:
			_str << m_data.IntVal();
			break;
		case VTYPE_INT64:
			_str << m_data.Int64Val();
			break;
		case VTYPE_FLOAT:
			_str << m_data.FloatVal();
			break;
		case VTYPE_DOUBLE:
			_str << m_data.DoubleVal();
			break;
		case VTYPE_STRING:
			strOut = m_data.StringVal();
			return true;
			break;
		case VTYPE_OBJECT:
		{
			char szId[32] = { 0 };
			const PERSISTID& pid = m_data.ObjectVal();
			_snprintf_s(szId, 32, "%d-%d", pid.nIdent, pid.nSerial);
			_str << szId;
			break;
		}
		default:
			break;
		}

		strOut = _str.str();
		return true;
	}

	bool asBool() const
	{
		return m_data.BoolVal();
	}

	int asInt() const
	{
		return m_data.IntVal();
	}

	int64 asInt64() const
	{
		return m_data.Int64Val();
	}

	float asFloat() const
	{
		return m_data.FloatVal();
	}

	double asDouble() const
	{
		return m_data.DoubleVal();
	}

	const char* asCString() const
	{
		return m_data.StringVal();
	}

	const PERSISTID& asObject() const
	{
		return m_data.ObjectVal();
	}
};


class HttpData : public IHttpNode
{
public:
	typedef std::map<std::string, std::shared_ptr<IHttpNode> > Nodes;
	typedef std::map<std::string, std::shared_ptr<IHttpNode> >::iterator iterator;
	typedef std::map<std::string, std::shared_ptr<IHttpNode> >::const_iterator const_iterator;

public:
	HttpData()
	{
	}
	HttpData(std::shared_ptr<IHttpFormater>& ptrFormater)
		: m_ptrFormater(ptrFormater)
	{

	}

	void SetFormator(std::shared_ptr<IHttpFormater>& ptrFormater)
	{
		m_ptrFormater = (ptrFormater);
	}

	void SetText( const char* szText, size_t nLen )
	{
		if (NULL != szText)
		{
			m_text.assign( szText, szText + nLen );
		}
		else
		{
			m_text.clear();
		}
	}

	void AppendText(const char* szText, size_t nLen)
	{
		m_text.insert( m_text.begin(), szText, szText + nLen );
	}

	const char* GetText() const
	{
		return &m_text[0];
	}

	void ClearText()
	{
		m_text.resize(1);
		m_text.clear();
	}

	size_t GetTextSize() const
	{
		return m_text.size();
	}

	size_t GetChildSize() const
	{
		return m_ptrNodes.size();
	}

	virtual bool SimpleNode() const
	{
		return false;
	}

	iterator Begin()
	{
		return m_ptrNodes.begin();
	}

	const_iterator Begin() const
	{
		return m_ptrNodes.begin();
	}

	iterator End()
	{
		return m_ptrNodes.end();
	}

	const_iterator End() const
	{
		return m_ptrNodes.end();
	}

	iterator Find(const char* key)
	{
		return m_ptrNodes.find(key);
	}

	const_iterator Find(const char* key) const
	{
		return m_ptrNodes.find(key);
	}

	bool Remove(const char* key)
	{
		return m_ptrNodes.erase(key) > 0;
	}

	bool IsExists(const char* key) const
	{
		return Find(key) != End();
	}

	bool AddValue(const char* key, bool value)
	{
		return InnerInsert(key, value);
	}

	bool AddValue(const char* key, int value)
	{
		return InnerInsert(key, value);
	}

	bool AddValue(const char* key, int64_t value)
	{
		return InnerInsert(key, value);
	}

	bool AddValue(const char* key, float value)
	{
		return InnerInsert(key, value);
	}

	bool AddValue(const char* key, double value)
	{
		return InnerInsert(key, value);
	}

	bool AddValue(const char* key, const PERSISTID& value)
	{
		return InnerInsert(key, value);
	}

	bool AddValue(const char* key, const char* value)
	{
		return InnerInsert(key, value);
	}

	bool AddValue(const char* key, std::shared_ptr<IHttpNode>& value)
	{
		return InnerInsert(key, value);
	}

	bool GetBool(const char* key) const
	{
		const HttpNode* _ptrNode = InnerGetNode(key);
		return NULL == _ptrNode ? false : _ptrNode->asBool();
	}

	int GetInt(const char* key) const
	{
		const HttpNode* _ptrNode = InnerGetNode(key);
		return NULL == _ptrNode ? 0 : _ptrNode->asInt();
	}

	int64_t GetInt64(const char* key) const
	{
		const HttpNode* _ptrNode = InnerGetNode(key);
		return NULL == _ptrNode ? 0 : _ptrNode->asInt64();
	}

	float GetFloat(const char* key) const
	{
		const HttpNode* _ptrNode = InnerGetNode(key);
		return NULL == _ptrNode ? 0.0f : _ptrNode->asFloat();
	}

	double GetDouble(const char* key) const
	{
		const HttpNode* _ptrNode = InnerGetNode(key);
		return NULL == _ptrNode ? 0.0f : _ptrNode->asDouble();
	}

	const char* GetCString(const char* key) const
	{
		const HttpNode* _ptrNode = InnerGetNode(key);
		return NULL == _ptrNode ? NULL : _ptrNode->asCString();
	}

	PERSISTID GetObject(const char* key) const
	{
		const HttpNode* _ptrNode = InnerGetNode(key);
		return NULL == _ptrNode ? PERSISTID() : _ptrNode->asObject();
	}

	virtual bool toString(std::string& strOut) const
	{
		if (m_ptrFormater.get() != NULL)
		{
			m_ptrFormater->Formater( this, strOut );
			return true;
		}
		return false;
	}

private:
	template<typename T>
	bool InnerInsert(const char* key, T& value)
	{
		if (NULL == key) return false;
		std::shared_ptr<IHttpNode> _ptrNode(new HttpNode(value));
		Nodes::_Pairib result = m_ptrNodes.insert(std::make_pair(key, _ptrNode));
		return result.second;
	}
	template<>
	bool InnerInsert<std::shared_ptr<IHttpNode> >(const char* key, std::shared_ptr<IHttpNode>& value)
	{
		if (NULL == key) return false;
		Nodes::_Pairib result = m_ptrNodes.insert(std::make_pair(key, value));
		return result.second;
	}

	const HttpNode* InnerGetNode(const char* key) const
	{
		const_iterator itr = Find(key);
		if (itr == End())
		{
			return NULL;
		}

		if (itr->second->SimpleNode())
		{
			HttpNode* _ptrNode = dynamic_cast<HttpNode*>(itr->second.get());
			return _ptrNode;
		}

		return NULL;
	}
private:
	Nodes m_ptrNodes;
	std::shared_ptr<IHttpFormater> m_ptrFormater;
	std::string m_text;
};

#endif // END __HTTP_POST_DATA_H__