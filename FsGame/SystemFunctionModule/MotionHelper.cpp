//--------------------------------------------------------------------
// 文件名:		MotionHelper.cpp
// 内  容:		motion helper
// 说  明:		
// 创建日期:	2017年06月06日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------

#include "MotionHelper.h"
#include "utils/cache/CacheHelper.hpp"
#include "FsGame\CommonModule\PathFindingModule.h"
#include "utils/geometry_func.h"

// 搜索路径的范围
const int C_MOTION_GET_PATH_RANGE = 20;
// 移动点周围范围值
const int C_MOTION_GRID_AROUND_VALUE = 1;
//寻路使用缓存路径点
const bool C_MOTION_USED_CACHE_PATH = true;

MotionHelper::MotionHelper()
{
	//
}
MotionHelper::~MotionHelper()
{
	//
}
bool MotionHelper::FindPathToTargets_Static(IKernel* pKernel, const PERSISTID& self, const MontionPos& pos_sef, const MontionPos& pos_target, int& dst_point_count, IVarList& dst_path_list)
{
	dst_point_count = 0;
	if (dst_path_list.GetCount()>0)
	{
		dst_path_list.Clear();
	}
	if (pKernel == NULL)
	{
		Assert(0);
		return false;
	}
	IGameObj* pSceneObj = pKernel->GetSceneObj();
	if (pSceneObj == NULL)
	{
		return false;
	}
	const char * cstrSceneRes = pSceneObj->QueryString("Resource");
	if (cstrSceneRes == NULL || strcmp(cstrSceneRes,"") == 0)
	{
		return false;
	}
	if (!C_MOTION_USED_CACHE_PATH)
	{
		Point beginPoint(pos_sef.pos_x_, pos_sef.pos_y_, pos_sef.pos_z_);
		Point endPoint(pos_target.pos_x_, pos_target.pos_y_, pos_target.pos_z_);
		dst_point_count = PathFindingModule::PointFindPath(cstrSceneRes, beginPoint, endPoint, dst_path_list);
		return true;
	}
	std::string scene_type = cstrSceneRes;
	MotionPathMap_t* p_path_list = NULL;
	MotionPathCache& p_cache = MotionHelper::GetMotionPathCache();
	if (!p_cache.path_static_.FindPathMap(scene_type, p_path_list))
	{
		return false;
	}
	MotionPathPos* find_path = NULL;
	int iTotalRow = (int)pKernel->GetPathGridRows();
	if (MotionHelper::FindCachePath(*p_path_list, pos_target.grid_, pos_sef, find_path, iTotalRow))
	{
		dst_point_count = find_path->find_result_;
		if (dst_point_count>0)
		{
			dst_path_list.Concat(find_path->path_);
		}
		return true;
	}
	//查询
	Point beginPoint(pos_sef.pos_x_, pos_sef.pos_y_, pos_sef.pos_z_);
	Point endPoint(pos_target.pos_x_, pos_target.pos_y_, pos_target.pos_z_);
	dst_point_count = PathFindingModule::PointFindPath(cstrSceneRes, beginPoint, endPoint, dst_path_list);
	//写入
	MotionPathPos save_path;
	save_path.SetPath(dst_point_count, pos_sef, dst_path_list);
	MotionHelper::SaveCachePath(*p_path_list, pos_target.grid_, save_path);
	return true;
}
bool MotionHelper::FindPathToTargets_Dynamic(IKernel* pKernel, const PERSISTID& self, const MontionPos& pos_sef, const MontionPos& pos_target, IVarList& dst_path_list)
{
	if (dst_path_list.GetCount() > 0)
	{
		dst_path_list.Clear();
	}
	if (pKernel == NULL)
	{
		Assert(0);
		return false;
	}
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}
	int groupId = pSelfObj->QueryInt("GroupID");
	if (!C_MOTION_USED_CACHE_PATH)
	{
		pKernel->GetPath(pos_sef.grid_, pos_target.grid_, groupId, dst_path_list, C_MOTION_GET_PATH_RANGE);
		return true;
	}
	MotionPathMap_t* p_path_list = NULL;
	MotionPathCache& p_cache = MotionHelper::GetMotionPathCache();
	if (!p_cache.path_dynamic_.FindPathMap(groupId, p_path_list))
	{
		return false;
	}
	MotionPathPos* find_path = NULL;
	int iTotalRow = (int)pKernel->GetPathGridRows();
	if (MotionHelper::FindCachePath(*p_path_list, pos_target.grid_, pos_sef, find_path, iTotalRow))
	{
		if (find_path->find_result_ > 0)
		{
			dst_path_list.Concat(find_path->path_);
		}
		return true;
	}
	//查询
	pKernel->GetPath(pos_sef.grid_, pos_target.grid_, groupId, dst_path_list, C_MOTION_GET_PATH_RANGE);
	//写入
	MotionPathPos save_path;
	save_path.SetPath((int)dst_path_list.GetCount(), pos_sef, dst_path_list);
	MotionHelper::SaveCachePath(*p_path_list, pos_target.grid_, save_path);
	return true;
}
bool MotionHelper::IsGridAround(int cur_grid, int dst_grid, int gridRows, int targetGrid)
{
	if (gridRows<=0)
	{
		gridRows = 1;
	}
	if (cur_grid == dst_grid)
	{
		return true;
	}
	// 当前格子坐标
	int cur_x = (cur_grid - 1) % gridRows + 1;
	int cur_z = (int)((cur_grid - 1) / gridRows);
	// 目标格子坐标
	int dst_x = (dst_grid - 1) % gridRows + 1;
	int dst_z = (int)((dst_grid - 1) / gridRows);

	// 是否在目标点周围
	if (cur_x >= (dst_x - targetGrid)
		&& cur_x <= dst_x + targetGrid
		&& cur_z >= dst_z - targetGrid
		&& cur_z <= dst_z + targetGrid)
	{
		return true;
	}
	return false;
}
void MotionHelper::ParseMotionPos(IKernel* pKernel, MontionPos& dst_pos, int dst_grid)
{
	dst_pos.Clear();
	if (pKernel == NULL)
	{
		Assert(0);
		return;
	}
	dst_pos.grid_ = dst_grid;
	
	pKernel->GetPosByIndex(dst_pos.grid_, dst_pos.pos_x_, dst_pos.pos_z_);
	dst_pos.pos_y_ = pKernel->GetMapHeight(dst_pos.pos_x_, dst_pos.pos_z_);
}
void MotionHelper::ParseMotionPosEx(IKernel* pKernel, MontionPos& dst_pos, float dst_x, float dst_z)
{
	dst_pos.Clear();
	if (pKernel == NULL)
	{
		Assert(0);
		return;
	}
	dst_pos.pos_x_ = dst_x;
	dst_pos.pos_z_ = dst_z;

	dst_pos.pos_y_ = pKernel->GetMapHeight(dst_pos.pos_x_, dst_pos.pos_z_);
	dst_pos.grid_  = pKernel->GetGridIndexByPos(dst_pos.pos_x_, dst_pos.pos_z_);
}
void MotionHelper::ParseMotionPosAll(IKernel* pKernel, MontionPos& dst_pos, float dst_x, float dst_z, float dst_y)
{
	dst_pos.Clear();
	if (pKernel == NULL)
	{
		Assert(0);
		return;
	}
	dst_pos.pos_x_ = dst_x;
	dst_pos.pos_z_ = dst_z;
	dst_pos.pos_y_ = dst_y;

	dst_pos.grid_ = pKernel->GetGridIndexByPos(dst_pos.pos_x_, dst_pos.pos_z_);
}
bool MotionHelper::FindCachePath(MotionPathMap_t& p_path_list, int target_grid, const MontionPos& pos_sef, MotionPathPos*& find_path, int iTotalRow)
{
	if (iTotalRow <= 0)
	{
		Assert(0);
		return false;
	}
	find_path = NULL;
	MotionPathGrid_t* path_grid = NULL;
	if (!MotionHelper::FindPathGrid(p_path_list, target_grid, path_grid))
	{
		return false;
	}
	int n_count = (int)path_grid->size();
	if (n_count<=0)
	{
		return false;
	}
	for (int index = 0; index < n_count;index++)
	{
		MotionPathPos& p_dst = path_grid->at(index);
		if (MotionHelper::IsGridAround(pos_sef.grid_, p_dst.pos_.grid_, iTotalRow, C_MOTION_GRID_AROUND_VALUE))
		{
			find_path = &p_dst;
			return true;
		}
	}
	return false;
}
bool MotionHelper::SaveCachePath(MotionPathMap_t& p_path_list, int target_grid, const MotionPathPos& find_path)
{
	MotionPathGrid_t* path_grid = NULL;
	if (!MotionHelper::FindPathGrid(p_path_list, target_grid, path_grid))
	{
		return false;
	}
	path_grid->push_back(find_path);
	return true;
}
bool MotionHelper::FindPathGrid(MotionPathMap_t& p_path_list, int target_grid, MotionPathGrid_t*& dst_path_grid)
{
	MotionPathMap_t::iterator it = p_path_list.find(target_grid);
	if (it != p_path_list.end())
	{
		dst_path_grid = &it->second;
	}
	else
	{
		MotionPathGrid_t t_path;
		p_path_list.insert(std::make_pair(target_grid, t_path));
		MotionPathMap_t::iterator it_tt = p_path_list.find(target_grid);
		if (it_tt == p_path_list.end())
		{
			Assert(0);
			return false;
		}
		dst_path_grid = &it_tt->second;
	}
	return true;
}
MotionPathCache& MotionHelper::GetMotionPathCache()
{
	static MotionPathCache t_path_cache_;
	return t_path_cache_;
}