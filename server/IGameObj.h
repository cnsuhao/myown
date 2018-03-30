//--------------------------------------------------------------------
// 文件名:		IGameObj.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年2月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SERVER_IGAMEOBJ_H
#define _SERVER_IGAMEOBJ_H

#include "../public/Macros.h"
#include "../public/PersistId.h"
#include "LenType.h"
#include "IRecord.h"

// 游戏对象接口

class IGameObj
{
public:
	virtual ~IGameObj() = 0;

	/// \brief 获得对象类型
	/// \return 对象类型（见KnlConst.h）
	virtual int GetClassType() = 0;
	/// \brief 获得对象号
	virtual PERSISTID GetObjectId() = 0;
	/// \brief 取对象脚本名
	virtual const char* GetScript() = 0;
	/// \brief 取对象配置名
	virtual const char* GetConfig() = 0;
	/// \brief 获得名字
	virtual const wchar_t* GetName() = 0;
	/// \brief 获得组号
	virtual int GetGroupId() = 0;
	/// \brief 取对象在容器中的位置
	/// \return 从1开始
	virtual int GetIndex() = 0;
	/// \brief 取父对象
	virtual IGameObj* GetParent() = 0;
	/// \brief 返回容器的容量
	virtual int GetCapacity() = 0;
	/// \brief 返回子对象数量
	virtual int GetChildCount() = 0;
	/// \brief 取容器指定位置的子对象
	/// \param index 容器中的位置（从1开始）
	virtual IGameObj* GetChildByIndex(int index) = 0;
	/// \brief 用名字查找子对象
	/// \param name 子对象的名字
	virtual IGameObj* GetChild(const wchar_t* name) = 0;
	/// \brief 获得被弱关联的次数
	virtual int GetWeakRefs() = 0;
	
	/// \brief 对象坐标X
	virtual len_t GetPosiX() = 0;
	/// \brief 对象坐标Y
	virtual len_t GetPosiY() = 0;
	/// \brief 对象坐标Z
	virtual len_t GetPosiZ() = 0;
	/// \brief 对象方向
	virtual len_t GetOrient() = 0;
	
	/// \brief 测试是否有一个属性
	/// \param name 属性名
	virtual bool FindAttr(const char* name) = 0;
	/// \brief 获得属性类型
	/// \param name 属性名
	virtual int GetAttrType(const char* name) = 0;
	/// \brief 获得属性数量
	virtual int GetAttrCount() = 0;
	/// \brief 获得属性名称列表
	/// \param result 结果属性名列表
	virtual int GetAttrList(IVarList& result) = 0;
	/// \brief 设置属性值
	/// \param name 属性名
	/// \param value 属性值
	virtual bool SetInt(const char* name, int value) = 0;
	virtual bool SetInt64(const char* name, int64_t value) = 0;
	virtual bool SetFloat(const char* name, float value) = 0;
	virtual bool SetDouble(const char* name, double value) = 0;
	virtual bool SetString(const char* name, const char* value) = 0;
	virtual bool SetWideStr(const char* name, const wchar_t* value) = 0;
	virtual bool SetObject(const char* name, const PERSISTID& value) = 0;
	/// \brief 查询属性
	/// \param name 属性名
	virtual int QueryInt(const char* name) = 0;
	virtual int64_t QueryInt64(const char* name) = 0;
	virtual float QueryFloat(const char* name) = 0;
	virtual double QueryDouble(const char* name) = 0;
	virtual const char* QueryString(const char* name) = 0;
	virtual const wchar_t* QueryWideStr(const char* name) = 0;
	virtual PERSISTID QueryObject(const char* name) = 0;

	/// \brief 获得属性的索引值（返回-1表示未找到）
	/// \param name 属性名
	virtual int GetAttrIndex(const char* name) = 0;
	/// \brief 通过索引值设置属性值
	/// \param index 属性索引
	/// \param value 属性值
	virtual bool SetIntByIndex(int index, int value) = 0;
	virtual bool SetInt64ByIndex(int index, int64_t value) = 0;
	virtual bool SetFloatByIndex(int index, float value) = 0;
	virtual bool SetDoubleByIndex(int index, double value) = 0;
	virtual bool SetStringByIndex(int index, const char* value) = 0;
	virtual bool SetWideStrByIndex(int index, const wchar_t* value) = 0;
	virtual bool SetObjectByIndex(int index, const PERSISTID& value) = 0;
	/// \brief 通过索引值查询属性
	/// \param index 属性索引
	virtual int QueryIntByIndex(int index) = 0;
	virtual int64_t QueryInt64ByIndex(int index) = 0;
	virtual float QueryFloatByIndex(int index) = 0;
	virtual double QueryDoubleByIndex(int index) = 0;
	virtual const char* QueryStringByIndex(int index) = 0;
	virtual const wchar_t* QueryWideStrByIndex(int index) = 0;
	virtual PERSISTID QueryObjectByIndex(int index) = 0;

	/// \brief 获得属性是否可视
	/// \param name 属性名
	virtual bool GetAttrVisible(const char* name) = 0;
	/// \brief 获得属性是否公共可视
	/// \param name 属性名
	virtual bool GetAttrPublicVisible(const char* name) = 0;
	/// \brief 获得可视属性是否即时刷新
	/// \param obj 对象
	/// \param name 属性名
	virtual bool GetAttrRealtime(const char* name) = 0;
	/// \brief 获得属性是否保存
	/// \param obj 对象
	/// \param name 属性名
	virtual bool GetAttrSaving(const char* name) = 0;
	/// \brief 设置可视属性是否隐藏
	/// \param name 属性名
	/// \param value 是否隐藏
	virtual bool SetAttrHide(const char* name, bool value) = 0;
	/// \brief 获得可视属性是否隐藏
	/// \param name 属性名
	virtual bool GetAttrHide(const char* name) = 0;


	// 用属性实现位标志操作（目的是节约存储，属性类型必须为整数）
	/// \brief 设置标志
	/// \param name 属性名
	/// \param pos 标志位（0-31）
	virtual bool SetAttrFlag(const char* name, int pos) = 0;
	/// \brief 清除标志
	/// \param name 属性名
	/// \param pos 标志位（0-31）
	virtual bool ClearAttrFlag(const char* name, int pos) = 0;
	/// \brief 测试标志
	/// \param obj 对象
	/// \param name 属性名
	/// \param pos 标志位（0-31）
	virtual bool TestAttrFlag(const char* name, int pos) = 0;

	

	/// \brief 获得表数量
	virtual int GetRecordCount() = 0;
	/// \brief 获得表名称列表
	/// \param result 结果表格名列表
	virtual int GetRecordList(IVarList& result) = 0;
	/// \brief 获得指定索引的表格访问接口
	/// \param index 表格索引
	virtual IRecord* GetRecordByIndex(int index) = 0;
	/// \brief 获得对象表格访问接口
	/// \param name 表格名
	virtual IRecord* GetRecord(const char* name) = 0;
	/// \brief 获得表格的索引值（返回-1表示未找到）
	/// \param name 表格名
	virtual int GetRecordIndex(const char* name) = 0;
	
	/// \brief 查找临时数据
	/// \param name 数据名
	virtual bool FindData(const char* name) = 0;
	/// \brief 获得临时数据数量
	virtual int GetDataCount() = 0;
	/// \brief 获得临时数据名称列表
	/// \param result 结构数据名列表
	virtual int GetDataList(IVarList& result) = 0;
	/// \brief 增加临时数据
	/// \param name 数据名
	/// \param value 数值
	virtual bool AddDataInt(const char* name, int value) = 0;
	virtual bool AddDataInt64(const char* name, int64_t value) = 0;
	virtual bool AddDataFloat(const char* name, float value) = 0;
	virtual bool AddDataDouble(const char* name, double value) = 0;
	virtual bool AddDataString(const char* name, const char* value) = 0;
	virtual bool AddDataWideStr(const char* name, const wchar_t* value) = 0;
	virtual bool AddDataObject(const char* name, const PERSISTID& value) = 0;
	virtual bool AddDataBinary(const char* name, const void* pdata, int len) = 0;
	/// \brief 删除临时数据
	/// \param name 数据名
	virtual bool RemoveData(const char* name) = 0;
	/// \brief 获得临时数据类型
	/// \param name 数据名
	virtual int GetDataType(const char* name) = 0;
	/// \brief 设置临时数据
	/// \param name 数据名
	/// \param value 数值
	virtual bool SetDataInt(const char* name, int value) = 0;
	virtual bool SetDataInt64(const char* name, int64_t value) = 0;
	virtual bool SetDataFloat(const char* name, float value) = 0;
	virtual bool SetDataDouble(const char* name, double value) = 0;
	virtual bool SetDataString(const char* name, const char* value) = 0;
	virtual bool SetDataWideStr(const char* name, const wchar_t* value) = 0;
	virtual bool SetDataObject(const char* name, const PERSISTID& value) = 0;
	virtual bool SetDataBinary(const char* name, const void* pdata, int len) = 0;
	/// \brief 查询临时数据
	/// \param name 数据名
	virtual int QueryDataInt(const char* name) = 0;
	virtual int64_t QueryDataInt64(const char* name) = 0;
	virtual float QueryDataFloat(const char* name) = 0;
	virtual double QueryDataDouble(const char* name) = 0;
	virtual const char* QueryDataString(const char* name) = 0;
	virtual const wchar_t* QueryDataWideStr(const char* name) = 0;
	virtual PERSISTID QueryDataObject(const char* name) = 0;
	virtual void* QueryDataBinary(const char* name, int* len = NULL) = 0;
	
	/// \brief 获得表是否可视
	/// \param obj 对象
	/// \param name 表格名
	virtual bool GetRecordVisible(const char* name) = 0;
	/// \brief 获得表是否公共可视
	/// \param obj 对象
	/// \param name 表格名
	virtual bool GetRecordPublicVisible(const char* name) = 0;
	/// \brief 获得表是否保存
	/// \param obj 对象
	/// \param name 表格名
	virtual bool GetRecordSaving(const char* name) = 0;

	// 用表实现位标志操作（目的是节约存储，表必须为单列，列类型为整数）
	/// \brief 设置标志
	/// \param obj 对象
	/// \param name 表格名
	/// \param pos 标志位序号（行号乘32加0-31）
	virtual bool SetRecordFlag(const char* name, int pos) = 0;
	/// \brief 清除标志
	/// \param obj 对象
	/// \param name 表格名
	/// \param pos 标志位序号（行号乘32加0-31）
	virtual bool ClearRecordFlag(const char* name, int pos) = 0;
	/// \brief 测试标志
	/// \param obj 对象
	/// \param name 表格名
	/// \param pos 标志位序号（行号乘32加0-31）
	virtual bool TestRecordFlag(const char* name, int pos) = 0;



	/// \brief 玩家的视窗是否存在
	/// \param id 容器编号
	virtual bool FindViewport(int id) = 0;

	/// \brief 获得当前移动模式
	virtual int GetMoveMode() = 0;
};

inline IGameObj::~IGameObj() {}

#endif // _SERVER_IGAMEOBJ_H
