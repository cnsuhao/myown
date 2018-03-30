#include "MsgEncoder.h"
#include "public/IVarList.h"
#include "public/AutoMem.h"
#include "public/Inlines.h"
#include "public/Var.h"

static void base64_decode_block(unsigned char* dst, const char* src)
{
	unsigned int x = 0;
	
	for (int i = 0; i < 4; ++i)
	{
		if (src[i] >= 'A' && src[i] <= 'Z')
		{
			x = (x << 6) + (unsigned int)(src[i] - 'A' + 0);
		}
		else if (src[i] >= 'a' && src[i] <= 'z')
		{
			x = (x << 6) + (unsigned int)(src[i] - 'a' + 26);
		}
		else if (src[i] >= '0' && src[i] <= '9')
		{
			x = (x << 6) + (unsigned int)(src[i] - '0' + 52);
		}
		else if (src[i] == '+')
		{
			x = (x << 6) + 62;
		}
		else if (src[i] == '/')
		{
			x = (x << 6) + 63;
		}
		else if (src[i] == '=')
		{
			x = (x << 6);
		}
	}
	
	dst[2] = (unsigned char)(x & 255); 
	x >>= 8;
	dst[1] = (unsigned char)(x & 255); 
	x >>= 8;
	dst[0] = (unsigned char)(x & 255); 
}

static int base64_decode(unsigned char* dst, const char* src, int inlen)
{
	int length = 0;

	while ((length < inlen) && (src[length] != '='))
	{
		length++;
	}

	int equal_num = 0;

	while (((length + equal_num) < inlen) && (src[length + equal_num] == '='))
	{
		equal_num++;
	}
	
	int block_num = (length + equal_num) / 4;

	for (int i = 0; i < (block_num - 1); ++i) 
	{
		base64_decode_block(dst, src);

		dst += 3;
		src += 4;
	}
	
	unsigned char last_block[3];

	base64_decode_block(last_block, src);

	for (int k = 0; k < (3 - equal_num); ++k)
	{
		dst[k] = last_block[k];
	}

	return (block_num * 3) - equal_num;
}

static int base64_encode(const void* inbuf, int inlen, char* outbuf)
{
	static char table64[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	char* indata = (char*)inbuf;
	
	if (0 == inlen)
	{
		inlen = (int)strlen(indata);
	}
	
	char* output = outbuf;

	while (inlen > 0)
	{
		unsigned char ibuf[3];
		unsigned char obuf[4];
		int input_num = 0;

		for (int i = 0; i < 3; ++i) 
		{
			if (inlen > 0) 
			{
				input_num++;
				ibuf[i] = *indata;
				indata++;
				inlen--;
			}
			else
			{
				ibuf[i] = 0;
			}
		}
		
		obuf[0] = (ibuf[0] & 0xFC) >> 2;
		obuf[1] = ((ibuf[0] & 0x03) << 4) | ((ibuf[1] & 0xF0) >> 4);
		obuf[2] = ((ibuf[1] & 0x0F) << 2) | ((ibuf[2] & 0xC0) >> 6);
		obuf[3] = ibuf[2] & 0x3F;
		
		switch (input_num) 
		{
		case 1:
			output[0] = table64[obuf[0]];
			output[1] = table64[obuf[1]];
			output[2] = '=';
			output[3] = '=';
			break;
		case 2:
			output[0] = table64[obuf[0]];
			output[1] = table64[obuf[1]];
			output[2] = table64[obuf[2]];
			output[3] = '=';
			break;
		default:
			output[0] = table64[obuf[0]];
			output[1] = table64[obuf[1]];
			output[2] = table64[obuf[2]];
			output[3] = table64[obuf[3]];
			break;
		}

		output += 4;
	}
	
	*output = 0;
	
	return (int)(output - outbuf);
}

static unsigned int hex_to_char(const char* s, size_t start, size_t count)
{
	unsigned int v = 0;
	
	for (size_t i = 0; i < count; ++i)
	{
		v <<= 4;
		
		char c = s[start + i];
		
		if ((c >= '0') && (c <= '9'))
		{
			v += (unsigned int)(c - '0');
		}
		else if ((c >= 'A') && (c <= 'F'))
		{
			v += (unsigned int)(c - 'A' + 10);
		}
		else if ((c >= 'a') && (c <= 'f'))
		{
			v += (unsigned int)(c - 'a' + 10);
		}
		else
		{
			return 0;
		}
	}
	
	return v;
}

static void char_to_hex(unsigned int ch, int bytes, char* buf)
{
	static char to_hex[] = "0123456789ABCDEF";
	
	for (int i = (bytes - 1); i >= 0; --i)
	{
		buf[bytes - 1 - i] = to_hex[(ch >> (i * 4)) & 0xF];
	}
}

static bool decode_data(const char* str, size_t len, IVarList& msg)
{
	if (0 == len)
	{
		return true;
	}

	char first = str[0];

	if (((first >= '0') && (first <= '9')) || (first == '.') 
		|| (first == '-'))
	{
		TAutoMem<char, 256> auto_buf(len + 1);
		char* s = auto_buf.GetBuffer();
		
		memcpy(s, str, len);
		s[len] = 0;
		
		if (strchr(s, '.') == NULL)
		{
			if (len < 10)
			{
				msg.AddInt(atoi(s));
			}
			else
			{
				int64_t val = Port_StringToInt64(s);
				int64_t v = val;

				if (v < 0)
				{
					v = -v;
				}

				if ((uint64_t)v > (uint64_t)2147483647) /* maximum (signed) int value */
				{
					msg.AddInt64(val);
				}
				else
				{
					msg.AddInt((int)val);
				}
			}
		}
		else
		{
			msg.AddDouble(atof(s));
		}
	}
	else if (first == '#')
	{
		str++;
		len--;
		
		TAutoMem<wchar_t, 256> auto_buf(len + 1);
		wchar_t* ws = auto_buf.GetBuffer();
		size_t count = 0;
		size_t i = 0;
		
		while (i < len)
		{
			if (str[i] == '\\')
			{
				++i;
				
				if (i >= len)
				{
					return false;
				}
				
				if (str[i] == '\\')
				{
					++i;
					ws[count++] = L'\\';
				}
				else if (str[i] == 'u')
				{
					++i;
					
					if ((i + 3) >= len)
					{
						return false;
					}
					
					wchar_t c = hex_to_char(str, i, 4);
					
					if (0 == c)
					{
						return false;
					}
					
					ws[count++] = c;
					i += 4;
				}
				else if (str[i] == 'x')
				{
					++i;
					
					if ((i + 1) >= len)
					{
						return false;
					}
					
					wchar_t c = hex_to_char(str, i, 2);
					
					if (0 == c)
					{
						return false;
					}
					
					ws[count++] = c;
					i += 2;
				}
				else
				{
					return false;
				}
			}
			else
			{
				ws[count++] = wchar_t(str[i]);
				++i;
			}
		}
		
		ws[count] = 0;
		msg.AddWideStr(ws);
	}
	else if (first == '*')
	{
		str++;
		len--;

		TAutoMem<unsigned char, 256> auto_buf(len + 1);
		unsigned char* output = auto_buf.GetBuffer();
		int size = base64_decode(output, str, (int)len);

		msg.AddUserData(output, size);
	}
	else
	{
		if (first == '$')
		{
			str++;
			len--;
		}

		TAutoMem<char, 256> auto_buf(len + 1);
		char* s = auto_buf.GetBuffer();
		size_t count = 0;
		size_t i = 0;
		
		while (i < len)
		{
			if (str[i] == '\\')
			{
				++i;
				
				if (i >= len)
				{
					return false;
				}
				
				if (str[i] == '\\')
				{
					++i;
					s[count++] = L'\\';
				}
				else if (str[i] == 'x')
				{
					++i;
					
					if ((i + 1) >= len)
					{
						return false;
					}
					
					char c = hex_to_char(str, i, 2);
					
					if (0 == c)
					{
						return false;
					}
					
					s[count++] = c;
					i += 2;
				}
				else
				{
					return false;
				}
			}
			else
			{
				s[count++] = str[i];
				++i;
			}
		}
		
		s[count] = 0;
		msg.AddString(s);
	}

	return true;
}

bool text_decode_msg(const char* value, size_t size, IVarList& msg)
{
	Assert(value != NULL);

	msg.Clear();
	
	const char* str = NULL;
	size_t len = 0;

	for (size_t i = 0; i < size; ++i)
	{
		const char* s = value + i;
		
		if (*s != ' ')
		{
			if (NULL == str)
			{
				str = s;
			}

			++len;
		}
		else
		{
			if (str != NULL)
			{
				if (!decode_data(str, len, msg))
				{
					return false;
				}
			}

			str = s + 1;
			len = 0;
		}
	}

	if (str != NULL)
	{
		if (str <= (value + size))
		{
			if (!decode_data(str, len, msg))
			{
				return false;
			}
		}
	}
	
	return true;
}

int text_encode_msg(const IVarList& msg, char* buf, size_t size)
{
	size_t arg_num = msg.GetCount();

	if (NULL == buf)
	{
		size_t len = arg_num;
		
		for (size_t i = 0; i < arg_num; ++i)
		{
			int type = msg.GetType(i);

			switch (type)
			{
			case VTYPE_INT:
				len += 16;
				break;
			case VTYPE_INT64:
				len += 32;
				break;
			case VTYPE_FLOAT:
				len += 16;
				break;
			case VTYPE_DOUBLE:
				len += 32;
				break;
			case VTYPE_STRING:
				{
					const char* s = msg.StringVal(i);
					
					len += strlen(s) * 4 + 1;
				}
				break;
			case VTYPE_WIDESTR:
				{
					const wchar_t* s = msg.WideStrVal(i);
					
					len += wcslen(s) * 6 + 1;
				}
				break;
			case VTYPE_USERDATA:
				{
					size_t data_size;
					const void* pdata = msg.UserDataVal(i, data_size);

					len += data_size * 2 + 4;
				}
				break;
			default:
				break;
			}
		}

		return (int)len;
	}

	size_t count = 0;

	for (size_t i = 0; i < arg_num; ++i)
	{
		int type = msg.GetType(i);

		switch (type)
		{
		case VTYPE_INT:
			{
				if ((count + 16) >= size)
				{
					return -1;
				}
				
				SafeSprintf(buf + count, 16, "%d", msg.IntVal(i));
				count += strlen(buf + count);
			}
			break;
		case VTYPE_INT64:
			{
				if ((count + 32) >= size)
				{
					return -1;
				}
				
				Port_Int64ToString(msg.Int64Val(i), buf + count, 32);
				count += strlen(buf + count);
			}
			break;
		case VTYPE_FLOAT:
			{
				if ((count + 64) >= size)
				{
					return -1;
				}
				
				SafeSprintf(buf + count, 64, "%f", (double)msg.FloatVal(i));
				count += strlen(buf + count);
			}
			break;
		case VTYPE_DOUBLE:
			{
				if ((count + 64) >= size)
				{
					return -1;
				}
				
				SafeSprintf(buf + count, 64, "%f", msg.DoubleVal(i));
				count += strlen(buf + count);
			}
			break;
		case VTYPE_STRING:
			{
				const char* s = msg.StringVal(i);
				
				if ((count + 1) >= size)
				{
					return -1;
				}
				
				buf[count++] = '$';
				
				for (; *s; ++s)
				{
					unsigned int ch = (unsigned int)*s;
					
					if ((ch > 0x20) && (ch < 0x7F) && (ch != 0x25))
					{
						if ((count + 2) >= size)
						{
							return -1;
						}
						
						buf[count++] = *s;
						
						if (*s == L'\\')
						{
							buf[count++] = '\\';
						}
					}
					else
					{
						if ((count + 4) >= size)
						{
							return -1;
						}
						
						buf[count++] = '\\';
						buf[count++] = 'x';
						char_to_hex(*s, 2, buf + count);
						count += 2;
					}
				}
			}
			break;
		case VTYPE_WIDESTR:
			{
				const wchar_t* s = msg.WideStrVal(i);
				
				if ((count + 1) >= size)
				{
					return -1;
				}

				buf[count++] = '#';
				
				for (; *s; ++s)
				{
					if ((*s > 0x20) && (*s < 0x7F) && (*s != 0x25))
					{
						if ((count + 2) >= size)
						{
							return -1;
						}
						
						buf[count++] = char(*s);
						
						if (*s == L'\\')
						{
							buf[count++] = '\\';
						}
					}
					else
					{
						if (*s >= 256)
						{
							if ((count + 6) >= size)
							{
								return -1;
							}
							
							buf[count++] = '\\';
							buf[count++] = 'u';
							char_to_hex(*s, 4, buf + count);
							count += 4;
						}
						else
						{
							if ((count + 4) >= size)
							{
								return -1;
							}
							
							buf[count++] = '\\';
							buf[count++] = 'x';
							char_to_hex(*s, 2, buf + count);
							count += 2;
						}
					}
				}
			}
			break;
		case VTYPE_USERDATA:
			{
				size_t data_size;
				const void* pdata = msg.UserDataVal(i, data_size);

				if ((count + 1) >= size)
				{
					return -1;
				}

				buf[count++] = '*';
				
				TAutoMem<char, 256> auto_buf(data_size * 2 + 1);
				char* output = auto_buf.GetBuffer();

				int len = base64_encode(pdata, (int)data_size, output);

				if ((count + len) >= size)
				{
					return -1;
				}

				memcpy(buf + count, output, len);
				count += len;
			}
			break;
		default:
			return -1;
		}

		if (i < (arg_num - 1))
		{
			if ((count + 1) >= size)
			{
				return -1;
			}

			buf[count++] = ' ';
		}
	}

	buf[count] = 0;
	
	return (int)count;
}

