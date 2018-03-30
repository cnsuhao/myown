/************************************************************************/
/*                            导出类接口                                */
/************************************************************************/

#ifndef IFindPath_H_
#define IFindPath_H_

#include "ISceneMapProcess.h"
#include "Point.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <list>

typedef void * (POpenFile)(const char *, const char *);
typedef bool (PCloseFile)(void *);
typedef size_t (PReadFile)(void *, void *, size_t);
typedef size_t (PGetFileSize)(const char *);

// 跨场景寻路的返回结果的结构
typedef struct _SCENEPATHITEM
{
    std::string sceneid;
    float xBegin;
    float yBegin;
    float zBegin;
    float xEnd;
    float yEnd;
    float zEnd;
} SCENEPATHITEM;

typedef struct _PATHLINK
{
    int index;
    float height; // 权重（距离）
} PATHLINK;

typedef struct _PATHPOINT
{
    _PATHPOINT()
        : x(0.0f), y(0.0f), z(0.0f),
          r(0.0f),
          bRemove(false)
    {
        vecLink.clear();
    }

    float x, y, z;                  // 坐标
    float r;                        // 该点的半径
    bool bRemove;                   // 需要删除的点
    std::vector<PATHLINK> vecLink;  // 有连通关系的点
} PATHPOINT;

enum eFindResult
{
    SCENE_FIND_PATH_ERROR       = -6,   // 跨场景寻路失败
    FIND_PATH_TIMEOUT           = -5,   // 寻路超时
    FIND_PATH_ERROR             = -4,   // 寻路A*函数返回失败
    NO_PATH_POINT_NEAR_END      = -3,   // 终点附近没有寻路点
    NO_PATH_POINT_REACH_BEGIN   = -2,   // 起点附近没有能直接到达的路点
    BEGIN_POINT_CANT_WALK       = -1,   // 当前所站的点，不是可行走点
    UNKNOW_ERROR                = 0,    // 未知错误!
    FIND_PATH_SUCCESS           = 1,    // 1及以上,寻路成功!
};

#ifdef FIND_PATH_DLL
#ifdef FINDPATHNEW_EXPORTS
#define FIND_PATH_API __declspec(dllexport)
#else
#define FIND_PATH_API __declspec(dllimport)
#endif
#else
#define FIND_PATH_API 
#endif

class FIND_PATH_API IFindPath
{
public:
    /// \brief  创建实例
    static IFindPath* Create();
    /// \brief  删除实例
    static void Destroy(IFindPath * pInstance);

    /// \brief  设置读取文件的函数指针。
    ///         不设置默认为fopen,fclose...
    /// \param  FunName      函数名称
    /// \param  FunPointer   函数指针
    /// \return 设置成功返回true，不成功返回false
    virtual bool SetFileFunc(const char * FunName, void * FunPointer) = 0;

public:
    /// \brief  初始化寻路模块。
    /// \param  path    寻路文件所在路径
    virtual void InitFindPath(const char * path) = 0;

    /// \brief  获得指定场景的路点总数
    /// \param  scene_res   场景Resource
    /// \return 路点数量
    virtual int GetPathPointCount(const char * scene_res) = 0;

    /// \brief  获得指定场景的指定范围的路点索引列表
    /// \param  scene_res   场景CONFIGID
    /// \param  radius      半径
    /// \param  point_lst   路点索引保存到此
    /// \param  max_count   point_lst数组的最大容量
    /// \return 成功返回true，失败返回false
    virtual int GetPathPointInRadius(const char * scene_res,
        float x, float z, float radius,
        int point_lst[], int max_count) = 0;

    /// \brief  获得指定场景的指定区域的路点索引列表
    /// \param  scene_res   场景CONFIGID
    /// \param  point_lst   路点索引保存到此
    /// \param  max_count   point_lst数组的最大容量
    /// \return 成功返回true，失败返回false
    virtual int GetPathPointInZone(const char * scene_res,
        float left, float top, float right, float bottom,
        int point_lst[], int max_count) = 0;

    /// \brief  查询路点
    /// \param  scene_res   场景CONFIGID
    /// \param  x, y, z     路点坐标
    /// \return 成功返回路点的索引号，失败返回-1
    virtual int PathPointExist(const char * scene_res,
        float x, float y, float z) = 0;

    /// \brief  获得指定场景的指定路点的信息
    /// \param  scene_res   场景CONFIGID
    /// \param  index       路点索引号
    /// \return 成功返回路点结构的指针，失败返回NULL
    virtual PATHPOINT * GetPathPointInfo(const char * scene_res, int index) = 0;

    /// \brief  添加路点
    /// \param  scene_res   场景CONFIGID
    /// \param  x, y, z     路点坐标
    /// \param  r           路点半径
    /// \return 成功返回路点的索引号，失败返回-1
    virtual int AddPathPoint(const char * scene_res,
        float x, float y, float z, float r) = 0;

    /// \brief  删除路点
    /// \param  scene_res   场景CONFIGID
    /// \param  index       路点索引
    virtual void RemovePathPoint(const char * scene_res, int index) = 0;
    virtual void RemovePathPoint(const char * scene_res, int index,
        int begin, int end) = 0;
    /// \brief  删除最后一个路点
    /// \param  scene_res   场景CONFIGID
    virtual void RemoveEndPathPoint(const char * scene_res) = 0;

    /// \brief  修改路点位置
    /// \param  scene_res   场景CONFIGID
    /// \param  index       路点索引
    /// \param  x, y, z     路点坐标
    /// \return 成功返回true
    virtual bool SetPathPointPos(const char * scene_res,
        int index, float x, float y, float z) = 0;

    /// \brief  添加连通关系
    /// \param  scene_res   场景CONFIGID
    /// \param  index       路点索引
    /// \param  link_index  连通的路点索引
    /// \return 成功返回true, 失败返回false
    virtual bool AddLink(const char * scene_res, int index, int link_index) = 0;

    /// \brief  删除连通关系
    /// \param  scene_res   场景CONFIGID
    /// \param  index       路点索引
    /// \param  link_index  连通的路点索引
    virtual void RemoveLink(const char * scene_res, int index, int link_index) = 0;

    /// \brief  场景资源是否已加载。
    /// \param  scene_res   要加载的场景CONFIG
    /// \return 加载成功返回true，否则返回false
    virtual bool IsLoaded(const char * scene_res) = 0;

    /// \brief  新建寻路资源。
    /// \param  scene_res   要新建的场景CONFIG
    /// \return 新建成功返回true，否则返回false
    virtual bool NewPath(const char * scene_res, ISceneMapProcess * pSceneMapProcess) = 0;

    /// \brief  加载寻路资源。
    /// \param  scene_res   要加载的场景CONFIG
    /// \return 加载成功返回true，否则返回false
    virtual bool LoadPath(const char * scene_res, ISceneMapProcess * pSceneMapProcess) = 0;

    /// \brief  保存寻路资源。
    /// \param  scene_res   场景CONFIGID
    /// \return 成功返回true，否则返回false
    virtual bool SavePath(const char * scene_res) = 0;

    /// \brief  释放寻路资源。
    /// \param  scene_res   要释放的场景CONFIG
    virtual void ReleasePath(const char * scene_res) = 0;

    /// \brief  释放所有寻路资源。
    virtual void ReleaseAllPath() = 0;

    /// \brief  【寻路接口】获取起点到终点的路径
    /// \param  xBegin, yBegin, zBegin  起点坐标
    /// \param  xEnd, yEnd, zEnd        终点坐标
    /// \return 返回寻路的点数，失败返回0或负数 eFindResult
    virtual int PointFindPath(const char * scene_res,
        float xBegin, float yBegin, float zBegin,
        float xEnd, float yEnd, float zEnd,
        float x[], float y[], float z[], int maxcount,
        bool bNormal = false, bool bServer = true) = 0;

    /// \brief  【寻路接口】获取起点到终点的路径
    ///         作为主干道寻路的第一步
    ///         暂时只在客户端用到
    /// \param  xBegin, yBegin, zBegin  起点坐标
    /// \param  xEnd, yEnd, zEnd        终点坐标
    /// \return 返回寻路的点数，失败返回0
    virtual int PointFindMainPath(const char * scene_res,
        float xBegin, float yBegin, float zBegin,
        float xEnd, float yEnd, float zEnd,
        float x[], float y[], float z[], int maxcount) { return 0; };

    /// \brief  【寻路接口】跨场景寻路
    /// \param  scene_resBegin          起点的场景号
    /// \param  xBegin, yBegin, zBegin  起点坐标
    /// \param  scene_resEnd            终点的场景号
    /// \param  xEnd, yEnd, zEnd        终点坐标
    /// \return 返回寻路的点数，失败返回0
    virtual int SceneFindPath(const char * begin_scene_res,
        float xBegin, float yBegin, float zBegin,
        const char * end_scene_res,
        float xEnd, float yEnd, float zEnd,
        SCENEPATHITEM scenepath[], int maxcount) = 0;

    /// \brief  测试指定点是否合法
    /// \param  x, y, z 坐标
    virtual bool TryPoint(const char * scene_res, float x, float y, float z) = 0;

    /// \brief  测试两点是否可以直走
    /// \param  xBegin, yBegin, zBegin  起始点坐标
    /// \param  xEnd, yEnd, zEnd        终点坐标
    virtual bool TryLine(const char * scene_res,
        float xBegin, float yBegin, float zBegin,
        float xEnd, float yEnd, float zEnd) = 0;

    /// \brief  最终路径点的优化
    /// \param  scene_res   场景标识
    /// \param   pointList  要优化的点列表
    virtual int Optimize(const char * scene_res, std::list<SPointf>& pointList) = 0;

    /// \brief  设置寻路超时时间
    /// \param  time 时间（毫秒）
    virtual void SetTimeOut(size_t time) = 0;
};

#endif  // IFindPath_H_