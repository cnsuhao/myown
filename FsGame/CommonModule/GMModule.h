//--------------------------------------------------------------------
// 文件名:		GMModule.h
// 内  容:		GM命令
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __GMModule_H__
#define __GMModule_H__

#include "Fsgame/Define/header.h"
#include <string>
#include <unordered_map>

#define GMLEVEL_MAX 51

class GMModule : public ILogicModule
{
public:
	// 初始化
	virtual bool Init( IKernel* pKernel );

	// 释放
	virtual bool Shut( IKernel* pKernel );
	
	// 读取GM权限定义文件
	bool LoadGMFile( IKernel* pKernel );

private:
	// 执行GM命令
	bool DoGMCommand( IKernel* pKernel, const PERSISTID& self, const wchar_t* content );

	// 判断GM是否有权限执行某个命令
	bool CanExecute( IKernel* pKernel, const PERSISTID& self, const char* command );

	// GM命令响应函数
	static int OnCustomGM( IKernel* pKernel, const PERSISTID& self, 
		const PERSISTID& sender, const IVarList& args );

	// 执行gm命令
	bool RunGmCommand(IKernel* pKernel, const PERSISTID& self, const char* cmd, const IVarList& args, int nOffset, int nCount, std::string& outMsg);

	// gm配置重读
	static void ReloadConfig(IKernel *pKernel);

	// GM命令级别限制（map<GM命令字，级别>）
	std::unordered_map< std::string, int > m_mapGMLimit;
	
	int m_nGMDefaultRight;
public:
	friend class GmccModule;
	static GMModule* m_pGMModule;

};

#endif	// __GMModule_H__