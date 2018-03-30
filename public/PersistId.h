//--------------------------------------------------------------------
// 文件名:		PersistId.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_PERSISTID_H
#define _PUBLIC_PERSISTID_H

#include "Macros.h"

// 对象ID

struct PERSISTID
{
	union
	{
		struct
		{
			uint64_t nData64;
		};

		struct
		{
			unsigned int nIdent;
			unsigned int nSerial;
		};
	};

	PERSISTID()
	{
		nData64 = 0;
		//nIdent = 0;
		//nSerial = 0;
	}
	
	PERSISTID(unsigned int ident, unsigned int serial)
	{
		nIdent = ident;
		nSerial = serial;
	}
	
	bool IsNull() const
	{
		return 0 == nData64;
		//return (0 == nIdent) && (0 == nSerial);
	}
};

inline bool operator==(const PERSISTID& source, const PERSISTID& other)
{
	return source.nData64 == other.nData64;
	//return (source.nIdent == other.nIdent)
	//	&& (source.nSerial == other.nSerial);
}

inline bool operator!=(const PERSISTID& source, const PERSISTID& other)
{
	return source.nData64 != other.nData64;
	//return (source.nIdent != other.nIdent)
	//	|| (source.nSerial != other.nSerial);
}

#endif // _PUBLIC_PERSISTID_H
