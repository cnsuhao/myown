#include "LogicClassModule.h"
#include "utils/extend_func.h"
#include "utils/util_func.h"
#include <string>

extern IKernel* g_pKernel;

LogicClassModule* LogicClassModule::m_pLogicClassModule = NULL;

bool LogicClassModule::Init(IKernel* pKernel)
{
	g_pKernel = pKernel;

	m_pLogicClassModule = this;

	InitLogicClass(pKernel);

	return true;
}

bool LogicClassModule::Shut(IKernel* pKernel)
{

    LoopBeginCheck(a);
	for (std::map<std::string, ClassStruct*>::iterator iter = m_mapClassStructFiles.begin();
		iter != m_mapClassStructFiles.end();
		++iter)
	{
        LoopDoCheck(a);

		if (iter->second != NULL)
		{
			delete iter->second;
		}
	}
	m_mapClassStructFiles.clear();
	return true;
}

int LogicClassModule::OnCreateClass(IKernel* pKernel, int index)
{

    LoopBeginCheck(b);
	for (std::map<std::string, ClassStruct*>::iterator iter = m_pLogicClassModule->m_mapClassStructFiles.begin();
		iter != m_pLogicClassModule->m_mapClassStructFiles.end();
		++iter)
	{
        LoopDoCheck(b);

		const std::string& clsname = iter->first;
		ClassStruct* cstruct = iter->second;
		if (clsname.empty() || NULL == cstruct)
		{
			continue;
		}
		int classIndex = pKernel->GetClassIndex(clsname.c_str());
		if (classIndex == index)
		{
			if (cstruct->bHasCreate)
			{
				return 0;
			}

			const std::string& strParentName = cstruct->m_strParentName;
			if (!strParentName.empty())
			{
				LoadParentPath(pKernel, index, strParentName);
			}

			const std::string& strPath = cstruct->m_strMyPath;
			if (!strPath.empty())
			{
				createclass_from_xml(pKernel, index, strPath.c_str());
			}

			cstruct->bHasCreate = true;
			break;
		}
	}

	return 0;
}

void LogicClassModule::LoadParentPath(IKernel* pKernel, int index, const std::string& parent_class)
{
	std::map<std::string, ClassStruct*>::iterator iter = m_pLogicClassModule->m_mapClassStructFiles.find(parent_class);
	if (iter == m_pLogicClassModule->m_mapClassStructFiles.end())
	{
		return;
	}

	ClassStruct* cstruct = iter->second;
	if (!cstruct)
	{
		return;
	}

	if (cstruct->m_pParentStruct != NULL)
	{
		LoadParentPath(pKernel, index, cstruct->m_strParentName);
	}

	const std::string& strPath = cstruct->m_strMyPath;
	if (!strPath.empty())
	{
		createclass_from_xml(pKernel, index, strPath.c_str());
	}
}

bool LogicClassModule::InitLogicClass(IKernel* pKernel)
{
	std::string xmlpath = pKernel->GetResourcePath();

#ifndef FSROOMLOGIC_EXPORTS
	xmlpath += "logic_class.xml";
#else
	xmlpath += "room_logic_class.xml";
#endif // FSROOMLOGIC_EXPORTS

	//判断文件是否存在
	TiXmlDocument doc(xmlpath.c_str());

	if (!doc.LoadFile())
	{
		return false;
	}

	TiXmlElement* pNode =
		doc.FirstChild("logicclass")->FirstChildElement("class");


    LoopBeginCheck(a);
	while (pNode != NULL)
	{
        LoopDoCheck(a);

		ProcessLogicClass(pKernel, pNode, NULL);

		//添加子类
		pNode = pNode->NextSiblingElement("class");
	}

	doc.Clear();

	return true;
}

//处理一个节点
bool LogicClassModule::ProcessLogicClass(IKernel* pKernel, TiXmlElement* pNode,
										 TiXmlElement* pParentNode)
{
	std::string parent_class;

	if (pParentNode != NULL)
	{
		parent_class = pParentNode->Attribute("id");
	}

	const char* name = pNode->Attribute("id");
	const char* type = pNode->Attribute("type");
	const char* path = pNode->Attribute("path");
	if (NULL == name)
	{
		Assert(false);
		return false;
	}

	if (NULL == type)
	{
		TiXmlElement* pTempNode = pNode;

		//自己没有定义类型，获取父类的定义

        LoopBeginCheck(o);
		while (NULL == type)
		{
            LoopDoCheck(o);

			pTempNode = dynamic_cast<TiXmlElement*>(pTempNode->Parent());

			if (pTempNode == NULL)
			{
				Assert(false);
				return false;
			}

			type = ((TiXmlElement*)pTempNode)->Attribute("type");
		}
	}

	//添加逻辑对象
	AddLogicClass(pKernel, name, type, parent_class.c_str(), path);

	//添加子类
	TiXmlElement* pChildNode = pNode->FirstChildElement("class");


    LoopBeginCheck(p);
	while (pChildNode != NULL)
	{
        LoopDoCheck(p);

		ProcessLogicClass(pKernel, pChildNode, pNode);

		pChildNode = pChildNode->NextSiblingElement("class");
	}

	return true;
}

//添加一个逻辑类
bool LogicClassModule::AddLogicClass(IKernel* pKernel,
									 const char* logic_class,
									 const char* sztype,
									 const char* parent_class,
									 const char* szPath)
{
	int type = convert_class_type(sztype);
	pKernel->AddLogicClass(logic_class, type, parent_class);

	ClassStruct *temp = new ClassStruct();
	temp->m_strMyPath = szPath? szPath : "";
	temp->m_strParentName = parent_class ? parent_class : "";
	temp->m_pParentStruct = NULL;
	temp->bHasCreate = false;
	if (parent_class && *parent_class)
	{
		temp->m_pParentStruct = m_mapClassStructFiles[parent_class];
	}
	m_mapClassStructFiles[logic_class] = temp;

	pKernel->AddClassCallback(logic_class, "OnCreateClass", OnCreateClass);
	return true;
}
