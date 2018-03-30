//----------------------------------------------------------
// 文件名:      geometry_func.h
// 内  容:      几何相关通用函数
// 说  明:
// 创建人:      shike
// 创建日期:    2011年06月
//    :        
//----------------------------------------------------------

#ifndef FSGAME_UTILITY_GEOMETRY_FUNC_H_
#define FSGAME_UTILITY_GEOMETRY_FUNC_H_

#include "util_func.h"
#include "extend_func.h"

// 拷贝构造函数和赋值操作符保护
#define DISALLOW_COPY(TypeName) \
    TypeName(const TypeName &);
#define DISALLOW_ASSIGN(TypeName) \
    void operator=(const TypeName &);
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    DISALLOW_COPY(TypeName); \
    DISALLOW_ASSIGN(TypeName);

// 点
class Point
{
public:
    explicit Point(const float & init_x, const float & init_y, const float & init_z) :
        x(init_x),
        y(init_y),
        z(init_z)
    {
    }

    Point(const Point & point) : x(point.x), y(point.y), z(point.z)
    {
    }

    ~Point()
    {
    }

public:
    Point & operator=(const Point & point)
    {
        x = point.x;
        y = point.y;
        z = point.z;

        return *this;
    }

public:
    // 3个坐标
    float x;
    float y;
    float z;
};

// 球体
class Sphere
{
public:
    explicit Sphere(const Point & init_center, const float & init_radius) :
        center(init_center),
        radius(init_radius)
    {
    }

    ~Sphere()
    {
    }

public:
    // 球心
    Point center;

    // 半径
    float radius;

private:
    DISALLOW_COPY_AND_ASSIGN(Sphere);
};

// 扇形(存在于在x方向和z方向构成的平面上)
class Sector
{
public:
    explicit Sector(const Point & init_center, const float & init_radius, const float & init_direction, const float & init_half_angle) :
        center(init_center),
        radius(init_radius)
    {
        direction = util_normalize_angle(init_direction);
        half_angle = util_normalize_angle(init_half_angle);
        if (half_angle > PI)
        {
            half_angle = PI;
        }
    }

    explicit Sector(const Sector & sector) :
        center(sector.center),
        radius(sector.radius),
        direction(sector.direction),
        half_angle(sector.half_angle)
    {
    }

    ~Sector()
    {
    }

public:
    // 圆心
    Point center;

    // 半径
    float radius;

    // 中心朝向(0~2*pi)
    float direction;

    // 偏离中心朝向左右的角度(0~pi)
    float half_angle;

private:
    DISALLOW_ASSIGN(Sector);
};

// 扇柱
class SectorCylinder
{
public:
    explicit SectorCylinder(const Sector & init_sector, const float & init_height) :
        sector(init_sector),
        height(init_height)
    {
    }

    ~SectorCylinder()
    {
    }

public:
    // 扇柱底面
    Sector sector;

    // 圆柱高度
    float height;

private:
    DISALLOW_COPY_AND_ASSIGN(SectorCylinder);
};

// 圆形(存在于在x方向和z方向构成的平面上)
class Circle
{
public:
    explicit Circle(const Point & init_center, const float & init_radius) :
        center(init_center),
        radius(init_radius)
    {
    }

    explicit Circle(const Sector & sector) :
        center(sector.center),
        radius(sector.radius)
    {
    }

    ~Circle()
    {
    }

public:
    // 圆心
    Point center;

    // 半径
    float radius;

private:
    DISALLOW_ASSIGN(Circle);
};

// 圆柱
class Cylinder
{
public:
    explicit Cylinder(const Circle & init_circle, const float & init_height) :
        circle(init_circle),
        height(init_height)
    {
    }

    explicit Cylinder(const SectorCylinder & sc) :
        circle(sc.sector.center, sc.sector.radius),
        height(sc.height)
    {
    }

    ~Cylinder()
    {
    }

public:
    // 圆柱底面
    Circle circle;

    // 圆柱高度
    float height;

private:
    DISALLOW_COPY_AND_ASSIGN(Cylinder);
};

// 判断点是否在球体内
bool util_is_point_in_sphere(const Point & point, const Sphere & sphere);

// 判断点是否在圆柱内
bool util_is_point_in_cylinder(const Point & point, const Cylinder & cylinder);

// 判断点是否在扇柱内
bool util_is_point_in_sector_cylinder(const Point & point, const SectorCylinder & sc);

// 在圆形区域内随机点
void util_random_point_in_circle(IKernel * pKernel, const Point & center, const float radius, Point & point);

// 三角函数包
#define PRECISION (0.001f) // 离散精度
class TrigonometricWrap
{
private: 
    enum
    {
        SIZE = static_cast<int>(PI2 / PRECISION), // 数组大小
    };

private: 
    TrigonometricWrap(); 

public: 
    ~TrigonometricWrap()
    {
    }

    static const TrigonometricWrap & instance();

    float sinf(const float dir) const; 
    
    float cosf(const float dir) const; 

private: 
    float m_sinValue[SIZE + 1]; 
    float m_cosValue[SIZE + 1]; 
};

// 获取dir弧度值对应的离散sin值
float dis_sinf(const float dir); 

// 获取dir弧度值对应的离散cos值
float dis_cosf(const float dir); 

#endif

// end of file geometry_func.h

