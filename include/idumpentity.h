//--------------------------------------------------------------------
// 文件名:		IEntity.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月31日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_IDUMPENTITY_H
#define _PUBLIC_IDUMPENTITY_H

#include "../../public/IEntity.h"
#include "../../public/Macros.h"
#include "../../public/PersistId.h"
#include "../../public/ICore.h"
#include "../../public/IVarTable.h"


enum module_status_type
{
	MST_UNKNOWN,	/// 未知状态
	MST_STARTED,	/// 已启动
	MST_RUNNING,	/// 正在处理中
	MST_STOPED,		/// 已停止

	MST_MAX,
};

// 实体接口

class IDumpManager;
//class IVar;

class IDumpEntity : public IEntity
{
public:
	IDumpEntity()
	{
		m_nModuleStatus = MST_UNKNOWN;
	}

	virtual ~IDumpEntity();

	//// 初始化（可创建其他附属实体）
	//virtual bool Init(const IVarList& args) = 0;
	//// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
	//virtual bool Shut() = 0;
	//
	//// 每帧执行逻辑（如果已添加到运行队列）
	//virtual void Execute(float seconds) {}	

	//// 获得内存占用
	//virtual size_t GetMemoryUsage() { return 0; }

	/// 接收命令消息函数
	virtual void OnManagerCommand(const IVarList& args, IVarList& ret) {};
	/// 用户自定义循环函数
	virtual void OnUserLoop() {}
	virtual void OnEvent() {}
	/// 接收网络连接
	virtual void OnConnectAccept(const char* name, int index, int serial, const char* addr, int port) {}
	/// 收到网络消息
	virtual void OnConnectReceive(const char* name, int index, int serial, const IVarList& msg) {}
	/// 断开网络连接
	virtual void OnConnectClose(const char* name, int index, int serial, const char* addr, int port) {}

	

	/// 模块状态
	int GetModuleStatus() { return m_nModuleStatus; }
	void SetModuleStatus(int status) { m_nModuleStatus = status; }

	bool IsStatusReady() { return m_nModuleStatus == MST_STARTED; }
	bool IsStatusProcessing() { return m_nModuleStatus == MST_RUNNING; }
	bool IsStatusStoped() { return m_nModuleStatus == MST_STOPED; }

	IDumpManager* GetDumpMgr() {
		return m_pDumpManager;
	}
	void SetDumpMgr(IDumpManager* pMgr) { m_pDumpManager = pMgr; }

private:
	IDumpEntity(const IDumpEntity&);
	IDumpEntity& operator=(const IDumpEntity&);

protected:
	IDumpManager* m_pDumpManager;
	int m_nModuleStatus;	
};

inline IDumpEntity::~IDumpEntity() {}

#endif // _PUBLIC_IDUMPENTITY_H

