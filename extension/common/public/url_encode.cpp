#include <stdio.h>
#include <ctype.h>
#include "url_encode.h"

#ifdef __cplusplus
extern "C"
{
#endif
	bool url_encode(unsigned char* dst, const unsigned char* src, size_t srclen)
	{
		if (src == NULL || dst == NULL )
			return false;

		if (srclen == 0)
		{
			dst[0] = 0;
			return true;
		}

		char baseChar = 'A';
		unsigned char c;
		int nCount = 0; //累加
		while (c = *src)
		{
			if (isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
			{
				*dst = c;
				++dst;
				++nCount;
			}
			else if (c == ' ')
			{
				*dst = '+';
				++dst;
				++nCount;
			}
			else
			{
				dst[0] = '%';
				dst[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
				dst[2] = ((c & 0xF) >= 0xA) ? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
				dst += 3;
				nCount += 3;
			}
			++src;
		}
		//null-terminator
		*dst = '\0';
		return true;
	}

	bool url_decode(unsigned char* dst, const unsigned char* src, size_t srclen)
	{
		if (dst == NULL || src == NULL)
			return false;

		if (srclen == 0)
		{
			dst[0] = 0;
			return true;
		}

		unsigned char c;
		while (c = *src)
		{
			if (c == '%')
			{
				*dst = 0;
				//高位
				if (src[1] >= 'A' && src[1] <= 'F')
					*dst += (src[1] - 'A' + 10) * 0x10;
				else if (src[1] >= 'a' && src[1] <= 'f')
					*dst += (src[1] - 'a' + 10) * 0x10;
				else
					*dst += (src[1] - '0') * 0x10;

				//低位
				if (src[2] >= 'A' && src[2] <= 'F')
					*dst += (src[2] - 'A' + 10);
				else if (src[2] >= 'a' && src[2] <= 'f')
					*dst += (src[2] - 'a' + 10);
				else
					*dst += (src[2] - '0');

				src += 3;
			}
			else if (c == '+')
			{
				*dst = ' ';
				++src;
			}
			else
			{
				*dst = *src;
				++src;
			}
			++dst;
		}

		//null-terminator
		*dst = '\0';
		return true;
	}

#ifdef __cplusplus
}
#endif