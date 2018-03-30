#ifndef __GUILD_BUILDING_MANAGE_H__
#define __GUILD_BUILDING_MANAGE_H__
#include "GuildBase.h"
#include <map>
#include "GuildNumManage.h"
#include "utils\string_util.h"

#define CLOSE_SERVER_UNIX_TIME "close_server_unix_time" //关服时间戳

#define BUILD_EXP_CYCLE_TIME 2000 //建筑经验增加周期

class CheckBuildLvUpCondition{
public:
	virtual int  Run(const wchar_t* guildName) = 0;
	virtual void LoadXml(const char* strValue) = 0;
};

class DoDecCostActionByBuildLvUp{
public:
	virtual bool  Run(const wchar_t* guildName) = 0;
	virtual void LoadXml(const char* strValue) = 0;
};

class GuildBuildingManage :public GuildBase{

public:
	GuildBuildingManage(){
		m_pInstance = this;
	}
	virtual bool Init(IPubKernel* pPubKernel);
	bool CreateGuildInit(IPubData* guildData);
	virtual int OnPublicMessage(IPubKernel* pPubKernel, int source_id,
		int scene_id, const IVarList& msg);
	
	//获得正在建设中的建筑物数量
	int GetBuildingLvingNum(IPubKernel* pPubKernel, const wchar_t* guildName);
	//获得维护资金消耗
	int GetMainTainBuildingCost(const wchar_t*guildName);
	static GuildBuildingManage* m_pInstance;
	bool LoadResConfig(IPubKernel*pPubKernel);
	
	
	//增加建筑经验
	void OnAddGuildildBuildExp(IPubKernel*pPubKernel,const wchar_t*guildName,int expValue);

	int GetBuildingLevel(const wchar_t* guildName,GUILD_BUILDING_TYPE type );

	//定时维护
	static void TimingMaintain(IPubKernel*pPubKernel, const char* space_name, const wchar_t* data_name);
	//维护是否成功
	bool IsMaintain(const wchar_t*guildName);
	// 通知帮会成员 帮会等级改变
	int CommandGuildLevelChange(IPubKernel* pPubKernel, const wchar_t* guildName, int level, bool bUp);
	// 通知帮会成员 建筑升级
	void BroadcastBuildingLevelup(IPubKernel* pPubKernel, const wchar_t* guildName, int nLevel, int building);
	bool OnLevelUpBuilding(IPubKernel* pPubKernel, int source_id,
		int scene_id, const IVarList& msg);
	// 检测活动状态心跳
	static int Update(IPubKernel* pPubKernel, const char* space_name, const wchar_t* data_name, int time);

	void AddAllGuildBuildingExp(IPubKernel* pPubKernel,int exp);
private:
	void GuildLvUp(IPubKernel* pPubKernel,const wchar_t*guildName,int level);
	//组织建筑升级经验配置	
	bool LoadGuildBuingLvUpExpConfig(IPubKernel*pPubKernel);
	//组织建筑升级配置
	bool LoadGuildBuingLvUpConfig(IPubKernel*pPubKernel);
	//组织维护消耗配置
	bool LoadGuildBuildingMaintainCost(IPubKernel*pPubKernel);

	
	void SendGuildingLevelUpResult(IPubKernel* pPubKernel, int sourceId, int sceneId,
		 const wchar_t* playerName, int result);
	int GetBuildingLvExp(int buildingType,int level);
	
	
	//停止研究
	bool StopLevelUpBuilding(IPubKernel* pPubKernel, int source_id,
		int scene_id, const IVarList& msg);


	//检查升级条件
	std::map<std::string, std::function<CheckBuildLvUpCondition*(void)> > m_CheckBuildingLvUpConditoinList;

	//<建筑类型,<建筑等级,检查函数>>
	std::map<int, std::map<int, std::vector<CheckBuildLvUpCondition*>>> m_checkBuildingLvUpCondition;
	//扣除升级消耗
	std::map<std::string, std::function<DoDecCostActionByBuildLvUp*(void)>> m_DoDecCostActionByBuildingLvUpList;
	//<建筑类型,<建筑等级,消耗函数>>
	std::map<int, std::map<int, std::vector<DoDecCostActionByBuildLvUp*>>> m_doDecCostActionByBuildingLvUp;
	//建筑经验升级表
	std::map<int, std::map<int,int>> m_guildBuildingLvUpExp;
	//维护表  <建筑类型,<建筑等级,消耗>>
	std::map<int, std::map<int, int>> m_guildBuildingMaintainCost;

};

enum
{
	BUILD_LVUP_RET_ERROR		= -1,
	BUILD_LVUP_RET_SUCCESS,
	BUILD_LVUP_RET_NO_ENOUGH_GUILD_CAPITAL,			 //建筑资金不足
	BUILD_LVUP_RET_NO_ENOUGH_GUILD_FANRONGDU,			 //繁荣度不足
	BUILD_LVUP_RET_NO_ENOUGH_GUILD_BUILD_TYPE,			 //建筑物种类不足
	BUILD_LVUP_RET_NO_ENOUGH_GUILD_BUILD_LEVEL,		 //建筑物等级不足
	BUILD_LVUP_RET_NO_ENOUGH_GUILD_BUILD_JUYITING_LEVEL,//聚义厅等级不足
	BUILD_LVUP_STOP_BUILDING_ERROR,//建筑停止建设失败
	BUILD_LVUP_STOP_BUILDING_SUCCESS,//建筑停止建设成功
	BUILD_LVUP_BUILDING_SAME_TIME_MAX_NUM,//到达同时建设建筑数量最大值   
	BUILD_LVUP_MI_MAO_ENOUGHT, // 秘宝数量不够
};




//检查组织资金
class CheckGuildCapital:public CheckBuildLvUpCondition{

public:
	int  Run(const wchar_t* guildName){
		if (GuildNumManage::m_pInstance->GetGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL) >= m_value)
		{
			return BUILD_LVUP_RET_SUCCESS;
		}
		return BUILD_LVUP_RET_NO_ENOUGH_GUILD_CAPITAL;
	}

	void LoadXml(const char* strValue)
	{
		m_value = StringUtil::StringAsInt(strValue);
	}
private:

	int m_value{0};
};
//检查聚义厅资金
class CheckMainTainCostAndSelfGuildCapital :public CheckBuildLvUpCondition{

public:
	int  Run(const wchar_t* guildName){
		int cost = m_value + GuildBuildingManage::m_pInstance->GetMainTainBuildingCost(guildName);
		if (GuildNumManage::m_pInstance->GetGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL) >= cost )
		{
			return BUILD_LVUP_RET_SUCCESS;
		}
		return BUILD_LVUP_RET_NO_ENOUGH_GUILD_CAPITAL;
	}
	void LoadXml(const char* strValue)
	{
		m_value = StringUtil::StringAsInt(strValue);
	}
private:

	int m_value{0};
};


//检查繁荣度
class CheckGuildFanRongDu :public CheckBuildLvUpCondition{
public:

	int  Run(const wchar_t* guildName){
		if (GuildNumManage::m_pInstance->GetGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_FANRONGDU) >=m_value)
		{
			return BUILD_LVUP_RET_SUCCESS;
		}
		return BUILD_LVUP_RET_NO_ENOUGH_GUILD_FANRONGDU;
	}

	void LoadXml(const char* strValue)
	{
		m_value = StringUtil::StringAsInt(strValue);
	}

private:
	int m_value{0};
};
//检查建筑类别和建筑等级
class ChekcGuildBuildTypeAndLevel :public CheckBuildLvUpCondition{
public:
	int  Run(const wchar_t* guildName){
		const std::wstring guildDataName = guildName + GuildBuildingManage::m_pInstance->m_ServerId;
		IPubData *guildData = GuildBuildingManage::m_pInstance->m_pPubSpace->GetPubData(guildDataName.c_str());
		if (guildData == NULL){
			return BUILD_LVUP_RET_ERROR;
		}
		IRecord * pGuildBuildingRc = guildData->GetRecord(GUILD_BUILDING_REC);
		if (pGuildBuildingRc == NULL){
			return BUILD_LVUP_RET_ERROR;
		}
		int rows = pGuildBuildingRc->GetRows();
		if (m_buildTypeNum > rows){
			return BUILD_LVUP_RET_NO_ENOUGH_GUILD_BUILD_TYPE;
		}
		int reachNum = 0;
		for (int row = 0; row < rows;row++)
		{
			int buildLevel = pGuildBuildingRc->QueryInt(row, GUILD_BUILDING_REC_COL_LEVEL);
			if (buildLevel >= m_buildLevel){
				++reachNum;
			}
		}
		if (m_buildTypeNum > reachNum)
		{
			return BUILD_LVUP_RET_NO_ENOUGH_GUILD_BUILD_LEVEL;
		}
		return BUILD_LVUP_RET_SUCCESS;
	}
	
	void LoadXml(const char* strValue){
		CVarList result;
		StringUtil::SplitString(result,strValue,",");
		if (result.GetCount() < 2){ return; }
		m_buildTypeNum = result.IntVal(0);
		m_buildLevel = result.IntVal(1);
	};

private:
	int m_buildTypeNum{0};
	int m_buildLevel{0};
};
//检查聚义厅等级
class CheckGuildJuYiTingBuildLv :public CheckBuildLvUpCondition{
public:
	int  Run(const wchar_t* guildName){
		int buildLevel = GuildBuildingManage::m_pInstance->GetBuildingLevel(guildName, GUILD_BUILDING_TYPE::BUILD_BUILDING_TYPE_JU_YI_TING);
		if (m_value > buildLevel)
		{
			return BUILD_LVUP_RET_NO_ENOUGH_GUILD_BUILD_JUYITING_LEVEL;
		}
		return BUILD_LVUP_RET_SUCCESS;
	}
	void LoadXml(const char* strValue){
		m_value = StringUtil::StringAsInt(strValue);
	}

private:

	int m_value{ 0 };

};


#define ADD_GUILD_BUILD_LVUP_CONDITION_FUNC(x)	\
	m_CheckBuildingLvUpConditoinList[#x] = []()->CheckBuildLvUpCondition*{return new x();}



class DoDecGuildCapital :public DoDecCostActionByBuildLvUp{


public:

	bool Run(const wchar_t* guildName){
		GuildNumManage::m_pInstance->DecGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_CAPITAL, m_value, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_BUILD_LVUP);
		return true;
	}



	void LoadXml(const char* strValue){
		m_value = StringUtil::StringAsInt(strValue);
	}

private :
	int m_value{0};

};

//扣除繁荣度
class DoDecGuildFanYongDu :public DoDecCostActionByBuildLvUp
{

public:

	bool Run(const wchar_t* guildName){
		GuildNumManage::m_pInstance->DecGuildNumValue(guildName, GUILD_NUM_TYPE::GUILD_NUM_FANRONGDU, m_value, GUILD_NUM_CHANGE::GUILD_NUM_CHANGE_BUILD_LVUP);
		
		return true;
	}

	void LoadXml(const char* strValue){
	 m_value = StringUtil::StringAsInt(strValue);
	}


private:
	int m_value{0};
};


#define ADD_GUILD_BUILD_LVUP_DO_DEC_ACTION(x) \
	m_DoDecCostActionByBuildingLvUpList[#x] = []()->DoDecCostActionByBuildLvUp*{return new x();};






#endif