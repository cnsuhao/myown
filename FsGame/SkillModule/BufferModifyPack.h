//--------------------------------------------------------------------
// 文件名:      BufferModifyPack.h
// 内  容:      Buffer属性包逻辑
// 说  明:
// 创建日期:    2016年3月3日
// 创建人:		 刘明飞
//    :       
//--------------------------------------------------------------------

#ifndef _BUFFER_MODIFYPACK_H_
#define _BUFFER_MODIFYPACK_H_

#include "Fsgame/Define/header.h"
#include "Fsgame/Define/ModifyPackDefine.h"
#include "utils/Singleton.h"

class BufferModifyPack
{
public:
    /**
    @brief  初始化
    @param  pKernel [IN] 引擎核心指针
    @return 返回初始化结果
    */
   bool Init(IKernel* pKernel);
private:
	//回调函数 玩家加载数据完成
	static int OnPlayerRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

	// 增加buff属性包
	bool UpdateBufferModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer, MapPropValue& mapPropValue);

	// 移除buff属性包
	bool RemoveBufferModifyPack(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer, MapPropValue& mapPropValue);

	// 增加buff
	static int OnBuffAfterAdd(IKernel* pKernel, const PERSISTID& buffcontainer, const PERSISTID& buffer, const IVarList& args);

	// 移除buff
	static int OnBuffRemove(IKernel* pKernel, const PERSISTID& buffcontainer, const PERSISTID& buffer, const IVarList& args);
};

typedef HPSingleton<BufferModifyPack> BufferModifyPackSingleton;

#endif
