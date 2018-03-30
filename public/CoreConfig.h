//--------------------------------------------------------------------
// 文件名:		CoreConfig.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_CORECONFIG_H
#define _PUBLIC_CORECONFIG_H

#include "FastStr.h"

// 创建核心配置

struct CCoreConfig
{
	// 程序句柄
	void* m_hInstance;
	// 主窗口句柄
	void* m_hWinHandle;
	// 主线程堆栈地址
	size_t m_nStackAddr;
	// 主线程堆栈大小
	size_t m_nStackSize;
	// 主循环休眠时间
	int m_nSleep;
	// 是否使用调试日志系统
	bool m_bDebugLog;
	// 是否即时重新加载脚本
	bool m_bScriptReload;
	// 是否使用内存池
	bool m_bUseMemoryPool;
	// 使用专用的内存分配器
	bool m_bUseExternMalloc;
	// 使用内存验证
	bool m_bMemoryValidate;
	// 工作路径
	TFastStr<char, 128> m_strWorkPath;
	// 脚本路径
	TFastStr<char, 128> m_strScriptPath;
	// 资源文件路径
	TFastStr<char, 128> m_strResourcePath;
	// 文件系统
	TFastStr<char, 32> m_strFileSystem;
	// 文件系统配置信息
	TFastStr<char, 32> m_strFileConfig;
	// 主实体名称
	TFastStr<char, 32> m_strMainEntity;
	// 启动脚本
	TFastStr<char, 32> m_strMainScript;
	// 启动逻辑类
	TFastStr<char, 32> m_strMainLogic;
	// 主配置文件名
	TFastStr<char, 32> m_strMainConfig;
	// 要加载的模块DLL列表
	TFastStr<char, 128> m_strLoadDlls;

	CCoreConfig()
	{
		m_hInstance = NULL;
		m_hWinHandle = NULL;
		m_nStackAddr = 0;
		m_nStackSize = 0;
		m_nSleep = 0;
		m_bDebugLog = false;
		m_bScriptReload = false;
		m_bUseMemoryPool = false;
		m_bUseExternMalloc = false;
		m_bMemoryValidate = false;
	}
};

#endif // _PUBLIC_CORECONFIG_H

