//----------------------------------------------------------
// 文件名:      OffLineDelay.h
// 内  容:      离线拉人
// 说  明:
// 创建日期:    2014年11月21日
// 创建人:       
// 修改人:    
//    :       
//----------------------------------------------------------
#ifndef __OFFLINE_DELAY_H_
#define __OFFLINE_DELAY_H_

#include "../../server/ISnsCallee.h"
#include "../../server/ISnsKernel.h"
#include "../ISnsLogic.h"
#include <vector>
#include "utils/cache/CacheString.hpp"

class OffLineDelay : public ISnsLogic 
{
public:
	OffLineDelay();

	int OnModuleCreate(ISnsKernel* pKernel, const IVarList& args);
	int OnReady(ISnsKernel* pKernel, const IVarList& args);

	// \brief sns数据在内存中创建
	// \param args
	// \uid  sns uid //sns数据关键字
	// \args  
	int OnCreate(ISnsKernel* pSnsKernel, const char* uid, const IVarList& args);

	int OnLoad(ISnsKernel* pKernel, const char* uid, const IVarList& args);
	
	//收到来自member的消息
	int OnMessage(ISnsKernel* pKernel, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);

public:
	static int Offline_delay_heart(ISnsKernel * pKernel, 
		const char* uid, int time);
private:
	// 数据信息
	struct MirDataInfo
	{
		MirDataInfo() : strName(""), nType(0), strSnsTable(""){}
		std::string		strName;			// 名字
		int				nType;				// 数据类型对应(MirDataType)
		std::string		strSnsTable;		// 容器对应的sns表
	};

	typedef std::vector<MirDataInfo> MirDataInfoVec;

	int OnPushPlayerOffline(ISnsKernel* pKernel, const char* uid, const IVarList& args);

	// 读取数据信息配置
	bool LoadDataInfoConfig(ISnsKernel* pKernel);

	// 生成查询数据
	void GenerateQueryData(ISnsData* pSnsData, IVarList& outData);

	// 获取某个属性数据
	void FillPropertyData(ISnsData* pSnsData, const char* strProName, IVarList& outData);

	// 获取容器或表数据
	void FillContainerOrRecData(ISnsData* pSnsData, const char* strProName, const char* strRec, IVarList& outData);

	// 解析查询玩家属性
	void LoadPlayerPropertys(ISnsKernel* pKernel, const char* attr_names, const IVarList& req_msg, IVarList& dst_msg);
	void LoadPlayerPropertys_Detal(ISnsData * pSnsData, const IVarList& attr_list_name, const IVarList& attr_list_type, int64_t ident_id, IVarList& dst_msg);
private:
	MirDataInfoVec			m_vDataInfo;		// 数据信息
	CacheString<512> temp_string_;
};

#endif