/************************************************************************/
/*                             导出类接口                               */
/************************************************************************/
#ifndef __ISceneMapProcess_h__
#define __ISceneMapProcess_h__

// 针对客户端或服务端的当前场景的一些操作
class ISceneMapProcess
{
public:
    /// \brief  测试指定点是否合法
	virtual bool TryPoint(float x, float y, float z) = 0;
	/// \brief  测试两点是否可以直走。
	virtual bool TryLine(float xb, float yb, float zb,
                         float xe, float ye, float ze) = 0;

	virtual bool IsExistWater(float xb, float yb, float zb) = 0;

	/// \brief  获得地图信息
	virtual bool GetMapSize(float & left, float & top, float & width, float & height) = 0;

	virtual int GetLastTryError() = 0;
};

#endif