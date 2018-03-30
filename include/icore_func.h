#if !defined __ICORE_FUNC_H__
#define __ICORE_FUNC_H__

#include "Var.h"
#include "VarList.h"
#include "IEntity.h"
#include "ILogic.h"
#include "../utils/PoolAlloc.h"


typedef bool (__cdecl* PROP_GET_FUNC)(IEntity*, IVar&);
typedef bool (__cdecl* PROP_SET_FUNC)(IEntity*, const IVar&);
typedef bool (__cdecl* METHOD_FUNC)(IEntity*, const IVarList&, IVarList&);
typedef bool (__cdecl* GOABLE_FUNC)(const IVarList&, IVarList&);

typedef bool (__cdecl* LOGIC_CALLBACK_FUNC)(ILogic*, const IVarList&, IVarList&);


extern CPoolAlloc* g_pAllocator;

class TAlloc
{
public:
	TAlloc() {}
	~TAlloc() {}

	// 申请内存
	void* Alloc(size_t size)
	{ 
		if (g_pAllocator)
		{
			return g_pAllocator->Alloc(size);
		}
		else
		{
			//return malloc(size);
			return NEW char[size];
		}
	}
	// 释放内存
	void Free(void* ptr, size_t size)
	{
		if (g_pAllocator)
		{
			g_pAllocator->Free(ptr, size);
		}
		else
		{
			//free(ptr);
			delete[] (char*)ptr;
		}
	}

	// 交换
	void Swap(TAlloc& src) {}
};


#endif // __ICORE_FUNC_H__