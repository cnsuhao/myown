#if !defined __READ_TXT_H__
#define __READ_TXT_H__

/**
* 只读txt文件解析
* txt必要的格式如下:
*
* col1	col2	col3...coln
* val1	val2	val3...valn
* 
* 第一行为列标题 下一行为值 列之间以tab键分割
*/


#if !defined __SAFERELEASE__
#define __SAFERELEASE__
#define SAFE_RELEASE_ARRAY(p) if (p) delete[] p; p = 0
#define SAFE_RELEASE_PTR(p) if (p) delete p; p = 0
#endif // __SAFERELEASE__

#define FILE_PATH_LEN 256

class IReadTxt
{
public:
	/**
	@brief      GetCols								获得txt数据的列数
	@return		virtual int							返回txt数据的列数(文件第一行的列数) 注意必须要先载入txt文件
	*/
	virtual int GetCols() = 0;

	/**
	@brief      GetRows								获得txt数据的行数	
	@return		virtual int							返回txt数据的行数 txt有效行数-1(除去标题行) 注意必须要先载入txt文件 否则返回-1
	*/
	virtual int GetRows() = 0;

	/**
	@brief      FindColIndex						查找列所在的索引	
	@param      [in]const char* strColName			要查找的列的名字
	@return		virtual int							返回-1表示找不到该列, 否则返回列索引 注意必须要先载入txt文件
	*/
	virtual int FindColIndex(const char* strColName) = 0;

	/**
	@brief      GetColName							获取列名称
	@param      [in]int col							列索引
	@return		virtual const char* 				返回第col列的名称
	*/
	virtual const char* GetColName(int col) = 0;

	/**
	@brief      GetValue							读取txt数据 第row行第col列的值
	@param      [in]int row							行
	@param      [in]int col							列
	@return		virtual const char*					如果row 或者col 超出范围 则返回"" 否则返回[row][col]单元格的值
	*/
	virtual const char* GetValue(int row, int col) = 0;

	/**
	@brief      SetFileName							设置txt文件名称
	@param      [in]const char* strFileName			文件全路径
	@return		virtual bool						设置成功则返回true. 否则返回false 文件全路径不得超过255个字符
	*/
	virtual bool SetFileName(const char* strFileName) = 0;

	/**
	@brief      SetRowMax							设置表格最大行数
	@param      [in]int nRowMax						允许的最大行数
	@return		virtual bool						该函数可能会根据设置的值重新分配内存 如果设置的值小于当前最大值 则直接返回false
	*/
	virtual bool SetRowMax(int nRowMax) = 0;

	/**
	@brief      LoadFromFile						载入txt文件
	@return		virtual bool						载入成功则返回true, 否则返回false
	*/
	virtual bool LoadFromFile(int nBeginRow = 0) = 0;

	//virtual bool InitLoadFileStep() = 0
	//

	//virtual bool LoadFromFileStep() = 0;

	/**
	@brief      LoadFromFile						载入txt文件
	@param      [in]const char* sMem				内存开始地址
	@param      [in]int len							内存字符数量
	@return		virtual bool						载入成功则返回true, 否则返回false
	*/
	virtual bool LoadFromMem(const char* sMem, int len) = 0;
};

class ReadTxt : public IReadTxt
{
public:
	ReadTxt() : m_nRows(0), m_nMaxRow(0), m_nCols(0), m_pColsMaxLen(0), m_pData(0), m_pRowInfo(0) 
	{}

	~ReadTxt()
	{
		Release();
	}

public:

	// 列数
	virtual int GetCols() 
	{
		return m_nCols;
	}

	// 行数
	virtual int GetRows()
	{
		return m_nRows - 1;
	}

	// 列索引
	virtual int FindColIndex(const char* strColName)
	{
		if (m_nRows <= 0)
		{
			return -1;
		}

		if (m_nCols <= 0)
		{
			return -1;
		}

		char** p = m_pRowInfo[0];

		for (int i = 0; i < m_nCols; ++i)
		{
			if (strcmp(*p++, strColName) == 0)
			{
				return i;
				break;
			}
		}

		return -1;
	}

	// 列名称
	virtual const char* GetColName(int col)
	{
		if (m_nRows < 0 || col < 0 || col >= m_nCols)
		{
			return "";
		}

		return m_pRowInfo[0][col];
	}

	// 获得单元格数值
	virtual const char* GetValue(int row, int col)
	{
		row += 1;

		if (row < 1 || row >= m_nRows)
		{
			return "";
		}

		if (col < 0 || col >= m_nCols)
		{
			return "";
		}

		char* p = m_pRowInfo[row][col];

		return p ? p : "";
	}

	inline int GetValueInt(int row, int col)
	{
		return atoi(GetValue(row, col));
	}
	inline float GetValueFloat(int row, int col)
	{
		return float(atof(GetValue(row, col)));
	}

	// 设置文件名称
	virtual bool SetFileName(const char* strFileName)
	{
		if (strFileName == 0)
		{
			return false;
		}

		int len = (int)strlen(strFileName);
		if (len >= FILE_PATH_LEN)
		{
			return false;
		}

		memcpy_s(m_pFileName, FILE_PATH_LEN, strFileName, len);

		m_pFileName[len] = 0;

		return true;
	}

	virtual bool SetRowMax(int nRowMax)
	{
		if (nRowMax <= m_nMaxRow)
		{
			return false;
		}

		int max_row = m_nMaxRow;
		m_nMaxRow = nRowMax;

		char*** pTemp = new char**[m_nMaxRow];

		memset(pTemp, 0, sizeof(char**)*m_nMaxRow);

		if (max_row > 0 && m_pRowInfo)
		{
			memcpy(pTemp, m_pRowInfo, max_row*sizeof(char**));

			delete[] m_pRowInfo;
		}

		m_pRowInfo = pTemp;

		return true;
	}

	// 加载 解析文件
	virtual bool LoadFromFile(int nBeginRow = 0)
	{
		Release();

		// 打开文件
		FILE* pfile;
		if (0 != fopen_s(&pfile, m_pFileName, "rb"))
		{
			return false;
		}

		// 文件长度
		int ret = fseek(pfile, 0, SEEK_END);
		int size = ftell(pfile);
		ret = fseek(pfile, 0, SEEK_SET);

		// 读取文件导内存
		m_pData = new char[size+3];
		int readsize = (int)fread(m_pData, size, 1, pfile);
		m_pData[size++] = '\r';
		m_pData[size++] = '\n';
		m_pData[size++] = 0;

		// 关闭文件
		fclose(pfile);

		// 文件格式是否utf8
		char* p = m_pData;
		if (IsUtf8File(m_pData, size))
		{
			p += 3;
			size -= 3;
		}

		if (nBeginRow > 0)
		{
			for (int i = 0; i < size; ++i)
			{
				switch (*(p+i))
				{
				case '\n':
					--nBeginRow;
					break;
				}

				if (nBeginRow == 0)
				{
					return Inner_Load(p+i+1, size-i-1);
				}
			}
		}
		else
		{
			return Inner_Load(p, size);
		}

		return false;
	}

	//virtual bool InitLoadFileStep()
	//{
	//	m_nFileFirstRowLen = 0;
	//	m_nFileOffset = -1;
	//	m_nFileLens = 0;
	//}

	//virtual bool LoadFromFileStep()
	//{
	//	Release();

	//	if (m_nFileOffset >= m_nFileLens)
	//	{
	//		return false;
	//	}

	//	// 打开文件
	//	FILE* pfile;
	//	if (0 != fopen_s(&pfile, m_pFileName, "rb"))
	//	{
	//		return false;
	//	}

	//	if (m_nFileLens <= 0)
	//	{
	//		// 文件长度
	//		int ret = fseek(pfile, 0, SEEK_END);
	//		m_nFileLens = ftell(pfile);
	//		fseek(pfile, 0, SEEK_SET);
	//	}

	//	// 16M
	//	int nBufferLen = 0x1000000;
	//	int nRemainLen = m_nFileLens - m_nFileOffset;

	//	if (nBufferLen > (nRemainLen + m_nFileFirstRowLen))
	//	{
	//		nBufferLen = nRemainLen + m_nFileFirstRowLen;
	//	}

	//	m_pData = new char[nBufferLen+2];
	//	int read_len = 0;

	//	if (m_nFileFirstRowLen <= 0)
	//	{
	//		read_len = nBufferLen;
	//		fseek(pfile, 0, SEEK_SET);
	//		fread(m_pData, read_len, 1, pfile);

	//		// 标题行长度
	//		char* p = m_pData;
	//		while (p < m_pData+nBufferLen-1)
	//		{
	//			if (*p == '\r' || *p == '\n')
	//			{
	//				if (*(p+1) == '\r' || *(p+1) == '\n')
	//				{
	//					++p;
	//				}

	//				m_nFileFirstRowLen = p-m_pData+1;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		// 读取第一行 (标题)
	//		fseek(pfile, 0, SEEK_SET);
	//		fread(m_pData, m_nFileFirstRowLen, 1, pfile);

	//		// 读取数据行
	//		read_len = nBufferLen-m_nFileFirstRowLen;
	//		fseek(pfile, m_nFileOffset, SEEK_SET);
	//		fread(m_pData+m_nFileFirstRowLen, read_len, 1, pfile);
	//	}

	//	m_nFileOffset = ftell(pfile);
	//	
	//	// 文件读取没有结束
	//	if (m_nFileOffset < m_nFileLens)
	//	{
	//		char*p = m_pData + nBufferLen-1;
	//		while (p > m_pData+m_nFileFirstRowLen)
	//		{
	//			if (*p == '\r' || *p == '\n')
	//			{
	//				break;
	//			}

	//			--p;
	//		}

	//		m_nFileOffset -= (nBufferLen+m_pData-p);
	//	}


	//	// 关闭文件
	//	fclose(pfile);

	//	// 文件格式是否utf8
	//	char* p = m_pData;
	//	if (IsUtf8File(m_pData, nBufferLen))
	//	{
	//		p += 3;
	//		size -= 3;
	//	}

	//	return Inner_Load(p, size);
	//}

	virtual bool LoadFromMem(const char* sMem, int len)
	{
		if (sMem == 0 || len <= 0)
		{
			return false;
		}

		Release();

		m_pData = new char[len+1];

		memcpy_s(m_pData, len, sMem, len);

		m_pData[len] = 0;

		return Inner_Load(m_pData, len);
	}

	int GetMaxColLen(int col)
	{
		if (col >= 0 && col < m_nCols)
		{
			return m_pColsMaxLen[col];
		}

		return 0;
	}

	// 释放内存 清空标记
	inline void Release()
	{
		// 回收文件内存
		SAFE_RELEASE_ARRAY(m_pData);
		SAFE_RELEASE_ARRAY(m_pColsMaxLen);

		// 回收每行内存
		for (int i = 0; i < m_nRows; ++i)
		{
			if (m_pRowInfo[i])
			{
				SAFE_RELEASE_ARRAY(m_pRowInfo[i]);
			}
		}

		// 回收行数组
		SAFE_RELEASE_ARRAY(m_pRowInfo);

		m_nRows = 0;
		m_nMaxRow = 0;
		m_nCols = 0;
	}

	const char* GetName() { return m_pFileName; }

private:

	// 文件是否utf8格式
	inline bool IsUtf8File(char* buff, int len)
	{
		if (len >= 3)
		{
			if ((unsigned char)buff[0] == 0xEF && (unsigned char)buff[1] == 0xBB && (unsigned char)buff[2] == 0xBF)
			{
				return true;
			}
		}

		return false;
	}

	//// 
	//inline bool GetFileRowsCols()
	//{
	//	//m_nFileRows = 0;
	//	int cols = 0;

	//	FILE* pFile = fopen(m_pFileName, "rb");
	//	if (pFile == NULL)
	//	{
	//		return false;
	//	}

	//	int nBuffSize = 0x100000;
	//	char * pBuffer = new char[nBuffSize];

	//	fseek(pFile, SEEK_END);
	//	int file_len = ftell(pFile);
	//	fseek(pFile, SEEK_SET);

	//	int read_index = ftell();
	//	int read_len = nBuffSize;

	//	while (read_index < file_len)
	//	{
	//		if (read_len > file_len)
	//		{
	//			read_len = file_len;
	//		}

	//		fseek(pFile, read_index);
	//		fread(pBuffer, read_len, 1, pFile);
	//		int offset = 0;

	//		// 列数
	//		if (read_index == 0)
	//		{
	//			cols = 1;
	//			char* p = pBuffer;
	//			while (p < pBuffer+read_len && *p != '\r' && *p != '\n')
	//			{
	//				if (*p == '\t')
	//				{
	//					++cols;
	//				}
	//			}
	//		}
	//		
	//		// 行数
	//		for (int i = 0; i < read_len-1; ++i)
	//		{
	//			char c = pBuffer[i];

	//			if (c == '\r' || c == '\n')
	//			{
	//				char cc = pBuffer[i+1];

	//				if (cc == '\r' || c == '\n')
	//				{
	//					++i;
	//				}

	//				++m_nFileRows;
	//				offset = read_len-i;
	//			}
	//		}

	//		// 下次读取点
	//		read_index = ftell();
	//		read_index -= offset;
	//	}
	//	
	//	fclose(pFile);
	//	delete[] pBuffer;
	//}

	// 解析一个单元格
	inline bool AddWorld(char* p, int col, int len = 0)
	{
		if (p == 0 || col < 0 || col >= m_nCols)
		{
			return false;
		}

		// 行数组不够 重新分配
		if (m_nRows >= m_nMaxRow)
		{
			if (m_nMaxRow == 0)
			{
				SetRowMax(16);
			}
			else
			{
				SetRowMax(2*m_nMaxRow);
			}
		}

		// 行是空行 分配内存
		char** prow = m_pRowInfo[m_nRows];
		if (prow == 0)
		{
			prow = new char* [m_nCols];
			memset(prow, 0, sizeof(char*)*m_nCols);
			m_pRowInfo[m_nRows] = prow;
		}

		// 设置单元格值
		prow[col] = p;
		if (m_pColsMaxLen[col] < len)
		{
			m_pColsMaxLen[col] = len;
		}

		return true;
	}

	bool Inner_Load(char* p, int size)
	{
		if (size <= 1)
		{
			return false;
		}

		// 找到有多少列
		char* pTemp = p;
		while (*(pTemp+1) != '\r' && *(pTemp+1) != '\n')
		{
			if (*pTemp++ == '\t')
			{
				++m_nCols;
			}
		}
		++m_nCols;

		m_pColsMaxLen = new int[m_nCols];

		// 逐行解析
		char* pline = 0;
		char* pworld = 0;
		int col = 0;
		for (int i = 0; i < size; ++i)
		{
			char& c = p[i];

			switch (c)
			{
				// 换行了 行尾的字符串要录入行最后一列 行号+1 行内容置空
			case '\r':
			case '\n':
				{
					c = 0;


					if (pworld)
					{
						AddWorld(pworld, col, int(i+p-pworld)+1);
						pworld = 0;
						++col;
					}
					else
					{
						if (col < m_nCols)
						{
							AddWorld("", col, 1);
							pworld = 0;
							++col;
						}
					}

					if (pline)
					{
						pline = 0;
						++m_nRows;
					}

					col = 0;
				}
				break;

				// 分割符 设置行内列的数值
			case '\t':
				{
					c = 0;

					int info_len = 0;
					if (pworld == 0)
					{
						pworld = "";
					}
					else
					{
						info_len = int(i+p-pworld);
					}

					AddWorld(pworld, col, info_len+1);
					pworld = 0;
					++col;
				}
				break;

				// 设置行的内容和单元格内容
			default:
				{
					if (pline == 0)
					{
						pline = &c;
					}

					if (pworld == 0)
					{
						pworld = &c;
					}
				}
				break;
			}
		}

		// 最后一行末尾的值要追加到尾行最后一列中
		if (pworld)
		{
			AddWorld(pworld, col, (int)strlen(pworld)+1);
			pworld = 0;
			++col;
		}
		else
		{
			if (col < m_nCols)
			{
				AddWorld("", col, 1);
				pworld = 0;
				++col;
			}
		}

		// 行号+1
		if (pline)
		{
			pline = 0;
			++m_nRows;
		}

		return true;
	}

private:
	int m_nFileLens;
	int m_nFileOffset;
	int m_nFileFirstRowLen;

	int m_nRows;
	int m_nMaxRow;
	int m_nCols;
	int* m_pColsMaxLen;
	char* m_pData;

	char*** m_pRowInfo;
	char m_pFileName[FILE_PATH_LEN];
};


#endif // __READ_TXT_H__