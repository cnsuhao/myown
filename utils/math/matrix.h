#ifndef _FM_MATRIX_H
#define _FM_MATRIX_H

/****************************************************************************
** Defines
****************************************************************************/
#define MAT00 0
#define MAT01 1
#define MAT02 2
#define MAT03 3
#define MAT10 4
#define MAT11 5
#define MAT12 6
#define MAT13 7
#define MAT20 8
#define MAT21 9
#define MAT22 10
#define MAT23 11
#define MAT30 12
#define MAT31 13
#define MAT32 14
#define MAT33 15


/****************************************************************************
** Typedefs
****************************************************************************/
/*!***************************************************************************
2D floating point vector
*****************************************************************************/
typedef struct
{
	float x;	/*!< x coordinate */
	float y;	/*!< y coordinate */
} FmVECTOR2f;

/*!***************************************************************************
2D fixed point vector
*****************************************************************************/
typedef struct
{
	int x;	/*!< x coordinate */
	int y;	/*!< y coordinate */
} FmVECTOR2x;

/*!***************************************************************************
3D floating point vector
*****************************************************************************/
typedef struct
{
	float x;	/*!< x coordinate */
	float y;	/*!< y coordinate */
	float z;	/*!< z coordinate */
} FmVECTOR3f;

/*!***************************************************************************
3D fixed point vector
*****************************************************************************/
typedef struct
{
	int x;	/*!< x coordinate */
	int y;	/*!< y coordinate */
	int z;	/*!< z coordinate */
} FmVECTOR3x;

/*!***************************************************************************
4D floating point vector
*****************************************************************************/
typedef struct
{
	float x;	/*!< x coordinate */
	float y;	/*!< y coordinate */
	float z;	/*!< z coordinate */
	float w;	/*!< w coordinate */
} FmVECTOR4f;

/*!***************************************************************************
4D fixed point vector
*****************************************************************************/
typedef struct
{
	int x;	/*!< x coordinate */
	int y;	/*!< y coordinate */
	int z;	/*!< z coordinate */
	int w;	/*!< w coordinate */
} FmVECTOR4x;

/*!***************************************************************************
  floating point plane
*****************************************************************************/
typedef struct
{
	float a; /*!< The a coefficient of the clipping plane in the general plane equation.*/
	float b; /*!< The b coefficient of the clipping plane in the general plane equation.*/
	float c; /*!< The c coefficient of the clipping plane in the general plane equation.*/
	float d; /*!< The d coefficient of the clipping plane in the general plane equation.*/
} FmPLANEf;

/*!***************************************************************************
  floating point plane
*****************************************************************************/
typedef struct
{
	int a; /*!< The a coefficient of the clipping plane in the general plane equation.*/
	int b; /*!< The b coefficient of the clipping plane in the general plane equation.*/
	int c; /*!< The c coefficient of the clipping plane in the general plane equation.*/
	int d; /*!< The d coefficient of the clipping plane in the general plane equation.*/
} FmPLANEx;



/*!***************************************************************************
4x4 floating point matrix
*****************************************************************************/

typedef struct FmMATRIXf
{
#ifdef __cplusplus
public:
	float* operator [] ( const int Row )
	{
		return &f[Row<<2];
	}
#endif //__cplusplus
	union
	{
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
		float f[16];	/*!< Array of float */
	};
}FmMATRIXf, *PFmMATRIXf;

/*!***************************************************************************
4x4 fixed point matrix
*****************************************************************************/
typedef struct FmMATRIXx
{
#ifdef __cplusplus
public:
	int* operator [] ( const int Row )
	{
		return &f[Row<<2];
	}
#endif //__cplusplus
	union 
	{
		struct {
			int        _11, _12, _13, _14;
			int        _21, _22, _23, _24;
			int        _31, _32, _33, _34;
			int        _41, _42, _43, _44;

		};
		int m[4][4];
		int f[16];
	};
}FmMATRIXx, *PFmMATRIXx;

/*!***************************************************************************
3x3 floating point matrix
*****************************************************************************/

typedef struct FmMATRIX3f
{
#ifdef __cplusplus
public:
	float* operator [] ( const int Row )
	{
		return &f[Row*3];
	}
#endif //__cplusplus
	float f[9];	/*!< Array of float */
}FmMATRIX3f, *PFmMATRIX3f;

/*!***************************************************************************
3x3 fixed point matrix
*****************************************************************************/
typedef struct FmMATRIX3x
{
#ifdef __cplusplus
public:
	int* operator [] ( const int Row )
	{
		return &f[Row*3];
	}
#endif //__cplusplus
	int f[9];
}FmMATRIX3x, *PFmMATRIX3x;


/****************************************************************************
** Float or fixed
****************************************************************************/
#ifdef FM_FIXED_POINT_ENABLE
typedef FmPLANEx        FmPLANE;
typedef FmVECTOR2x		FmVECTOR2;
typedef FmVECTOR3x		FmVECTOR3;
typedef FmVECTOR4x		FmVECTOR4;
typedef FmMATRIX3x		FmMATRIX3;
typedef FmMATRIXx		FmMATRIX;
#else
typedef FmPLANEf        FmPLANE;
typedef FmVECTOR2f		FmVECTOR2;
typedef FmVECTOR3f		FmVECTOR3;
typedef FmVECTOR4f		FmVECTOR4;
typedef FmMATRIX3f		FmMATRIX3;
typedef FmMATRIXf		FmMATRIX;
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif
