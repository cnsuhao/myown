
#ifndef _SERVER_ISNSKERNEL_H
#define _SERVER_ISNSKERNEL_H
// ISnsKernel
// 核心功能接口
#include "KnlConst.h"
#include "../public/Macros.h"
#include "../public/IVarList.h"

// 公共数据逻辑接口版本号
#define SNS_LOGIC_MODULE_VERSION 0x3001

class ISnsKernel;
class IVarList;
struct PERSISTID;
class ISnsData;

typedef int (__cdecl * SNS_HEARTBEAT_FUNC)(ISnsKernel * pKernel, 
									   const char* sns_name, int time) ;
//
//typedef int (__cdecl * LUA_EXT_FUNC)(void * state) ;
//
//#define DECL_LUA_EXT(p)			pKernel->DeclareLuaExt(#p, p)
#define DECL_SNS_HEARTBEAT(p)		pKernel->DeclareHeartBeat(#p, p)

class ISnsKernel
{
	public:
	// 下面这个函数不要直接调用
	virtual bool DeclareHeartBeat(const char * func_name, SNS_HEARTBEAT_FUNC func) = 0;

public:
	virtual ~ISnsKernel() = 0;

	// 输出到系统LOG文件
	virtual void Trace(const char * info) = 0;

	// 输出到屏幕
	virtual void Echo(const char * info) = 0;

	// 获得资源文件目录
	virtual const char * GetResourcePath() = 0;

	//获取 sns cache
	//virtual int SetSnsCache(int nSize) = 0;
	virtual int GetSnsCache() = 0;
	//获取 数量
	virtual int GetSnsCount() = 0;

	//判断 sns 数据是否在内存中
	virtual bool FindSns(const char* uid) = 0;
	//创建sns 数据
	virtual bool CreateSns(const char* uid) = 0;
	// 带回传参数的Sns数据创建
	// 服务器不会对param做任何处理 ，只是简单的回传
	virtual bool CreateSns(const char* uid,const IVarList& param) = 0;
	//是否存在，数据库中查找
	virtual bool IsExistSns(const char* uid) = 0;
	//是否能加载该sns数据
	virtual bool CanLoadSns(const char* uid) = 0;
	//加载 sns 数据
	virtual bool LoadSns(const char* uid) = 0;
	virtual bool LoadSns(const char* uid,const IVarList& param,bool bSafeOnLoad = false) = 0;
	// 保存 sns 数据
	virtual bool SaveSns(const char* uid) = 0;
	//保存所有的sns 数据
	virtual bool SaveSnsAll() = 0;
	//删除Sns
	virtual bool RemoveSns(const char* uid) = 0;
	// 保存 snslog 数据
	virtual bool SaveSnsLog(const char* uid,int type, 
		const wchar_t * content, const wchar_t * comment) = 0;

	virtual ISnsData * GetSnsData(const char* uid)=0;

	/*
	//增加属性
	virtual bool AddAttr(const char* uid, const char * attr_name, int type, bool saving = false) = 0;
	//移除属性
	virtual bool RemoveAttr(const char* uid, const char * attr_name) = 0;	
	//属性是否存在
	virtual bool IsAttrExists(const char* uid, const char * attr_name) = 0;
	
	//查询属性
	virtual int QueryAttrInt(const char* uid, const char * attr_name) = 0;
	virtual int64_t QueryAttrInt64(const char* uid, const char* name) = 0;
	virtual float QueryAttrFloat(const char* uid, const char * attr_name) = 0;
	virtual double QueryAttrDouble(const char* uid, const char* name) = 0;
	virtual const char* QueryAttrString(const char* uid, const char * attr_name) = 0;
	virtual const wchar_t* QueryAttrWideStr(const char* uid, const char * attr_name) = 0;
	virtual PERSISTID QueryAttrObject(const char* uid, const char * attr_name) = 0;

	//设置属性
	virtual bool SetAttrInt(const char* uid, const char * attr_name, int value) = 0;
	virtual bool SetAttrInt64(const char* uid, const char * attr_name, int64_t value) = 0;
	virtual bool SetAttrFloat(const char* uid, const char * attr_name, float value) = 0;
	virtual bool SetAttrDouble(const char* uid, const char * attr_name, double value) = 0;
	virtual bool SetAttrString(const char* uid, const char * attr_name, const char * value) = 0;
	virtual bool SetAttrWideStr(const char* uid, const char * attr_name, const wchar_t* value) = 0;
	virtual bool SetAttrObject(const char* uid, const char * attr_name, const PERSISTID & value) = 0;


	// 判断指定表格是否存在于内存中
	virtual bool IsExistRecord(const char* uid, const char * rec_name) = 0;
	// 添加一个表，表名称，列数，最大行数,列属性，是否保存
	virtual bool AddRecord(const char* uid, const char * rec_name, 
		int cols, int maxRows,const IVarList& colType ,bool saving = false) = 0;
	// 取表列数
	virtual int GetRecordCols(const char* uid, const char * rec_name) = 0;
	// 取表行数
	virtual int GetRecordRows(const char* uid, const char * rec_name) = 0;
	// 取表最大行数
	virtual int GetRecordMax(const char* uid, const char * rec_name) = 0;
	// 获得列数据类型
	virtual int GetRecordColType(const char* uid, const char * rec_name, int col) = 0;
	// 添加一行，表名，行号（行号为-1表示添加新行），返回插入的行号
	virtual int AddRecordRow(const char* uid, const char * rec_name, int row) = 0;
	// 添加一行并初始化，表名，行号（行号为-1表示添加新行），返回插入的行号
	virtual int AddRecordRowValue(const char* uid, const char * rec_name,
		int row, const IVarList & value) = 0;
	// 删除一行，表名，行号
	virtual bool RemoveRecordRow(const char* uid, const char * rec_name, int row) = 0;
	// 清除所有表数据
	virtual bool ClearRecord(const char* uid, const char * rec_name) = 0;

	// 写表数据
	virtual bool SetRecordRowValue(const char* uid, const char * rec_name,
		int row, const IVarList & value) = 0;
	virtual bool SetRecordInt(const char* uid, const char * rec_name, 
		int row, int col, int value) = 0;
	virtual bool SetRecordInt64(const char* uid, const char * rec_name, 
		int row, int col, int64_t value) = 0;
	virtual bool SetRecordFloat(const char* uid, const char * rec_name, 
		int row, int col, float value) = 0;
	virtual bool SetRecordDouble(const char* uid, const char * rec_name, 
		int row, int col, double value) = 0;
	virtual bool SetRecordString(const char* uid, const char * rec_name, 
		int row, int col, const char * value) = 0;
	virtual bool SetRecordWideStr(const char* uid, const char * rec_name, 
		int row, int col, const wchar_t * value) = 0;
	virtual bool SetRecordObject(const char* uid, const char * rec_name, 
		int row, int col, const PERSISTID & value) = 0;

	
	// 读表数据
	virtual bool QueryRecordRowValue(const char* uid, const char * rec_name, 
		int row, IVarList & value) = 0;
	virtual int QueryRecordInt(const char* uid, const char * rec_name, 
		int row, int col) = 0;
	virtual int64_t QueryRecordInt64(const char* uid, const char * rec_name, 
		int row, int col) = 0;
	virtual float QueryRecordFloat(const char* uid, const char * rec_name, 
		int row, int col) = 0;
	virtual double QueryRecordDouble(const char* uid, const char * rec_name, 
		int row, int col) = 0;
	virtual const char * QueryRecordString(const char* uid, const char * rec_name, 
		int row, int col) = 0;
	virtual const wchar_t * QueryRecordWideStr(const char* uid, const char * rec_name, 
		int row, int col) = 0;
	virtual PERSISTID QueryRecordObject(const char* uid, const char * rec_name, 
		int row, int col) = 0;

	// 查找指定列相符的某行，返回-1表示没有
	virtual int FindRecordInt(const char* uid, const char * rec_name, 
		int col, int value) = 0;
	virtual int FindRecordInt64(const char* uid, const char * rec_name, 
		int col, int64_t value) = 0;
	virtual int FindRecordFloat(const char* uid, const char * rec_name, 
		int col, float value) = 0;
	virtual int FindRecordDouble(const char* uid, const char * rec_name, 
		int col, double value) = 0;
	virtual int FindRecordString(const char* uid, const char * rec_name, 
		int col, const char * value) = 0;
	virtual int FindRecordWideStr(const char* uid, const char * rec_name, 
		int col, const wchar_t * value) = 0;
	virtual int FindRecordObject(const char* uid, const char * rec_name, 
		int col, const PERSISTID & value) = 0;

	*/

	////发送 sns 消息到 name对应的玩家 场景服务器响应OnCommand
	virtual bool SendToScenePlayer(int server_id,int memeber_id,int scene_id,const char* uid,const IVarList& msg) = 0;
	//发送 sns 消息到member服务器 场景服务器响应OnSnsMessage
	virtual bool SendToSceneMessage(int server_id,int memeber_id,int scene_id,const IVarList & msg) = 0;



	/*
	

	陈伟伟(陈伟伟) 11:34:06
	{"appPackage":"com.example.servicetest","badge":0,"client":"android","expandMessage":"http://www.baidu.com","message":"发送内容","sound":"default","title":"标题","token":["12-34-53-62-01","12-42-56-12"]}
	---------------------------------------------
	这个是格式键是固定的，值自己定义


	#include "../../public/Converts.h"
	#include "../../public/Inlines.h"

	//消息推送
	wchar_t buffer[1024];
	SafeSwprintf(buffer,sizeof(buffer),
		L"{appPackage\":\"%s\",\"badge\":%d,\"client\":\"%s\",\"expandMessage\":\"%s\",\"message\":\"%s\",\"sound\":\"%s\",\"title\":\"%s\",\"token\":[\"%s\",\"%s\"]}",
		L"com.example.servicetest",0,L"android",L"http://www.baidu.com",L"发送内容",L"default",L"标题",L"12-34-53-62-01",L"12-42-56-12");

	//大小不小于宽串长度的3倍
	char utf8_buffer[3072];
	Port_WideStrToUTF8(buffer, utf8_buffer, sizeof(utf8_buffer));


	pKernel->MessagePush(utf8_buffer);
	
	*/
	//发送消息到推送服务器（字符串要用utf8 编码,参考上面代码）
	virtual bool MessagePush(const char *msg) = 0;

	/*
	心跳函数的声明在OnCreate 中做
	有可能心跳触发时，sns 已从内存中移除，需要判断 FindSns，CanLoadSns
	*/
	//添加心跳
	virtual bool AddHeartBeat(const char* name,const char * func, int time) = 0;
	// 添加计数心跳
	virtual bool AddCountBeat(const char* name,const char * func, int time, int count) = 0;
	// 删除心跳函数
	virtual bool RemoveHeartBeat(const char* name,const char * func) = 0;
	// 查找心跳函数
	virtual bool FindHeartBeat(const char* name,const char * func) = 0;

};

inline ISnsKernel::~ISnsKernel() {}

#endif