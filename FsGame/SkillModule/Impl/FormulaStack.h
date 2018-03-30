
//--------------------------------------------------------------------
// 文件名:      mystack.h
// 内  容:      
// 说  明:      
// 创建日期:    2015年4月15日
// 创建人:      longch
//    :       
//--------------------------------------------------------------------


#ifndef __MYSTACK_H__
#define __MYSTACK_H__


template <class ElemType> 
class FormulaStack
{
private:
    enum {MAXSIZE = 100};
    ElemType data[MAXSIZE];
    int top;

public:
    FormulaStack();
    void Clear();
    bool Empty();
    int Size();
    bool Top(ElemType& e);
    bool Push(const ElemType& e);
    bool Pop();
};

template<class T> 
FormulaStack<T>::FormulaStack()
{
    Clear();
}

template<class T> 
void FormulaStack<T>::Clear()
{
    this->top = 0;
}

template<class T> 
bool FormulaStack<T>::Empty()
{
    return this->top == 0? true : false;
}

template<class T> 
int FormulaStack<T>::Size()
{
    return this->top;
}

template<class T>
bool FormulaStack<T>::Top(T& e)
{
    if(Empty())
    {
        return false;
    }
    e = this->data[this->top-1];
    return true;
}

template<class T> 
bool FormulaStack<T>::Push(const T& x)
{
    if(this->top == MAXSIZE)
    {
        return false;
    }
    this->data[this->top] =x;
    this->top ++;
    return true;
}

template<class T>
bool FormulaStack<T>::Pop()
{
    if(this->Empty())
    {
        return false;
    }

    //e =this->data[this->top-1];
    this->top --;
    return true;
}

#endif // __MYSTACK_H__
