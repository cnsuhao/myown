
//--------------------------------------------------------------------
// 文件名:      code\mobile\tools\shape_collision.cpp
// 内  容:      图形碰撞检测
// 说  明:      纯数学计算，业务相关逻辑不要加到这里
// 创建日期:    2014年4月3日
// 创建人:      longch
//    :       
//--------------------------------------------------------------------


#include "utils/shape_collision.h"
#include "utils/util_func.h"

namespace ShapeCollision
{
    const float EPSILON = 0.00001f;
    const float MY_PI = 3.1415927f;

    ECollideResult CheckCricleInCricle(const Cricle2D& targetShape, const Cricle2D& rangeShape);
    ECollideResult CheckCricleInSector(const Cricle2D& targetShape, const Sector2D& rangeShape);
    ECollideResult CheckCricleInRectangle(const Cricle2D& targetShape, const Rectangle2D& rangeShape);

    ECollideResult CheckCricleInCricleRing(const Cricle2D& targetShape, const CricleRing2D& rangeShape);
    ECollideResult CheckCricleInSectorRing(const Cricle2D& targetShape, const SectorRing2D& rangeShape);


    // 检查高度差
    ECollideResult CheckHeightDiff(float y, float dy, float fHeightDiff);

    // 类型声明
    struct Rect;
    struct RectStruct;
    struct FmVec3Wrap;

    // 检测线段是否与矩形相交，这是一个正交判断，矩形区域rect是一个与轴平行的
    bool CheckRectLine(const FmVec2& start, const FmVec2& end, const Rect& rect);
    // CheckRectLineH是水平方向上的检测，就是检测矩形的上边线与下边线，CheckRectLineV检测矩形的左边线和右边线。
    bool CheckRectLineH(const FmVec2& start, const FmVec2& end, float y0, float x1, float x2);
    bool CheckRectLineV(const FmVec2& start, const FmVec2& end, float x0, float y1, float y2);

    // 检测两点距离是否在范围内
    bool CheckPointToPointLessDist(const FmVec2& pt1, const FmVec2& pt2, float distance);

    // 判断点是否在矩形范围内
    bool CheckPointInRectangle(const FmVec2& pt, const Rectangle2D& rangeShape);

    // 返回距离线段最近的点的距离
    // ptA,ptB是线段两个端点坐标，pt是目标点，dist返回最近点距离
    // 返回值：-2，距离是与A点距离；-1，距离是点到B点的距离；0：点在线段上；1，返回距离是点到线段的垂直距离
    int GetNearestDistance(const FmVec2& ptA, const FmVec2& ptB, const FmVec2& pt, float& dist);

    //////////////////////////////////////////////////////////////////////////
    /// 以下试验用
    ECollideResult TestCircleInSectorRing(const Cricle2D& targetShape, const SectorRing2D& rangeShape);

    /// 检测目标点是否在扇形区域内
    ECollideResult TestRectangleInSectorRing(const Rectangle2D& targetShape, const SectorRing2D& rangeShape);

    /// \brief 检测矩形目标是否在一个指定的矩形范围内
    ECollideResult TestRectangleInRectangle(const Rectangle2D& targetShape, const Rectangle2D& rangeShape);

    //private:
    /// \brief  检测3维空间中的两个长方体是否碰撞
    /// \param [in] baseRect 长方体结构
    /// \param [in] otherRect 另一个长方体结构
    /// \return 返回检测结果 ps：这个函数算法是某本书借鉴的
    bool CheckRectToRect(RectStruct const& a, RectStruct const& b);

    // 用于判断扇形角度是否符合要求
    bool CheckFanToPoint(FmVec2 const& basePos, FmVec2 const& targetPos, float orien, float anglet);
}; // end of namespace ShapeCollision



// 3维点
struct ShapeCollision::FmVec3Wrap : public FmVec3
{
public:
    FmVec3Wrap(void) 
    {
    }

    FmVec3Wrap(const VERTTYPE& init_x, const VERTTYPE& init_y, const VERTTYPE& init_z) 
        : FmVec3(init_x, init_y, init_z)
    {
    }

    FmVec3Wrap(const FmVec3Wrap& point)
        : FmVec3(point.x, point.y, point.z)
    {
    }

    operator VERTTYPE* ()
    {
        return (VERTTYPE*) &x;
    }

    operator const VERTTYPE* () const
    {
        return (const VERTTYPE*) &x;
    }

    FmVec3Wrap operator - (const FmVec3Wrap& v) const
    {
        return FmVec3Wrap(x - v.x, y - v.y, z - v.z);
    }

    ~FmVec3Wrap()
    {
    }
};


// 矩形描述结构体
struct ShapeCollision::RectStruct
{
    //float x;    // x，z为一个点坐标，可以是矩形的起点，也可以是矩形的顶点中一个，或者是中点
    //float z;
    //float orient;   // 角度，width是垂直于orient的方向，height是平行于orient的方向
    //float width;    // 宽
    //float height;

    FmVec3Wrap c; // OBB center point
    FmVec3Wrap u[3]; // Local x-, y-, and z-axes
    FmVec3Wrap e; // Positive halfwidth extents of OBB along each axis
};

// Region R = { x | x = c+r*u[0]+s*u[1]+t*u[2] }, |r|<=e[0], |s|<=e[1], |t|<=e[2]
//struct OBB {
//    D3DXVECTOR3 c; // OBB center point
//    D3DXVECTOR3 u[3]; // Local x-, y-, and z-axes
//    D3DXVECTOR3 e; // Positive halfwidth extents of OBB along each axis
//};


struct ShapeCollision::Rect
{
    FmVec2 leftTop;
    FmVec2 rightBottom;

public:
    Rect(float x1, float y1, float x2, float y2)
    {
        if (x1 < x2)
        {
            leftTop.x = x1;
            rightBottom.x = x2;
        }
        else
        {
            leftTop.x = x2;
            rightBottom.x = x1;
        }

        if (y1 < y2)
        {
            leftTop.y = y1;
            rightBottom.y = y2;
        }
        else
        {
            leftTop.y = y2;
            rightBottom.y = y1;
        }
    }
    bool Contains(const FmVec2& pt) const
    {
        bool result = true;
        result = result && leftTop.x <= pt.x;
        result = result && leftTop.y <= pt.y;
        result = result && pt.x <= rightBottom.x;
        result = result && pt.y <= rightBottom.y;
        return result;
    }
};



ECollideResult ShapeCollision::CheckCricleInCricle(const Cricle2D& targetShape, const Cricle2D& rangeShape)
{
    if (!CheckPointToPointLessDist(targetShape.pos, rangeShape.pos, targetShape.radius + rangeShape.radius))
    {
        return ECOLLIDE_RESULT_ERR_TOOFAR;
    }
    return ECOLLIDE_RESULT_INTERSECT;
}

ECollideResult ShapeCollision::CheckCricleInSector(const Cricle2D& targetShape, const Sector2D& rangeShape)
{
    const FmVec2 dist = targetShape.pos - rangeShape.pos;
    float fDistance = dist.length();

    float fHalfAngle = rangeShape.angle * 0.5f;

    //首先判断角度
    FmVec2 vec2LineOrient = dist;
    //vec2LineOrient.normalize(); // 图形中心点连线方向，指向目标形状
    FmVec2Normalize(&vec2LineOrient, &vec2LineOrient);// 图形中心点连线方向，指向目标形状
    FmVec2 rangeShapeOrient;// = rangeShape.orient.normalized();
    FmVec2Normalize(&rangeShapeOrient, &(rangeShape.orient));
    float fDeltaOrient = FMACOS(FmVec2Dot(&vec2LineOrient, &rangeShapeOrient));
    if (fDeltaOrient > fHalfAngle)
    { // 角度在扇形外
        if (targetShape.radius < 0.001f)
        { // 不在水平角度范围内
            return  ECOLLIDE_RESULT_ERR_HORI;
        }

        //////////////////////////////////////////////////////////////////////////
        float fBeltaAngle = fDeltaOrient - fHalfAngle; // 目标点方向扇形边的差角
        float d = fDistance * sin(fBeltaAngle);  // 目标点到扇形边的距离
        if (d > targetShape.radius)
        {
            return ECOLLIDE_RESULT_ERR_HORI;
        }
    }

    // 判断距离
    if (fDistance > rangeShape.radius + targetShape.radius)
    {
        return ECOLLIDE_RESULT_ERR_TOOFAR;
    }
    return ECOLLIDE_RESULT_INTERSECT;
}

ECollideResult ShapeCollision::CheckCricleInRectangle(const Cricle2D& targetShape, const Rectangle2D& rangeShape)
{
    // 矩形的外接圆半径
    float fOuterRadius = (rangeShape.height + rangeShape.width) * 0.5f;
    // 两个对象的外接圆无碰撞，直接返回
    if (ECOLLIDE_RESULT_INTERSECT != CheckCricleInCricle(targetShape, Cricle2D(rangeShape.pos, fOuterRadius)))
    {
        return ECOLLIDE_RESULT_ERR_TOOFAR;
    }

    // 目标中心在矩形范围内，则相交
    if (CheckPointInRectangle(targetShape.pos, rangeShape))
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }

    float fHalfWidth = rangeShape.width * 0.5f;
    float fHalfHeight = rangeShape.height * 0.5f;
    FmVec2 vorient = rangeShape.orient.rotated90();
    // 偏移向量
    FmVec2 hVec2 = rangeShape.orient * fHalfHeight;
    FmVec2 wVec2 = vorient * fHalfWidth;

    // 矩形四个顶点
    FmVec2 A = rangeShape.pos - hVec2 - wVec2;
    FmVec2 B = rangeShape.pos + hVec2 - wVec2;
    FmVec2 C = rangeShape.pos + hVec2 + wVec2;
    FmVec2 D = rangeShape.pos - hVec2 + wVec2;
    //extend_warning(LOG_TRACE, "[ShapeCollision::CheckCricleInRectangle] A:(%f,%f) B:(%f,%f) C:(%f,%f) D:(%f,%f)", A.x, A.y, B.x, B.y, C.x, C.y, D.x, D.y);
    //extend_warning(LOG_TRACE, "[ShapeCollision::CheckCricleInRectangle] 矩形的半长宽w=%f len:%f", fHalfWidth, fHalfHeight);

    // 判断目标中心到四条边的距离
    float dist = 0.0f;
    int ret = GetNearestDistance(A, B, targetShape.pos, dist);
    //extend_warning(LOG_TRACE, "[ShapeCollision::CheckCricleInRectangle] 距线段AB的最近点距离dist:%f", dist);
    if (dist < targetShape.radius)
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }
    ret = GetNearestDistance(B, C, targetShape.pos, dist);
    //extend_warning(LOG_TRACE, "[ShapeCollision::CheckCricleInRectangle] 距线段BC的最近点距离dist:%f", dist);
    if (dist < targetShape.radius)
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }
    ret = GetNearestDistance(C, D, targetShape.pos, dist);
    //extend_warning(LOG_TRACE, "[ShapeCollision::CheckCricleInRectangle] 距线段CD的最近点距离dist:%f", dist);
    if (dist < targetShape.radius)
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }
    ret = GetNearestDistance(D, A, targetShape.pos, dist);
    //extend_warning(LOG_TRACE, "[ShapeCollision::CheckCricleInRectangle] 距线段DA的最近点距离dist:%f", dist);
    if (dist < targetShape.radius)
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }
    return ECOLLIDE_RESULT_ERR_TOOFAR;
}

ECollideResult ShapeCollision::CheckCricleInCricleRing(const Cricle2D& targetShape, const CricleRing2D& rangeShape)
{
    FmVec2 dist = targetShape.pos - rangeShape.pos;
    float distSq = dist.lenSqr();
    float d = rangeShape.outerRadius + targetShape.radius;
    if (distSq > d*d)
    {// 不在大圆内，则目标太远
        return ECOLLIDE_RESULT_ERR_TOOFAR;
    }

    d = rangeShape.innerRadius - targetShape.radius;
    if (distSq < d*d)
    { // 目标完全在内圆中，太近
        return ECOLLIDE_RESULT_ERR_TOONEAR;
    }
    return ECOLLIDE_RESULT_INTERSECT;
}

ECollideResult ShapeCollision::CheckCricleInSectorRing(const Cricle2D& targetShape, const SectorRing2D& rangeShape)
{
    const FmVec2 dist = targetShape.pos - rangeShape.pos;
    float fDistance = dist.length();

    float fHalfAngle = rangeShape.angle * 0.5f;

    //首先判断角度
    FmVec2 vec2LineOrient = dist;
    //vec2LineOrient.normalize(); // 图形中心点连线方向，指向目标形状
    FmVec2Normalize(&vec2LineOrient, &vec2LineOrient);
    FmVec2 rangeShapeOrient;// = rangeShape.orient.normalized();
    FmVec2Normalize(&rangeShapeOrient, &(rangeShape.orient));
    float fDeltaOrient = FMACOS(FmVec2Dot(&vec2LineOrient, &rangeShapeOrient));
    if (fDeltaOrient > fHalfAngle)
    { // 角度在扇形外
        if (targetShape.radius < 0.001f)
        { // 不在水平角度范围内
            return  ECOLLIDE_RESULT_ERR_HORI;
        }

        //////////////////////////////////////////////////////////////////////////
        float fBeltaAngle = fDeltaOrient - fHalfAngle; // 目标点方向扇形边的差角
        float d = fDistance * sin(fBeltaAngle);  // 目标点到扇形边的距离
        if (d > targetShape.radius)
        {
            return ECOLLIDE_RESULT_ERR_HORI;
        }
    }

    // 判断距离
    if (fDistance > rangeShape.outerRadius + targetShape.radius)
    {
        return ECOLLIDE_RESULT_ERR_TOOFAR;
    }
    if (fDistance < rangeShape.innerRadius - targetShape.radius)
    {
        return ECOLLIDE_RESULT_ERR_TOONEAR;
    }

    return ECOLLIDE_RESULT_INTERSECT;
}



ECollideResult ShapeCollision::CheckHeightDiff(float y, float dy, float fHeightDiff)
{
    // 如果没有传递高度差数据，使用默认高差数据
    if (fHeightDiff < 0.0001f)
    {
        fHeightDiff = TARGET_FILTER_HIGHT_LIMIT;
    }

    //计算高度差是否在允许范围内
    if (fHeightDiff < (y - dy))
    {
        return ECOLLIDE_RESULT_ERR_TOOLOW;
    }
    else if (fHeightDiff < (dy - y))
    {
        return ECOLLIDE_RESULT_ERR_TOOHIGH;
    }

    return ECOLLIDE_RESULT_INTERSECT;
}


bool ShapeCollision::CheckRectLine(const FmVec2& start, const FmVec2& end, const Rect& rect)
{
    bool result = false;
    if (rect.Contains(start) || rect.Contains(end))
        result = true;
    else
    {
        result = result || CheckRectLineH(start, end, rect.leftTop.y, rect.leftTop.x, rect.rightBottom.x);
        result = result || CheckRectLineH(start, end, rect.rightBottom.y, rect.leftTop.x, rect.rightBottom.x);
        result = result || CheckRectLineV(start, end, rect.leftTop.x, rect.leftTop.y, rect.rightBottom.y);
        result = result || CheckRectLineV(start, end, rect.rightBottom.x, rect.leftTop.y, rect.rightBottom.y);
    }

    return result;
}

// 线段与矩形是否相交的方法就变成了线段与矩形的4条边是否相交进行检测。
// 这里，该方法CheckRectLineH是水平方向上的检测，就是检测矩形的上边线与下边线，
// 那CheckRectLineV就是检测矩形的左边线和右边线。
// 这个函数其实就是检测X1与Y0的交点与X2与Y0的交点是否与红色线段是否相交，
// 如果相交，则判断其交点是否在x1与x2之间的范围。
bool ShapeCollision::CheckRectLineH(const FmVec2& start, const FmVec2& end, float y0, float x1, float x2)
{
    //直线在点的上方
    if ((y0 < start.y) && (y0 < end.y))  return false;
    //直线在点的下方
    if ((y0 > start.y) && (y0 > end.y))  return false;
    //水平直线
    if (start.y == end.y)
    {
        //水平直线与点处于同一水平。
        if (y0 == start.y)
        {
            //直线在点的左边
            if ((start.x < x1) && (end.x < x1))  return false;
            //直线在x2垂直线右边
            if ((start.x > x2) && (end.x > x2))  return false;
            //直线的部分或者全部处于点与x2垂直线之间
            return true;
        }
        
        //水平直线与点不处于同一水平。
        return false;
    }
    //斜线
    float x = (end.x - start.x) * (y0 - start.y) / (end.y - start.y) + start.x;
    return ((x >= x1) && (x <= x2));
}


bool ShapeCollision::CheckRectLineV(const FmVec2& start, const FmVec2& end, float x0, float y1, float y2)
{
    if ((x0 < start.x) && (x0 < end.x))  return false;
    if ((x0 > start.x) && (x0 > end.x))  return false;
    if (start.x == end.x)
    {
        if (x0 == start.x)
        {
            if ((start.y < y1) && (end.y < y1)) 
            {
                return false;
            }
            if ((start.y > y2) && (end.y > y2))
            {
                return false;
            }
            return true;
        }
        
        return false;
    }
    float y = (end.y - start.y) * (x0 - start.x) / (end.x - start.x) + start.y;
    return ((y >= y1) && (y <= y2));
}


bool ShapeCollision::CheckPointToPointLessDist(const FmVec2& pt1, const FmVec2& pt2, float distance)
{
    FmVec2 dist = pt1 - pt2;
    return distance * distance >= dist.x * dist.x + dist.y * dist.y;
}

// 判断点是否在矩形范围内
bool ShapeCollision::CheckPointInRectangle(const FmVec2& pt, const Rectangle2D& rangeShape)
{
    // 以矩形中心为坐标原点，计算原点到P的向量OP
    FmVec2 OP = pt - rangeShape.pos;

    // P点相对于矩形坐标系的坐标(h,w)
    float h = FmVec2Dot(&OP, &(rangeShape.orient));    
    if (FMABS(h) > rangeShape.height * 0.5f)
    {
        return false;
    }

    FmVec2 vorient = rangeShape.orient.rotated90();
    float w = FmVec2Dot(&OP, &vorient);
    if (FMABS(w) > rangeShape.width*0.5f)
    {
        return false;
    }

    return true;
}

// 返回距离线段最近的点的距离
// ptA,ptB是线段两个端点坐标，pt是目标点，dist返回最近点距离
// 返回值：-2，距离是与A点距离；-1，距离是点到B点的距离；0：点在线段上；1，返回距离是点到线段的垂直距离
int ShapeCollision::GetNearestDistance(const FmVec2& ptA, const FmVec2& ptB, const FmVec2& pt, float& dist)
{
    FmVec2 PA = ptA - pt;
    FmVec2 PB = ptB - pt;
    FmVec2 AB = ptB - ptA;
    
    float a = PB.length();
    if (a <= EPSILON)
    {// P与B点重合
        dist = 0.0f;
        return 0;
    }

    float b = PA.length();
    if (b <= EPSILON)
    {// P与A点重合
        dist = 0.0f;
        return 0;
    }

    float c = AB.length();
    if (c <= EPSILON)
    {// A点与B点重合
        dist = a;
        return 1;
    }

    if (a*a >= b*b + c*c)
    {// 在BA延长线方向，与A点最近
        dist = b;
        return -2;
    }
//     else if(FMABS(a*a - (b*b + c*c)) < EPSILON)
//     {
//         dist = b;
//         return 1;
//     }

    if (b*b >= a*a + c*c)
    { // 在AB延长线方向，与B点最近
        dist = a;
        return -1;
    }
//     else if (FMABS(b*b - (a*a + c*c)) < EPSILON)
//     {
//         dist = a;
//         return 1;
//     }

    float len = (a+b+c)*0.5f; // 周长的一半
    float s = sqrt(len * (len-a) * (len-b) * (len-c)); // 海伦公式求面积
    dist = 2*s/c;
    return 1;
}


//////////////////////////////////////////////////////////////////////////

ECollideResult ShapeCollision::TestCircleInSectorRing(const Cricle2D& targetShape, const SectorRing2D& rangeShape)
{
    FmVec2 vec2BasePos = rangeShape.pos;
    FmVec2 vec2TargetPos = targetShape.pos;
    float fHalfAngle = rangeShape.angle * 0.5f;
    const FmVec2 vec2ShapeOrient = rangeShape.orient;

    FmMat4 mat;
    FmVec2 rightLine, leftLine, rightNormal, leftNormal;

    //right line
    mat.RotationY(-fHalfAngle);
    FmVec2TransformNormal(&rightLine, &vec2ShapeOrient, &mat);
    //rightLine.normalize();
    FmVec2Normalize(&rightLine, &rightLine);
    rightNormal = rightLine.rotated90();//FmVec2(-rightLine.y, rightLine.x);

    //left line
    mat.RotationY(fHalfAngle);
    FmVec2TransformNormal(&leftLine, &vec2ShapeOrient, &mat);
    //leftLine.normalize();
    FmVec2Normalize(&leftLine, &leftLine);
    leftNormal = -leftNormal.rotated90();//FmVec2(leftNormal.y, -leftNormal.x);

    float cr = 0 - FmVec2Dot(&rightNormal, &vec2BasePos);
    float cl = 0 - FmVec2Dot(&leftNormal, &vec2BasePos);

    //a*x + b*y +c = 0
    float distToRight = FmVec2Dot(&vec2TargetPos, &rightNormal) + cr;
    float distToLeft = FmVec2Dot(&vec2TargetPos, &leftNormal) + cl;

    // 排除两边的位置
    if (distToRight< 0.0f-targetShape.radius)
        return ECOLLIDE_RESULT_ERR_HORI;

    if (distToLeft< 0.0f-targetShape.radius)
        return ECOLLIDE_RESULT_ERR_HORI;

    // 排除圆弧外的位置
    FmVec2 dirWithLength = vec2TargetPos - vec2BasePos;
    float fDistance = dirWithLength.length();
    if (fDistance > rangeShape.outerRadius + targetShape.radius)
    {
        return ECOLLIDE_RESULT_ERR_TOOFAR;
    }
    if (fDistance < rangeShape.innerRadius - targetShape.radius)
    {
        return ECOLLIDE_RESULT_ERR_TOONEAR;
    }

    return ECOLLIDE_RESULT_INTERSECT;
}


// 扇形和矩形碰撞
ECollideResult ShapeCollision::TestRectangleInSectorRing(const Rectangle2D& targetShape, const SectorRing2D& rangeShape)
{
    // 矩形的外接圆半径
    float fOuterRadius = (targetShape.height + targetShape.width) * 0.5f;
    // 两个对象的外接圆无碰撞，直接返回
    if (!CheckPointToPointLessDist(targetShape.pos, rangeShape.pos, rangeShape.outerRadius + fOuterRadius))
    {
        return ECOLLIDE_RESULT_ERR_TOOFAR;
    }

    // 先求矩形中心点的世界矩阵，方便求矩形的四个点的位置
    FmMat4 rectWorldMat, rectRotaMat;
    FmMat4Translation(&rectWorldMat, targetShape.pos.x, 0, targetShape.pos.y);
    float fOrient = util_normalize_angle(targetShape.height);
    FmMat4RotationY(&rectRotaMat, fOrient);
    FmMat4Multiply(&rectWorldMat, &rectRotaMat, &rectWorldMat);

    float x0 = -targetShape.width * 0.5f;
    float x1 = targetShape.width * 0.5f;
    float z0 = -targetShape.height * 0.5f;
    float z1 = targetShape.height * 0.5f;
    // 矩形的四个顶点
    FmVec3 posTemp[4];
    posTemp[0] = FmVec3(x1, 0.0f, z1);
    posTemp[1] = FmVec3(x0, 0.0f, z1);
    posTemp[2] = FmVec3(x1, 0.0f, z0);
    posTemp[3] = FmVec3(x0, 0.0f, z0);

    FmVec3 pos[4];
    FmVec3TransformCoordStream(pos, sizeof(FmVec3), posTemp, sizeof(FmVec3), 4, &rectWorldMat);

    // 扇形圆心到矩形四个顶点和中心点的带权向量
    FmVec2 vert[5];    // 最后一个vec为中点到扇形圆心的向量
    vert[0] = FmVec2(pos[0].x, pos[0].z) - rangeShape.pos;
    vert[1] = FmVec2(pos[1].x, pos[1].z) - rangeShape.pos;
    vert[2] = FmVec2(pos[2].x, pos[2].z) - rangeShape.pos;
    vert[3] = FmVec2(pos[3].x, pos[3].z) - rangeShape.pos;
    vert[4] = targetShape.pos - rangeShape.pos;

    // 只要有一个点在角度和距离上满足要求，就可以了
    // 求扇形的两条边的角度
    float fHalfAngle = rangeShape.angle * 0.5f;
    float leng;
    float fMaxDistSquare, fMinDistSquare;
    fMaxDistSquare = rangeShape.outerRadius * rangeShape.outerRadius;
    fMinDistSquare = rangeShape.innerRadius * rangeShape.innerRadius;
    float fShapeOrient = FMACOS(rangeShape.orient.y);
    for (int i = 0; i < 4; ++i)
    {
        if (CheckFanToPoint(rangeShape.pos, FmVec2(pos[i].x, pos[i].z), fShapeOrient, fHalfAngle))
        {
            // 角度符合，判断距离
            leng = vert[i].x * vert[i].x + vert[i].y * vert[i].y; //长度的平方
            if (leng <= fMaxDistSquare && leng >= fMinDistSquare)
            {
                return ECOLLIDE_RESULT_INTERSECT;
            }
        }
    }

    if (CheckFanToPoint(rangeShape.pos, targetShape.pos, fShapeOrient, fHalfAngle))
    {
        // 角度符合，判断距离
        leng = vert[4].x * vert[4].x + vert[4].y * vert[4].y; //长度的平方
        if (leng <= fMaxDistSquare && leng >= fMinDistSquare)
        {
            return ECOLLIDE_RESULT_INTERSECT;
        }
    }

    // 下面考虑特殊情况
    // 1.扇形在矩形内部，那圆心必在矩形内
    //目标对象与矩形起始点的方向
    float fRelateionOrient = util_dot_angle(rangeShape.pos.x, rangeShape.pos.y, targetShape.pos.x, targetShape.pos.y) - fOrient;
    //目标对象与矩形起始点的距离
    float fDistance = util_dot_distance(rangeShape.pos.x, rangeShape.pos.y, targetShape.pos.x, targetShape.pos.y);

    //将目标点的坐标转换成相对矩形起始点的相对坐标
    float fTargetX = fDistance * ::sin(fRelateionOrient);
    float fTargetZ = fDistance * ::cos(fRelateionOrient);

    if (fTargetX >= x0 && fTargetX <= x1 && fTargetZ >= z0 && fTargetZ <= z1)
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }

    // 扇形的另外两点分别是
    float fx1 = fTargetX + rangeShape.outerRadius*::sin(fRelateionOrient + fHalfAngle);
    float fz1 = fTargetZ + rangeShape.outerRadius*::cos(fRelateionOrient + fHalfAngle);
    float fx2 = fTargetX + rangeShape.outerRadius*::sin(fRelateionOrient - fHalfAngle);
    float fz2 = fTargetZ + rangeShape.outerRadius*::cos(fRelateionOrient - fHalfAngle);

    // 求两天直线的交点是否在矩形上
    if (CheckRectLine(FmVec2(fTargetX, fTargetZ), FmVec2(fx1, fz1), Rect(x0, z0, x1,z1)))
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }

    if (CheckRectLine(FmVec2(fTargetX, fTargetZ), FmVec2(fx2, fz2), Rect(x0, z0, x1,z1)))
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }

    if (CheckRectLine(FmVec2(fx2, fz2), FmVec2(fx1, fz1), Rect(x0, z0, x1,z1)))
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }

    return ECOLLIDE_RESULT_ERR_TOOFAR;;
}

// 矩形和矩形碰撞
ECollideResult ShapeCollision::TestRectangleInRectangle(const Rectangle2D& targetShape, const Rectangle2D& rangeShape)
{
    //rangeShape是攻击范围，targetShape是被攻击者形状，这里的碰撞是指，攻击者放的技能伤害区域是矩形，而被攻击者的受伤区域也是矩形

    //外接圆半径
    float rangeOuterRadius = (rangeShape.height + rangeShape.width) * 0.5f;
    // 另一个外接圆半径
    float targetOuterRadius = (targetShape.height + targetShape.width) * 0.5f;
    // 如果外接圆没有相交，则直接返回
    if (!CheckPointToPointLessDist(rangeShape.pos, targetShape.pos, rangeOuterRadius + targetOuterRadius))
    {
        return ECOLLIDE_RESULT_ERR_TOOFAR;
    }

    RectStruct baseRect, otherRect;
    baseRect.c = FmVec3Wrap(rangeShape.pos.x, 0.0f, rangeShape.pos.y);
    baseRect.u[0] = FmVec3Wrap(rangeShape.pos.x, 0.0f, rangeShape.orient.y);
    baseRect.u[1] = FmVec3Wrap(0.0f, 1.0f, 0.0f);
    baseRect.u[2] = FmVec3Wrap(rangeShape.orient.y, 0.0f, -rangeShape.orient.x);
    baseRect.e = FmVec3Wrap(rangeShape.height * 0.5f, 3.0f, rangeShape.width * 0.5f);

    otherRect.c = FmVec3Wrap(targetShape.pos.x, 0.0f, targetShape.pos.y);
    otherRect.u[0] = FmVec3Wrap(targetShape.pos.x, 0.0f, targetShape.pos.y);
    otherRect.u[1] = FmVec3Wrap(0.0f, 1.0f, 0.0f);
    otherRect.u[2] = FmVec3Wrap(targetShape.pos.y, 0.0f, -targetShape.pos.x);
    otherRect.e = FmVec3Wrap(targetShape.height * 0.5f, 3.0f, targetShape.width * 0.5f);

    if (CheckRectToRect(baseRect, otherRect))
    {
        return ECOLLIDE_RESULT_INTERSECT;
    }

    return ECOLLIDE_RESULT_ERR_RECTANGLE;
}


// ps:这个函数是从某本国外书上拷过来的，两个长方体碰撞。
bool ShapeCollision::CheckRectToRect(RectStruct const& a, RectStruct const& b)
{
    float ra, rb;
    float R[3][3], AbsR[3][3];
    // Compute rotation matrix exprescosg b in a鈥檚 coordinate frame
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            R[i][j] = FmVec3Dot(&(a.u[i]), &(b.u[j]));
        }
    }

    // Compute translation vector t
    FmVec3Wrap t = b.c - a.c;
    // Bring translation into a鈥檚 coordinate frame
    t = FmVec3Wrap(FmVec3Dot(&t, &(a.u[0])), FmVec3Dot(&t, &(a.u[2])), FmVec3Dot(&t, &(a.u[2])));
    // Compute common subexpressions. Add in an epsilon term to
    // counteract arithmetic errors when two edges are parallel and
    // their cross product is (near) null (see text for details)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            AbsR[i][j] = fabs(R[i][j]) + EPSILON;
        }
    }
    // Test axes L = A0, L = A1, L = A2
    for (int i = 0; i < 3; i++)
    {
        ra = a.e[i];
        rb = b.e[0] * AbsR[i][0] + b.e[1] * AbsR[i][1] + b.e[2] * AbsR[i][2];
        if (fabs(t[i]) > ra + rb)
        {
            return false;
        }
    }
    // Test axes L = B0, L = B1, L = B2
    for (int i = 0; i < 3; i++)
    {
        ra = a.e[0] * AbsR[0][i] + a.e[1] * AbsR[1][i] + a.e[2] * AbsR[2][i];
        rb = b.e[i];
        if (fabs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)
        {
            return false;
        }
    }
    // Test axis L = A0 x B0
    ra = a.e[1] * AbsR[2][0] + a.e[2] * AbsR[1][0];
    rb = b.e[1] * AbsR[0][2] + b.e[2] * AbsR[0][1];
    if (fabs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)
    {
        return false;
    }
    // Test axis L = A0 x B1
    ra = a.e[1] * AbsR[2][1] + a.e[2] * AbsR[1][1];
    rb = b.e[0] * AbsR[0][2] + b.e[2] * AbsR[0][0];
    if (fabs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)
    {
        return false;
    }
    // Test axis L = A0 x B2
    ra = a.e[1] * AbsR[2][2] + a.e[2] * AbsR[1][2];
    rb = b.e[0] * AbsR[0][1] + b.e[1] * AbsR[0][0];
    if (fabs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)
    {
        return false;
    }
    // Test axis L = A1 x B0
    ra = a.e[0] * AbsR[2][0] + a.e[2] * AbsR[0][0];
    rb = b.e[1] * AbsR[1][2] + b.e[2] * AbsR[1][1];

    if (fabs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)
    {
        return false;
    }
    // Test axis L = A1 x B1
    ra = a.e[0] * AbsR[2][1] + a.e[2] * AbsR[0][1];
    rb = b.e[0] * AbsR[1][2] + b.e[2] * AbsR[1][0];
    if (fabs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)
    {
        return false;
    }
    // Test axis L = A1 x B2
    ra = a.e[0] * AbsR[2][2] + a.e[2] * AbsR[0][2];
    rb = b.e[0] * AbsR[1][1] + b.e[1] * AbsR[1][0];
    if (fabs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)
    {
        return false;
    }
    // Test axis L = A2 x B0
    ra = a.e[0] * AbsR[1][0] + a.e[1] * AbsR[0][0];
    rb = b.e[1] * AbsR[2][2] + b.e[2] * AbsR[2][1];
    if (fabs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)
    {
        return false;
    }
    // Test axis L = A2 x B1
    ra = a.e[0] * AbsR[1][1] + a.e[1] * AbsR[0][1];
    rb = b.e[0] * AbsR[2][2] + b.e[2] * AbsR[2][0];
    if (fabs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)
    {
        return false;
    }
    // Test axis L = A2 x B2
    ra = a.e[0] * AbsR[1][2] + a.e[1] * AbsR[0][2];
    rb = b.e[0] * AbsR[2][1] + b.e[1] * AbsR[2][0];
    if (fabs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)
    {
        return false;
    }
    // cosce no separating axis is found, the OBBs must be intersecting
    return true;
}


bool ShapeCollision::CheckFanToPoint(FmVec2 const& basePos, FmVec2 const& targetPos, float orien, float anglet)
{
    float fOrient = ::util_normalize_angle(orien);
    float fLineOrient = util_dot_angle(basePos.x, basePos.y, targetPos.x, targetPos.y);
    fLineOrient = util_normalize_angle(fLineOrient);

    float fDeltaOrient = util_normalize_angle(fLineOrient - fOrient);
    if (fDeltaOrient > MY_PI)
    {
        fDeltaOrient = util_normalize_angle(fOrient - fLineOrient);
    }

    if (fDeltaOrient > anglet)
    {
        return false;
    }
    return true;
}


namespace ShapeCollision
{
    // 两个图形的碰撞检测;判断目标形状是否与范围形状相交或者在范围形状内
    ECollideResult Intersects(const Cricle2D& targetShape, const Cricle2D& rangeShape)
    {
        ECollideResult result = CheckCricleInCricle(targetShape, rangeShape);

//         extend_warning(LOG_TRACE,
//             "[ShapeCollision::Intersects] target Cricle2D pos:(%f,%f) r:%f  range Cricle2D pos:(%f,%f) r:%f  result: %d",
//             targetShape.pos.x, targetShape.pos.y, targetShape.radius,
//             rangeShape.pos.x, rangeShape.pos.y, rangeShape.radius,
//             result);

        return result;
    }

    ECollideResult Intersects(const Cricle2D& targetShape, const Sector2D& rangeShape)
    {
        ECollideResult result = CheckCricleInSector(targetShape, rangeShape);

//         extend_warning(LOG_TRACE,
//             "[ShapeCollision::Intersects] target Cricle2D pos:(%f,%f) r:%f  range Sector2D pos:(%f,%f) r:%f angle:%f  result: %d",
//             targetShape.pos.x, targetShape.pos.y, targetShape.radius,
//             rangeShape.pos.x, rangeShape.pos.y, rangeShape.radius, rangeShape.angle,
//             result);

        return result;
    }

    ECollideResult Intersects(const Cricle2D& targetShape, const Rectangle2D& rangeShape)
    {
        ECollideResult result = CheckCricleInRectangle(targetShape, rangeShape);

//         extend_warning(LOG_TRACE,
//             "[ShapeCollision::Intersects] target Cricle2D pos:(%f,%f) r:%f  range Rectangle2D pos:(%f,%f) orient(%f,%f) w:%f h:%f  result: %d",
//             targetShape.pos.x, targetShape.pos.y, targetShape.radius,
//             rangeShape.pos.x, rangeShape.pos.y, rangeShape.orient.x, rangeShape.orient.y, 
//             rangeShape.width, rangeShape.height,
//             result);

        return result;
    }

    ECollideResult Intersects(const Cricle2D& targetShape, const CricleRing2D& rangeShape)
    {
        ECollideResult result = CheckCricleInCricleRing(targetShape, rangeShape);

//         extend_warning(LOG_TRACE,
//             "[ShapeCollision::Intersects] target Cricle2D pos:(%f,%f) r:%f  range CricleRing2D pos:(%f,%f) R:%f r:%f  result: %d",
//             targetShape.pos.x, targetShape.pos.y, targetShape.radius,
//             rangeShape.pos.x, rangeShape.pos.y, rangeShape.outerRadius, rangeShape.innerRadius,
//             result);

        return result;
    }

    ECollideResult Intersects(const Cricle2D& targetShape, const SectorRing2D& rangeShape)
    {
        ECollideResult result = CheckCricleInSectorRing(targetShape, rangeShape);

//         extend_warning(LOG_TRACE,
//             "[ShapeCollision::Intersects] target Cricle2D pos:(%f,%f) r:%f  range SectorRing2D pos:(%f,%f) orient(%f,%f) R:%f r:%f angle:%f  result: %d",
//             targetShape.pos.x, targetShape.pos.y, targetShape.radius,
//             rangeShape.pos.x, rangeShape.pos.y, rangeShape.orient.x, rangeShape.orient.y,
//             rangeShape.outerRadius, rangeShape.innerRadius, rangeShape.angle,
//             result);

        return result;
    }
}

