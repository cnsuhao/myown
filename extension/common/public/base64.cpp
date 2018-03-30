#include "base64.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char Decode_GetByte(unsigned char c);
	unsigned char Encode_GetChar(unsigned char num);

	//===================================
	//    Base64 ½âÂë
	//===================================
	unsigned char Decode_GetByte(unsigned char c)
	{
		if (c == '+')
			return 62;
		else if (c == '/')
			return 63;
		else if (c <= '9')
			return (unsigned char)(c - '0' + 52);
		else if (c == '=')
			return 64;
		else if (c <= 'Z')
			return (unsigned char)(c - 'A');
		else if (c <= 'z')
			return (unsigned char)(c - 'a' + 26);
		return 64;
	}

	size_t base64_decode(unsigned char* dst, const unsigned char* src, size_t srclen)
	{
		unsigned char input[4];
		size_t i, index = 0;
		for (i = 0; i < srclen; i += 4)
		{
			//byte[0]
			input[0] = Decode_GetByte(src[i]);
			input[1] = Decode_GetByte(src[i + 1]);
			dst[index++] = (input[0] << 2) + (input[1] >> 4);

			//byte[1]
			if (src[i + 2] != '=')
			{
				input[2] = Decode_GetByte(src[i + 2]);
				dst[index++] = ((input[1] & 0x0f) << 4) + (input[2] >> 2);
			}

			//byte[2]
			if (src[i + 3] != '=')
			{
				input[3] = Decode_GetByte(src[i + 3]);
				dst[index++] = ((input[2] & 0x03) << 6) + (input[3]);
			}
		}

		//null-terminator
		dst[index] = 0;
		return index;
	}

	//===================================
	//    Base64 ±àÂë
	//===================================
	static unsigned char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789"
		"+/=";

	unsigned char Encode_GetChar(unsigned char num)
	{
		return base64[num];
	}

	size_t base64_encode(unsigned char* dst, const unsigned char* src, size_t srclen)
	{
		unsigned char input[3], output[4];
		size_t i, index_src = 0, index_dest = 0;
		for (i = 0; i < srclen; i += 3)
		{
			//char [0]
			input[0] = src[index_src++];
			output[0] = (unsigned char)(input[0] >> 2);
			dst[index_dest++] = Encode_GetChar(output[0]);

			//char [1]
			if (index_src < srclen)
			{
				input[1] = src[index_src++];
				output[1] = (unsigned char)(((input[0] & 0x03) << 4) + (input[1] >> 4));
				dst[index_dest++] = Encode_GetChar(output[1]);
			}
			else
			{
				output[1] = (unsigned char)((input[0] & 0x03) << 4);
				dst[index_dest++] = Encode_GetChar(output[1]);
				dst[index_dest++] = '=';
				dst[index_dest++] = '=';
				break;
			}

			//char [2]
			if (index_src < srclen)
			{
				input[2] = src[index_src++];
				output[2] = (unsigned char)(((input[1] & 0x0f) << 2) + (input[2] >> 6));
				dst[index_dest++] = Encode_GetChar(output[2]);
			}
			else
			{
				output[2] = (unsigned char)((input[1] & 0x0f) << 2);
				dst[index_dest++] = Encode_GetChar(output[2]);
				dst[index_dest++] = '=';
				break;
			}

			//char [3]
			output[3] = (unsigned char)(input[2] & 0x3f);
			dst[index_dest++] = Encode_GetChar(output[3]);
		}
		//null-terminator
		dst[index_dest] = 0;
		return index_dest;
	}

#ifdef __cplusplus
}
#endif