//--------------------------------------------------------------------
// 文件名:		SnsManager.h
// 内  容:		数据服务器逻辑模块:数据的管理
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __SnsManager_h__
#define __SnsManager_h__

#include "ISnsLogic.h"
#include "../../FsGame/Define/SnsDefine.h"
#include <unordered_map>

class SnsManager : public ISnsCallee 
{
public:
	SnsManager();

	virtual ~SnsManager();

	static ISnsCallee* Instance();

	virtual const char * GetSnsName();

	// \brief Sns服务器已启动，此时逻辑模块已加载完成
	// \param pKernel 核心指针，下同
	virtual int OnServerStarted(ISnsKernel* pSnsKernel);

	// \brief Sns逻辑对象被创建
	// \param pKernel 核心指针，下同
	// \param args 无意义
	virtual int OnSnsModuleCreate(ISnsKernel* pSnsKernel, const IVarList& args);

	// \brief Sns服务器所以的Sns数据已经加载完成准备就绪(在若次OnSnsLoad之后)
	// \param pKernel 核心指针，下同
	// \param args 无意义
	virtual int OnServerReady(ISnsKernel* pSnsKernel, const IVarList& args);

	// \brief sns数据在内存中创建
	// \param args
	// \uid  sns uid //sns数据关键字
	// \args  
	virtual int OnSnsDataCreate(ISnsKernel* pSnsKernel, const char* uid, const IVarList& args);

	// \brief 加载sns数据
	// \uid  sns uid //sns数据关键字
	// \args  
	virtual int OnSnsDataLoad(ISnsKernel* pSnsKernel, const char* uid, const IVarList& args);

	// \brief 收到消息
	// \param args
	// \distribute_id 分区id
	// \server_id  服务器编号
	// \memeber_id  场景服务器编号
	// \scene_id  发送场景的id
	// \uid  sns uid //sns数据关键字
	// \args  member发送的消息体
	virtual int OnMemberMessage(ISnsKernel* pSnsKernel, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);

	/// \brief sns服务器关闭通知
	virtual int OnServerClose(ISnsKernel* pSnsKernel);

	//查找模块
	virtual ISnsLogic * FindSnsLogic(const wchar_t * module_name);

private:
	// 处理通用消息
	bool ProcessCommonMessage(ISnsKernel* pSnsKernel, int distribute_id, int server_id,
		int memeber_id, int scene_id, const char* uid, const IVarList& args);

	/*
	表相关的通用操作
	*/
	//修改属性，格式：string snsspace, wstring snsdata, int msgid, string prop, var value, ...;
	bool SetPubDataProp(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);
	//增加属性，格式：string snsspace, wstring snsdata, int msgid, string prop, int value, ...;
	bool IncPubDataProp(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);

	//清除某个表的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, ...;
	bool ClearPubDataRec(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);
	//删除某个表的某行数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int row, ...;
	bool RemovePubDataRecRow(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);
	//添加某个表的一行内容，命令格式：string snsspace, wstring snsdata, int msgid, string rec, ...;
	bool AddPubDataRecRowValue(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);
	//设置某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int row, int col, var value, ...;
	bool SetPubDataRecValue(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);
	//增加某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int row, int col, var value, ...;
	bool IncPubDataRecValue(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);

	//根据关键值删除某个表的某行数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int keycol, var keyvalue;
	bool RemovePubDataRecRowByKey(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);
	//根据关健值设置公会的某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
	bool SetPubDataRecValueByKey(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);
	//根据关健增加公会的某个表的某一行某一列的数据，命令格式：string snsspace, wstring snsdata, int msgid, string rec, int keycol, var keyvalue, int col, var value, ...;
	bool IncPubDataRecValueByKey(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,
		int memeber_id,int scene_id, const char* uid,const IVarList& args);

private:
	//更新配置，命令格式：string snsspace, wstring snsdata, int msgid
	bool UpdateConfig(ISnsKernel* pSnsKernel, ISnsData* pSnsData, int distribute_id,int server_id,	 
		int memeber_id,int scene_id, const char* uid,const IVarList& args);

protected:
	typedef std::unordered_map<std::wstring, ISnsLogic *> HashMapTable;
	HashMapTable m_mapSnsLogicList;

private:
	static SnsManager* m_pInstance;
private:
	//sms 是否准备就绪
	bool is_sns_ready_;
};

#endif //__SnsManager_h__