//--------------------------------------------------------------------
// 文件名:		PathFindingModule.cpp
// 内  容:		寻路接口
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "PathFindingModule.h"
#include "utils/geometry_func.h"

IFindPath * PathFindingModule::m_pFindPath = NULL;
PathFindingModule::SceneMapProcessType PathFindingModule::m_sceneMapProcessMap;
const float WALK_HEIGHT = 1.0f;
bool SceneMapProcess::TryLine( float xb, float yb, float zb, float xe, float ye, float ze )
{
    float bx, by, bz;
	if( m_pKernel->TraceLineWalk( 1.0f, xb, yb, zb, xe, ze, bx, by, bz ) )
	{
		return true;
	}
	else
	{
		float x_dist = xe - xb;
		float z_dist = ze - zb;
		float dist = fast_sqrt( x_dist * x_dist + z_dist * z_dist );
		if( dist > 0.0001f )
		{
			float vector_x = x_dist / dist;
			float vector_z = z_dist / dist;
			int step = ( int )( dist * 2.0f );
			LoopBeginCheck(a)
			for( int i = 0;i < step; ++i )
			{
				LoopDoCheck(a)
				xb += ( float )vector_x * 0.5f;
				zb += ( float )vector_z * 0.5f;
				float tempY;
				if(m_pKernel->GetWalkWaterExists( xb, zb ) )
				{
					tempY =  m_pKernel->GetWalkWaterHeight( xb, zb );
					if( fabs( tempY - yb ) < WALK_HEIGHT )
					{
						yb = tempY;
						continue;
					}
					else
					{
						return false;
					}
		
				}
				tempY = get_valid_height( m_pKernel, xb, yb, zb );
				if( fabs( tempY - yb ) < WALK_HEIGHT )
				{
					yb = tempY;
					continue;
				}
				else
				{
					return false;
				}
			}

			if( fabs( ye - yb ) < WALK_HEIGHT )
			{
				return true;
			}			
		}
		else
		{
			return true;
		}	
	}
    	
	return false;
}

bool SceneMapProcess::TryPoint( float x, float y, float z )
{
    // 遍历层
    int count = m_pKernel->GetFloorCount( x, z );
	LoopBeginCheck(a);
    for ( int i = 0; i < count; ++i )
    {
		LoopDoCheck(a);
        if ( m_pKernel->GetFloorCanStand( x, z, i ) )
        {
            return true;
        }
    }

	if( m_pKernel->GetWalkWaterExists( x, z ) )
	{
		float dist = y - m_pKernel->GetWalkWaterHeight( x, z );
		//高度差小于1.0
		if( fabs( dist ) < WALK_HEIGHT )
		{
			return true;
		}
	}

    return false;
}

bool SceneMapProcess::GetMapSize( float & left, float & top, float & width, float & height )
{
    float right, bottom;
    if ( !m_pKernel->GetMapBound( left, top, right, bottom ) )
    {
        return false;
    }

    width = right - left;
    height = bottom - top;

    return true;
}

// =============================================================================

bool PathFindingModule::Init( IKernel * pKernel )
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

    std::string path_dir = pKernel->GetResourcePath();
    path_dir += "path/";

    // 创建并初始化m_pFindPath
    m_pFindPath = IFindPath::Create();
    m_pFindPath->InitFindPath( path_dir.c_str() );
    m_pFindPath->SetTimeOut( 300 );

    m_sceneMapProcessMap.clear();

    pKernel->AddEventCallback( "scene", "OnCreate", OnSceneCreate );

    return true;
}

bool PathFindingModule::Shut( IKernel * pKernel )
{
	LoopBeginCheck(a);
    for ( SceneMapProcessTypeItr itr = m_sceneMapProcessMap.begin();
         itr != m_sceneMapProcessMap.end();
         ++itr )
    {
		LoopDoCheck(a);
        if ( 0 != itr->second )
        {
            delete( itr->second );
            itr->second = 0;
        }
    }
    m_sceneMapProcessMap.clear();

    // 销毁m_pFindPath
    if ( NULL != m_pFindPath )
    {
        m_pFindPath->ReleaseAllPath();
        IFindPath::Destroy( m_pFindPath );
        m_pFindPath = NULL;
    }

    return true;
}

int PathFindingModule::OnSceneCreate( IKernel * pKernel,
                                     const PERSISTID & scene,
                                     const PERSISTID & sender,
                                     const IVarList & args )
{
	IGameObj* pSceneObj = pKernel->GetGameObj(scene);
	if (pSceneObj == NULL)
	{
		return 0;
	}

    const std::string scene_res = pSceneObj->QueryString( "Resource" );
    if ( scene_res.empty() )
    {
        return 0;
    }

    const std::string scene_res_main = scene_res + "_main";

    SceneMapProcessTypeConstItr itr = m_sceneMapProcessMap.find( scene_res );
    if ( itr != m_sceneMapProcessMap.end() )
    {
        // 副本可能被创建多次, map中已经存在就不再考虑
        return 0;
    }

    SceneMapProcess * pProcess = NEW SceneMapProcess( pKernel );
    SceneMapProcess * pProcessMain = NEW SceneMapProcess( pKernel );
    if ( NULL == pProcess || NULL == pProcessMain )
    {
        if ( pProcess != NULL )
        {
            delete pProcess;
        }
        if ( pProcessMain != NULL )
        {
            delete pProcessMain;
        }
        char str[ 256 ];
        SPRINTF_S( str,
                  "SceneMapProcess create failed: %s ...",
                  scene_res.c_str() );

        extend_warning( pKernel, "PathFindingModule::OnSceneCreate(): " );
        extend_warning( pKernel, str );

        return 0;
    }

    m_sceneMapProcessMap.insert( std::make_pair( scene_res, pProcess ) );
    m_sceneMapProcessMap.insert( std::make_pair( scene_res_main, pProcessMain ) );

    if ( m_pFindPath->IsLoaded( scene_res.c_str() ) )
    {
        // 已经load过
        return 0;
    }

    if ( !m_pFindPath->LoadPath( scene_res.c_str(), pProcess ) ||
        !m_pFindPath->LoadPath( scene_res_main.c_str(), pProcessMain ) )
    {
        char str[ 256 ];
        SPRINTF_S( str,
                  "load path failed: %s ...",
                  scene_res.c_str() );

        extend_warning( pKernel, "PathFindingModule::OnSceneCreate(): " );
        extend_warning( pKernel, str );

        return 0;

    }

    return 0;
}

int PathFindingModule::PointFindPath( const char * scene_id,
                                     const Point & p1,
                                     const Point & p2,
                                     IVarList & point_lst )
{
    const int max_count = 1024;
    float x_lst[ max_count ], y_lst[ max_count ], z_lst[ max_count ];

    int count = m_pFindPath->PointFindPath( scene_id, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, x_lst, y_lst, z_lst, max_count );
    if ( count > max_count )
    {
        count = max_count;
    }

    if ( count > 0 )
    {
		LoopBeginCheck(b)
        for ( int i = 0; i < count; i++ )
        {
			LoopDoCheck(b)
            point_lst << x_lst[ i ] << y_lst[ i ] << z_lst[ i ];
        }
    }
    else
    {
        extend_warning(LOG_INFO, "PointFindPath Error:%d,sceneId:%s,BeginX:%f,BeginY:%f,BeginZ:%f,Endx:%f,EndY:%f,EndZ:%f",
            count, scene_id, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
    }

    return count;
}

// end of PathFindingModule.cpp

