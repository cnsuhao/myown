//--------------------------------------------------------------------
// 文件名:		url_encode.h
// 内  容:		
// 说  明:		
// 创建日期:		2016年10月22日
// 创建人:		李海罗
//--------------------------------------------------------------------

#ifndef __UTILS_URL_ENCODE_H__
#define __UTILS_URL_ENCODE_H__

#ifdef __cplusplus
extern "C"
{
#endif

	// UrlEncode （百分号编码）。方法是把输入的字符串先用 UTF-8 编码，然后把基本字符以外的字节用百分号加16进制的形式编码。
	// UrlEncode 的最后一个参数含义是，编码结果中的16进制字符是否采用大写字母表示 传入值需要是utf8编码 编码长度*3
	//百分号编码 
	//http://zh.wikipedia.org/zh-cn/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81

	bool url_encode(unsigned char* dst, const unsigned char* src, size_t srclen);
	bool url_decode(unsigned char* dst, const unsigned char* src, size_t srclen);

#ifdef __cplusplus
}
#endif

#endif // __UTILS_BASE64_H__
