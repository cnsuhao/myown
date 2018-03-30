/*@FileName:	FsSingleton.h
* @Author:		Àîº£ÂÞ
* @Function:	µ¥Àý
* @Modify:		2012-11-11
*/
#ifndef __FAST_FRAMEWORK_SINGLETON_INCLUDE__
#define __FAST_FRAMEWORK_SINGLETON_INCLUDE__

template<typename T>
class FsSingleton
{
	FsSingleton& operator=(FsSingleton& src) = delete;
	FsSingleton& operator=(FsSingleton&& src) = delete;

	FsSingleton(FsSingleton& src) = delete;
	FsSingleton(FsSingleton&& src) = delete;
protected:
	FsSingleton() { }
public:
	virtual ~FsSingleton(){}

	static T* Instance()
	{
		static T s_Inst;
		return &s_Inst;
	}
};

#endif	// END __FAST_FRAMEWORK_SINGLETON_INCLUDE__