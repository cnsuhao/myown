//--------------------------------------------------------------------
// 文件名:      NpcCreatorModule.h
// 内  容:      NPC创建
// 说  明:		
// 创建日期:    2014年10月17日
// 创建人:        
// 修改人:        
//    :       
//--------------------------------------------------------------------
#ifndef _NPCCREATE_MODULE_H_
#define _NPCCREATE_MODULE_H_

#include "Fsgame/Define/header.h"
#include <string>
#include <map>
#include <vector>
#include "utils/util_ini.h"
#include "utils/util_func.h"

#define MAX_HERAT_BEAT_TIME (500)

#define GROUP_STATIC_CREATE_FLOW "create_group_static_flow"
#define GROUP_RANDOM_CREATE_FLOW "create_group_random_flow"
#define CONTROL_BUFFID            "Buff_MJ_001"
#define POSITION_INFO_REC "position_info_rec"       //野怪坐标点分布信息表
#define CREATE_OBJECT_LIST_REC "object_list_rec"    //生成器生成的对象列表
// 分组场景NPC创建进度表内容
enum
{
	CREATE_FLOW_SUB_SCENE_NO = 0,
	CREATE_FLOW_GROUP_ID,
	CREATE_FLOW_FILE_INDEX,
	CREATE_FLOW_ITEM_INDEX,
	CREATE_FLOW_SCENE_CFG_NAME,
};

// 需要刷新的对象表
enum PositionInfoRec
{
	POSIINFOREC_COL_X = 0,					// 坐标
	POSIINFOREC_COL_Y,					// 坐标
	POSIINFOREC_COL_Z,					// 坐标
	POSIINFOREC_COL_O,					// 方向
	POSIINFOREC_COL_USED,				// 是否已经被使用
	POSIINFOREC_COL_TIME,				// 上次死亡时间
	POSIINFOREC_COL_R,					// 随机半径
	POSIINFOREC_COL_COUNT
};

// 随机参数表
enum RandomParamListRec
{
	RPL_CONFIG = 0,		// 对象的编号
	RPL_REFRESH_NUM,	// 刷新数量	
	RPL_REFRESH_TIME,	// 刷新时间
};
//创建器刷新怪物的类型
enum
{
    CREATE_NPC_TYPE_COMMON = 0,
    CREATE_NPC_TYPE_ELITE = 1,
    CREATE_NPC_TYPE_BOSS = 2,
    CREATE_NPC_TYPE_BOX = 3,
};

// 每个场景的随机对象生成器通过该类管理
class NpcCreatorModule : public ILogicModule
{
	/*-----------------------------------系统回调函数----------------------------------------*/


private:
private:


	struct NpcPosition
	{
		float m_x{.0f};
		float m_y{.0f};
		float m_z{.0f};
		float m_ay{.0f};
	};

	struct RandomCreator
	{
		int m_no{0};
		std::string m_npcID;
		int m_createNum{0};
		int m_refreshTime{0};
		int m_step{0};
		int m_limit{0};
		std::string m_desc;
		std::vector<NpcPosition> m_positionGroup;


	};

	struct CreateSpringNpcInfo
	{
		std::string m_no;
		NpcPosition m_springPosition;// 触发器坐标
		float m_maxScope{.0f};
		float m_minScope{.0f};
		float m_springRange{.0f};
		int m_createType{0};
		int m_totalStep{0};
		bool m_isDestroy{true};
		int m_sceneAction{0};
		int m_totalNpcNum{0};
		std::vector<RandomCreator> m_NpcGroup;
	};

	struct CreateStaticNpcInfo
	{

		NpcPosition m_position;
		std::string m_npcID;
		int m_groupID;
		int m_nation;


	};

	struct CreateSceneNpcInfo
	{
		std::vector<CreateSpringNpcInfo> m_createRandGroup;
		std::vector<CreateStaticNpcInfo> m_createStaticGroup;
	};








public:
	// 系统初始化
	virtual bool Init(IKernel* pKernel);

	virtual bool Shut(IKernel* pKernel);

	// 随机生成器对象的回调
	static int OnRandomNpcCreatorCreateClass(IKernel* pKernel, int index);

	static int OnRandomNpcCreatorItemCreateClass(IKernel* pKernel, int index);

	static int OnRandomNpcCreatorSpring(IKernel* pKernel, const PERSISTID& creator,
		const PERSISTID& sender, const IVarList& args);

	static int OnRandomNpcCreatorEndSpring(IKernel* pKernel, const PERSISTID& creator,
		const PERSISTID& sender, const IVarList& args);

	static int OnNpcSpring(IKernel* pKernel, const PERSISTID& creator,
		const PERSISTID& sender, const IVarList& args);

	static int OnNpcEndSpring(IKernel* pKernel, const PERSISTID& creator,
		const PERSISTID& sender, const IVarList& args);

	// 场景回调
	static int OnSceneCreate(IKernel* pKernel, const PERSISTID& scene,
		const PERSISTID& sender, const IVarList& args);

	// Npc被删除
	static int OnNpcDestroy(IKernel* pKernel, const PERSISTID& npc,
		const PERSISTID& sender, const IVarList& args);
	//属性回调
	static int C_OnCurStepChanged(IKernel* pKernel, const PERSISTID& self, const char* property, const IVar& old);
	//清除
	void CleanSceneCfgName(IKernel*pKernel, int groupID);



	/*----------------------------------心跳函数----------------------------------------*/
public:
	// 心跳
	static int H_Refresh(IKernel* pKernel, const PERSISTID& creator, int slice);

	static int H_EndRefresh(IKernel* pKernel, const PERSISTID& creator, int slice);

	// 副本中分步创建静态npc的心跳
	static int H_DelayCreateStaticNpc(IKernel* pKernel, const PERSISTID& creator, int slice);

	// 副本中分步创建random npc的心跳
	static int H_DelayCreateRandomNpc(IKernel* pKernel, const PERSISTID& creator, int slice);

	// 分组场景中分步创建静态npc的心跳
	static int H_GroupSceneCreateStaticNpc(IKernel* pKernel, const PERSISTID& creator, int slice);

	// 分组场景中分步创建random npc的心跳
	static int H_GroupSceneCreateRandomNpc(IKernel* pKernel, const PERSISTID& creator, int slice);
	//分组场景中按时间创建 npc
	//static int H_GroupCreateNpcByTime(IKernel*pKernel, const PERSISTID& creator, int slice);


	/*----------------------------------外部接口----------------------------------------*/
public:
	// 根据场景id创建副本Npc
	bool CreateCloneNpcFromXml(IKernel* pKernel, const PERSISTID& scene);

	// 根据子id创建分组Npc
	bool CreateGroupNpcFromXml(IKernel* pKernel, const PERSISTID& scene,
		int nSubSceneNo, int nGroupId, const char* npcCfgName);

	// 创建任务刷怪器
	const PERSISTID CreateTaskrCreator(IKernel* pKernel, const PERSISTID& scene,
		const int task_id, const int group_id, const int level);
// 	void CreateGuildInbreakCreator(IKernel* pKernel, const PERSISTID& scene, const char* cfgName,
// 		const int group_id, const int level);
	/*----------------------------------内部接口----------------------------------------*/
protected:
	// 开始刷新
	static void BeginRefresh(IKernel* pKernel, const PERSISTID& creator);

	// 结束刷新
	static void EndRefresh(IKernel* pKernel, const PERSISTID& creator);

	// --------------静态npc创建相关函数---------------

	// 从xml文件中生成场景需要的静态或功能NPC
	// 创建静态npc
	bool CreateStaticNpc(IKernel* pKernel, const PERSISTID& scene,
		std::vector<CreateStaticNpcInfo>& staticNpc, int nSubSceneNo = -1, int nGroupID = -1);

	// 根据xml配置创建某个静态npc
	PERSISTID CreateStaticNpcUnit(IKernel* pKernel, const PERSISTID& scene,
		CreateStaticNpcInfo&pItem, int nSubSceneNo = -1, int nGroupId = -1);

	// --------------随即npc创建器创建相关函数---------------

	// 从xml文件中生成场景需要的随机NPC生成器
	// 创建随机Npc
	bool CreateRandomNpc(IKernel* pKernel, const PERSISTID& scene,
		std::vector<CreateSpringNpcInfo>&npcCfg, int nSubSceneNo = -1, int nGroupID = -1);

	bool CreateARandomCreator(IKernel* pKernel, const PERSISTID& scene,
		CreateSpringNpcInfo&pCreate, int nSubSceneNo, int nGroupId);

	//提前设定精英怪刷新位置，方便后续小怪跟随
	bool PreSetCreateCenterPos(IKernel* pKernel, const PERSISTID& creator);
	//按照刷新时间创建npc
	bool CreateRandomNpcByTime(IKernel*pkernel, const PERSISTID& scene, int sceneId, int groupId);

	void AddSceneGroupNpcCfgName(int sceneId, int groupId, const std::string& sceneCfgName);
	//车轮战npc
	void CreateWheelNpc(IKernel*pKernel, int sceneId, int groupId, int killNumNow = 0);




public:
	// 加载副本npc配置资源
	bool LoadNpcInfo(IKernel* pKernel);
	// 加载任务刷怪器配置
	bool LoadTaskCreatorRes(IKernel* pKernel);
	//重置容器
	void CleanCfg();
	//克隆场景NPC 
	void PraseCloneNpc(IKernel* pKernel, int sceneID, const fast_string& cfgPath);
	//分组场景NPC )
	void ParseGroupNpc(IKernel* pKernel, const std::string& sceneCfgName);
	// 解析时间刷新npc
	void ParseCreateNpcByTime(const std::string& npcCfgName, const std::vector<xml_document<>*>& tempDocVec);
	//车轮战npc
	void ParseWheelNpc(const std::string& npcCfgName, const std::vector<xml_document<>*>& tempDocVec);
	// 注册生成器到场景表中
	bool RegisterRandomCreator(IKernel* pKernel, const PERSISTID& scene, const char* id, const PERSISTID& creator);
	//随机创建npc解析
	void ParseSceneNpc(IKernel* pKernel, std::string npcCfgName, const std::vector<xml_document<>*>& tempDocVec);

	void ParseRandomNpcCreate(IKernel* pKernel, std::vector<CreateSpringNpcInfo>& springNpcGroup, xml_node<>*pCreator);
	void ParseStaticNpcCreate(IKernel* pKernel, std::vector<CreateStaticNpcInfo>& staticNpcGroup, xml_node<>*pCreator);
	
	int ParseRandomNpcItem(IKernel* pKernel, std::vector<RandomCreator>&  createSpringNpcInfoGroup, xml_node<>*pCreator);
	void ParseRandomNpcPosition(IKernel* pKernel, std::vector<NpcPosition> & positionGroup, xml_node<>*pCreator);

	//获得目录下文件夹列表
	void GetDirFileList(IKernel* pKernel, const fast_string& pathCfg, std::vector<xml_document<>*>& outFileList);


	//记录
	void RecordKillNpc(IKernel* pKernel, const PERSISTID& npc);
	//加载NPC组池配置
	bool LoadNpcPool(IKernel* pKernel);
	//获得NPC的configid
	int GetConfigID(IKernel* pKernel, const PERSISTID& pCreator, const char* npcPoolID, std::string& newConfigID);
	//获得场景内NPC数量信息（副本分组场景使用）
	int GetSceneNpcNum(IKernel* pKernel, const PERSISTID& scene);
	//组织入侵npc
	//bool LoadGuildInbreak(IKernel* pKernel);
	

private:
	struct NpcPool
	{
		NpcPool() :configID(""), isSame(0), weight(0), minWeight(0), maxWeight(0){};
		std::string configID;    //NPC的configID
		int isSame;              //是否可重复
		int weight;              //权值
		int minWeight;
		int maxWeight;
	};

	std::vector<int> v_vecSceneNpc;//保存已计算过NPC数量的分组场景号

	



	//时间刷新npc
	struct NpcInfo
	{
		NpcInfo(const std::string& id, float x, float y, float z, float ay, int mount) :
		_id(id), _x(x), _y(y), _z(z), _ay(ay), _mount(mount) {}
		std::string _id;
		float _x;
		float _y;
		float _z;
		float _ay;
		int _mount;
	};

	struct RefreshNpcRule {
		RefreshNpcRule() : _totalStep(0), _refreshTime(0), _waitTime(0) {};
		RefreshNpcRule(int totalStep, int refreshTime, int waitTime) :
			_totalStep(totalStep), _refreshTime(refreshTime), _waitTime(waitTime) {};
		int _totalStep;
		int _refreshTime;
		int _waitTime;
		std::vector<NpcInfo> _npcInfo;

	};

	struct WheelNpc
	{
		WheelNpc(const std::string& id,float x,float y,float z,float ay,int mount,int needKill):
		_id(id), _x(x), _y(y), _z(z), _ay(ay), _mount(mount), _needKillNum(needKill)
		{
			
		}
		std::string _id;
		float _x;
		float _y;
		float _z;
		float _ay;
		int _mount;
		int _needKillNum;//需要击杀npc数量
	};


public:
    //计算总权值
    int CalWeight(std::vector<NpcPool>& vec);
    //获得可出生的NPC
    void GetNpcFromPool(IKernel* pKernel, const PERSISTID& pCreator, std::vector<NpcPool> srcVec, std::vector<NpcPool>& tarVec);
    //获得可出生的BOSS
//     void GetBossFromPool(IKernel* pKernel, const char* npcPool, std::string& newConfig);
// 	//更新车轮npc
// 	void upDateWheelNpc(IKernel*pKernel, int sceneID, int groupID,int killNumNow);
	
	const char* GetSceneGroupNpcCfgName(int sceneID ,int groupID);



	/*----------------------------------XML操作接口----------------------------------------*/
private:
	void ClearXMLData();
	
	// 副本中创建对象的xml配置文件
	std::map<int, std::vector<xml_document<>* > > m_clone_npc_info;

	// 分组场景中创建的对象的xml配置文件
	std::map<std::string, std::vector<CreateSceneNpcInfo> > m_group_npc_info;

	std::vector<char*> m_vecbuff;
	//时间刷新npc
	std::map<std::string, std::vector<RefreshNpcRule>>   m_refresh_npc_by_time;
    //NPC组池配置
    std::map<std::string, std::vector<NpcPool>> m_mapNpcPool;
	//车轮战npc
	std::map<std::string, std::map<int, std::vector<WheelNpc> > > m_wheel_npc;
	//車輪NPC保存配置容器 < sceneid < <groupid> scenecfg> 当前组配置
	std::map<int,std::map<int, std::string> > m_sceneGroupNpcCfgName;
	// 组队悬赏刷怪器
	std::map<int, CreateSpringNpcInfo> m_TaskCreatorMap;
	//组织入侵
//	std::map<std::string, CreateSpringNpcInfo> m_guildInbreak;
public:
	// 模块指针
	static NpcCreatorModule* m_pNpcCreatorModule;
};

#endif // __NpcCreatorModule_h__