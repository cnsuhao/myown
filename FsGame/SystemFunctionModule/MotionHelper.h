//--------------------------------------------------------------------
// 文件名:		MotionHelper.h
// 内  容:		motion helper
// 说  明:		
// 创建日期:	2017年06月06日
// 创建人:		kevin
// 版权所有:	WhalesGame Technology co.Ltd
//--------------------------------------------------------------------
#ifndef __CACHE_MOTION_HELPER_H__
#define	__CACHE_MOTION_HELPER_H__
#include "Fsgame/Define/header.h"
#include "public/VarList.h"
#include <vector>
#include <map>
#include "utils/cache/CacheString.hpp"

//与kernel保持一致，暂不使用namespace
//注意： Ikernel,IRoomKernel,的区分：由于底层没有抽象，继承。各自独立实现，helper函数要注意。

// 寻路方式
enum MotionMethod
{
	MOTION_METHOD_NORMAL = 1,	// 001普通寻路 
	MOTION_METHOD_STATIC = 2,	// 010静态寻路 
	MOTION_METHOD_DYNAMIC = 4,	// 100动态寻路 
	MOTION_METHOD_NORMAL_STATIC = 3,	// 011普通寻路 + 静态寻路
	MOTION_METHOD_NORMAL_DYNAMIC = 5,	// 101普通寻路 + 动态寻路 
	MOTION_METHOD_ALL = 7,	// 111普通寻路 + 静态寻路 + 动态寻路 
};

/// \brief 移动坐标
struct MontionPos
{
public:
	int  grid_;		//网格
public:	
	float pos_x_;	//坐标x
	float pos_z_;	//坐标z
	float pos_y_;	//坐标y
public:
	MontionPos() :grid_(0), pos_x_(0.0f), pos_z_(0.0f), pos_y_(0.0f){}
public:
	bool IsValid() const
	{
		return true; //边界未知，暂直接返回true
	}
	void Clear()
	{
		grid_ = 0;
		pos_x_ = 0.0f;
		pos_z_ = 0.0f;
		pos_y_ = 0.0f;
	}
public:
	///// \brief 设值
	void Set(const MontionPos& ref)
	{
		grid_  = ref.grid_;
		pos_x_ = ref.pos_x_;
		pos_z_ = ref.pos_z_;
		pos_y_ = ref.pos_y_;
	}
public:
	///// \brief 网格是否相同
	bool IsEqualGrid(const MontionPos& ref) const
	{
		if (grid_ == ref.grid_)
		{
			return true;
		}
		return false;
	}
};
//路径点
struct MotionPathPos 
{
public:
	int		   find_result_;  // struct eFindResult
	MontionPos pos_;
	CVarList   path_;
public:
	MotionPathPos() :find_result_(0){}
	~MotionPathPos(){ Clear(); }
public:
	void Clear()
	{
		find_result_ = 0;
		pos_.Clear();
		if (path_.GetCount()>0)
		{
			path_.Clear();
		}
	}
public:
	void SetPath(int find_result, const MontionPos& dst_pos, const IVarList& dst_path)
	{
		Clear();
		find_result_ = find_result;
		pos_.Set(dst_pos);
		if (find_result>0)
		{
			if (dst_path.GetCount() <= 0)
			{
				find_result = 0;
			}
			else
			{
				path_.Concat(dst_path);
			}
		}
	}
};
//到达某个网格路径
typedef std::vector<MotionPathPos>  MotionPathGrid_t;

//路径点
typedef std::map<int,MotionPathGrid_t> MotionPathMap_t;

//静态动态寻路点
template< typename scene_type >
struct MotionPathMap_Case
{
private:
	typedef std::map<scene_type, MotionPathMap_t> MotionPathMap_Case_t;
private:
	MotionPathMap_Case_t map_;
public:
	MotionPathMap_Case(){}
	~MotionPathMap_Case(){Clear();}
public:
	void Clear()
	{
		if (map_.size() > 0)
		{
			map_.clear();
		}
	}
public:
	bool FindPathMap(const scene_type& key_value,MotionPathMap_t*& p_path_list)
	{
		MotionPathMap_Case_t::iterator it = map_.find(key_value);
		if (it != map_.end())
		{
			p_path_list = &it->second;
		}
		else
		{
			MotionPathMap_t t_path;
			map_.insert(std::make_pair(key_value, t_path));
			MotionPathMap_Case_t::iterator it_tt = map_.find(key_value);
			if (it_tt == map_.end())
			{
				Assert(0);
				return false;
			}
			p_path_list = &it_tt->second;
		}
		return true;
	}
};

//Cache缓存路径点
struct MotionPathCache
{
public:
	MotionPathMap_Case<std::string> path_static_;	//静态路径
	MotionPathMap_Case<int> path_dynamic_;  //动态路径
public:
	MotionPathCache(){}
	~MotionPathCache()
	{
		path_static_.Clear();
		path_static_.Clear();
	}
};


/// \brief 寻路助手
class MotionHelper
{
public:
	MotionHelper();
	~MotionHelper();
public:
	/// \brief 寻路,静态寻路
	/// \brief pKernel         kernel
	/// \brief self		       self
	/// \brief pos_sef         源位置
	/// \brief pos_target	   目标位置
	/// \brief dst_point_count 返回寻路的点数，失败返回0或负数 eFindResult
	/// \brief dst_path_list   路径点列表
	static bool FindPathToTargets_Static(IKernel* pKernel, const PERSISTID& self, const MontionPos& pos_sef, const MontionPos& pos_target, int& dst_point_count, IVarList& dst_path_list);
	/// \brief 寻路,动态寻路
	/// \brief pKernel         kernel
	/// \brief self		       self
	/// \brief pos_sef         源位置
	/// \brief pos_target	   目标位置
	/// \brief dst_path_list   路径点列表
	static bool FindPathToTargets_Dynamic(IKernel* pKernel, const PERSISTID& self, const MontionPos& pos_sef, const MontionPos& pos_target, IVarList& dst_path_list);
public:
	/// \brief 当前格子是否在目标点周围
	/// \param cur_grid 当前格子
	/// \param dst_grid 目标格子
	/// \param gridRows 网格行数
	/// \param targetGrid 目标所占格子上
	/// \param memo     坐标转换使用相对坐标，和kernel提供的方式不同，但是结果相同，效率更高。
	static bool IsGridAround(int cur_grid, int dst_grid, int gridRows,int targetGrid = 1);
public:
	/// \brief 解析移动坐标
	/// \brief pKernel kernel
	/// \brief dst_pos  目的格子
	/// \brief dst_grid/(dst_x,dst_z)  目的格子坐标
	static void ParseMotionPos(IKernel* pKernel, MontionPos& dst_pos, int dst_grid);
	static void ParseMotionPosEx(IKernel* pKernel, MontionPos& dst_pos, float dst_x, float dst_z);
	static void ParseMotionPosAll(IKernel* pKernel, MontionPos& dst_pos, float dst_x, float dst_z,float dst_y);
private:
	/// \brief 查询，存储缓存路径
	static bool FindCachePath(MotionPathMap_t& p_path_list, int target_grid,const MontionPos& pos_sef, MotionPathPos*& find_path, int iTotalRow);
	static bool SaveCachePath(MotionPathMap_t& p_path_list, int target_grid, const MotionPathPos& find_path);
private:
	/// \brief 查询目标路径网格列表
	static bool FindPathGrid(MotionPathMap_t& p_path_list, int target_grid, MotionPathGrid_t*& dst_path_grid);
private:
	//私有变量: 寻路缓存
	static MotionPathCache& GetMotionPathCache();
};

#endif