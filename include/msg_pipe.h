#if !defined __MSG_PIPE_H__
#define __MSG_PIPE_H__


#include "fast_thread_lock.h"


#define MSGPIPE_RESET_VALUE 0x00ffffff


template<typename TYPE, unsigned int SIZE>
class TArrayMsgPipe
{
public:
	TArrayMsgPipe()
	{
		m_nReadIndex	= 0;
		m_nMaxReadIndex = 0;
		m_nWriteIndex	= 0;
	}
	
public:
	inline bool Push(const TYPE& value)
	{
		CAS_TYPE nCurrentReadIndex, nCurrentWriteIndex;

		do 
		{
			nCurrentWriteIndex	= m_nWriteIndex;
			nCurrentReadIndex	= m_nReadIndex;

			/// full
			if (CountToIndex(nCurrentWriteIndex+1) == CountToIndex(nCurrentReadIndex))
			{
				return false;
			}
		}
		while (!CAS(&m_nWriteIndex, nCurrentWriteIndex, nCurrentWriteIndex+1));

		m_Data[CountToIndex(nCurrentWriteIndex)] = value;

		while (!CAS(&m_nMaxReadIndex, nCurrentWriteIndex, nCurrentWriteIndex+1))
		{
			/// wait
			Sleep(0);
		}

		return true;
	}

	inline bool Pop(TYPE& value)
	{
		CAS_TYPE nCurrentMaxReadIndex, nCurrentReadIndex;

		do 
		{
			nCurrentReadIndex		= m_nReadIndex;
			nCurrentMaxReadIndex	= m_nMaxReadIndex;
			/// empty
			if (CountToIndex(nCurrentReadIndex) == CountToIndex(nCurrentMaxReadIndex))
			{
				return false;
			}

			value = m_Data[CountToIndex(nCurrentReadIndex)];

			if (CAS(&m_nReadIndex, nCurrentReadIndex, nCurrentReadIndex+1))
			{
				return true;
			}
		}
		while (true);

		/// 肯定走不到这里
		return false;
	}

	inline double UseRatio()
	{
		return double(m_nMaxReadIndex-m_nReadIndex)/double(SIZE);
	}

	inline void ResetIndex()
	{
		if (m_nWriteIndex > MSGPIPE_RESET_VALUE)
		{
			/// reset read index
			CAS_TYPE nCurrentReadIndex;
			do 
			{
				nCurrentReadIndex = m_nReadIndex;
			}
			while (!CAS(&m_nReadIndex, nCurrentReadIndex, CountToIndex(nCurrentReadIndex)));


			/// reset write index
			CAS_TYPE nCurrentWriteIndex, nSetWriteIndex;
			do 
			{
				nCurrentWriteIndex	= m_nWriteIndex;
				nSetWriteIndex		= (m_nReadIndex/SIZE + 1) * SIZE + CountToIndex(nCurrentWriteIndex);
			}
			while (!CAS(&m_nWriteIndex, nCurrentWriteIndex, nSetWriteIndex));

			while (!CAS(&m_nMaxReadIndex, nCurrentWriteIndex, nSetWriteIndex))
			{
				/// wait
				Sleep(0);
			}			
		}
	}

private:
	inline CAS_TYPE CountToIndex(CAS_TYPE count)
	{
		return count % SIZE;
	}

private:
	TYPE m_Data[SIZE];

	CAS_TYPE m_nReadIndex;
	CAS_TYPE m_nWriteIndex;
	CAS_TYPE m_nMaxReadIndex;	
};


/// http://coolshell.cn/articles/8239.html
/// http://software.intel.com/en-us/articles/single-producer-single-consumer-queue
/// http://www.oschina.net/translate/a-fast-lock-free-queue-for-cpp?print
/// http://www.codeproject.com/Articles/153898/Yet-another-implementation-of-a-lock-free-circular

#endif // __MSG_PIPE_H__