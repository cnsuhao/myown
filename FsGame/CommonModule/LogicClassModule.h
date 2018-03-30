
//管理服务端所有逻辑类型的模块
//必须保证此模块第一个被引擎加载

#ifndef __LogicClassModule_H__
#define __LogicClassModule_H__

#include "Fsgame/Define/header.h"

#include "SDK/tinyxml/tinyxml.h"
#include <map>

class LogicClassModule : public ILogicModule
{
public:
	virtual bool Init(IKernel* pKernel);
	virtual bool Shut(IKernel* pKernel);

protected:
	//初始化逻辑类型的定义
	bool InitLogicClass(IKernel* pKernel);

	//处理一个节点
	bool ProcessLogicClass(IKernel* pKernel, TiXmlElement* pNode,
		TiXmlElement* pParentNode);

	//添加一个逻辑类
	bool AddLogicClass(IKernel* pKernel, const char* logic_class,
		const char* sztype, const char* parent_class, const char* szPath);

private:
	static int OnCreateClass(IKernel* pKernel, int index);

	static void LoadParentPath(IKernel* pKernel, int index, const std::string& parent_class);

private:
	static LogicClassModule* m_pLogicClassModule;

	struct ClassStruct 
	{
		ClassStruct() : m_pParentStruct(NULL), bHasCreate(false){}
		ClassStruct* m_pParentStruct;
		std::string m_strMyPath;
		std::string m_strParentName;
		bool bHasCreate;
	};
	std::map<std::string, ClassStruct*> m_mapClassStructFiles;
};

#endif //__LogicClassModule_H__