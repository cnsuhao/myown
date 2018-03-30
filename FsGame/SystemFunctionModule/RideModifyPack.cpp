//--------------------------------------------------------------------
// 文件名:		RideModifyPack.cpp
// 内  容:		坐骑修正包处理函数
// 说  明:		
// 创建日期:	2014年12月32日
// 创建人:		
// 修改人:		 tongzt
//--------------------------------------------------------------------

#include "RideModule.h"
#include "FsGame/CommonModule/PropRefreshModule.h"
#include "FsGame/Define/GameDefine.h"

#include "utils/string_util.h"
#include "Define/CommandDefine.h"
#include "Define/BattleAbilityDefine.h"
#include "StaticDataQueryModule.h"
#include "Define/StaticDataDefine.h"

/*!
* @brief	更新坐骑修正包
*/
void RideModule::UpdateRideAddPkg(IKernel *pKernel, const PERSISTID &self, 
	const PERSISTID& ride, const char *pkg_id, const IVarList& up_msg)
{
	if (!pKernel->Exists(self) 
		|| !pKernel->Exists(ride))
	{
		return;
	}

	if (up_msg.GetCount() < 2)
	{
		return;
	}

	// 修正包更新信息
	const UpdatePkgTypes up_pkg_type = UpdatePkgTypes(up_msg.IntVal(0));	// 更新修正包方式
	const ERefreshDataType refresh_type = ERefreshDataType(up_msg.IntVal(1));	// 属性刷新类型

	CVarList prop_names;
	CVarList prop_values;
	bool ret = StaticDataQueryModule::m_pInstance->GetOneRowData(STATIC_DATA_RIDE_ATTRIBUTE_PACK, 
		pkg_id, prop_names, prop_values, VTYPE_FLOAT);
	if (!ret) 
	{
		return;
	}

	// 属性-值
	MapPropValue map_prop_value;

	// 取得修正包属性-值
	if (!GetPckMapPropValue(pKernel, self, up_pkg_type, prop_names, prop_values, map_prop_value))
	{
		return;
	}

	// 属性更新
	PropRefreshModule::m_pInstance->RefreshData(pKernel, self, map_prop_value, refresh_type);

	// 战斗力更新
	pKernel->Command(self, self, CVarList() << COMMAND_MSG_UPDATE_BATTLE_ABILITY << BA_PROPERTY_TYPE);
}

/*!
* @brief	为升级操作更新修正包
*/
void RideModule::Upgrade2UpdateAddPkg(IKernel *pKernel, const PERSISTID &self, 
									  const PERSISTID& ride)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	IGameObj *pRideObj = pKernel->GetGameObj(ride);
	if (NULL == pRideObj)
	{
		return;
	}

	// 激活状态及时更新属性
	if (pRideObj->QueryInt(FIELD_PROP_RIDE_ACTIVED) == RideState_Actived)
	{
		CVarList up_rm_msg;
		CVarList up_add_msg;

		// 移除
		up_rm_msg << GetRideUpPckType(pKernel, self, ride)
					<< EREFRESH_DATA_TYPE_REMOVE;

		// 增加
		up_add_msg << GetRideUpPckType(pKernel, self, ride)
					<< EREFRESH_DATA_TYPE_ADD;

		// 移除旧的修正包
		std::string pre_pck_id = StringUtil::IntAsString(pRideObj->QueryInt(FIELD_PROP_PRE_RIDE_ADD_PKG));
		UpdateRideAddPkg(pKernel, self, ride, pre_pck_id.c_str(), up_rm_msg);

		// 增加新的修正包
		std::string pck_id = StringUtil::IntAsString(pRideObj->QueryInt(FIELD_PROP_RIDE_ADD_PKG));
		UpdateRideAddPkg(pKernel, self, ride, pck_id.c_str(), up_add_msg);
	}
}

/*!
* @brief	更新使用皮肤修正包
*/
void RideModule::UpdateUseSkinPkg(IKernel* pKernel, const PERSISTID& self, 
	const PERSISTID& ride, const IVarList& up_msg)
{
	if (!pKernel->Exists(self))
	{
		return;
	}

	IGameObj *pRideObj = pKernel->GetGameObj(ride);
	if (NULL == pRideObj)
	{
		return;
	}

	// 皮肤表
	IRecord *pSkinRec = pRideObj->GetRecord(FIELD_RECORD_RIDE_SKIN_REC);
	if (NULL == pSkinRec)
	{
		return;
	}

	const int inuse_row = pSkinRec->FindInt(COLUMN_RIDE_SKIN_REC_STATE, SKIN_USE_STATE_INUSE);
	if (inuse_row < 0)
	{
		return;
	}

	const int skin_id = pSkinRec->QueryInt(inuse_row, COLUMN_RIDE_SKIN_REC_ID);
	const RideSkin *ride_skin = GetRideSkin(skin_id);
	if (NULL == ride_skin)
	{
		return;
	}

	std::string att_pck_id = StringUtil::IntAsString(ride_skin->att_add_pck_id);
	UpdateRideAddPkg(pKernel, self, ride, att_pck_id.c_str(), up_msg);
}

/*!
* @brief	设置属性包
* @param	pKernel
* @param	Player 
* @param	Ride  
* @return	bool
*/
bool RideModule::SetRideAddPckID(IKernel* pKernel, 
							   const PERSISTID& Player, 
							   const PERSISTID& Ride)
{
	// 保护判断
	IGameObj* pRide = pKernel->GetGameObj(Ride);
	if (NULL == pRide)
	{
		return false;
	}

	// 基础修正包
	const int base_pkg = pRide->QueryInt(FIELD_PROP_BASE_ADD_PKG);
	if (0 == base_pkg)
	{
		return false;
	}

	// 当前星级和阶级
	const int ride_star = pRide->QueryInt(FIELD_PROP_RIDE_STAR);
	const int ride_step = pRide->QueryInt(FIELD_PROP_RIDE_STEP);

	// 上一个修正包
	const int pre_add_pkg = pRide->QueryInt(FIELD_PROP_RIDE_ADD_PKG);

	// 坐骑修正包
	const int ride_add_pkg = base_pkg + ride_step*RIDE_BASE_PACKAGE_COEFFICIENT + ride_star;
	
	if (pRide->SetInt(FIELD_PROP_RIDE_ADD_PKG, ride_add_pkg))
	{
		return pRide->SetInt(FIELD_PROP_PRE_RIDE_ADD_PKG, pre_add_pkg);
	}
	
	return false;
}

/*!
* @brief	取得要修正包中要修正的属性
* @param	up_pkg_type 修正包更新方式
* @param	prop_names 属性名列表 
* @param	prop_values 属性值列表
* @param	map_prop_value 最终修正包属性-值
* @return	bool
*/
bool RideModule::GetPckMapPropValue(IKernel *pKernel, const PERSISTID &self, 
									const UpdatePkgTypes up_pkg_type, const IVarList& prop_names, 
									const IVarList& prop_values, std::map<std::string, double>& map_prop_value)
{
	// 更新修正包所有属性
	if (UPDATE_PKG_TYPE_ALL == up_pkg_type)
	{
		LoopBeginCheck(b);
		for (int i = 0; i < (int)prop_names.GetCount(); ++i)
		{
			LoopDoCheck(b);

			PropRefreshModule::m_pInstance->CountRefreshData(pKernel, self, 
				prop_names.StringVal(i), prop_values.DoubleVal(i), EModify_ADD_VALUE, map_prop_value);
		}
	}

	// 更新修正包的speed属性
	if (UPDATE_PKG_TYPE_SPEED == up_pkg_type)
	{
		LoopBeginCheck(c);
		for (int i = 0; i < (int)prop_names.GetCount(); ++i)
		{
			LoopDoCheck(c);
			const char *prop_name = prop_names.StringVal(i);
			float prop_value = prop_values.FloatVal(i);

			// 非0
			if (DoubleEqual(prop_value, 0.0))
			{
				continue;
			}

			// 非speed属性，返回
			if (strcmp(prop_name, "RunSpeedAdd") != 0)
			{
				continue;
			}

			MapPropValue::iterator find_it = map_prop_value.find(prop_name);
			if (find_it != map_prop_value.end())
			{
				//有就累加
				(*find_it).second = (*find_it).second + prop_value;
			}
			else
			{
				//没有就插入
				map_prop_value.insert(std::make_pair(prop_name, prop_value));
			}
		}
	}

	// 更新修正包除speed外的属性
	if (UPDATE_PKG_TYPE_EXCEPT_SPEED == up_pkg_type)
	{
		LoopBeginCheck(d);
		for (int i = 0; i < (int)prop_names.GetCount(); ++i)
		{
			LoopDoCheck(d);
			const char *prop_name = prop_names.StringVal(i);
			double prop_value = prop_values.DoubleVal(i);

			// 非0
			if (DoubleEqual(prop_value, 0.0))
			{
				continue;
			}

			// 不处理speed属性
			if (strcmp(prop_name, "RunSpeedAdd") == 0)
			{
				continue;
			}

			MapPropValue::iterator find_it = map_prop_value.find(prop_name);
			if (find_it != map_prop_value.end())
			{
				//有就累加
				(*find_it).second = (*find_it).second + prop_value;
			}
			else
			{
				//没有就插入
				map_prop_value.insert(std::make_pair(prop_name, prop_value));
			}
		}
	}

	// 无修正属性
	if (map_prop_value.empty())
	{
		return false;
	}

	return true;
}

/*!
* @brief	取得坐骑显示和隐藏时的更新修正包信息
* @param	self   自己
* @param	ride   坐骑
* @param	up_msg	更新信息
* @return	bool
*/
const UpdatePkgTypes RideModule::GetRideUpPckType(IKernel* pKernel, const PERSISTID& self, 
							  const PERSISTID& ride)
{
	if (!pKernel->Exists(self) 
		|| !pKernel->Exists(ride))
	{
		return UPDATE_PKG_TYPE_NONE;
	}

	// 骑
	if (GetRidingState(pKernel, self) == RIDE_UP_DOWN_OPERATE_UP)
	{
		return UPDATE_PKG_TYPE_ALL;
	}
	else	// 下
	{
		return UPDATE_PKG_TYPE_EXCEPT_SPEED;
	}
}