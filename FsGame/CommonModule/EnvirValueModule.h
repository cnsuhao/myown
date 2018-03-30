//--------------------------------------------------------------------
// 文件名:      EnvirValueModule.h
// 内  容:      工程中的常量
// 说  明:
// 创建日期:    2016年03月09日
// 创建人:       
// 修改人:        
//    :       
//--------------------------------------------------------------------

#ifndef _ENVIR_VALUE_MODULE_H_
#define _ENVIR_VALUE_MODULE_H_

#include "Fsgame/Define/header.h"
#include <string>
#include <map>
#include "public/Var.h"
#include "utils/EnvirDefine.h"

/* ----------------------
 * 事件计时器类
 * ----------------------*/
class EnvirValueModule : public ILogicModule
{
public:
    virtual bool Init(IKernel *pKernel);
    virtual bool Shut(IKernel *pKernel);

	// 查询int常量 type 对应 EnvirValueTypes
	static int	EnvirQueryInt(int type);

	// 查询float常量 type 对应 EnvirValueTypes
	static float EnvirQueryFloat(int type);

	// 查询string常量 type 对应 EnvirValueTypes
	static const char*	EnvirQueryString(int type);
private:
	// 重新加载配置
	static void ReloadConfig(IKernel *pKernel);
private:
	static CVar	m_arrEnvirData[ENVIR_VALUE_MAX];
};

#endif // _EVENT_TIMER_MODULE_H_
