//--------------------------------------------------------------------
// 文件名:		ExceptMsg.h
// 内  容:		
// 说  明:		
// 创建日期:	2002年7月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_EXCEPTMSG_H
#define _PUBLIC_EXCEPTMSG_H

#include "Macros.h"

// CExceptMsg
// 异常信息

class CExceptMsg
{
public:
	CExceptMsg(const char* msg)
	{
		Assert(msg != NULL);
		
		m_pMsg = msg;
	}

	~CExceptMsg()
	{
	}

	const char* GetMsg() const
	{
		return m_pMsg;
	}

private:
	CExceptMsg();

private:
	const char* m_pMsg;
};

#endif // _PUBLIC_EXCEPTMSG_H
