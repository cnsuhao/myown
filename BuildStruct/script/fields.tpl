//--------------------------------------------------------------------
// 文件名:		Fields.h
// 内  容:		字段名定义
// 说  明:		字段名称定义
// 创建者:		
// 日  期:      
//--------------------------------------------------------------------
#ifndef _FIELDS_DEFINE_H_
#define _FIELDS_DEFINE_H_

// 字段定义===============================================================
{{#properties}}
//<property name="{{name}}" 
// 	type="{{type}}" 
//	public="{{public}}" 
//	private="{{private}}" 
//	save="{{save}}" 
//	desc="{{desc}}"/>
#define FIELD_PROP_{{name:x-name}}			"{{name}}"
{{/properties}}

// 表定义==============================================================
{{#records}}
{{#attributes}}
//<record name="{{name}}"
//	cols="{{cols}}" 
//	maxrows="{{maxrows}}" 
//	public="{{public}}" 
//	private="{{private}}" 
//	save="{{save}}" 
//	desc="{{desc}}"/>
{{/attributes}}
#define FIELD_RECORD_{{rec_name:x-name}}			"{{rec_name}}"
// {{rec_name}} column index define
enum COLUMNS_OF_{{rec_name:x-name}}
{
{{#columns}}
	// <column type="{{type}}" desc="{{desc}}"/>
	COLUMN_{{rec_name:x-name}}_{{name:x-name}},
{{/columns}}
};
{{/records}}

#endif // _FIELDS_DEFINE_H_