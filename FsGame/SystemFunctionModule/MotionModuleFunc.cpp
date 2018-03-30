//--------------------------------------------------------------------
// 文件    MotionModuleFunc.cpp
// 说明	   老移动模块，提供外界使用的函数
//			需要被迭代掉
//--------------------------------------------------------------------
#include "MotionModule.h"
#include "FsGame/Define/GameDefine.h"
#include "utils/util_func.h"
#include "FsGame/NpcBaseModule/AI/AIFunction.h"


const int LITTLE_STEP_GRID = 1;

// 寻找目标点附近点扩散范围
const float NEAR_DISTANCE_MAX = 30.0f; 

//************************************
// Method:    GetGridCanStandPoint
// Access:    public static 
// Returns:   int 格子索引
// Qualifier: 查找目标点附近的可使用坐标
// Parameter: IKernel * pKernel 核心类
// Parameter: const PERSISTID & self 自身
// Parameter: float destX	目标点x坐标
// Parameter: float destZ	目标点z坐标
// Parameter: float & nearX 目标点附近可使用x坐标
// Parameter: float & nearZ 目标点附近可使用z坐标
//************************************
int MotionModule::GetEnableGridNearTarget(IKernel* pKernel, const PERSISTID& self, float destX, float destZ, float& nearX, float& nearZ)
{
	
	if (!pKernel->Exists(self))
	{
		return 0;
	}

	int distanceMax = static_cast<int>(NEAR_DISTANCE_MAX);
	int gridIndex = 0;
	// 循环保护
	LoopBeginCheck(eh);
	for (int i = 0; i < distanceMax; ++i)
	{
		LoopDoCheck(eh);
		// 围绕目标一圈一圈查找
		gridIndex = m_pMotionModule->GetNearestEnableGridInAllQuadrant(
			pKernel, self, destX, destZ, 
			static_cast<float>(i), 
			static_cast<float>(i + 1));

		// 找到合适点 跳出
		if (gridIndex > 0)
		{
			break;
		}
	}

	if (gridIndex <= 0)
	{
		return 0;
	}

	// 转换为坐标
	pKernel->GetPosByIndex(gridIndex, nearX, nearZ);
	
	return gridIndex;
}

//方格是否可用
bool MotionModule::GridEnabled(IKernel * pKernel, const int iIndex, int groupId)
{
	if (iIndex <= 0)
	{
		return false;
	}

	//参数
	bool canwalk, logicbalk, objectoccupy;
	pKernel->QueryPathGridStatus( iIndex, canwalk, logicbalk, objectoccupy, groupId);
	//不可行走
	if (!canwalk) 
	{
		return false;
	}

	//存在逻辑占用
	if (logicbalk)
	{
		return false;
	}

	//被对象占用
	if (objectoccupy)
	{
		return false;
	}

	return true;
}

//  方格是否可用 
//  相对于self 是否可用
//  self所占方格数不同，格子的占用情况不同
bool MotionModule::GridEnabled(IKernel * pKernel, const PERSISTID& self, const int iIndex, bool bSpace/* = true*/)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	if (iIndex <= 0)
	{
		return false;
	}

	// 格子转换坐标
	float indexx = 0.0f;
	float indexz = 0.0f;
	pKernel->GetPosByIndex(iIndex, indexx, indexz);

	// 是否可用 返回所占用格子
	CVarList gridList;
	bool ret = pKernel->CanStandPointByPathGrid(self, indexx, indexz, gridList) == MRESULT_CONTINUE;

	// 计算逻辑占格
	if (bSpace && ret)
	{
		ret = IsGridSpace(pKernel, self, gridList);
	}

	return ret;
}

// 相互间的间隔不允许占用
bool MotionModule::IsGridSpace(IKernel* pKernel, const PERSISTID& self, const IVarList& gridList)
{
	IGameObj* pSelfObj = pKernel->GetGameObj(self);
	if (pSelfObj == NULL)
	{
		return false;
	}

	// 所占格子为空
	if (gridList.IsEmpty())
	{
		return true;
	}

	// 间隔格子数
	int spaceGrid = pSelfObj->QueryInt("SpaceGrid");
	if (spaceGrid <= 0)
	{
		return true;
	}

	// 所占格子数（-1为了方便下面计算）
	int pathGrid = pSelfObj->QueryInt("PathGrid") - 1;
	if (pathGrid < 0)
	{
		pathGrid = 0;
	}

	// 所占格子左上角格子索引
	int gridLeftTop = gridList.IntVal(0);
	if (gridLeftTop <= 0)
	{
		return true;
	}

	// 地图总行数
	size_t iTotalRow = pKernel->GetPathGridRows();
	if (iTotalRow == 0)
	{
		return false;
	}

	// 索引转换为坐标（不是地图坐标）
	size_t x = (gridLeftTop - 1) % iTotalRow + 1;
	size_t y = (gridLeftTop - 1) / iTotalRow;

	size_t tempX = x - spaceGrid;
	size_t tempY = y - spaceGrid;
	int tempIndex = 0;

	int groupId = pSelfObj->QueryInt("GroupID");

	// 总遍历次数
	// 总格子数 - 对象占用格子数
	int totalCount = ((pathGrid + 1) + (spaceGrid * 2)) * ((pathGrid + 1) + (spaceGrid * 2)) - ((pathGrid + 1) * (pathGrid + 1));
	// 循环保护
	LoopBeginCheck(ej);
	do 
	{
		LoopDoCheck(ej);
		// 检测该格子是否可用
		tempIndex = int(tempX + tempY * iTotalRow);
		if (!GridEnabled(pKernel, tempIndex, groupId))
		{
			return false;
		}

		// 遍历占用格子周围的所有格子
		// y到末尾
		if (tempY >= y + pathGrid + spaceGrid)
		{
			// x 到末尾 遍历完成
			if (tempX >= x + pathGrid + spaceGrid)
			{
				return true;
			}
			else
			{
				++tempX;
				tempY = y - spaceGrid;
			}
		}
		else
		{
			// 循环保护
			LoopBeginCheck(ei);
			do 
			{
				LoopDoCheck(ei);
				++tempY;
			}
			while (tempX >= x  && tempX <= x + pathGrid 
				&& tempY >= y && tempY <= y + pathGrid);
		}

		--totalCount;

	} while (totalCount >= 0);

	return false;
}