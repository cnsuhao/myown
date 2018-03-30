//----------------------------------------------------------
// 文件名:      HPSingleton.h
// 内  容:      单一实例模板类,非线程安全
// 说  明:
// 创建人:      hehui
// 创建日期:    2012年12月24日
//    :       
//----------------------------------------------------------
#ifndef FSGAME_INTERFACE_SINGLETON_H_
#define FSGAME_INTERFACE_SINGLETON_H_

// for fs singleton
template<class T>
class HPSingleton
{
public:
    static T* Instance()
    {
        static T instace;
        return &instace;
    };

private:
    HPSingleton(void) {};
    ~HPSingleton(void) {};
    HPSingleton(const HPSingleton<T>&) {};
    HPSingleton& operator=(const HPSingleton<T>&) {};
};

// for base singleton
template<class T>
class ISingleton
{
public:
	static T* Instance()
	{
		static T instace;
		return &instace;
	};

public:
	ISingleton(void) {};
	~ISingleton(void) {};
};

// for bind singleton
template<class T>
class IBindSingleton
{
public:
	static T* Instance(T* p_dc=NULL)
	{
		static T* instace;
		if (p_dc != NULL)
		{
			instace = p_dc;
		}
		if (instace == NULL)
		{
			Assert(0);
		}
		return instace;
	};

public:
	IBindSingleton(void) {};
	~IBindSingleton(void) {};
};

#endif // FSGAME_INTERFACE_SINGLETON_H_
