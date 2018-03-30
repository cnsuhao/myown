#if !defined __CustomDef_h__
#define __CustomDef_h__



/// 自定义消息是否合并的标记
enum CUSTOM_MERGE_FLAG_ENUM
{
	CMF_NOT_MERGE = 0,	/// 不合并，立即发送
	CMF_MERGE = 1,		/// 合并消息， 延迟发送
};


#endif // __CustomDef_h__