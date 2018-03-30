//--------------------------------------------------------------------
// 文件名:		base64.h
// 内  容:		
// 说  明:		
// 创建日期:		2016年10月22日
// 创建人:		李海罗
//--------------------------------------------------------------------

#ifndef __UTILS_BASE64_H__
#define __UTILS_BASE64_H__

#ifdef __cplusplus
extern "C"
{
#endif
	// Base64 编码。Base64 编码是把每 3 个字节转换成 4 个ascii 字符（根据字符表映射）。
	// 把文本编码后，对人来说难以直接阅读。结尾不足时可能有一到两个 "=" 字符的补齐。
	size_t base64_encode(unsigned char* dst, const unsigned char* src, size_t srclen);
	size_t base64_decode(unsigned char* dst, const unsigned char* src, size_t srclen);
#ifdef __cplusplus
}
#endif

#endif // __UTILS_BASE64_H__
