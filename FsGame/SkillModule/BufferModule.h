//--------------------------------------------------------------------
// 文件名:      BufferModule.h
// 内  容:      Buffer系统
// 说  明:
// 创建日期:    2014年11月4日
// 创建人:		 
//    :       
//--------------------------------------------------------------------

#ifndef _BufferModule_H_
#define _BufferModule_H_

#include "Fsgame/Define/header.h"
#include "FsGame/Define/BufferDefine.h"

struct BuffBaseData;

class BufferModule : public ILogicModule
{
public:
    /**
    @brief  初始化
    @param  pKernel [IN] 引擎核心指针
    @return 返回初始化结果
    */
    virtual bool Init(IKernel* pKernel);

    /**
    @brief  关闭
    @param  pKernel [IN] 引擎核心指针
    @return 返回关闭结果
    */
    virtual bool Shut(IKernel* pKernel);

public:
    // --------------------------公共对外接口-----------------------------------
    /**
    @brief  查找某玩家指定ID的所有BUFFER
    @param  pKernel [IN] 引擎核心指针
    @param  self    [IN] 被检查的玩家
    @param  bufferid [IN] 指定的ID
    @param  result [OUT] 找到的BUFF
    @return 查找成功返回true，否则返回false
    */
    bool FindBuffer(IKernel* pKernel,
                    const PERSISTID& self,
                    const char* szBufferID,
                    IVarList& result);


    /**
    @brief  获得BUFFER容器
    @param  pKernel [IN] 引擎核心指针
    @param  self [IN] 待查找的对象
    @return 返回对象的buffer容器
    */
    PERSISTID GetBufferContainer(IKernel* pKernel, const PERSISTID& self);

    /**
    *@brief  添加buff
    *@param  [IN] pKernel 引擎内核指针
    *@param  [IN] self buff接受者
    *@param  [IN] sender buff释放者
    *@param  [IN] szBufferConfigID buff的ConfigID
    *@return bool 添加成功返回true，否则返回false
    */
    bool AddBuffer(IKernel* pKernel,
                   const PERSISTID& self,
                   const PERSISTID& sender,
                   const char* szBufferConfigID);

    /**
    *@brief  清除指定对象的某个BUFFER
    *@param  [IN] pKernel 引擎核心指针
    *@param  [IN] self 指定对象
    *@param  [IN] szBufferID buff的ConfigID
    *@return bool 清除成功返回true，否则返回false
    */
    bool RemoveBuffer(IKernel* pKernel,
                      const PERSISTID& self,
                      const char* szBufferID);

    // 添加BUFF同时修改时间
    PERSISTID AddBuffWithLifeTime(IKernel* pKernel,
                                  const PERSISTID& self,
                                  const PERSISTID& sender,
                                  const char* szBufferID,
                                  const int iLifeTime);


    // 清除角色身上的所有BUFFER
    void ClearAllBuffer(IKernel* pKernel, const PERSISTID& self);

	PERSISTID GetBufferOwner(IKernel* pKernel, const PERSISTID& buffer);

	// buff触发事件CD处理(只有增益buff才有CD)
	//bool DealCoolDown(IKernel* pKernel, const PERSISTID& self, const BuffBaseData* pBuffBaseData);

	// 按照类型清除buff nClearType(对应BufferClear)
	int RemoveBuffByClearType(IKernel* pKernel, const PERSISTID &self, int nClearType);
protected:
    static int OnBufferContainerCreate(IKernel* pKernel, const PERSISTID& container,
                                       const PERSISTID& buffer, const IVarList& args);

	static int OnBufferOnRecover(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
	// Buffer加载完成
	static int OnBufferLoad(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    static int OnLoad(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
    static int OnStore(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
    static int OnReady(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);
    static int OnLeaveScene(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    // 被杀
    static int OnCommandBeKill(IKernel* pKernel, const PERSISTID& self,
                               const PERSISTID& sender, const IVarList& args);

	//事件响应
	//说明：
	//1:BUFFER触发时可能会改变对象的属性或表格
	//  当改变对象是事件对象时，这些事件只能是类似BeAttack、BeDamage、BeKill等事件，事件对象是这些事件的sender
	//2:只能响应战斗技能相关的Command
	static int OnCommandEffectBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& sender, const IVarList& args);

    //buffer激活后开始倒计时心跳
    static int H_BufferUpdate(IKernel* pKernel, const PERSISTID& buffer, int slice);
private:
    // ------------------------模块内部功能处理函数-----------------------------

    // 添加某个BUFFER
    bool InterAddBuffer(IKernel* pKernel, const PERSISTID& self,
                        const PERSISTID& sender, const PERSISTID& buffer);

    // 初始化buffer属性数据
    bool InitBufferData(IKernel* pKernel, const PERSISTID& self, 
                        const PERSISTID& sender, const PERSISTID& buffer);

    // 清除某个BUFFER
    bool InterRemoveBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer,
                           int iRemoveReason = BUFFER_REMOVE_NONE);

    //瞬发状态的效果
    int InstantActiveBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer);

    //启动状态的效果
    bool ActiveBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer);

    //关闭状态的效果
    bool DeactiveBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& buffer);

    //计时或计次时的效果
    bool TimerBuffer(IKernel* pKernel, const PERSISTID& self, const PERSISTID& eventsender, const PERSISTID& buffer);
 
    // 替换相关逻辑
    bool DoBufferReplace(IKernel* pKernel, const PERSISTID& self, const PERSISTID& container, const PERSISTID& buffer);

    //获得BUFFER的剩余时间，单位为ms
    int GetBufferLeaveTime(int timeType, int lifeTime, int exitTime, time_t addTime);

    //处理事件
    void BufferEventEffect(IKernel* pKernel, const EBufferEventType type, const PERSISTID& self,
                           const PERSISTID& sender, const PERSISTID& buffer, const IVarList& params = CVarList());
	// 生成显示buff的可视属性BuffDisplayID
	bool ChangeBuffDisplay(IKernel* pKernel, const PERSISTID& self, const std::string& szBufferConfigID);

	// buff的直接替换处理
	bool DoBuffDirectReplace(IKernel* pKernel, const PERSISTID& curbuff, const PERSISTID& oldbuff);

	// 是否需要清除buff
	bool IsNeedClearBuff(IKernel* pKernel, const char* strBufferConfigID, int nClearType, int& outReason);
private:
	bool PreloadBuffConfig(IKernel* pKernel);

public:
    static BufferModule* m_pInstance;
};

#endif
