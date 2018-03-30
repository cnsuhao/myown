//-------------------------------------------------------------------
// 文 件 名：tqueue.h
// 内    容：
// 说    明：
// 创建日期：2013年12月18日
// 创 建 人：Liangxk
// 版权所有：苏州蜗牛电子有限公司
//-------------------------------------------------------------------


#if !defined __TQUEUE_H__
#define __TQUEUE_H__

#include "../tools/FreeList.h"

template<unsigned int SIZE>
class TLoopInt
{
public:
	typedef TLoopInt<SIZE> self_type;
public:
	TLoopInt() : m_nValue(0) {}
	TLoopInt(unsigned int value) : m_nValue(value%SIZE) {}

	unsigned int Value() const { return m_nValue; }
	unsigned int Inc(unsigned int value = 1) { m_nValue = (m_nValue+value)%SIZE; return m_nValue; }
	unsigned int Dec(unsigned int value = 1) { m_nValue = (m_nValue+SIZE-value%SIZE)%SIZE; return m_nValue; }
	unsigned int operator-(const self_type& value) const
	{
		return (m_nValue+SIZE-value.Value()) % SIZE;
	}
private:
	unsigned int m_nValue;
};


template<typename value_type, unsigned int SIZE, typename INDEX>
class TBlockQueue;

template<typename value_type, unsigned int SIZE>
class TBlockQueueIterator
{
private:
	typedef TBlockQueue<value_type, SIZE, TLoopInt<SIZE>> container_type;

public:
	TBlockQueueIterator(container_type* pContainer, unsigned int idx)
	{
		m_pContainer = pContainer;
		m_nIndex = idx;
	}
	TBlockQueueIterator(const TBlockQueueIterator& other)
	{
		m_pContainer = other.m_pContainer;
		m_nIndex = other.m_nIndex.Value();
	}

private:
	TBlockQueueIterator() {}

public:
	TBlockQueueIterator& operator++()
	{
		m_nIndex.Inc();
		return *this;
	}
	TBlockQueueIterator operator++(int)
	{
		TBlockQueueIterator tmp(*this);
		m_nIndex.Inc();

		return tmp;
	}
	bool operator ==(const TBlockQueueIterator& other)
	{
		return m_pContainer == other.m_pContainer && m_nIndex.Value() == other.m_nIndex.Value();
	}

	bool operator !=(const TBlockQueueIterator& other)
	{
		return m_pContainer != other.m_pContainer || m_nIndex.Value() != other.m_nIndex.Value();
	}

	int Index()
	{
		return m_nIndex.Value();
	}

	value_type* Value()
	{
		return m_pContainer?m_pContainer->m_pValues[m_nIndex.Value()] : 0;
	}
private:
	container_type* m_pContainer;
	TLoopInt<SIZE> m_nIndex;
};

template<typename value_type, unsigned int SIZE, typename INDEX = TLoopInt<SIZE> >
class TBlockQueue
{
private:
	typedef TFreeList<value_type> FreePool;

public:
	typedef TBlockQueueIterator<value_type,SIZE> iterator;
	typedef TBlockQueueIterator<value_type,SIZE> const_iterator;
public:
	TBlockQueue()
	{
		memset(m_pValues, 0, sizeof(value_type*)*SIZE);
		m_nMaxValueCount = SIZE;
		m_nBeginIndex = 0;
		m_nEndIndex = 0;
	}
	~TBlockQueue()
	{
		Clear();
	}

public:
	void Clear()
	{
		for (int i = 0; i < SIZE; ++i)
		{
			value_type* p = m_pValues[i];
			if (p)
			{
				m_Pool.Delete(p);
			}
		}

		memset(m_pValues, 0, sizeof(value_type*)*SIZE);
		m_nBeginIndex = 0;
		m_nEndIndex = 0;
	}
	bool Empty()
	{
		return m_nEndIndex.Value() == m_nBeginIndex.Value();
	}
	bool Full()
	{
		return m_nBeginIndex-m_nEndIndex == 1;
	}
	unsigned int Size()
	{
		return m_nEndIndex-m_nBeginIndex;
	}
	value_type* Front()
	{
		if (Empty())
		{
			return 0;
		}

		return m_pValues[m_nBeginIndex.Value()];
	}
	value_type* Back()
	{
		if (Empty())
		{
			return 0;
		}

		INDEX temp(m_nEndIndex.Value());
		temp.Dec();
		return m_pValues[temp.Value()];
	}
	void Pop()
	{
		if (Empty())
		{
			return;
		}

		unsigned int index = m_nBeginIndex.Value();
		value_type* p = m_pValues[index];
		if (p)
		{
			m_Pool.Delete(p);
			m_pValues[index] = 0;
		}

		m_nBeginIndex.Inc();
	}
	value_type* Push()
	{
		if (Full())
		{
			return 0;
		}

		unsigned int current = m_nEndIndex.Value();
		m_pValues[current] = m_Pool.New();
		new(m_pValues[current]) value_type();

		m_nEndIndex.Inc();

		return m_pValues[current];
	}

	iterator Begin()
	{
		iterator tmp(this, m_nBeginIndex.Value());

		return tmp;
	}
	iterator End()
	{
		iterator tmp(this, m_nEndIndex.Value());

		return tmp;
	}

private:
	value_type* m_pValues[SIZE];
	unsigned int m_nMaxValueCount;
	INDEX m_nBeginIndex;
	INDEX m_nEndIndex;
	FreePool m_Pool;

	friend class iterator;
};

#endif // __TQUEUE_H__
