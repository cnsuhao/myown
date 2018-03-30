/*@FileName:	FsStream.h
* @Author:		李海罗
* @Function:	串行化数据包
* @Modify:		2012-11-11
*/
#ifndef __FAST_FRAMEWORK_STREAM_INCLUDE__
#define __FAST_FRAMEWORK_STREAM_INCLUDE__
#include <string>
#include <map>
#include <vector>
#include <list>
#include <deque>
#include <assert.h>

#ifndef _UINT64_
typedef  unsigned long long			uint64;	//!< Unsigned 64bit integer for win32.
#define _UINT64_
#endif

#ifndef _INT64_
typedef long long					int64;
#define _INT64_
#endif

#ifndef _UINT32_
typedef  unsigned int				uint32;				//!< Unsigned 32bit integer.
#define _UINT32_
#endif

#ifndef _INT32_
typedef  signed  int				int32;					//!< Signed 32bit integer.
#define _INT32_
#endif

#ifndef _UINT16_
typedef  unsigned short				uint16;				//!< Unsigned 16bit integer.
#define _UINT16_
#endif

#ifndef _INT16_
typedef  signed short				int16;					//!< Signed 16bit integer.
#define _INT16_
#endif

#ifndef _UINT8_
typedef  unsigned char				uint8;					//!< Unsigned 8bit integer.
#define _UINT8_
#endif

#ifndef _INT8_
typedef  signed char				int8;					//!< Signed 8bit integer.
#define _INT8_
#endif

#ifndef _BYTE_
#define _BYTE_
typedef  unsigned char				byte;					//!< Unsigned 8bit integer.
typedef   char						sbyte;					//!< signed 8bit integer.
#endif

#ifndef _FLOAT32_
#define _FLOAT32_
typedef float						f32;					//!< float 32bit
#endif

#ifndef _FLOAT64_
#define _FLOAT64_
typedef double						f64;					//!< float 64bit
#endif

#ifndef _WCHAR_
#ifdef _MSC_VER
	typedef wchar_t	wchar;
#else
	typedef uint16 wchar;								//!< 16bit text character type.
#endif
#define _WCHAR_
#endif

#define FS_SREAM_SIZE	128

#define SERIAL_BASETYPE_DECL(type) \
	virtual FsIStream& operator<<(const type& t) = 0; \
	virtual FsIStream& operator>>(type& t) = 0;		\
	virtual	bool write_##type(const type &t) = 0;	\
	virtual bool read_##type(type& t) = 0;

#define SERIAL_BASETYPE(type) \
	virtual FsIStream& operator<<(const type& t) override \
	{ \
		write_##type(t); \
		return *this; \
	} \
	virtual FsIStream& operator>>(type& t) override \
	{ \
		read_##type(t); \
		return *this; \
	}\
	virtual bool write_##type(const type &t)  override\
	{	\
		return AddStream(&t, sizeof(type)); \
	}	\
	virtual bool read_##type(type& t)  override\
	{	\
		return GetStream(&t, sizeof(type)); \
	}

#define SERIAL_BASETYPE_NET(type) \
	virtual FsIStream& operator<<(const type& t) override \
	{ \
		write_##type(t); \
		return *this; \
	} \
		virtual FsIStream& operator>>(type& t) override \
	{ \
		read_##type(t); \
		return *this; \
	}\
	virtual bool write_##type(const type &t)  override\
	{	\
		type v = t; \
		ByteConverter::LocalToNetEndian(v);	\
		return AddStream(&v, sizeof(type)); \
	}	\
	virtual bool read_##type(type& t)  override\
	{	\
		if( GetStream(&t, sizeof(type)) ) \
		{\
			ByteConverter::NetToLocalEndian(t); \
			return true;\
		}\
		return false; \
	}


class FsIStream
{
public:
	enum SEEK_POS
	{
		SET_CUR,			// 当前位置
		SET_BEGIN,			// 开始位置
		SET_END,			// 尾部
	};

	virtual ~FsIStream() {};

	// 最大长度
	virtual size_t GetSize() const = 0;
	// 读取位置
	virtual size_t GetReadPos() const = 0;
	// 写入位置
	virtual size_t GetWritePos() const = 0;
	// 有效读数据大小
	virtual size_t GetReadLeft() const = 0;
	// 有效写数据大小
	virtual size_t GetWriteLeft() const = 0;
	// 获取所有数据(从指针头开始)
	virtual const char*	GetBuffer() const = 0;
	// 获取当前数据(从当前读取位置开始）
	virtual const char*	GetData() const = 0;
	// 重指读写指针值
	virtual void Reset() const = 0;

	virtual bool GetStream(void* lpBuf, size_t nLen) const = 0;
	virtual bool AddStream(const void* lpBuf, size_t nLen) = 0;
	virtual bool CanReadSize(size_t size) const = 0;
	virtual bool CanWriteSize(size_t size) const = 0;

	virtual size_t SeekReadPos(int nOffset, SEEK_POS pos = SET_CUR) const = 0;
	virtual size_t SeekWritePos(int nOffset, SEEK_POS pos = SET_CUR) const = 0;

	SERIAL_BASETYPE_DECL(wchar)
	SERIAL_BASETYPE_DECL(int8)
	SERIAL_BASETYPE_DECL(int16)
	SERIAL_BASETYPE_DECL(int)
	SERIAL_BASETYPE_DECL(int64)
	SERIAL_BASETYPE_DECL(uint8)
	SERIAL_BASETYPE_DECL(uint16)
	SERIAL_BASETYPE_DECL(size_t)
#if !defined( _WIN64 )
	SERIAL_BASETYPE_DECL(uint64)
#endif
	SERIAL_BASETYPE_DECL(f32)
	SERIAL_BASETYPE_DECL(f64)
	SERIAL_BASETYPE_DECL(bool)
};

/// 直接引用外部char指针的串行化缓冲区
class FsRefStream : public FsIStream
{
public:
	FsRefStream(char* lpBuf, size_t nLength)
		:m_pbuffer( lpBuf ), m_nReadPos( 0 ), m_nWritePos( 0 ), m_nMaxSize( nLength )
	{
	};

	virtual ~FsRefStream(){};

	virtual size_t GetSize() const { return m_nMaxSize; };
	virtual size_t GetReadPos() const { return m_nReadPos; };
	virtual size_t GetWritePos() const { return m_nWritePos; };
	// 有效读数据大小
	virtual size_t GetReadLeft() const { return m_nReadPos >= m_nWritePos ? 0 : m_nWritePos - m_nReadPos; };
	// 有效数据大小
	virtual size_t GetWriteLeft() const { return m_nWritePos >= m_nMaxSize ? 0 : m_nMaxSize - m_nWritePos; };
	// 获取所有数据(从指针头开始)
	virtual const char*	GetBuffer() const { return m_pbuffer; };
	// 获取当前数据(从当前读取位置开始）
	virtual const char*	GetData() const { return m_pbuffer + m_nReadPos; };
	// 重指读写指针值
	virtual void Reset() const { m_nReadPos = 0; m_nWritePos = 0; };

	virtual bool GetStream( void* lpBuf, size_t nLen ) const
	{
		assert( NULL != lpBuf);
		assert(nLen <= m_nMaxSize - m_nReadPos);
		if (nLen > m_nMaxSize - m_nReadPos)
		{
			//int iSize = m_nMaxSize - m_nReadPos;
			//memcpy(lpBuf, m_pbuffer + m_nReadPos, iSize);
			//m_nReadPos += iSize;
			//memset( (char*)lpBuf + iSize, 0, nMax - iSize );
			memset((char*)lpBuf, 0, nLen);
			return false;
		}

		memcpy(lpBuf, m_pbuffer + m_nReadPos, nLen);
		m_nReadPos += nLen;
		return true;
	};

	virtual bool AddStream(const void* lpBuf, size_t nLen)
	{
		assert(NULL != lpBuf);
		while (m_nWritePos + nLen > m_nMaxSize)
		{
			if (!IncrementBuffer())
			{
				return false;
			}
		}

		memcpy(m_pbuffer + m_nWritePos, lpBuf, nLen);
		m_nWritePos += nLen;
		return true;
	};

	virtual bool CanReadSize(size_t size) const
	{
		return m_nReadPos + size <= m_nWritePos;
	};

	virtual bool CanWriteSize(size_t size) const
	{
		return m_nWritePos + size <= m_nMaxSize;
	};

	virtual size_t SeekReadPos(int nOffset, SEEK_POS pos = SET_CUR) const
	{
		switch( pos )
		{
		case SET_CUR:
			m_nReadPos += nOffset;
			break;
		case SET_BEGIN:
			m_nReadPos = nOffset;
			break;
		case SET_END:
			m_nReadPos = m_nMaxSize + nOffset;
			break;
		};

		if ( m_nReadPos < 0 )
		{
			m_nReadPos = 0;
		}
		else if ( m_nReadPos > m_nMaxSize )
		{
			m_nReadPos = m_nMaxSize;
		}

		return m_nReadPos;
	};

	virtual size_t SeekWritePos(int nOffset, SEEK_POS pos = SET_CUR) const
	{
		switch( pos )
		{
		case SET_CUR:
			m_nWritePos += nOffset;
			break;
		case SET_BEGIN:
			m_nWritePos = nOffset;
			break;
		case SET_END:
			m_nWritePos = m_nMaxSize + nOffset;
			break;
		};

		if ( m_nWritePos < 0 )
		{
			m_nWritePos = 0;
		}
		else if ( m_nWritePos > m_nMaxSize )
		{
			m_nWritePos = m_nMaxSize;
		}
		return m_nWritePos;
	};

	SERIAL_BASETYPE(wchar)
	SERIAL_BASETYPE(int8)
	SERIAL_BASETYPE(int16)
	SERIAL_BASETYPE(int)
	SERIAL_BASETYPE(int64)
	SERIAL_BASETYPE(uint8)
	SERIAL_BASETYPE(uint16)
	SERIAL_BASETYPE(size_t)
#if !defined( _WIN64 )
	SERIAL_BASETYPE(uint64)
#endif
	SERIAL_BASETYPE(f32)
	SERIAL_BASETYPE(f64)
	SERIAL_BASETYPE(bool)

protected:
	// 增加缓冲区大小
	virtual bool IncrementBuffer()
	{
		return false;
	};

	char*				m_pbuffer;
	size_t				m_nMaxSize;

	mutable size_t		m_nReadPos;
	mutable size_t		m_nWritePos;
};

class FsStream : public FsRefStream
{
public:
	FsStream(void) : FsRefStream(NULL, FS_SREAM_SIZE)
	{
		m_pbuffer = m_szbuffer;
	};
	~FsStream(void)
	{
		if (m_nMaxSize > FS_SREAM_SIZE)
		{
			delete[] m_pbuffer;
		}
	};

protected:
	// 增加缓冲区大小
	virtual bool IncrementBuffer()
	{
		size_t nSize = m_nMaxSize << 1;
		char* pbuf = new char[nSize];
		memcpy( pbuf, m_pbuffer, m_nMaxSize );
		if (m_nMaxSize > FS_SREAM_SIZE)
		{
			delete[] m_pbuffer;
		}
		m_pbuffer = pbuf;
		m_nMaxSize = nSize;
		return true;
	};

	char		m_szbuffer[FS_SREAM_SIZE];
};

#endif