//--------------------------------------------------------------------
// 文件名:		Classes.h
// 内  容:		类名定义模块
// 说  明:		类名称定义
// 创建者:		
// 日  期:      	
//--------------------------------------------------------------------
#ifndef __CLASS_DEFINE_H__ 
#define __CLASS_DEFINE_H__

{{#class}}
//<class id="{{id}}" 
//	type="{{type}}" 
//	desc="{{desc}}" 
//	path="{{path}}" />
#ifndef CLASS_NAME_{{id:x-name}}
#define CLASS_NAME_{{id:x-name}}				"{{id}}"
#endif
{{/class}}

#endif // __CLASS_DEFINE_H__