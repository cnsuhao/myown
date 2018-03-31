//----------------------------------------------------------
// 文件名:      CreateMirrorModule.h
// 内  容:      创建离线玩家镜像
// 说  明:		
// 创建日期:    2015年3月16日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#include "CopyOffLineModule.h"
#include "utils\XmlFile.h"
#include "..\Define\PubDefine.h"
#include "utils\extend_func.h"
#include "utils\string_util.h"
#include "..\Define\SnsDefine.h"
#include "..\Interface\FightInterface.h"
#include "..\CommonModule\LuaExtModule.h"
#include "..\SystemFunctionModule\StaticDataQueryModule.h"
#include "..\Define\StaticDataDefine.h"
#include "utils\util_func.h"
#include "..\SkillModule\SkillToSprite.h"
#include "../Define/GameDefine.h"
#include "../Define/FightNpcDefine.h"
#include "../Define/CommandDefine.h"
#include "..\CommonModule\EnvirValueModule.h"
#include <algorithm>
#include "utils\util.h"

std::wstring			CopyOffLineModule::m_wsDomainName = L"";	// 公共区名
CopyOffLineModule*		CopyOffLineModule::m_pCopyOffLineModule		= NULL;

#define LEVEL_SECTION_NUM 21		// 每个职业等级段配置个数
#define MAX_JOB_NUM 2				// 当前的职业

// 初始化
bool CopyOffLineModule::Init(IKernel* pKernel)
{
	m_pCopyOffLineModule = this;

	Assert(NULL != m_pCopyOffLineModule);

	// 接受PUB服务器消息
	pKernel->AddEventCallback("scene", "OnPublicMessage", CopyOffLineModule::OnPublicMessage);

	// 接受SNS服务器消息
	pKernel->AddEventCallback("scene", "OnSnsMessage", CopyOffLineModule::OnSNSMessage);

	// 读取配置
	if (!LoadResource(pKernel))
	{
		return false;
	}
	return true;
}

// 关闭
bool CopyOffLineModule::Shut(IKernel* pKernel)
{
	return true;
}

/*!
 * @brief	拷贝离线玩家数据
 * @param	self	NPC对象
 * @param   nLevel	等级
 * @return	int
 */
bool CopyOffLineModule::CopyOffLineData(IKernel* pKernel, const PERSISTID& self)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (NULL == pSelfObj)
	{
		return false;
	}

	// 查询加载数据所属玩家的uid 格式：string "domain", wstring L"Domain_Offline_serverid", int msgid, int nLevel, int nSceneId, PERSISTID npc, int nGroupId
	int nSceneId = pKernel->GetSceneId();

	int nGroupId = -1;
	if (pSelfObj->GetClassType() == TYPE_PLAYER)
	{
		nGroupId = pSelfObj->QueryInt("GroupID");
	}
	int nNationId = pSelfObj->QueryInt("Nation");
	// 此等级为当前玩家的等级
	int nLevel = (std::max)(pSelfObj->QueryInt("Level"), 1); // 最小为1级
	
	// 去向公共服务器发送查询玩家uid的消息
	CVarList pub_msg;
	pub_msg << PUBSPACE_DOMAIN;
	pub_msg << CopyOffLineModule::GetDomainName(pKernel).c_str();
	pub_msg << SP_OFFLINE_MSG_QUERY_PLAYER_UID;
	pub_msg << nLevel;
	pub_msg << nSceneId;
	pub_msg << self;
	pub_msg << nGroupId;
	pub_msg << nNationId;

	pKernel->SendPublicMessage(pub_msg);
	return true;
}

/*!
 * @brief	清除拷贝记录表
 * @param	int nGroupId
 * @return	void
 */
void CopyOffLineModule::ClearOfflinePubRec(IKernel* pKernel, int nGroupId)
{
	int nSceneId = pKernel->GetSceneId();
	// 去向公共服务器发送查询玩家uid的消息
	CVarList pub_msg;
	pub_msg << PUBSPACE_DOMAIN;
	pub_msg << CopyOffLineModule::GetDomainName(pKernel).c_str();
	pub_msg << SP_OFFLINE_MSG_CLEAR_GROUP_REC;
	pub_msg << nSceneId;
	pub_msg << nGroupId;

	pKernel->SendPublicMessage(pub_msg);
}

/*!
 * @brief	清除某一条记录表的数据
 * @param	int nGroupId
 * @param	PERSISTID self
 * @return	void
 */
void CopyOffLineModule::ClearOfflinePubRecOneRow(IKernel* pKernel, int nGroupId, const PERSISTID& self)
{
	int nSceneId = pKernel->GetSceneId();
	// 去向公共服务器发送查询玩家uid的消息
	CVarList pub_msg;
	pub_msg << PUBSPACE_DOMAIN;
	pub_msg << CopyOffLineModule::GetDomainName(pKernel).c_str();
	pub_msg << SP_OFFLINE_MSG_CLEAR_GROUP_REC_ONE_ROW;
	pub_msg << nSceneId;
	pub_msg << nGroupId;
	pub_msg << self;

	pKernel->SendPublicMessage(pub_msg);
}

/*!
 * @brief	拷贝离线玩家数据通过玩家uid
 * @param	self	NPC对象
 * @param	strUid	玩家uid
 * @return	bool
 */
bool CopyOffLineModule::CopyPlayerDataByUid(IKernel* pKernel, const PERSISTID& self, const char* strUid)
{
	if (!pKernel->Exists(self) || pKernel->Type(self) != TYPE_NPC)
	{
		return false;
	}
	if (!StringUtil::CharIsNull(strUid))
	{
		CVarList arg_list;
		int iDistribute_id = pKernel->GetDistrictId();
		int iServer_id = pKernel->GetServerId();
		int iMember_id = pKernel->GetMemberId();
		int iscene_id = pKernel->GetSceneId();
		arg_list << SPACE_SNS << L"OffLineDelay" << SNS_MSG_QUERY_PLAYER_FIGHT_DATA << self;
		pKernel->SendSnsMessage(iDistribute_id, iServer_id, iMember_id, iscene_id, strUid, arg_list);
	}
	return true;
}

// 加载配置
bool CopyOffLineModule::LoadResource(IKernel* pKernel)
{
	// 读取数据信息配置
	if (!LoadDataInfoConfig(pKernel))
	{
		return false;
	}
	// 读取默认名字
	if (!LoadDefaultName(pKernel))
	{
		return false;
	}

	return true;
}

// 读取数据信息配置
bool CopyOffLineModule::LoadDataInfoConfig(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/OffLine/MirrorPorperty.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}
	
	const int iSectionCount = (int)xmlfile.GetSectionCount();
	m_vDataInfo.reserve(iSectionCount);
	LoopBeginCheck(a)
	for (int i = 0; i < iSectionCount; i++)
	{
		LoopDoCheck(a)
		const char* strSectionName = xmlfile.GetSectionByIndex(i); 
		// 第一行配置为系数配置数据
		if (0 == i)
		{
			m_kRatioInfo.nLevelDiff		= xmlfile.ReadInteger(strSectionName, "LevelDiff", 0);
			m_kRatioInfo.fArenaRatio	= xmlfile.ReadFloat(strSectionName, "ArenaModifyRatio", 1.0f);
			const char* strRatio		= xmlfile.ReadString(strSectionName, "ModifyRatio", "");
			const char* strDCRatio		= xmlfile.ReadString(strSectionName, "DCChangeRatio", "");

			// 镜像npc等级差系数的值读取
			CVarList args;
			StringUtil::SplitString(args, strRatio, ",");
			int nCount = (int)args.GetCount();
			if (MAX_RATIO_NUM != nCount)
			{
				::extend_warning(LOG_ERROR, "Config %s Section 0 ModifyRatio is error", pathName.c_str());
				return false;
			}
			LoopBeginCheck(b)
			for (int j = 0;j < MAX_RATIO_NUM;++j)
			{
				LoopDoCheck(b)
				m_kRatioInfo.fRatio[j] = args.FloatVal(j);
			}
			// 默认属性变化率
			args.Clear();
			StringUtil::SplitString(args, strDCRatio, ",");
			nCount = (int)args.GetCount();
			if (2 != nCount)
			{
				::extend_warning(LOG_ERROR, "Config %s Section 0 DCChangeRatio is error", pathName.c_str());
				return false;
			}
			m_kRatioInfo.fMinDCRatio = args.FloatVal(0);
			m_kRatioInfo.fMaxDCRatio = args.FloatVal(1);
		}
		else
		{
			MirDataInfo info;
			info.nType		= xmlfile.ReadInteger(strSectionName, "Type", 0);
			info.strName	= xmlfile.ReadString(strSectionName, "DataName", "");
			info.bIsModify	= xmlfile.ReadInteger(strSectionName, "IsModify", 0) == 1;
			info.bIsLoadConfig = xmlfile.ReadInteger(strSectionName, "IsLoadConfig", 0) == 1;
			if (DATA_CONTAINER_TYPE == info.nType)
			{
				info.strSnsTable = xmlfile.ReadString(strSectionName, "SnsTable", "");
			}

			m_vDataInfo.push_back(info);
		}
	}

	return true;
}

// 读取默认的名字配置
bool CopyOffLineModule::LoadDefaultName(IKernel* pKernel)
{
	// 配置文件路径
	std::string pathName = pKernel->GetResourcePath();
	pathName += "ini/SystemFunction/OffLine/MirrorNameLib.xml";

	CXmlFile xmlfile(pathName.c_str());
	if (!xmlfile.LoadFromFile())
	{
		std::string info = "Not Exist " + pathName;
		::extend_warning(LOG_ERROR, info.c_str());
		return false;
	}

	const int iSectionCount = (int)xmlfile.GetSectionCount();
	m_vDefaultName.reserve(iSectionCount);
	LoopBeginCheck(c);
	for (int i = 0; i < iSectionCount; i++)
	{
		LoopDoCheck(c);
		const char* strSectionName = xmlfile.GetSectionByIndex(i); 
		std::wstring wsName	= StringUtil::StringAsWideStr(xmlfile.ReadString(strSectionName, "Name", ""));
		m_vDefaultName.push_back(wsName);
	}

	return true;
}

// 随机取出不同一组的名字
bool CopyOffLineModule::FindNameList(int nNameNum, std::vector<std::wstring>& vecOutNameList)
{
	if (nNameNum <= 0)
	{
		return false;
	}
	vecOutNameList.clear();
	int nSize = (int)m_vDefaultName.size();
	nNameNum = __min(nSize, nNameNum);
	std::vector<int> vIndexList;
	util_different_Random_int(0, nSize, nNameNum, vIndexList);

	int nIndexSize = vIndexList.size();
	vecOutNameList.reserve(nIndexSize);
	LoopBeginCheck(y);
	for (int i = 0; i < nIndexSize; ++i)
	{
		LoopDoCheck(y);
		int nIndex = vIndexList[i];
		if (nIndex < 0 || nIndex >= nSize)
		{
			continue;
		}
		vecOutNameList.push_back(m_vDefaultName[nIndex]);
	}

	return true;
}

// 处理公共数据服务器下传的消息
int CopyOffLineModule::OnPublicMessage(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	/*
        消息格式
            [sub_msg_id][...]
    */
    int main_msg_id = args.IntVal(2);
    if (main_msg_id != PS_OFFLINE_MSG_FINDED_PLAYER_UID)
    {
        return 0;
    }
	// 待查询的玩家uid
	const char* strUid = args.StringVal(3);

	// 待赋值数据的npc对象
	PERSISTID npc = args.ObjectVal(4);
	if (!StringUtil::CharIsNull(strUid))
	{
		CVarList arg_list;
		int iDistribute_id = pKernel->GetDistrictId();
		int iServer_id = pKernel->GetServerId();
		int iMember_id = pKernel->GetMemberId();
		int iscene_id = pKernel->GetSceneId();
		arg_list << SPACE_SNS << L"OffLineDelay" << SNS_MSG_QUERY_PLAYER_FIGHT_DATA << npc;
		pKernel->SendSnsMessage(iDistribute_id, iServer_id, iMember_id, iscene_id, strUid, arg_list);
	}
	return 0;
}

// 处理SNS服务器下传的消息
int CopyOffLineModule::OnSNSMessage(IKernel* pKernel, const PERSISTID& self,
	const PERSISTID& sender, const IVarList& args)
{
	int msgid = args.IntVal(0);
	if (SNS_MSG_QUERY_PLAYER_FIGHT_DATA != msgid)
	{
		return 0;
	}
	
	PERSISTID npc = args.ObjectVal(1);
	IGameObj* pNpcObj = pKernel->GetGameObj(npc);
	if (NULL == pNpcObj)
	{
		return 0;
	}
	// 按照配置顺序取出数据,拷贝到npc上
	int nIndex = 2;

	// 如果第一个属性值为无效 则认定读取失败 加载默认配置
	int nDataType = args.GetType(nIndex);
	// 是否为无效值
	if (args.GetCount() <= 3 || VTYPE_INT == nDataType && NOT_VALID_VALUE == args.IntVal(nIndex))
	{
		return 0;
	}
	
	// 拷贝属性前等级为玩家的等级
	int nSelfLevel = pNpcObj->QueryInt("Level");

	int nSize = (int)m_pCopyOffLineModule->m_vDataInfo.size();
	LoopBeginCheck(e)
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(e)
		MirDataInfo& info = m_pCopyOffLineModule->m_vDataInfo[i];
		// 填充属性数据
		if (DATA_PROPERTY_TYPE == info.nType)
		{
			m_pCopyOffLineModule->CopyPropertyData(pKernel, pNpcObj, info.strName.c_str(), args, nIndex);
		}
		else if(DATA_CONTAINER_TYPE == info.nType) // 填充容器数据
		{
			m_pCopyOffLineModule->CopyContainerData(pKernel, pNpcObj, info.strName.c_str(), args, nIndex);
		}
		else // 填充表数据
		{
			m_pCopyOffLineModule->CopyRecordData(pKernel, pNpcObj, info.strName.c_str(), args, nIndex);
		}
	}

	CopyOffLineModule::m_pCopyOffLineModule->OnAferCopyData(pKernel, pNpcObj, nSelfLevel);
	return 0;
}

// 拷贝属性数据
bool CopyOffLineModule::CopyPropertyData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex)
{
	if (NULL == pSelfObj)
	{
		++nIndex;
		return false;
	}
	int nDataType = data.GetType(nIndex);
	// 是否为无效值 忽略处理
	if (VTYPE_INT == nDataType && NOT_VALID_VALUE == data.IntVal(nIndex))
	{
		++nIndex;
		return false;
	}

	// 属性没找到 忽略处理 防止策划配置错误
	if (!pSelfObj->FindAttr(strName))
	{
		++nIndex;
		return false;
	}

	// 分类型赋值属性
	int nAttrType = pSelfObj->GetAttrType(strName);
	if (nAttrType != nDataType)
	{
		++nIndex;
		return false;
	}
	switch(nAttrType)
	{
	case VTYPE_INT:
		{
			int nValue = data.IntVal(nIndex);
			pSelfObj->SetInt(strName, nValue);
		}
		break;
	case VTYPE_INT64:
		{
			int64_t nValue = data.Int64Val(nIndex);
			pSelfObj->SetInt64(strName, nValue);
		}
		break;
	case VTYPE_FLOAT:
		{
			float fValue = data.FloatVal(nIndex);
			pSelfObj->SetFloat(strName, fValue);
		}
		break;
	case VTYPE_STRING:
		{
			std::string strValue = data.StringVal(nIndex);
			pSelfObj->SetString(strName, strValue.c_str());
		}
		break;
	case VTYPE_WIDESTR:
		{
			std::wstring wsValue = data.WideStrVal(nIndex);
			pSelfObj->SetWideStr(strName, wsValue.c_str());
		}
		break;
	}

	++nIndex;
	return true;
}

// 拷贝容器数据
bool CopyOffLineModule::CopyContainerData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex)
{
	bool bResult = false;
	// 技能容器处理
	if (0 == strcmp(strName, "SkillContainer"))
	{
		bResult = CopySkillContainerData(pKernel, pSelfObj, strName, data, nIndex);
	}
	else if (0 == strcmp(strName, "PetBox"))
	{
		//bResult = CopyPetContainerData(pKernel, pSelfObj, strName, data, nIndex);
	}

	return bResult;
}

// 拷贝技能容器数据
bool CopyOffLineModule::CopySkillContainerData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex)
{
	int nRows = data.IntVal(nIndex++);
	int nCols = data.IntVal(nIndex++);
	// 数据不存在
	if (NULL == pSelfObj || 0 == nRows || MAX_SSR_SKILL_COLS != nCols)
	{
		return false;
	}
	
	// 先清空技能
	SKillToSpriteSingleton::Instance()->ClearAllSkill(pKernel, pSelfObj->GetObjectId());
	
	LoopBeginCheck(g)
	// 技能容器添加数据
	for (int i = 0;i < nRows;++i)
	{
		LoopDoCheck(g)
		const char* strSkill = data.StringVal(nIndex++);
		int nLevel = data.IntVal(nIndex++);

		PERSISTID skill = FightInterfaceInstance->AddSkill(pKernel, pSelfObj->GetObjectId(), strSkill);
		if (skill.IsNull())
		{
			continue;
		}
		pKernel->SetInt(skill, "Level", nLevel);
	}

	return true;
}

// 拷贝宠物容器数据
// bool CopyOffLineModule::CopyPetContainerData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex)
// {
// 	int nRows = data.IntVal(nIndex++);
// 	int nCols = data.IntVal(nIndex++);
// 	// 数据不存在
// 	if (NULL == pSelfObj || 0 == nRows)
// 	{
// 		return false;
// 	}
// 	// 只有竞技场npc 需要拷贝宠物数据
// 	if (ARENA_BATTLE_NPC != pSelfObj->QueryInt("PurposeType"))
// 	{
// 		return false;
// 	}
// 
// 	//创建宠物容器
// 	PERSISTID petBox = pKernel->CreateContainer(pSelfObj->GetObjectId(), PET_BOX_NAME, PetConfigManage::GetMaxPetNum());
// 	if (petBox.IsNull())
// 	{
// 		return false;
// 	}
// 	pKernel->SetWideStr(petBox, "Name", PET_BOX_NAME_W);
// 
// 	LoopBeginCheck(h)
// 	// 创建宠物对象
// 	for (int i = 0;i < nRows;++i)
// 	{
// 		LoopDoCheck(h)
// 		const char* strPetId = data.StringVal(nIndex++);
// 		PERSISTID pet = PetModule::AddPet(pKernel, pSelfObj->GetObjectId(), strPetId);
// 		IGameObj* pPetObj = pKernel->GetGameObj(pet);
// 		if (NULL == pPetObj)
// 		{
// 			continue;
// 		}
// 		// 设置宠物的属性值
// 		// 等级
// 		int nTmpVal = data.IntVal(nIndex++);
// 		pPetObj->SetInt("Level", nTmpVal);
// 		// 星级
// 		nTmpVal = data.IntVal(nIndex++);
// 		pPetObj->SetInt("StarLevel", nTmpVal);
// 		// 阶级
// 		nTmpVal = data.IntVal(nIndex++);
// 		pPetObj->SetInt("StepLevel", nTmpVal);
// 		// 出战状态
// 		nTmpVal = data.IntVal(nIndex++);
// 		pPetObj->SetInt("Work", nTmpVal);
// 		// 评分
// 		nTmpVal = data.IntVal(nIndex++);
// 		pPetObj->SetInt(FIELD_PROP_PET_SCORE, nTmpVal);
// 		// 宠物属性更新
// 		PetModule::UpdateMasterWorkAP(pKernel, pSelfObj->GetObjectId(), pet);
// 		PetModule::UpdatePetNpcAttribute(pKernel, pet);
// 		PetModule::UpdateSkillActivate(pKernel, pSelfObj->GetObjectId(), pet);
// 
// 		// 主角属性更新
// 		PetModule::PetHelpOnOrOff(pKernel, pSelfObj->GetObjectId(), strPetId, true);
// 
//  		const char* strPetSkillData = data.StringVal(nIndex++);
//  		SetPetSkillData(pKernel, pSelfObj, pet, strPetSkillData);
// 
// 		const char* strPetTalent = data.StringVal(nIndex++);
// 		SetPetTalentData(pKernel, pSelfObj, pet, strPetTalent);
// 	}
// 
// 	const int pet_num = PetModule::GetPetAll(pKernel, pSelfObj->GetObjectId(), CVarList());
// 	PetModule::UpdateRecruitPck(pKernel, pSelfObj->GetObjectId(), 0, pet_num);
// 	
// 	return true;
// }

// 拷贝表数据
bool CopyOffLineModule::CopyRecordData(IKernel* pKernel, IGameObj* pSelfObj, const char* strName, const IVarList& data, int& nIndex)
{
	int nRows = data.IntVal(nIndex++);
	int nCols = data.IntVal(nIndex++);
	// 数据不存在
	if (NULL == pSelfObj || 0 == nRows)
	{
		return false;
	}
	IRecord* pRecord = pSelfObj->GetRecord(strName);
	// 合理保护
	if (NULL == pRecord || nCols > pRecord->GetCols())
	{
		return false;
	}
	// 将sns数据填充至表中
	CVarList recdata;
	LoopBeginCheck(m)
	for (int i = 0;i < nRows;++i)
	{
		LoopDoCheck(m)
		recdata.Clear();
		LoopBeginCheck(n)
		for (int j = 0;j < nCols; ++j)
		{
			LoopDoCheck(n)
			int nColType = pRecord->GetColType(j);
			if(data.GetType(nIndex) != nColType)
			{
				continue;
			}
			switch(nColType)
			{
			case VTYPE_INT:
				recdata << data.IntVal(nIndex++);
				break;
			case VTYPE_INT64:
				recdata << data.Int64Val(nIndex++);
				break;
			case VTYPE_FLOAT:
				recdata << data.FloatVal(nIndex++);
				break;
			case VTYPE_STRING:
				recdata << data.StringVal(nIndex++);
				break;
			case VTYPE_WIDESTR:
				recdata << data.WideStrVal(nIndex++);
				break;
			}
		}

		pRecord->AddRowValue(-1, recdata);
	}
	return true;
}

// 设置宠物技能数据
// void CopyOffLineModule::SetPetSkillData(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& petitem, const char* strSkillData)
// {
// 	if (NULL == pSelfObj || !pKernel->Exists(petitem) || StringUtil::CharIsNull(strSkillData))
// 	{
// 		return;
// 	}
// 	CVarList skillArray;
// 	StringUtil::SplitString(skillArray, strSkillData, ",");
// 	// 设置所有宠物技能等级
// 	LoopBeginCheck(i)
// 	int nSkillNum = (int)skillArray.GetCount();
// 	for (int i = 0;i < nSkillNum;++i)
// 	{
// 		LoopDoCheck(i)
// 		const char* strData = skillArray.StringVal(i);
// 		CVarList data;
// 		StringUtil::SplitString(data, strData, ":");
// 		// 数据格式: 技能id:技能等级
// 		if (data.GetCount() != 2)
// 		{
// 			continue;
// 		}
// 		const char* strSkillId = data.StringVal(0);
// 		int nSkillLevel = data.IntVal(1);
// 		// 获取宠物技能,并设置技能等级
// 		PERSISTID skill = PetModule::GetPetSkill(pKernel, petitem, strSkillId);
// 		if(skill.IsNull())
// 		{
// 			return;
// 		}
// 		PetModule::SetPetSkillLevel(pKernel, pSelfObj->GetObjectId(), petitem, skill, nSkillLevel);
// 	}	
// }
// 
// // 设置宠物天赋数据
// void CopyOffLineModule::SetPetTalentData(IKernel* pKernel, IGameObj* pSelfObj, const PERSISTID& petitem, const char* strTalentData)
// {
// 	if (NULL == pSelfObj || !pKernel->Exists(petitem) || StringUtil::CharIsNull(strTalentData))
// 	{
// 		return;
// 	}
// 
// 	CVarList skillArray;
// 	StringUtil::SplitString(skillArray, strTalentData, ",");
// 	// 设置所有宠物天赋数据
// 	LoopBeginCheck(i);
// 	int nSkillNum = (int)skillArray.GetCount();
// 	for (int i = 0; i < nSkillNum; ++i)
// 	{
// 		LoopDoCheck(i);
// 		const char* strData = skillArray.StringVal(i);
// 	
// 		PetModule::AddTalentOnIdleSlot(pKernel, pSelfObj->GetObjectId(), petitem, strData);
// 	}
// }

// 获取domain的名字
const std::wstring& CopyOffLineModule::GetDomainName(IKernel * pKernel)
{
	if (m_wsDomainName.empty())
	{
		wchar_t wstr[256];
		const int server_id = pKernel->GetServerId();
		SWPRINTF_S(wstr, L"Domain_Offline_%d", server_id);

		m_wsDomainName = wstr;
	}

	return m_wsDomainName;
}

// 拷贝数据后的操作
void CopyOffLineModule::OnAferCopyData(IKernel* pKernel, IGameObj* pSelfObj, int nPlayerLevel)
{
	if (NULL == pSelfObj)
	{
		return;
	}
	
	float fModifyRatio = GetPropertyRatio(pKernel, pSelfObj, nPlayerLevel);

	int nSize = (int)m_pCopyOffLineModule->m_vDataInfo.size();
	LoopBeginCheck(l)
	for (int i = 0;i < nSize;++i)
	{
		LoopDoCheck(l)
		MirDataInfo& info = m_pCopyOffLineModule->m_vDataInfo[i];
		// 根据系数修改属性数据
		if (DATA_PROPERTY_TYPE == info.nType && info.bIsModify)
		{
			int nDataType = pSelfObj->GetAttrType(info.strName.c_str()); 
			if (VTYPE_INT == nDataType)
			{
				int nSrcVal = pSelfObj->QueryInt(info.strName.c_str());
				int nCurVal = (int)(fModifyRatio * (float)nSrcVal);
				pSelfObj->SetInt(info.strName.c_str(), nCurVal);
			}
			else if (VTYPE_FLOAT == nDataType)
			{
				float  fSrcVal = pSelfObj->QueryFloat(info.strName.c_str());
				float fCurVal = fModifyRatio * fSrcVal;
				pSelfObj->SetFloat(info.strName.c_str(), fCurVal);
			}	
		}	
	}

	// 拉取离线玩家实力补偿,竞技场npc不加buff
// 	const char* buffID = m_pCopyOffLineModule->GetLevelBuffer(nPlayerLevel);
// 	if( !StringUtil::CharIsNull(buffID) && ARENA_BATTLE_NPC != pSelfObj->QueryInt("PurposeType"))
// 	{
// 		FightInterfaceInstance->AddBuffer(pKernel, pSelfObj->GetObjectId(),  pSelfObj->GetObjectId(), buffID);
// 	}

	// 出生时满血
	int64_t nMaxHP = pSelfObj->QueryInt64("MaxHP");
	pSelfObj->SetInt64("HP", nMaxHP);
	// 设置为玩家的行走高度差
	float fWalkStep = EnvirValueModule::EnvirQueryFloat(ENV_VALUE_PLAYER_WALK_STEP_DEFAULT);
	pSelfObj->SetFloat("WalkStep", fWalkStep);

	// 设置攻击距离
	float fMinDis = 0.0f;
	float fMaxDis = 0.0f;
	FightInterfaceInstance->ReadyUseRandomSkill(pKernel, pSelfObj->GetObjectId(), fMinDis, fMaxDis);

	// 防止攻击距离为0,保护下
	fMaxDis = __max(2, fMaxDis);

	pSelfObj->SetFloat("MinAttackDist", fMinDis);
	pSelfObj->SetFloat("MaxAttackDist", fMaxDis);
	
	// 设置出生点坐标
	pSelfObj->SetFloat("BornX", pSelfObj->GetPosiX());
	pSelfObj->SetFloat("BornZ", pSelfObj->GetPosiZ());
	pSelfObj->SetInt("Invisible", 0);
	pSelfObj->SetInt("CantAttack", 0);
	pSelfObj->SetInt("CantBeAttack", 0);

	// 设置移动速度
	float fRunSpeed = EnvirValueModule::EnvirQueryFloat(ENV_VALUE_DEFAULT_RUN_SPEED);
	pSelfObj->SetFloat("RunSpeed", fRunSpeed);

	// 新手配置数据调整后可能会出现MinAttack > MaxAttack的情况, 做个保护
	int nMaxAttack = pSelfObj->QueryInt("MaxAttack");
	int nMinAttack = pSelfObj->QueryInt("MinAttack");
	if (nMinAttack > nMaxAttack)
	{
		pSelfObj->SetInt("MaxAttack", nMinAttack);
	}

// 	if (ARENA_BATTLE_NPC == pSelfObj->QueryInt("PurposeType"))
// 	{
// 		PetModule::RestoreWorkPetNpcAll(pKernel, pSelfObj->GetObjectId(), true);
// 	}

	// 发送内部消息
	pKernel->Command(pSelfObj->GetObjectId(), pSelfObj->GetObjectId(), CVarList() << COMMAND_OFFLINE_DATA_COPY_FINISH);
}

// 获取属性变化系数
const float CopyOffLineModule::GetPropertyRatio(IKernel* pKernel, IGameObj* pSelfObj, int nPlayerLevel)
{
	if (NULL == pSelfObj)
	{
		return 0.0f;
	}
	int nPurpose = NORMAL_BATTLE_NPC;
	if (pSelfObj->FindAttr("PurposeType"))
	{
		nPurpose = pSelfObj->QueryInt("PurposeType");
	}
	// 竞技场npc的实力系数
	if (ARENA_BATTLE_NPC == nPurpose)
	{
		return m_kRatioInfo.fArenaRatio;
	}

	int nSelfLevel = pSelfObj->QueryInt("Level");
	int nLevelDiff = nSelfLevel - nPlayerLevel;
	int nRatioType = GREATER_THAN_RATIO;
	if (nLevelDiff > m_kRatioInfo.nLevelDiff)
	{
		nRatioType = GREATER_THAN_RATIO;
	}
	else if (nLevelDiff <= m_kRatioInfo.nLevelDiff && nLevelDiff > 0)
	{
		nRatioType = LESS_THAN_RATIO;
	}
	else if(nLevelDiff <= 0)
	{
		nRatioType = LESS_THAN_ZERO_RATIO;
	}

	return m_kRatioInfo.fRatio[nRatioType];
}

// 计算默认属性变化率
const float CopyOffLineModule::ComputeDCRatio()
{
	// 获取随机的变化率
	float fRatio = m_kRatioInfo.fMinDCRatio + ::util_random_float(m_kRatioInfo.fMaxDCRatio - m_kRatioInfo.fMinDCRatio);
	return fRatio;
}