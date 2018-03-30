//--------------------------------------------------------------------
// 文件名:		QuickHash.h
// 内  容:		实现Hash功能的模板类
// 说  明:		
// 创建日期:	2014年06月26日
// 整理日期:	2014年06月26日
// 创建人:		  ( )
//    :	    
//--------------------------------------------------------------------

#ifndef __QuickHash_H__
#define __QuickHash_H__

#include "../public/Macros.h"
#include <string.h>
#include <cstdio>
#include <cstring>

#define HASH_A  63689
#define HASH_B  378551

struct Equal
{
	static bool equal( int a, int b )
	{
		return a == b;
	}

	static bool equal( const char* pStr1, const char* pStr2 )
	{
		return strcmp( pStr1, pStr2 ) == 0;
	}

	static bool equal( const wchar_t* pStr1, const wchar_t* pStr2 )
	{
		return wcscmp( pStr1, pStr2 ) == 0;
	}
};

struct Hash
{
	static int hash( int a )
	{
		return a;
	}

	static int hash( const char* pStr )
	{
		int b = HASH_B;
		int a = HASH_A;
		int hash = 0;
		while ( *pStr )
		{
			hash = hash * a + ( *pStr++ );
			a *= b;
		}

		return ( hash & 0x7FFFFFFF );
	}
};

template< typename Key, typename Value, typename Equal >
class HashNode
{
public:
	typedef HashNode<Key, Value, Equal> Node;

	HashNode( const Key& key, const Value& value )
	{
		m_key = key;
		m_value = value;
		m_pPrev = NULL;
		m_pNext = NULL;
	}

	~HashNode()
	{
		if( m_pPrev != NULL )
		{
			delete m_pPrev;
			m_pPrev = NULL;
		}

		if( m_pNext != NULL )
		{
			delete m_pNext;
			m_pNext = NULL;
		}
	}

	const Key& GetKey()
	{
		return m_key;
	}

	Value& GetValue()
	{
		return m_value;
	}

	void SetValue( const Value& value )
	{
		m_value = value;
	}

	void SetPrev( HashNode* pPrev )
	{
		m_pPrev = pPrev;
	}

	void SetNext( HashNode* pNext )
	{
		m_pNext = pNext;
	}

	Node* PrevNode()
	{
		return m_pPrev;
	}

	Node* NextNode()
	{
		return m_pNext;
	}

	void FrontInsert( Node* pNode )
	{
		if (pNode != NULL && this != pNode)
		{
			pNode->m_pNext = this;
			pNode->m_pPrev = m_pPrev;
		}
	}

	void BackInsert( Node* pNode )
	{
		if (pNode != NULL && this != pNode)
		{
			pNode->m_pNext = m_pNext;
			m_pNext = pNode;
		}
	}

	void Remove()
	{
		if( m_pPrev != NULL )
		{
			m_pPrev->m_pNext = m_pNext;
			if( m_pNext != NULL )
			{
				m_pNext->m_pPrev = m_pPrev;
			}
		}
		else
		{
			m_pNext->m_pPrev = NULL;
		}

		m_pPrev = NULL;
		m_pNext = NULL;
	}

	void Swap( Node* pNode )
	{
		if( NULL != pNode && this != pNode )
		{   
			Node* pTemp = pNode->m_pPrev;
			pNode->m_pPrev = m_pPrev;
			m_pPrev = pTemp;

			pTemp = pNode->m_pNext;
			pNode->m_pNext = m_pNext;
			m_pNext = pTemp;
		}
	}

	Node* Find( const Key& key )
	{
		if( Equal::equal( m_key, key ) )
		{
			return this;
		}

		Node* pTemp = m_pNext;
		LoopBeginCheck(a)
		while( pTemp != NULL )
		{
			LoopDoCheck(a)
			if( Equal::equal( pTemp->m_key, key ) )
			{
				return pTemp;
			}
			pTemp = pTemp->m_pNext;
		}

		pTemp = m_pPrev;
		LoopBeginCheck(b)
		while( pTemp != NULL )
		{
			LoopDoCheck(b)
			if( Equal::equal( pTemp->m_key, key) )
			{
				return pTemp;
			}
			pTemp = pTemp->m_pPrev;
		}

		return NULL;
	}

	bool Adjacent( Node* pNode )
	{
		return pNode->m_pNext == this || pNode->m_pPrev == this;
	}

	int NextNum()
	{
		Node* pTemp = m_pNext;
		int num = 0;
		LoopBeginCheck(c)
		while( pTemp != NULL )
		{
			LoopDoCheck(c)
			++num;
			pTemp = pTemp->m_pNext;
		}

		return num;
	}

	int PrevNum()
	{
		Node* pTemp = m_pPrev;
		int num = 0; 
		LoopBeginCheck(d)
		while( pTemp != NULL )
		{
			LoopDoCheck(d)
			++num;
			pTemp = pTemp->m_pPrev;
		}

		return num;
	}

protected:
	Node* m_pPrev;
	Node* m_pNext;

	Key m_key;
	Value m_value;//要求保存的数据可以复制
	
	template<typename Key, typename Value, typename Equal, typename Hash, int Size> friend class QuickHash;  
};

template< typename Key, typename Value, typename Equal, typename Hash, int Size = 1000 >
class QuickHash
{
public:
	typedef HashNode< Key, Value, Equal > Node;

	QuickHash()
	{
		m_HashSize = Size;
		m_pHashTable = NEW Node*[m_HashSize];
		LoopBeginCheck(e)
		for( int i = 0; i < m_HashSize; ++i )
		{
			LoopDoCheck(e)
			m_pHashTable[i] = NULL;
		}
	}

	~QuickHash()
	{
		LoopBeginCheck(f)
		for( int i = 0; i < m_HashSize; ++i )
		{
			LoopDoCheck(f)
			Node* pNode = m_pHashTable[i];
			if( pNode != NULL )
			{
				delete m_pHashTable[i];
			}
		}

		delete[] m_pHashTable;
	}

	void Resize( int size )
	{
		if( size > m_HashSize )
		{
			Node** pHashTable = NEW Node[size];
			Node* pNode;
			Node* pTemp;
			int key;
			LoopBeginCheck(g)
			for( int i = 0; i < m_HashSize; ++i )
			{
				LoopDoCheck(g)
				pNode = m_pHashTable[i];
				if( pNode != NULL )
				{
					key = Hash::hash( pNode->GetKey() ) % size;
					pTemp = pHashTable[key];
					if( pTemp != NULL )
					{
						pTemp->FrontInsert( pNode );
					}
					else
					{
						pHashTable[key] = pNode;
					}
				}
			}

			delete[] m_pHashTable;
			m_pHashTable = pHashTable;
			m_HashSize = size;
		}
	}

	bool Insert( const Key& key, const Value& value )
	{
		int index = Hash::hash(key) % m_HashSize;
		if( index < 0 || index >= m_HashSize )
		{
			return false;
		}

		Node* pNode = m_pHashTable[index];
		Node* pNew = NEW Node( key, value );
		if( pNode != NULL )
		{
			pNode->BackInsert( pNew );
		}
		else 
		{
			m_pHashTable[index] = pNew;
		}

		return true;
	}

	bool Delete( const Key& key, const Value& value )
	{
		int index = Hash::hash( key ) % m_HashSize;
		if( index < 0 || index >= m_HashSize )
		{
			return false;
		}

		Node* pNode = m_pHashTable[index];
		Node* pFind;
		if( pNode != NULL && (pFind = pNode->Find( key )) != NULL )
		{
			if( pFind == pNode )
			{
				pNode = pNode->NextNode();
				pFind->Remove();
				m_pHashTable[index] = pNode;
			}
			else 
			{
				pFind->Remove();
			}

			delete pFind;

			return true;
		}

		return false;
	}

	bool Delete( Node* pNode )
	{
		int index = Hash::hash( pNode->GetKey() ) % m_HashSize;
		if( index < 0 || index >= m_HashSize )
		{
			return false;
		}

		Node* pTemp = m_pHashTable[index];
		if( pTemp != NULL && pTemp == pNode )
		{
			Node* pNext = pNode->NextNode();

			pNode->Remove();

			if (pNext != NULL)
			{
				delete pNode;
			}

			m_pHashTable[index] = pNext;

			return true;
		}
		else
		{
			pNode->Remove();

			if (pNode != NULL)
			{
				delete pNode;
			}

			return true;
		}

		return false;
	}

	Node* Find( const Key& key )
	{
		int index = Hash::hash(key) % m_HashSize;
		if( index < 0 || index >= m_HashSize )
		{
			return NULL;
		}

		Node* pNode = m_pHashTable[index];
		Node* pFind;
		if( pNode != NULL && ( (pFind = pNode->Find( key )) != NULL ) )
		{
			return pFind;
		}

		return NULL;
	}

	void Stat()
	{
		int count = 0;
		int times = 0;
		Node* pNode;
		LoopBeginCheck(h)
		for( int i = 0; i < m_HashSize; ++i )
		{
			LoopDoCheck(h)
			pNode = m_pHashTable[i];
			if( pNode != NULL )
			{
				++count;
				count += pNode->NextNum();
				++times;
			}
		}

		SPRINTF_S( "collision rate:%f cover rate:%f\n", ( count / ( double )times ),( times / ( double )m_HashSize ) );
	}

private:
	Node** m_pHashTable;
	int m_HashSize;
};

template<typename TYPE, int SIZE>
class QuickHashFix
{
private:
	typedef QuickHashFix<TYPE, SIZE> self_type;

public:
	QuickHashFix()
	{
	}

	~QuickHashFix()
	{
	}

	size_t size() const
	{
		return SIZE;
	}

	size_t byte_size() const
	{
		return sizeof(TYPE) * SIZE;
	}

	const TYPE* data() const
	{
		return m_Data;
	}

	TYPE* data()
	{
		return m_Data;
	}

	TYPE& operator[](int index)
	{
		int key = Hash::hash(index) % SIZE;
		if( key < 0 || key >= SIZE )
		{
			Assert(false);
		}

		return m_Data[key];
	}

	const TYPE& operator[](int index) const
	{
		int key = Hash::hash(index) % SIZE;
		if( key < 0 || key >= SIZE )
		{
			Assert(false);
		}

		return m_Data[key];
	}

	//void copy(size_t offset, TYPE* pdata, int count)
	//{
	//	Assert(offset <= SIZE);
	//	Assert(count <= (SIZE - offset));
	//	memcpy(m_Data + offset, pdata, sizeof(TYPE) * count);
	//}

	size_t get_memory_usage() const
	{
		return sizeof(self_type);
	}

	bool insert(int index)
	{
		int key = Hash::hash(index) % SIZE;

		m_Data[key] = key;

		return true;
	}

	bool find(int index)
	{
		int key = Hash::hash(index) % SIZE;

		return m_Data[key] == key;
	}

private:
	TYPE m_Data[SIZE];
};

#endif //__QuickHash_H__
