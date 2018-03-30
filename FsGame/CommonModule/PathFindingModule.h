//--------------------------------------------------------------------
// 文件名:		PathFindingModule.h
// 内  容:		寻路接口
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#ifndef __PathFindingModule_H__
#define __PathFindingModule_H__

#include "public/FastStr.h"
#include "Fsgame/Define/header.h"
#include "IFindPath.h"
#include <map>
#include <math.h>

class Point;

class SceneMapProcess : public ISceneMapProcess
{
public:
    SceneMapProcess( IKernel * pKernel ) : m_pKernel( pKernel )
    {
    }

    ~SceneMapProcess()
    {
    }

public:
	/// \brief  测试指定点是否合法
    virtual bool TryPoint( float x, float y, float z );
	/// \brief  测试两点是否可以直走。
    virtual bool TryLine( float xb, float yb, float zb, float xe, float ye, float ze );

	/// \brief  获得地图信息
    virtual bool GetMapSize( float & left, float & top, float & width, float & height );
    virtual int GetLastTryError()
    {
        return 0;
    }
	virtual bool IsExistWater( float xb, float yb, float zb )
	{
		return 0;
	}

private:
    IKernel * m_pKernel;
};

// 寻路系统
class PathFindingModule : public ILogicModule
{
private:
	typedef std::map< std::string, SceneMapProcess * > SceneMapProcessType;
    typedef std::map< std::string, SceneMapProcess * >::const_iterator
    SceneMapProcessTypeConstItr;
    typedef std::map< std::string, SceneMapProcess * >::iterator
    SceneMapProcessTypeItr;

#define TRACEMSG( s, ... ) \
    {\
        char msg[ 128 ];\
        ::sprintf_s( msg, 128, s, __VA_ARGS__ );\
        pKernel->Trace( msg );\
    }

public:
    virtual bool Init( IKernel * pKernel );
    virtual bool Shut( IKernel * pKernel );

private:
    // 场景创建回调
    static int OnSceneCreate( IKernel * pKernel,
                             const PERSISTID & scene,
                             const PERSISTID & sender,
                             const IVarList & args );

public:
    // 在scene_res对应场景中的两点间进行寻路
    static int PointFindPath( const char * scene_res,
                             const Point & p1,
                             const Point & p2,
                             IVarList & point_lst );

private:
    static IFindPath * m_pFindPath;

    static SceneMapProcessType m_sceneMapProcessMap;
};

#endif //__PathFindingModule_H__

