//--------------------------------------------------------------------
// 文件名:      Server\FsGame\SkillModule\CoolDownModule.cpp
// 内  容:      冷却系统定义
// 说  明:
// 创建日期:    2008/01/28
// 创建人:      冯红文
// 修改人:		  lixiongwen
//    :       
//--------------------------------------------------------------------
#include "CoolDownModule.h"

#include "utils/util_func.h"
#include "FsGame/Define/CoolDownDefine.h"
#include "../Define/ServerCustomDefine.h"
#include "utils/custom_func.h"
#include "utils/XmlFile.h"
#include "../CommonModule/LuaExtModule.h"
#include <algorithm>

#ifndef FSROOMLOGIC_EXPORTS
#include "CommonModule/ReLoadConfigModule.h"
#endif

const char* COOLDOWN_REC = "cooldown_rec";                  //冷却表

CoolDownModule* CoolDownModule::m_pInstance = NULL;

// 功能：清空CD表
int nx_clear_cd_rec(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	// 检查参数数量
	CHECK_ARG_NUM(state, nx_clear_cd_rec, 1);
	// 检查参数类型
	CHECK_ARG_OBJECT(state, nx_clear_cd_rec, 1);

	// 获取参数
	PERSISTID self = pKernel->LuaToObject(state, 1);

	CoolDownModule::m_pInstance->ClearAllCD(pKernel, self);
	return 1;
}

// 读取配置
int nx_reload_cd_config(void* state)
{
	IKernel* pKernel = LuaExtModule::GetKernel(state);

	CoolDownModule::m_pInstance->LoadConfig(pKernel);

	return 1;
}

//初始化
bool CoolDownModule::Init(IKernel* pKernel)
{
	m_pInstance = this;
	if (!LoadConfig(pKernel))
	{
		return false;
	}

	DECL_LUA_EXT(nx_clear_cd_rec);
	DECL_LUA_EXT(nx_reload_cd_config);

	pKernel->AddEventCallback("player", "OnStore", CoolDownModule::OnPlayerStore);

#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("CooldownConfig", CoolDownModule::ReloadConfig);
#endif // FSROOMLOGIC_EXPORTS
	return true;
}

//释放
bool CoolDownModule::Shut(IKernel* pKernel)
{
	return true;
}

CoolDownModule* CoolDownModule::Instance()
{
	return m_pInstance;
}
//////////////////////////////////////////////////////////////////////////
//开始一个冷却分类的冷却
bool CoolDownModule::BeginCoolDown(IKernel* pKernel, const PERSISTID& self,
								   int cooldowncategory, int cooldowntime )
{
    IGameObj* pSelfObj = pKernel->GetGameObj(self);
    if (NULL == pSelfObj)
    {
        return false;
    }

	if (cooldowntime <= 0)
	{
		return true;
	}

	if (pSelfObj->FindData("NoCoolDown") &&
		pSelfObj->QueryDataInt("NoCoolDown") > 0)
	{//GM命令 开启或者关闭技能CD
		return true;
	}

	IRecord* pRecordCool = pSelfObj->GetRecord(COOLDOWN_REC);
	if (NULL == pRecordCool)
	{
		return false;
	}

	//开始时间
	int64_t now = ::util_get_time_64();

	//结束时间
	int64_t endtime = now + cooldowntime;

	if (!InnerBeginCoolDown(pRecordCool, cooldowncategory, now, endtime))
	{
		return false;
	}

	return true;
}

/**
@brief        结束一个冷却分类的冷却
@param        参数
[IN]cooldowncategory:冷却分类编号
@remarks
@return       如果成功开始冷却，返回true，否则，返回false
*/
bool CoolDownModule::EndCoolDown(IKernel* pKernel, const PERSISTID& self, int cooldowncategory)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	IRecord* pRecordCool = pSelfObj->GetRecord(COOLDOWN_REC);
	if (NULL == pRecordCool)
	{
		return false;
	}

	//是否存在该了冷却分类
	int row = pRecordCool->FindInt(COOLDOWN_REC_COL_ID, cooldowncategory);
	if (row < 0)
	{
		return false;
	}
	pRecordCool->RemoveRow(row);
	return true;
}

//指定分类是否正在冷却中
bool CoolDownModule::IsCoolDown(IKernel* pKernel, const PERSISTID& self, int cooldowncategory)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	IRecord* pRecord = pSelfObj->GetRecord(COOLDOWN_REC);

	if (pRecord == NULL)
	{
		return false;
	}

	// 现在的时间
	int64_t now = ::util_get_time_64();

	if (InnerIsCoolDown(pRecord, cooldowncategory, now) >= 0)
	{
		return true;
	}

	return false;
}

// 取得冷却的剩余秒数
int64_t CoolDownModule::GetMillisecRemain(IKernel* pKernel, const PERSISTID& self, int cooldowncategory)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return 0;
	}

    IRecord* pRecord = pSelfObj->GetRecord(COOLDOWN_REC);
    if (pRecord == NULL)
    {
        return 0;
    }

    // 现在的时间
    int64_t now = ::util_get_time_64();
    
    int row = pRecord->FindInt(COOLDOWN_REC_COL_ID, cooldowncategory);

    if (row < 0)
    {
        return 0;
    }

    //结束时间
    int64_t endtime = pRecord->QueryInt64(row, COOLDOWN_REC_COL_ENDTIME);
    
    return (endtime - now);
}

//清除全部CD
void CoolDownModule::ClearAllCD(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}

	IRecord* pRecordCool = pSelfObj->GetRecord(COOLDOWN_REC);
	if (NULL != pRecordCool)
	{
		pRecordCool->ClearRow();
	}
}

//清除技能CD
void CoolDownModule::ClearPlayerNormalSkillCD(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return;
	}

	IRecord* pRecordCool = pSelfObj->GetRecord(COOLDOWN_REC);
	if (NULL == pRecordCool)
	{
		return;
	}
	int nRows = pRecordCool->GetRows();
	LoopBeginCheck(a);
	for (int i = nRows - 1; i >= 0; i--)
	{
		LoopDoCheck(a);

		int nCategory = pRecordCool->QueryInt(i, COOLDOWN_REC_COL_ID);
		if (nCategory >= COOLDOWN_SKILL_BEGIN && nCategory <= COOLDOWN_PlAYER_NORMAL_SKILL)
		{
			pRecordCool->RemoveRow(i);
		}
	}
}

// 玩家下线
int CoolDownModule::OnPlayerStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return 0;
	}

	IRecord* pRecordCool = pSelfObj->GetRecord(COOLDOWN_REC);
	if (NULL == pRecordCool)
	{
		return 0;
	}
	int nRows = pRecordCool->GetRows();
	LoopBeginCheck(c)
	for (int i = nRows - 1; i >= 0; i --)
	{
		LoopDoCheck(c)
		int nCategory = pRecordCool->QueryInt(i, COOLDOWN_REC_COL_ID);
		// 不需要保存的CD,下线后清除
		if (!m_pInstance->IsNeedSaveCategory(nCategory))
		{
			pRecordCool->RemoveRow(i);
		}
	}
	return 0;
}

//添加冷却信息倒冷却表
bool CoolDownModule::InnerBeginCoolDown(IRecord* pRecordCool, const int& cooldowncategory,
										const int64_t& begin_time, const int64_t& end_time)
{
	//如果冷却表满了 清理一次
	if (pRecordCool->GetRows() >= pRecordCool->GetRowMax())
	{
		ClearCoolDownRec(pRecordCool);
	}

	//是否存在该了冷却分类
	int row = pRecordCool->FindInt(COOLDOWN_REC_COL_ID, cooldowncategory);
	if (row < 0)
	{
		pRecordCool->AddRowValue(-1, CVarList() << cooldowncategory << begin_time << end_time);
	}
	else
	{
		pRecordCool->SetInt64(row, COOLDOWN_REC_COL_BEGINTIME, begin_time);
		pRecordCool->SetInt64(row, COOLDOWN_REC_COL_ENDTIME, end_time);
	}
	return true;
}


// 检查是否正在冷却，如果在冷却返回行号，如果不在冷却返回-1
int CoolDownModule::InnerIsCoolDown(IRecord* pRecordCool, const int cooldowncategory, const int64_t now)
{
	int row = pRecordCool->FindInt(COOLDOWN_REC_COL_ID, cooldowncategory);

	if (row < 0)
	{
		return -1;
	}

	//int n = pRecordCool->GetRows();
	//结束时间
	int64_t endtime = pRecordCool->QueryInt64(row, COOLDOWN_REC_COL_ENDTIME);

	if (now < endtime)
	{
		//冷却中
		return row;
	}
	else
	{
		//不在冷却中，可以删除此行数据(防止误删聊天类型的冷却时间间隔)
		if (cooldowncategory > COOLDOWN_CAREGOTYID_CHATTYPE_END)
		{
			pRecordCool->RemoveRow(row);
		}
		return -1;
	}
}


bool CoolDownModule::ClearCoolDownRec(IRecord* pRecord)
{
	if (pRecord == NULL)
	{
		return false;
	}
	int nRows = pRecord->GetRows();

	int64_t now = ::util_get_time_64();

	LoopBeginCheck(b);
	for (int i = nRows - 1; i >= 0; i --)
	{
		LoopDoCheck(b);

		int64_t endtime = pRecord->QueryInt64(i, COOLDOWN_REC_COL_ENDTIME);

		if (now >= endtime)
		{
			pRecord->RemoveRow(i);
		}
	}

	return true;
}

// 读取配置
bool CoolDownModule::LoadConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/Skill/Char/SaveCoolDownCategory.xml";
	CXmlFile xml(pathName.c_str());
	std::string errMsg;
	if (!xml.LoadFromFile(errMsg))
	{
		// 路径配置文件无法打开
		extend_warning(LOG_WARNING, errMsg.c_str());
		return false;
	}

	m_vSavedCategoryIds.clear();

	CVarList vSectionList;
	xml.GetSectionList(vSectionList);
	int nSize = (int)vSectionList.GetCount();
	m_vSavedCategoryIds.resize(nSize);
	LoopBeginCheck(c)
	for (int index = 0; index < nSize; ++index)
	{
		LoopDoCheck(c)
		const char* section = vSectionList.StringVal(index);
		if (NULL == section)
		{
			continue;
		}
		
		m_vSavedCategoryIds[index] = xml.ReadInteger(section, "Category", 0);
	}

	return true;
}

// 是否需要保存的CD分类
bool CoolDownModule::IsNeedSaveCategory(int nCategory)
{
	bool bSave = false;
	std::vector<int>::iterator iter = find(m_vSavedCategoryIds.begin(), m_vSavedCategoryIds.end(), nCategory);
	if (iter != m_vSavedCategoryIds.end())
	{
		bSave = true;
	}

	return bSave;
}

void CoolDownModule::ReloadConfig(IKernel* pKernel)
{
	m_pInstance->LoadConfig(pKernel);
}