

#ifndef _I_RECORDINDEX_H
#define _I_RECORDINDEX_H

#define MAX_ORDER_COLS_NUM	5

enum E_INDEX_ORDER_TYPE
{
	E_INDEX_ORDER_NULL = 0,
	E_INDEX_ORDER_UP,
	E_INDEX_ORDER_DOWN
};

#pragma pack(push, 1)

struct index_param_t
{
	index_param_t():m_nNum(0), m_nMexRows(0)
	{

	}

	bool Add( unsigned int col, E_INDEX_ORDER_TYPE order )
	{
		Assert(m_nNum != MAX_ORDER_COLS_NUM);

		if ( m_nNum < MAX_ORDER_COLS_NUM )
		{
			m_nCols[m_nNum] = col;
			m_nOrder[m_nNum] = order;
			m_nNum++;
			return true;
		}
		return false;
	}

	unsigned int m_nMexRows;
	unsigned int m_nNum;
	unsigned int m_nCols[MAX_ORDER_COLS_NUM];
	unsigned int m_nOrder[MAX_ORDER_COLS_NUM];
};

#pragma pack(pop)

class IRecordIndex
{
public:
	virtual ~IRecordIndex(){}
	virtual void Create( index_param_t& param ) = 0;
	virtual size_t GetRows() const = 0;
	virtual size_t GetIndex( int pos ) = 0;
};

#endif // _I_RECORDINDEX_H
