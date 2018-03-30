//--------------------------------------------------------------------
// 文件名:		Domain_Task.h
// 内  容:		任务信息域
// 说  明:		
// 创建日期:	2016年07月01日
// 创建人:		  tongzt
// 修改人:        
//--------------------------------------------------------------------

#ifndef __Domain_Task_h__
#define __Domain_Task_h__

#include "IDomainData.h"
//#include <vector>
//#include <string>

class Domain_Task : public IDomainData
{
public:
	// 构造
	Domain_Task();

public:
	// 取域名
	virtual const std::wstring & GetDomainName(IPubKernel *pPubKernel);

	// 保存
	virtual bool GetSave(){return true;}

	// 创建
	virtual int OnCreate(IPubKernel *pPubKernel, IPubSpace *pPubSpace); 

	// 加载
	virtual int OnLoad(IPubKernel *pPubKernel, IPubSpace *pPubSpace);

	///  \brief 接收到场景服务器的消息
	///  \param source_id 发消息的服务器标识
	///  \param msg 消息内容
	virtual int OnMessage(IPubKernel *pPubKernel, IPubSpace *pPubSpace, 
		int source_id, int scene_id, const IVarList & msg);

private:
	// 建表
	void CreateRecord(IPubKernel *pPubKernel, IPubData *pPubData, 
		const char *rec_name, IVarList& cols_type, const int cols, 
		const int max_row, IVarList& new_col_val, const int rec_key, bool is_save = true);

	// 任务定时器
	static int ResetTaskTimer(IPubKernel *pPubKernel, const char *space_name, 
		const wchar_t *data_name, int time);

public:
	static Domain_Task * m_pDomain_Task;

};

#endif // __Domain_Task_h__
