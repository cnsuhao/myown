//-------------------------------------------------------------------
// 文 件 名：igame_public.h
// 内    容：
// 说    明：
// 创建日期：2014年2月18日
// 创 建 人：Liangxk
// 版权所有：苏州蜗牛电子有限公司
//-------------------------------------------------------------------


#if !defined __IGAME_PUBLIC_H__
#define __IGAME_PUBLIC_H__

class igame_public
{
public:
	// 获得属性数量
	virtual int GetPropertyNum() const = 0;
	virtual const char* GetPropertyName(int index) const = 0;
	virtual int GetPropertyType(int index) const = 0;
	virtual int GetPropertyInt(int index) const = 0;
	virtual int64_t GetPropertyInt64(int index) const = 0;
	virtual float GetPropertyFloat(int index) const = 0;
	virtual double GetPropertyDouble(int index) const = 0;
	virtual const char* GetPropertyString(int index) const = 0;
	virtual const wchar_t* GetPropertyWideStr(int index) const = 0;
	virtual PERSISTID GetPropertyObject(int index) const = 0;
	virtual int FindPropertyIndex(const char* name) const = 0;

	// 获得表格数量
	virtual int GetRecordNum() const = 0;
	virtual const char* GetRecordName(int index) const = 0;
	virtual int GetRecordRowMax(int index) const = 0;
	virtual int GetRecrodRows(int index) const = 0;
	virtual int GetRecordCols(int index) const = 0;
	virtual int GetRecordColType(int index, int col) const = 0;
	virtual int GetRecordInt(int index, int row, int col) const = 0;
	virtual int64_t GetRecordInt64(int index, int row, int col) const = 0;
	virtual float GetRecordFloat(int index, int row, int col) const = 0;
	virtual double GetRecordDouble(int index, int row, int col) const = 0;
	virtual const char* GetRecordString(int index, int row, int col) const = 0;
	virtual const wchar_t* GetRecordWideStr(int index, int row, int col) const = 0;
	virtual PERSISTID GetRecordObject(int index, int row, int col) const = 0;
	virtual int FindRecordIndex(const char* name) const = 0;
};

#endif // __IGAME_PUBLIC_H__
