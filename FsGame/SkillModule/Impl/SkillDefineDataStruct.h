//--------------------------------------------------------------------
// 文件名:      SkillDataDefineStruct.h
// 内  容:      技能属性数据结构
// 说  明:
// 创建日期:    2014年10月20日
// 创建人:       
//    :       
//--------------------------------------------------------------------
#ifndef _SKILLDEFINEDATASTRUCT_H_
#define _SKILLDEFINEDATASTRUCT_H_
#include <map>
#include "public/IVarList.h"
#include "Fsgame/Define/header.h"

#include "utils/string_util.h"
#include "utils/XmlFile.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/string_buffer.h"
#include "FsGame/Define/SkillDefine.h"


//////////////////////////////////////////////////////////////////////////
// bool
#define  DECLARE_INLINE_BOOL_PROP_VARIANT(name) \
public:\
	inline const bool Get##name() const {return b##name;} \
public:\
	inline void Set##name(const int value){value > 0 ? b##name = true :b## name = false;}\
	inline void Set##name(IKernel* pKernel, const PERSISTID& skill) const\
	{\
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);\
	if (pSkillObj && pSkillObj->FindAttr(#name)) pSkillObj->SetInt(#name, Get##name() ? 1:0);\
	}\
	bool b##name;


// int
#define DECLARE_INLINE_INT_PROP_VARIANT(name) \
public:\
	inline const int Get##name() const {return i##name;} \
public:\
	inline void Set##name(const int value){i##name = value;}\
	inline void Set##name(IKernel* pKernel, const PERSISTID& skill) const\
	{\
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);\
	if (pSkillObj && pSkillObj->FindAttr(#name)) pSkillObj->SetInt(#name, Get##name());\
	}\
	int i##name;


// float
#define DECLARE_INLINE_FLOAT_PROP_VARIANT(name) \
public:\
	inline const float Get##name() const {return f##name;} \
public:\
	inline void Set##name(const float value){f##name = value;}\
	inline void Set##name(IKernel* pKernel, const PERSISTID& skill) const\
	{\
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);\
	if (pSkillObj && pSkillObj->FindAttr(#name)) pSkillObj->SetFloat(#name, Get##name());\
	}\
	float f##name;


// string
#define DECLARE_INLINE_STRING_PROP_VARIANT(name) \
public:\
	inline const char* Get##name() const {const char* p = m_pStringBufferRef->Get(offset_##name); return p;} \
public:\
	inline void Set##name(const char* value){offset_##name = m_pStringBufferRef->Add(value);}\
	inline void Set##name(IKernel* pKernel, const PERSISTID& skill) const\
	{\
	IGameObj* pSkillObj = pKernel->GetGameObj(skill);\
	if (pSkillObj && pSkillObj->FindAttr(#name)) pSkillObj->SetString(#name, Get##name());\
	}\
	size_t offset_##name;


// CVarList
#define DECLARE_INLINE_CVARLIST_PROP_VARIANT(name) \
public:\
	inline const StringArray& Get##name() const {return v##name;} \
public:\
	inline void Set##name(const std::string &value, const char* flag = ",")\
	{\
	v##name.Init(4, 4, m_pStringBufferRef);\
	CVarList tmp;\
	util_split_string(tmp, value, flag);\
	v##name.AddVarList(tmp);\
	}\
	StringArray v##name;



#define _STRING_BUFFER_DEFINED_ \
	StringBuffer* m_pStringBufferRef

#define _STRING_BUFFER_INIT_ \
	m_pStringBufferRef = &stringBuffer

//////////////////////////////////////////////////////////////////////////
// skill data struct define

struct SkillBaseData
{
	_STRING_BUFFER_DEFINED_;

	DECLARE_INLINE_STRING_PROP_VARIANT(ID);//技能编号
	DECLARE_INLINE_STRING_PROP_VARIANT(script);//脚本类型
public:
	void SetPropData(CXmlFile &xml, const char* szSectionName, StringBuffer& stringBuffer)
	{
		_STRING_BUFFER_INIT_;

		SetID(szSectionName);
		Setscript(xml.ReadString(szSectionName, "script", "SkillNormal"));
	}

	void SetPropData(IKernel* pKernel, const PERSISTID& skill) const
	{
		Setscript(pKernel, skill);
	}
}; // end of struct SkillBaseData

struct SkillMainData
{
	_STRING_BUFFER_DEFINED_;

	DECLARE_INLINE_STRING_PROP_VARIANT(ID);//主体编号
	DECLARE_INLINE_INT_PROP_VARIANT(CoolDownCategory);//冷却分类
	DECLARE_INLINE_INT_PROP_VARIANT(CoolDownTime);//冷却时间
	DECLARE_INLINE_INT_PROP_VARIANT(PublicCDTime);//公共冷却
	DECLARE_INLINE_INT_PROP_VARIANT(SelectType);//技能释放选择类型
	DECLARE_INLINE_INT_PROP_VARIANT(DamageType);// 技能伤害类型
	DECLARE_INLINE_INT_PROP_VARIANT(ConsumeMP);// 技能耗蓝
	DECLARE_INLINE_STRING_PROP_VARIANT(SkillFormula);// 技能公式
	DECLARE_INLINE_BOOL_PROP_VARIANT(IsGrowProperty);// 是否可升级技能
	DECLARE_INLINE_FLOAT_PROP_VARIANT(RangeDistMax);//┏射程范围-最大有效距离
	DECLARE_INLINE_FLOAT_PROP_VARIANT(RangeDistMin);//┣射程范围-最小有效距离
	DECLARE_INLINE_FLOAT_PROP_VARIANT(DamageRate);//伤害比例
	DECLARE_INLINE_BOOL_PROP_VARIANT(IsHitSelf);//是否锁定自身
	DECLARE_INLINE_CVARLIST_PROP_VARIANT(HitTargetRelationType);//┏可锁定的对象关系 (EHitTargetRelationType)
	DECLARE_INLINE_CVARLIST_PROP_VARIANT(HitTargetType);//┗可锁定的对象类型
	DECLARE_INLINE_CVARLIST_PROP_VARIANT(HitRangeID);//┏技能作用范围
	DECLARE_INLINE_INT_PROP_VARIANT(HitCount);//┣范围内最大波及人数
	DECLARE_INLINE_BOOL_PROP_VARIANT(IncludeDead);//┣是否包含死亡对象
	DECLARE_INLINE_BOOL_PROP_VARIANT(IsByHP);//是否按血量百分比排序
	DECLARE_INLINE_BOOL_PROP_VARIANT(IsByDist);//┗是否优先选择距离最近的对象
	DECLARE_INLINE_BOOL_PROP_VARIANT(IsPrepare);//是否需要吟唱
	DECLARE_INLINE_INT_PROP_VARIANT(PrepareTime);//┣吟唱-持续时间ms
	DECLARE_INLINE_BOOL_PROP_VARIANT(CantBreakPrepareAction);//┣吟唱-动作能否打断

	DECLARE_INLINE_BOOL_PROP_VARIANT(IsLead);//是否需要引导
	DECLARE_INLINE_INT_PROP_VARIANT(LeadTime);//┣引导-持续时间ms
	DECLARE_INLINE_INT_PROP_VARIANT(LeadSepTime);//┣引导-间隔时间ms
	DECLARE_INLINE_BOOL_PROP_VARIANT(CantBreakLeadAction);//┣引导动作能否打断
	DECLARE_INLINE_CVARLIST_PROP_VARIANT(Skill_Effect);//技能高级效果包ID		
	DECLARE_INLINE_INT_PROP_VARIANT(HitTime);//命中时间
	DECLARE_INLINE_INT_PROP_VARIANT(CastStiffTime);// 使用技能硬直时间(在此时间内对象不能执行其他AI行为,只对npc或离线玩家有效)
	DECLARE_INLINE_INT_PROP_VARIANT(HateValue);//仇恨基础值
	DECLARE_INLINE_FLOAT_PROP_VARIANT(HateRate);//仇恨系数
	DECLARE_INLINE_CVARLIST_PROP_VARIANT(ExtraHitTimes);//多次命中时间
	DECLARE_INLINE_CVARLIST_PROP_VARIANT(ExtraHitDamage);//多次命中伤害比例
	DECLARE_INLINE_INT_PROP_VARIANT(DisplaceRange);//施法者位移技能范围保护参数
	DECLARE_INLINE_INT_PROP_VARIANT(DisplaceType);//施法者位移技能类型
	DECLARE_INLINE_FLOAT_PROP_VARIANT(DisplaceSpeed);//施法者位移速度
	DECLARE_INLINE_INT_PROP_VARIANT(TargetDisplaceRange);//目标位移技能范围保护参数
	DECLARE_INLINE_INT_PROP_VARIANT(TargetDisplaceType);//目标位移技能类型
	DECLARE_INLINE_INT_PROP_VARIANT(ValidDistance);//验证距离
	DECLARE_INLINE_INT_PROP_VARIANT(ValidTime);//验证时间
	DECLARE_INLINE_BOOL_PROP_VARIANT(IsClientHit);//是否有客户端主导击中
	DECLARE_INLINE_CVARLIST_PROP_VARIANT(PassiveSkill);//影响技能的被动技能
	DECLARE_INLINE_BOOL_PROP_VARIANT(IsUnSilent);//是否对沉默无效的技能
public:
	void SetPropData(CXmlFile &xml, const char* szSectionName, StringBuffer& stringBuffer)
	{
		_STRING_BUFFER_INIT_;

		SetID(szSectionName);
		SetCoolDownCategory(xml.ReadInteger(szSectionName,"CoolDownCategory",0));
		SetCoolDownTime(xml.ReadInteger(szSectionName,"CoolDownTime",0));
		SetPublicCDTime(xml.ReadInteger(szSectionName,"PublicCDTime",0));
		SetSelectType(xml.ReadInteger(szSectionName,"SelectType",0));
		SetDamageType(xml.ReadInteger(szSectionName,"DamgeType",0));
		SetConsumeMP(xml.ReadInteger(szSectionName, "ConsumeMP", 0));
		SetSkillFormula(xml.ReadString(szSectionName, "SkillFormula", ""));
		SetIsGrowProperty(xml.ReadInteger(szSectionName,"IsGrowProperty",0));
		SetRangeDistMax(xml.ReadFloat(szSectionName,"RangeDistMax",1.0f));
		SetRangeDistMin(xml.ReadFloat(szSectionName,"RangeDistMin",0.0f));
		SetDamageRate(xml.ReadFloat(szSectionName,"DamageRate",1.0f));
		SetIsHitSelf(xml.ReadInteger(szSectionName,"IsHitSelf",0));
		SetHitTargetRelationType(xml.ReadString(szSectionName,"HitTargetRelationType",""),",");
		SetHitTargetType(xml.ReadString(szSectionName,"HitTargetType",""),",");
		SetHitRangeID(xml.ReadString(szSectionName,"HitRangeID",""), ",");
		SetHitCount(xml.ReadInteger(szSectionName,"HitCount",0));
		SetIncludeDead(xml.ReadInteger(szSectionName,"IncludeDead",0));
		SetIsByHP(xml.ReadInteger(szSectionName,"IsByHP",0));
		SetIsByDist(xml.ReadInteger(szSectionName,"IsByDist",0));
		SetIsPrepare(xml.ReadInteger(szSectionName,"IsPrepare",0));
		SetPrepareTime(xml.ReadInteger(szSectionName,"PrepareTime",0));
		SetCantBreakPrepareAction(xml.ReadInteger(szSectionName,"CantBreakPrepareAction",0));
		SetIsLead(xml.ReadInteger(szSectionName,"IsLead",0));
		SetLeadTime(xml.ReadInteger(szSectionName,"LeadTime",0));
		SetLeadSepTime(xml.ReadInteger(szSectionName,"LeadSepTime",0));
		SetCantBreakLeadAction(xml.ReadInteger(szSectionName,"CantBreakLeadAction",0));
		SetSkill_Effect(xml.ReadString(szSectionName,"Skill_Effect",""), ",");
		SetHitTime(xml.ReadInteger(szSectionName,"HitTime",0));
		SetHateValue(xml.ReadInteger(szSectionName, "HateValue", 0));
		SetHateRate(xml.ReadFloat(szSectionName, "HateRate", 0.0f));
		SetCastStiffTime(xml.ReadInteger(szSectionName,"CastStiffTime",0));
		SetExtraHitTimes(xml.ReadString(szSectionName,"ExtraHitTimes",""),",");
		SetExtraHitDamage(xml.ReadString(szSectionName,"ExtraHitDamage",""),",");
		SetDisplaceRange(xml.ReadInteger(szSectionName,"DisplaceRange",0));
		SetDisplaceType(xml.ReadInteger(szSectionName,"DisplaceType",0));
		SetDisplaceSpeed(xml.ReadFloat(szSectionName,"DisplaceSpeed",15.0f));
		SetTargetDisplaceRange(xml.ReadInteger(szSectionName,"TargetDisplaceRange",0));
		SetTargetDisplaceType(xml.ReadInteger(szSectionName,"TargetDisplaceType",0));
		SetValidDistance(xml.ReadInteger(szSectionName,"ValidDistance",0));
		SetValidTime(xml.ReadInteger(szSectionName,"ValidTime",0));
		SetIsClientHit(xml.ReadInteger(szSectionName,"IsClientHit",0));
		SetPassiveSkill(xml.ReadString(szSectionName,"PassiveSkill",""), ",");
		SetIsUnSilent(xml.ReadInteger(szSectionName,"IsUnSilent",0));
	}
}; // end of struct SkillMainData

struct SkillHitRangeData 
{
	_STRING_BUFFER_DEFINED_;

	DECLARE_INLINE_STRING_PROP_VARIANT(ID);//技能作用范围ID
	DECLARE_INLINE_INT_PROP_VARIANT(HitRangeType);//技能作用范围类型
public:
	const SkillHitRangeParams &GetHitRangeParams() const {return skillHitRangeParams;}
	void SetPropData(const CVarList& Params,int iHitRangeType)
	{
		skillHitRangeParams.eHitRangeType = (ESkillHitRangeType)iHitRangeType;
		SetHitRangeType(iHitRangeType);
		switch (iHitRangeType)
		{
		case SKILL_HIT_RANGE_TYPE_SPHERE:
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistCentre = StringUtil::ParseStrAsFloat(Params.StringVal(0), 0.0f);    // 球形范围-施法者距球形中心距离
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistMax = StringUtil::ParseStrAsFloat(Params.StringVal(1), 0.0f);       // 球形范围-最大有效距离
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereDistMin = StringUtil::ParseStrAsFloat(Params.StringVal(2), 0.0f);       // 球形范围-最小有效距离
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereTurnAngle = StringUtil::ParseStrAsFloat(Params.StringVal(3), 0.0f);     // 球形范围-面向有效转角
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereUpAngle = StringUtil::ParseStrAsFloat(Params.StringVal(4), 0.0f);       // 球形范围-面向有效仰角
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereOffsetOrient = StringUtil::ParseStrAsFloat(Params.StringVal(5), 0.0f);  // 球形范围-顺时针转角
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereBasepointOffsetOrient = StringUtil::ParseStrAsFloat(Params.StringVal(6), 0.0f);  // 球形范围-基点顺时针转角

			// 角度制转换成弧度制
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereTurnAngle = skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereTurnAngle * PI / 180.0f;
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereUpAngle = skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereUpAngle * PI / 180.0f;
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereOffsetOrient = skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereOffsetOrient * PI / 180.0f;
			skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereBasepointOffsetOrient = skillHitRangeParams.skillhitrangeunion.sphere.fHitRangeSphereBasepointOffsetOrient * PI / 180.0f;

			break;
		case SKILL_HIT_RANGE_TYPE_CYLINDER:
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistCentre = StringUtil::ParseStrAsFloat(Params.StringVal(0), 0.0f);      // 圆柱范围-施法者距圆柱中心线距离
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistMax = StringUtil::ParseStrAsFloat(Params.StringVal(1), 0.0f);         // 圆柱范围-最大有效距离
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderDistMin = StringUtil::ParseStrAsFloat(Params.StringVal(2), 0.0f);         // 圆柱范围-最小有效距离
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderTurnAngle = StringUtil::ParseStrAsFloat(Params.StringVal(3), 0.0f);       // 圆柱范围-面向有效转角
			//hitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderHeight = ::convert_float(configdata.StringVal(4), 0.0f);          // 圆柱范围-圆柱有效高度
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderOffsetOrient = StringUtil::ParseStrAsFloat(Params.StringVal(4), 0.0f);    // 圆柱范围-顺时针转角
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderBasepointOffsetOrient = StringUtil::ParseStrAsFloat(Params.StringVal(5), 0.0f);   // 圆柱范围-基点顺时针转角

			// 角度制转换成弧度制
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderTurnAngle = skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderTurnAngle * PI / 180.0f;
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderOffsetOrient = skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderOffsetOrient * PI / 180.0f;
			skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderBasepointOffsetOrient = skillHitRangeParams.skillhitrangeunion.cylinder.fHitRangeCylinderBasepointOffsetOrient * PI / 180.0f;

			break;
		case SKILL_HIT_RANGE_TYPE_RECTANGULAR:
			skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularDistCentre = StringUtil::ParseStrAsFloat(Params.StringVal(0), 0.0f);   // 矩形范围-施法者距矩形中心距离
			skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularLength = StringUtil::ParseStrAsFloat(Params.StringVal(1), 0.0f);       // 矩形范围-有效长度
			skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularWidth = StringUtil::ParseStrAsFloat(Params.StringVal(2), 0.0f);        // 矩形范围-有效宽度
			//hitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularHeight = ::convert_float(configdata.StringVal(3), 0.0f);       // 矩形范围-有效高度
			skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularOffsetOrient = StringUtil::ParseStrAsFloat(Params.StringVal(3), 0.0f); // 矩形范围-顺时针转角
			skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularBasepointOffsetOrient = StringUtil::ParseStrAsFloat(Params.StringVal(4), 0.0f); // 矩形范围-基点顺时针转角

			// 角度制转换成弧度制
			skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularOffsetOrient = skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularOffsetOrient * PI / 180.0f;
			skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularBasepointOffsetOrient = skillHitRangeParams.skillhitrangeunion.rectangular.fHitRangeRectangularBasepointOffsetOrient * PI / 180.0f;

			break;
		case SKILL_HIT_RANGE_TYPE_POS_CYLINDER:
			skillHitRangeParams.skillhitrangeunion.poscylinder.fHitRangeRadius = StringUtil::ParseStrAsFloat(Params.StringVal(0), 0.0f);   // 以某个位置为中心圆的半径
			break;
		}
	}
	//
	void SetPropData(CXmlFile &xml, const char* szSectionName, StringBuffer& stringBuffer)
	{
		_STRING_BUFFER_INIT_;

		SetID(szSectionName);
		int iHitRangeType = xml.ReadInteger(szSectionName, "HitRangeType",0);
		CVarList Params;	//参数列表
		Params.Clear();
		switch (iHitRangeType){
		case SKILL_HIT_RANGE_TYPE_SPHERE:
			util_split_string(Params, xml.ReadString(szSectionName, "HitRangeBall",""), ",");
			break;
		case SKILL_HIT_RANGE_TYPE_CYLINDER:
			util_split_string(Params, xml.ReadString(szSectionName, "HitRangeCylinder",""),",");
			break;
		case SKILL_HIT_RANGE_TYPE_RECTANGULAR:
			util_split_string(Params, xml.ReadString(szSectionName, "HitRangeRectangular",""), ",");
			break;
		case SKILL_HIT_RANGE_TYPE_POS_CYLINDER:
			util_split_string(Params, xml.ReadString(szSectionName, "HitRangePosCylinder",""), ",");
			break;
		}

		SetPropData(Params,iHitRangeType);
	}
private:	
	SkillHitRangeParams skillHitRangeParams;
	//CVarList Params;	//参数列表

}; // end of struct SkillHitRangeData

struct SkillEffectData 
{
	_STRING_BUFFER_DEFINED_;

	DECLARE_INLINE_STRING_PROP_VARIANT(ID);//效果编号
	DECLARE_INLINE_INT_PROP_VARIANT(EventType);//事件号 ESkillEventType
	DECLARE_INLINE_INT_PROP_VARIANT(TargetType);//强制目标转换 ETargetType
	DECLARE_INLINE_INT_PROP_VARIANT(FuncNo);//处理功能序号
public:
	inline const StringArray& GetParams() const {return Params;}

	void SetPropData(CXmlFile &xml,const char* szSectionName, const std::map<int,std::vector<std::string> > &eventParamsMap, StringBuffer& stringBuffer)
	{
		_STRING_BUFFER_INIT_;

		SetID(szSectionName);
		SetEventType(xml.ReadInteger(szSectionName,"EventType",0));
		SetTargetType(xml.ReadInteger(szSectionName,"TargetType",0));
		SetFuncNo(xml.ReadInteger(szSectionName,"FuncNo",0));

		Params.Init(4, 4, &stringBuffer);
		Params << szSectionName;
		std::map<int,std::vector<std::string> >::const_iterator it = eventParamsMap.find(iFuncNo);
		if (it != eventParamsMap.end())
		{
			const std::vector<std::string> &propname = (*it).second;
			LoopBeginCheck(a);
			for (size_t j = 0; j < propname.size(); ++j)
			{
				LoopDoCheck(a);
				Params<<xml.ReadString(szSectionName,propname[j].c_str(),"");
			}
		}
	}

	void SetPropData(IKernel* pKernel,const PERSISTID& skill) const
	{
	}

private:
	StringArray Params;	//参数列表
}; // end of struct SkillEffectData

//////////////////////////////////////////////////////////////////////////
/// buffer data struct defined
struct BuffBaseData
{
	_STRING_BUFFER_DEFINED_;

	DECLARE_INLINE_STRING_PROP_VARIANT(ID);//Buff编号
	DECLARE_INLINE_INT_PROP_VARIANT(LifeTimeType); //BUFF持续时间类别
	DECLARE_INLINE_INT_PROP_VARIANT(EffectType); //作用类型(BUFFER_EFFECT_TYPE)
	DECLARE_INLINE_INT_PROP_VARIANT(EffectTimer); //作用时间
	DECLARE_INLINE_INT_PROP_VARIANT(AffectType); // buff作用类型EBufferAffectType
	DECLARE_INLINE_INT_PROP_VARIANT(ReplaceCategory); //替换分类号
	DECLARE_INLINE_INT_PROP_VARIANT(ReplaceType); //替换类别
	DECLARE_INLINE_INT_PROP_VARIANT(OffLineClear); //下线是否清除
	DECLARE_INLINE_INT_PROP_VARIANT(ChangeSceneClear); //跨场景是否清除
	DECLARE_INLINE_INT_PROP_VARIANT(SkillBeginClear); //技能开始是否清除
	DECLARE_INLINE_INT_PROP_VARIANT(DeadClear); //死亡是否清除
	DECLARE_INLINE_STRING_PROP_VARIANT(BuffDisplay); //特效文件编号
	DECLARE_INLINE_CVARLIST_PROP_VARIANT(BuffEffect); //特殊效果
	DECLARE_INLINE_STRING_PROP_VARIANT(BasePackage); //基础属性包
	DECLARE_INLINE_INT_PROP_VARIANT(CalculateType); //计算类型(EModifyPackComputeType)
	DECLARE_INLINE_BOOL_PROP_VARIANT(IsModifyProp); //是否要修改对象属性
public:
	void SetPropData(CXmlFile &xml, const char* szSectionName, StringBuffer& stringBuffer)
	{
		_STRING_BUFFER_INIT_;

		SetID(szSectionName);
		SetLifeTimeType(xml.ReadInteger(szSectionName, "LifeTimeType", 0));
		SetEffectType(xml.ReadInteger(szSectionName, "EffectType", 0));
		SetEffectTimer(xml.ReadInteger(szSectionName, "EffectTimer", 0));
		SetAffectType(xml.ReadInteger(szSectionName, "AffectType", 0));
		SetReplaceCategory(xml.ReadInteger(szSectionName, "ReplaceCategory", 0));
		SetReplaceType(xml.ReadInteger(szSectionName, "ReplaceType", 0));
		SetOffLineClear(xml.ReadInteger(szSectionName, "OffLineClear", 0));
		SetChangeSceneClear(xml.ReadInteger(szSectionName, "ChangeSceneClear", 0));
		SetSkillBeginClear(xml.ReadInteger(szSectionName, "SkillBeginClear", 0));
		SetDeadClear(xml.ReadInteger(szSectionName, "DeadClear", 0));
		SetBuffDisplay(xml.ReadString(szSectionName, "BuffDisplay", ""));
		SetBuffEffect(xml.ReadString(szSectionName,"BuffEffect",""), ",");
		SetBasePackage(xml.ReadString(szSectionName, "BasePackage", ""));
		SetCalculateType(xml.ReadInteger(szSectionName, "CalculateType", 0));
		SetIsModifyProp(xml.ReadInteger(szSectionName, "IsModifyProp", 1));
	}
};	// end of struct BuffBaseData

struct BuffEffectData 
{
	_STRING_BUFFER_DEFINED_;

	DECLARE_INLINE_STRING_PROP_VARIANT(ID);//效果编号
	DECLARE_INLINE_INT_PROP_VARIANT(EventType); // EBufferEventType
	DECLARE_INLINE_INT_PROP_VARIANT(FuncNo); //处理事件功能号
public:
	inline const StringArray &GetParams() const {return Params;}
	void SetPropData(CXmlFile &xml,const char* szSectionName, const std::map<int,std::vector<std::string> > &eventParamsMap, StringBuffer& stringBuffer)
	{
		_STRING_BUFFER_INIT_;

		SetID(szSectionName);
		SetEventType(xml.ReadInteger(szSectionName, "EventType", 0));
		SetFuncNo(xml.ReadInteger(szSectionName, "FuncNo", 0));
		Params.Init(4, 4, &stringBuffer);
		Params << szSectionName;
		std::map<int,std::vector<std::string> >::const_iterator it = eventParamsMap.find(iFuncNo);
		if (it != eventParamsMap.end())
		{
			const std::vector<std::string> &propname = (*it).second;
			LoopBeginCheck(a);
			for (size_t j = 0; j < propname.size(); ++j)
			{
				LoopDoCheck(a);
				Params<<xml.ReadString(szSectionName,propname[j].c_str(),"");
			}
		}
	}
private:
	StringArray Params;	//参数列表
}; // end of struct BuffEffectData
#endif
