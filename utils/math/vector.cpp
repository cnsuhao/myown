#include "vector.h"
#include <math.h>

/****************************************************************************
** Constants
****************************************************************************/
static const FmMATRIXf	c_mIdentity = {
	{
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	}
};


/*!***************************************************************************
** FmVec2 2 component vector
****************************************************************************/

/*!***************************************************************************
@Function			FmVec2
@Input				v3Vec a Vec3
@Description		Constructor from a Vec3
*****************************************************************************/
FmVec2::FmVec2(const FmVec3& vec3)
{
	x = vec3.x; y = vec3.y;
}

/*!***************************************************************************
** FmVec3 3 component vector
****************************************************************************/

/*!***************************************************************************
@Function			FmVec3
@Input				v4Vec a FmVec4
@Description		Constructor from a FmVec4
*****************************************************************************/
FmVec3::FmVec3(const FmVec4& vec4)
{
	x = vec4.x; y = vec4.y; z = vec4.z;
}

/*!***************************************************************************
@Function			*
@Input				rhs a FmMat3
@Returns			result of multiplication
@Description		matrix multiplication operator FmVec3 and FmMat3
****************************************************************************/
FmVec3 FmVec3::operator*(const FmMat3& rhs) const
{
	FmVec3 out;

	out.x = VERTTYPEMUL(x,rhs.f[0])+VERTTYPEMUL(y,rhs.f[1])+VERTTYPEMUL(z,rhs.f[2]);
	out.y = VERTTYPEMUL(x,rhs.f[3])+VERTTYPEMUL(y,rhs.f[4])+VERTTYPEMUL(z,rhs.f[5]);
	out.z = VERTTYPEMUL(x,rhs.f[6])+VERTTYPEMUL(y,rhs.f[7])+VERTTYPEMUL(z,rhs.f[8]);

	return out;
}

/*!***************************************************************************
@Function			*=
@Input				rhs a FmMat3
@Returns			result of multiplication and assignment
@Description		matrix multiplication and assignment operator for FmVec3 and FmMat3
****************************************************************************/
FmVec3& FmVec3::operator*=(const FmMat3& rhs)
{
	VERTTYPE tx = VERTTYPEMUL(x,rhs.f[0])+VERTTYPEMUL(y,rhs.f[1])+VERTTYPEMUL(z,rhs.f[2]);
	VERTTYPE ty = VERTTYPEMUL(x,rhs.f[3])+VERTTYPEMUL(y,rhs.f[4])+VERTTYPEMUL(z,rhs.f[5]);
	z = VERTTYPEMUL(x,rhs.f[6])+VERTTYPEMUL(y,rhs.f[7])+VERTTYPEMUL(z,rhs.f[8]);
	x = tx;
	y = ty;

	return *this;
}

/*!***************************************************************************
** FmVec4 4 component vector
****************************************************************************/

/*!***************************************************************************
@Function			*
@Input				rhs a FmMat4
@Returns			result of multiplication
@Description		matrix multiplication operator FmVec4 and FmMat4
****************************************************************************/
FmVec4 FmVec4::operator*(const FmMat4& rhs) const
{
	FmVec4 out;
	out.x = VERTTYPEMUL(x,rhs.f[0])+VERTTYPEMUL(y,rhs.f[1])+VERTTYPEMUL(z,rhs.f[2])+VERTTYPEMUL(w,rhs.f[3]);
	out.y = VERTTYPEMUL(x,rhs.f[4])+VERTTYPEMUL(y,rhs.f[5])+VERTTYPEMUL(z,rhs.f[6])+VERTTYPEMUL(w,rhs.f[7]);
	out.z = VERTTYPEMUL(x,rhs.f[8])+VERTTYPEMUL(y,rhs.f[9])+VERTTYPEMUL(z,rhs.f[10])+VERTTYPEMUL(w,rhs.f[11]);
	out.w = VERTTYPEMUL(x,rhs.f[12])+VERTTYPEMUL(y,rhs.f[13])+VERTTYPEMUL(z,rhs.f[14])+VERTTYPEMUL(w,rhs.f[15]);
	return out;
}

/*!***************************************************************************
@Function			*=
@Input				rhs a FmMat4
@Returns			result of multiplication and assignment
@Description		matrix multiplication and assignment operator for FmVec4 and FmMat4
****************************************************************************/
FmVec4& FmVec4::operator*=(const FmMat4& rhs)
{
	VERTTYPE tx = VERTTYPEMUL(x,rhs.f[0])+VERTTYPEMUL(y,rhs.f[1])+VERTTYPEMUL(z,rhs.f[2])+VERTTYPEMUL(w,rhs.f[3]);
	VERTTYPE ty = VERTTYPEMUL(x,rhs.f[4])+VERTTYPEMUL(y,rhs.f[5])+VERTTYPEMUL(z,rhs.f[6])+VERTTYPEMUL(w,rhs.f[7]);
	VERTTYPE tz = VERTTYPEMUL(x,rhs.f[8])+VERTTYPEMUL(y,rhs.f[9])+VERTTYPEMUL(z,rhs.f[10])+VERTTYPEMUL(w,rhs.f[11]);
	w = VERTTYPEMUL(x,rhs.f[12])+VERTTYPEMUL(y,rhs.f[13])+VERTTYPEMUL(z,rhs.f[14])+VERTTYPEMUL(w,rhs.f[15]);
	x = tx;
	y = ty;
	z = tz;
	return *this;
}

/*!***************************************************************************
** FmMat3 3x3 matrix
****************************************************************************/
/*!***************************************************************************
@Function			FmMat3
@Input				mat a FmMat4
@Description		constructor to form a FmMat3 from a FmMat4
****************************************************************************/
FmMat3::FmMat3(const FmMat4& mat)
{
	VERTTYPE *dest = (VERTTYPE*)f, *src = (VERTTYPE*)mat.f;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			(*dest++) = (*src++);
		}
		src++;
	}
}

/*!***************************************************************************
@Function			RotationX
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 3x3 rotation matrix about the X axis
****************************************************************************/
FmMat3 FmMat3::RotationX(VERTTYPE angle)
{
	FmMat4 out;
	FmMat4RotationX(&out,angle);
	return FmMat3(out);
}
/*!***************************************************************************
@Function			RotationY
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 3x3 rotation matrix about the Y axis
****************************************************************************/
FmMat3 FmMat3::RotationY(VERTTYPE angle)
{
	FmMat4 out;
	FmMat4RotationY(&out,angle);
	return FmMat3(out);
}
/*!***************************************************************************
@Function			RotationZ
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 3x3 rotation matrix about the Z axis
****************************************************************************/
FmMat3 FmMat3::RotationZ(VERTTYPE angle)
{
	FmMat4 out;
	FmMat4RotationZ(&out,angle);
	return FmMat3(out);
}


/*!***************************************************************************
** FmMat4 4x4 matrix
****************************************************************************/
/*!***************************************************************************
@Function			RotationX
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 4x4 rotation matrix about the X axis
****************************************************************************/
FmMat4 FmMat4::RotationX(VERTTYPE angle)
{
	FmMat4 out;
	FmMat4RotationX(&out,angle);
	return out;
}
/*!***************************************************************************
@Function			RotationY
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 4x4 rotation matrix about the Y axis
****************************************************************************/
FmMat4 FmMat4::RotationY(VERTTYPE angle)
{
	FmMat4 out;
	FmMat4RotationY(&out,angle);
	return out;
}
/*!***************************************************************************
@Function			RotationZ
@Input				angle the angle of rotation
@Returns			rotation matrix
@Description		generates a 4x4 rotation matrix about the Z axis
****************************************************************************/
FmMat4 FmMat4::RotationZ(VERTTYPE angle)
{
	FmMat4 out;
	FmMat4RotationZ(&out,angle);
	return out;
}

/*!***************************************************************************
@Function			*
@Input				rhs another FmMat4
@Returns			result of multiplication
@Description		Matrix multiplication of two 4x4 matrices.
*****************************************************************************/
FmMat4 FmMat4::operator*(const FmMat4& rhs) const
{
	FmMat4 out;
	FmMat4Multiply(&out, this, &rhs);
	return out;
}


/*!***************************************************************************
@Function			inverse
@Returns			inverse mat4
@Description		Calculates multiplicative inverse of this matrix
The matrix must be of the form :
A 0
C 1
Where A is a 3x3 matrix and C is a 1x3 matrix.
*****************************************************************************/
FmMat4 FmMat4::inverse() const
{
	FmMat4 out;
	VERTTYPE	det_1;
	VERTTYPE	pos, neg, temp;

	/* Calculate the determinant of submatrix A and determine if the
	the matrix is singular as limited by the double precision
	floating-point data representation. */
	pos = neg = f2vt(0.0);
	temp =  VERTTYPEMUL(VERTTYPEMUL(f[ 0], f[ 5]), f[10]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(f[ 4], f[ 9]), f[ 2]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(f[ 8], f[ 1]), f[ 6]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(-f[ 8], f[ 5]), f[ 2]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(-f[ 4], f[ 1]), f[10]);
	if (temp >= 0) pos += temp; else neg += temp;
	temp =  VERTTYPEMUL(VERTTYPEMUL(-f[ 0], f[ 9]), f[ 6]);
	if (temp >= 0) pos += temp; else neg += temp;
	det_1 = pos + neg;

	/* Is the submatrix A singular? */
	if (det_1 == f2vt(0.0)) //|| (VERTTYPEABS(det_1 / (pos - neg)) < 1.0e-15)
	{
		/* Matrix M has no inverse */
		//_RPT0(_CRT_WARN, "Matrix has no inverse : singular matrix\n");
	}
	else
	{
		/* Calculate inverse(A) = adj(A) / det(A) */
		//det_1 = 1.0 / det_1;
		det_1 = VERTTYPEDIV(f2vt(1.0f), det_1);
		out.f[ 0] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 5], f[10]) - VERTTYPEMUL(f[ 9], f[ 6]) ), det_1);
		out.f[ 1] = - VERTTYPEMUL(( VERTTYPEMUL(f[ 1], f[10]) - VERTTYPEMUL(f[ 9], f[ 2]) ), det_1);
		out.f[ 2] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 1], f[ 6]) - VERTTYPEMUL(f[ 5], f[ 2]) ), det_1);
		out.f[ 4] = - VERTTYPEMUL(( VERTTYPEMUL(f[ 4], f[10]) - VERTTYPEMUL(f[ 8], f[ 6]) ), det_1);
		out.f[ 5] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 0], f[10]) - VERTTYPEMUL(f[ 8], f[ 2]) ), det_1);
		out.f[ 6] = - VERTTYPEMUL(( VERTTYPEMUL(f[ 0], f[ 6]) - VERTTYPEMUL(f[ 4], f[ 2]) ), det_1);
		out.f[ 8] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 4], f[ 9]) - VERTTYPEMUL(f[ 8], f[ 5]) ), det_1);
		out.f[ 9] = - VERTTYPEMUL(( VERTTYPEMUL(f[ 0], f[ 9]) - VERTTYPEMUL(f[ 8], f[ 1]) ), det_1);
		out.f[10] =   VERTTYPEMUL(( VERTTYPEMUL(f[ 0], f[ 5]) - VERTTYPEMUL(f[ 4], f[ 1]) ), det_1);

		/* Calculate -C * inverse(A) */
		out.f[12] = - ( VERTTYPEMUL(f[12], out.f[ 0]) + VERTTYPEMUL(f[13], out.f[ 4]) + VERTTYPEMUL(f[14], out.f[ 8]) );
		out.f[13] = - ( VERTTYPEMUL(f[12], out.f[ 1]) + VERTTYPEMUL(f[13], out.f[ 5]) + VERTTYPEMUL(f[14], out.f[ 9]) );
		out.f[14] = - ( VERTTYPEMUL(f[12], out.f[ 2]) + VERTTYPEMUL(f[13], out.f[ 6]) + VERTTYPEMUL(f[14], out.f[10]) );

		/* Fill in last row */
		out.f[ 3] = f2vt(0.0f);
		out.f[ 7] = f2vt(0.0f);
		out.f[11] = f2vt(0.0f);
		out.f[15] = f2vt(1.0f);
	}

	return out;
}

/*!***************************************************************************
@Function			FmLinearEqSolve
@Input				pSrc	2D array of floats. 4 Eq linear problem is 5x4
matrix, constants in first column
@Input				nCnt	Number of equations to solve
@Output			pRes	Result
@Description		Solves 'nCnt' simultaneous equations of 'nCnt' variables.
pRes should be an array large enough to contain the
results: the values of the 'nCnt' variables.
This fn recursively uses Gaussian Elimination.
*****************************************************************************/
void FmLinearEqSolve(VERTTYPE * const pRes, VERTTYPE ** const pSrc, const int nCnt)
{
	int			i, j, k;
	VERTTYPE	f;

	if (nCnt == 1)
	{
		Assert(pSrc[0][1] != 0);
		pRes[0] = VERTTYPEDIV(pSrc[0][0], pSrc[0][1]);
		return;
	}

	// Loop backwards in an attempt avoid the need to swap rows
	i = nCnt;
	while(i)
	{
		--i;

		if(pSrc[i][nCnt] != f2vt(0.0f))
		{
			// Row i can be used to zero the other rows; let's move it to the bottom
			if(i != (nCnt-1))
			{
				for(j = 0; j <= nCnt; ++j)
				{
					// Swap the two values
					f = pSrc[nCnt-1][j];
					pSrc[nCnt-1][j] = pSrc[i][j];
					pSrc[i][j] = f;
				}
			}

			// Now zero the last columns of the top rows
			for(j = 0; j < (nCnt-1); ++j)
			{
				Assert(pSrc[nCnt-1][nCnt] != f2vt(0.0f));
				f = VERTTYPEDIV(pSrc[j][nCnt], pSrc[nCnt-1][nCnt]);

				// No need to actually calculate a zero for the final column
				for(k = 0; k < nCnt; ++k)
				{
					pSrc[j][k] -= VERTTYPEMUL(f, pSrc[nCnt-1][k]);
				}
			}

			break;
		}
	}

	// Solve the top-left sub matrix
	FmLinearEqSolve(pRes, pSrc, nCnt - 1);

	// Now calc the solution for the bottom row
	f = pSrc[nCnt-1][0];
	for(k = 1; k < nCnt; ++k)
	{
		f -= VERTTYPEMUL(pSrc[nCnt-1][k], pRes[k-1]);
	}
	Assert(pSrc[nCnt-1][nCnt] != f2vt(0));
	f = VERTTYPEDIV(f, pSrc[nCnt-1][nCnt]);
	pRes[nCnt-1] = f;
}




/*!***************************************************************************
@Function			dot
@Input				rhs a single value
@Returns			scalar product
@Description		calculate the scalar product of two Vec3s
****************************************************************************/
VERTTYPE FmVec2Dot(const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pv1 != NULL) && (pv2 != NULL ) );
	return VERTTYPEMUL((*pv1).x, (*pv2).x) + VERTTYPEMUL((*pv1).y, (*pv2).y);
}

/*!***************************************************************************
@Function			length
@Returns			the of the magnitude of the vector
@Description		calculates the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec2Length(const FmVec2* pv)
{
	Assert( pv != NULL );
	return (VERTTYPE) f2vt(sqrt(vt2f((*pv).x)*vt2f((*pv).x) + vt2f((*pv).y)*vt2f((*pv).y)));
}

/*!***************************************************************************
@Function			lenSqr
@Returns			the square of the magnitude of the vector
@Description		calculates the square of the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec2LengthSq(const FmVec2* pv)
{
	Assert( pv != NULL );
	return VERTTYPEMUL((*pv).x,(*pv).x)+VERTTYPEMUL((*pv).y,(*pv).y);
}

/*!***************************************************************************
@Function			normalize
@Returns			the normalized value of the vector
@Description		normalizes the vector
****************************************************************************/
FmVec2* FmVec2Normalize(FmVec2* pOut,const FmVec2* pv)
{
	Assert( (pOut != NULL ) && ( pv != NULL ) );
	VERTTYPE len = FmVec2Length(pv);
	if(len <= FM_SMALL_NUMBER)
	{
		(*pOut).x = 0.0;
		(*pOut).y = 0.0;
		return pOut;
	}
	(*pOut).x =VERTTYPEDIV((*pv).x,len);
	(*pOut).y =VERTTYPEDIV((*pv).y,len);
	return pOut;
}

/*!***************************************************************************
@Function			-
@Input				rhs another Vec2
@Returns			result of negation
@Description		negation operator for a Vec2
****************************************************************************/
FmVec2* FmVec2Subtract(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x - (*pv2).x;
	(*pOut).y = (*pv1).y - (*pv2).y;
	return pOut;
}

/*!***************************************************************************
@Function			*
@Input				lhs scalar
@Input				rhs a Vec2
@Returns			result of multiplication
@Description		multiplication operator for a Vec2
****************************************************************************/
FmVec2* FmVec2Scale(FmVec2* pOut,const FmVec2* pv,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv != NULL) );
	(*pOut).x = (*pv).x * s;
	(*pOut).y = (*pv).y * s;
	return pOut;
}

/*!***************************************************************************
@Function			lerp
@Input				lhs scalar
@Input				rhs a Vec2
@Returns			result of multiplication
@Description		Performs a linear interpolation between two 2D vectors.
****************************************************************************/
FmVec2* FmVec2Lerp(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x + s * ( (*pv1).x - (*pv2).x);
	(*pOut).y = (*pv1).y + s * ( (*pv1).y - (*pv2).y);
	return pOut;
}

/*!***************************************************************************
@Function			max
@Input				rhs another Vec2
@Returns			result of negation
@Description		Returns a 2D vector that is made up of the largest components of two 2D vectors.
****************************************************************************/
FmVec2* FmVec2Maximize(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x > (*pv2).x ? (*pv1).x : (*pv2).x;
	(*pOut).y = (*pv1).y > (*pv2).y ? (*pv1).y : (*pv2).y;
	return pOut;
}


/*!***************************************************************************
@Function			min
@Input				rhs another Vec2
@Returns			result of negation
@Description		Returns a 2D vector that is made up of the smallest components of two 2D vectors.
****************************************************************************/
FmVec2* FmVec2Minimize(FmVec2* pOut,const FmVec2* pv1,const FmVec2* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	(*pOut).x = (*pv1).x < (*pv2).x ? (*pv1).x : (*pv2).x;
	(*pOut).y = (*pv1).y < (*pv2).y ? (*pv1).y : (*pv2).y;
	return pOut;
}


/*!***************************************************************************
@Function			min
@Input				rhs another Vec2
@Returns			result of negation
@Description		Transforms a 2D vector by a given matrix.
****************************************************************************/
FmVec4* FmVec2Transform(FmVec4* pOut,const FmVec2* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL));

	(*pOut).x = (*pm)(0,0) * (*pv).x + (*pm)(0,1) * (*pv).y  + (*pm)(0,3);
	(*pOut).y = (*pm)(1,0) * (*pv).x + (*pm)(1,1) * (*pv).y  + (*pm)(1,3);
	(*pOut).z = (*pm)(2,0) * (*pv).x + (*pm)(2,1) * (*pv).y  + (*pm)(2,3);
	(*pOut).w = (*pm)(3,0) * (*pv).x + (*pm)(3,1) * (*pv).y  + (*pm)(3,3);

	return pOut;
}


/*!***************************************************************************
@Function			TransformCoord
@Input				rhs another Vec2
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 2D vector by a given matrix, projecting the result back into w = 1.
****************************************************************************/
FmVec2* FmVec2TransformCoord(FmVec2* pOut,const FmVec2* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	FmVec2 v;
    VERTTYPE norm;
	v =*pv;

	norm = (*pm)(3,0) * (*pv).x + (*pm)(3,1) * (*pv).y + (*pm)(3,3);

	(*pOut).x = (*pm)(0,0) * v.x + (*pm)(0,1) * v.y + (*pm)(0,3) / norm;
	(*pOut).y = (*pm)(1,0) * v.x + (*pm)(1,1) * v.y + (*pm)(1,3) / norm;
	return pOut;
}

/*!***************************************************************************
@Function			TransformNormal
@Input				rhs another Vec2
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms the 2D vector normal by the given matrix.
****************************************************************************/
FmVec2* FmVec2TransformNormal(FmVec2* pOut,const FmVec2* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	const FmVec2 v = *pv;
	(*pOut).x = (*pm)(0,0) * v.x + (*pm)(0,1) * v.y;
	(*pOut).y = (*pm)(1,0) * v.x + (*pm)(1,1) * v.y;
	return pOut;
}


/*!***************************************************************************
@Function			+
@Input				rhs another Vec3
@Returns			result of addition
@Description		componentwise addition operator for two Vec3s
****************************************************************************/
FmVec3* FmVec3Add(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );

	(*pOut).x = (*pv1).x + (*pv2).x;
	(*pOut).y = (*pv1).y + (*pv2).y;
	(*pOut).z = (*pv1).z + (*pv2).z;
	return pOut;
}


/*!***************************************************************************
@Function			dot
@Input				rhs a single value
@Returns			scalar product
@Description		calculate the scalar product of two Vec3s
****************************************************************************/
VERTTYPE FmVec3Dot(const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pv1 != NULL) && (pv2 != NULL ) );
	return VERTTYPEMUL((*pv1).x, (*pv2).x) + VERTTYPEMUL((*pv1).y, (*pv2).y) + VERTTYPEMUL((*pv1).z, (*pv2).z);
}

/*!***************************************************************************
@Function			length
@Returns			the of the magnitude of the vector
@Description		calculates the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec3Length(const FmVec3* pv)
{
	Assert( pv != NULL );
	return (VERTTYPE) f2vt(sqrt(vt2f((*pv).x)*vt2f((*pv).x) + vt2f((*pv).y)*vt2f((*pv).y) + vt2f((*pv).z)*vt2f((*pv).z) ));
}

/*!***************************************************************************
@Function			lenSqr
@Returns			the square of the magnitude of the vector
@Description		calculates the square of the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec3LengthSq(const FmVec3* pv)
{
	Assert( pv != NULL );
	return VERTTYPEMUL((*pv).x,(*pv).x)+VERTTYPEMUL((*pv).y,(*pv).y)+VERTTYPEMUL((*pv).z,(*pv).z);
}


/*!***************************************************************************
@Function			normalize
@Returns			the normalized value of the vector
@Description		normalizes the vector
****************************************************************************/
FmVec3* FmVec3Normalize(FmVec3* pOut,const FmVec3* pv)
{
	Assert( (pOut != NULL ) && ( pv != NULL ) );
	FmVec3 v;
	VERTTYPE len = FmVec3Length(pv);
	if(len <= FM_SMALL_NUMBER)
	{
		v.x = 0.0;
		v.y = 0.0;
		v.z = 0.0;
		*pOut = v;
		return pOut;
	}


	v.x =VERTTYPEDIV((*pv).x,len);
	v.y =VERTTYPEDIV((*pv).y,len);
	v.z =VERTTYPEDIV((*pv).z,len);
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			-
@Input				rhs another Vec2
@Returns			result of negation
@Description		negation operator for a Vec3
****************************************************************************/
FmVec3* FmVec3Subtract(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL) && (pv2 != NULL) );
	FmVec3 v;
	v.x = (*pv1).x - (*pv2).x;
	v.y = (*pv1).y - (*pv2).y;
	v.z = (*pv1).z - (*pv2).z;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			*
@Input				lhs scalar
@Input				rhs a Vec3
@Returns			result of multiplication
@Description		multiplication operator for a Vec3
****************************************************************************/
FmVec3* FmVec3Scale(FmVec3* pOut,const FmVec3* pv,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv != NULL) );
	FmVec3 v;
	v.x = (*pv).x * s;
	v.y = (*pv).y * s;
	v.z = (*pv).z * s;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			lerp
@Input				lhs scalar
@Input				rhs a Vec3
@Returns			result of multiplication
@Description		Performs a linear interpolation between two 3D vectors.
****************************************************************************/
FmVec3* FmVec3Lerp(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
    FmVec3 v;
	v.x = (*pv1).x + s * ( (*pv2).x - (*pv1).x);
	v.y = (*pv1).y + s * ( (*pv2).y - (*pv1).y);
	v.z = (*pv1).z + s * ( (*pv2).z - (*pv1).z);
	*pOut = v;
	return pOut;
}


/*!***************************************************************************
@Function			max
@Input				rhs another Vec3
@Returns			result of negation
@Description		Returns a 3D vector that is made up of the largest components of two 3D vectors.
****************************************************************************/
FmVec3* FmVec3Maximize(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
    FmVec3 v;
	v.x = (*pv1).x > (*pv2).x ? (*pv1).x : (*pv2).x;
	v.y = (*pv1).y > (*pv2).y ? (*pv1).y : (*pv2).y;
	v.z = (*pv1).z > (*pv2).z ? (*pv1).z : (*pv2).z;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			min
@Input				rhs another Vec3
@Returns			result of negation
@Description		Returns a 3D vector that is made up of the smallest components of two 3D vectors.
****************************************************************************/
FmVec3* FmVec3Minimize(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	FmVec3 v;
	v.x = (*pv1).x < (*pv2).x ? (*pv1).x : (*pv2).x;
	v.y = (*pv1).y < (*pv2).y ? (*pv1).y : (*pv2).y;
	v.z = (*pv1).z < (*pv2).z ? (*pv1).z : (*pv2).z;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			Transform
@Input				rhs another Vec3
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 3D vector by a given matrix.
****************************************************************************/
FmVec4* FmVec3Transform(FmVec4* pOut,const FmVec3* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL ) && (pv != NULL ) && (pm != NULL ) );
    FmVec4 v;
	v.x = (*pm)(0,0) * (*pv).x + (*pm)(0,1) * (*pv).y  + (*pm)(0,2) * (*pv).z  + (*pm)(0,3);
	v.y = (*pm)(1,0) * (*pv).x + (*pm)(1,1) * (*pv).y  + (*pm)(1,2) * (*pv).z  + (*pm)(1,3);
	v.z = (*pm)(2,0) * (*pv).x + (*pm)(2,1) * (*pv).y  + (*pm)(2,2) * (*pv).z  + (*pm)(2,3);
	v.w = (*pm)(3,0) * (*pv).x + (*pm)(3,1) * (*pv).y  + (*pm)(3,2) * (*pv).z  + (*pm)(3,3);

	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			TransformCoord
@Input				rhs another Vec3
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 3D vector by a given matrix, projecting the result back into w = 1.
****************************************************************************/
FmVec3* FmVec3TransformCoord(FmVec3* pOut,const FmVec3* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	FmVec3 out;

	VERTTYPE norm;
	norm = (*pm)(3,0)*(*pv).x + (*pm)(3,1)*(*pv).y + (*pm)(3,2)*(*pv).z + (*pm)(3,3);

	if(norm)
	{
		out.x = ((*pm)(0,0)*(*pv).x + (*pm)(0,1)*(*pv).y + (*pm)(0,2)*(*pv).z + (*pm)(0,3))/norm;
		out.y = ((*pm)(1,0)*(*pv).x + (*pm)(1,1)*(*pv).y + (*pm)(1,2)*(*pv).z + (*pm)(1,3))/norm;
		out.z = ((*pm)(2,0)*(*pv).x + (*pm)(2,1)*(*pv).y + (*pm)(2,2)*(*pv).z + (*pm)(2,3))/norm;
	}
	else
	{
		out.x = 0.0;
		out.y = 0.0;
		out.z = 0.0;
	}

	*pOut = out;
	return pOut;
}

/*!***************************************************************************
@Function			TransformCoordStream
@Input              rhs another int
@Input				rhs another Vec3
@Input              rhs another int
@Input              rhs another int
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 3D vector by a given matrix, projecting the result back into w = 1.
****************************************************************************/
FmVec3* FmVec3TransformCoordStream(FmVec3* pOut, int nOutputStride, const FmVec3* pIn, int nInputStride, int loopCount, const FmMat4* pm)
{
	for (int i = 0; i < loopCount; i++)
	{
		FmVec3TransformCoord(
			(FmVec3*)((char*)pOut + nOutputStride * i),
			(const FmVec3*)((const char*)pIn + nInputStride * i),
			pm);
	}

	return pOut;
}

/*!***************************************************************************
@Function			TransformNormal
@Input				rhs another Vec3
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms the 3D vector normal by the given matrix.
****************************************************************************/
FmVec3* FmVec3TransformNormal(FmVec3* pOut,const FmVec3* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	const FmVec3 v = *pv;
	(*pOut).x = (*pm)(0,0)*v.x + (*pm)(0,1)*v.y + (*pm)(0,2)*v.z;
	(*pOut).y = (*pm)(1,0)*v.x + (*pm)(1,1)*v.y + (*pm)(1,2)*v.z;
	(*pOut).z = (*pm)(2,0)*v.x + (*pm)(2,1)*v.y + (*pm)(2,2)*v.z;

	return pOut;
}

/*!***************************************************************************
@Function			TransformNormal
@Input				rhs another Vec3
@Input				rhs another Vec3
@Returns			result of negation
@Description		Determines the cross-product of two 3D vectors.
****************************************************************************/
FmVec3* FmVec3Cross(FmVec3* pOut,const FmVec3* pv1,const FmVec3* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL ));
	FmVec3 v;
	v.x = (*pv1).y * (*pv2).z - (*pv1).z * (*pv2).y;
	v.y = (*pv1).z * (*pv2).x - (*pv1).x * (*pv2).z;
	v.z = (*pv1).x * (*pv2).y - (*pv1).y * (*pv2).x;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			+
@Input				rhs another Vec4
@Returns			result of addition
@Description		componentwise addition operator for two Vec4s
****************************************************************************/
FmVec4* FmVec4Add(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	FmVec4 v;
	v.x = (*pv1).x + (*pv2).x;
	v.y = (*pv1).y + (*pv2).y;
	v.z = (*pv1).z + (*pv2).z;
	v.w = (*pv1).w + (*pv2).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			-
@Input				rhs another Vec4
@Returns			result of negation
@Description		negation operator for a Vec4
****************************************************************************/
FmVec4* FmVec4Subtract(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
	FmVec4 v;
	v.x = (*pv1).x - (*pv2).x;
	v.y = (*pv1).y - (*pv2).y;
	v.z = (*pv1).z - (*pv2).z;
	v.w = (*pv1).w - (*pv2).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			Vec4Cross
@Input				rhs another Vec4
@Input				rhs another Vec4
@Returns			result of negation
@Description		Determines the cross-product of two 4D vectors.
****************************************************************************/
FmVec4* FmVec4Cross(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2,const FmVec4* pv3)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) &&(pv3 != NULL));
	FmVec4 v;
    v.x =       (*pv1).y*((*pv2).z*(*pv3).w - (*pv3).z*(*pv2).w) 
              - (*pv1).z*((*pv2).y*(*pv3).w - (*pv3).y*(*pv2).w)
			  + (*pv1).w*((*pv2).y*(*pv3).z - (*pv2).z*(*pv3).y);

	v.y =       -((*pv1).x*((*pv2).z*(*pv3).w - (*pv3).z*(*pv2).w)
		        - (*pv1).z*((*pv2).x*(*pv3).w - (*pv3).x*(*pv2).w)
				+ (*pv1).w*((*pv2).x*(*pv3).z - (*pv3).x*(*pv2).z));

	v.z =       (*pv1).x*((*pv2).y*(*pv3).w - (*pv3).y*(*pv2).w)
		       -(*pv1).y*((*pv2).x*(*pv3).w - (*pv3).x*(*pv2).w)
			   +(*pv1).w*((*pv2).x*(*pv3).y - (*pv3).x*(*pv2).y);

	v.w =       -((*pv1).x*((*pv2).y*(*pv3).z - (*pv3).y*(*pv2).z)
		        - (*pv1).y*((*pv2).x*(*pv3).z - (*pv3).x*(*pv2).z)
                + (*pv1).z*((*pv2).x*(*pv3).y - (*pv3).x*(*pv2).y));
	*pOut = v;

	return pOut;
}

/*!***************************************************************************
@Function			length
@Returns			the of the magnitude of the vector
@Description		calculates the magnitude of the vector
****************************************************************************/
VERTTYPE FmVec4Length(const FmVec4* pv)
{
	Assert( pv != NULL );
	return (VERTTYPE) f2vt(sqrt(vt2f((*pv).x)*vt2f((*pv).x) + vt2f((*pv).y)*vt2f((*pv).y) + vt2f((*pv).z)*vt2f((*pv).z)+ vt2f((*pv).w)*vt2f((*pv).w) ));
}

/*!***************************************************************************
@Function			normalize
@Returns			the normalized value of the vector
@Description		normalizes the vector
****************************************************************************/
FmVec4* FmVec4Normalize(FmVec4* pOut,const FmVec4* pv)
{
	Assert( (pOut != NULL) && (pv != NULL) );

	FmVec4 v;

	VERTTYPE len = FmVec4Length(pv);

	if(len <= FM_SMALL_NUMBER)
	{
		v.x = 0.0;
		v.y = 0.0;
		v.z = 0.0;
		v.w = 0.0;
		*pOut = v;
		return pOut;
	}


	v.x =VERTTYPEDIV((*pv).x,len);
	v.y =VERTTYPEDIV((*pv).y,len);
	v.z =VERTTYPEDIV((*pv).z,len);
	v.w =VERTTYPEDIV((*pv).w,len);
	*pOut = v;
	return pOut;
}


/*!***************************************************************************
@Function			max
@Input				rhs another Vec4
@Returns			result of negation
@Description		Returns a 4D vector that is made up of the largest components of two 4D vectors.
****************************************************************************/
FmVec4* FmVec4Maximize(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	FmVec4 v;
	v.x = (*pv1).x > (*pv2).x ? (*pv1).x : (*pv2).x;
	v.y = (*pv1).y > (*pv2).y ? (*pv1).y : (*pv2).y;
	v.z = (*pv1).z > (*pv2).z ? (*pv1).z : (*pv2).z;
	v.w = (*pv1).w > (*pv2).w ? (*pv1).w : (*pv2).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			min
@Input				rhs another Vec4
@Returns			result of negation
@Description		Returns a 4D vector that is made up of the smallest components of two 4D vectors.
****************************************************************************/
FmVec4* FmVec4Minimize(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2)
{
	Assert( (pOut != NULL ) && (pv1 != NULL ) && (pv2 != NULL) );
	FmVec4 v;
	v.x = (*pv1).x < (*pv2).x ? (*pv1).x : (*pv2).x;
	v.y = (*pv1).y < (*pv2).y ? (*pv1).y : (*pv2).y;
	v.z = (*pv1).z < (*pv2).z ? (*pv1).z : (*pv2).z;
	v.w = (*pv1).w < (*pv2).w ? (*pv1).w : (*pv2).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			lerp
@Input				lhs scalar
@Input				rhs a Vec4
@Returns			result of multiplication
@Description		Performs a linear interpolation between two 4D vectors.
****************************************************************************/
FmVec4* FmVec4Lerp(FmVec4* pOut,const FmVec4* pv1,const FmVec4* pv2,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv1 != NULL) && (pv2 != NULL) );
    FmVec4 v;
	v.x = (*pv1).x + s * ( (*pv1).x - (*pv2).x);
	v.y = (*pv1).y + s * ( (*pv1).y - (*pv2).y);
	v.z = (*pv1).z + s * ( (*pv1).z - (*pv2).z);
	v.w = (*pv1).w + s * ( (*pv1).w - (*pv2).w);
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			*
@Input				lhs scalar
@Input				rhs a Vec4
@Returns			result of multiplication
@Description		multiplication operator for a Vec4
****************************************************************************/
FmVec4* FmVec4Scale(FmVec4* pOut,const FmVec4* pv,VERTTYPE s)
{
	Assert( (pOut != NULL) && (pv != NULL) );
    FmVec4 v;
	v.x = (*pv).x * s;
	v.y = (*pv).y * s;
	v.z = (*pv).z * s;
	v.w = (*pv).w * s;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			Transform
@Input				rhs another Vec4
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Transforms a 3D vector by a given matrix.
****************************************************************************/
FmVec4* FmVec4Transform(FmVec4* pOut,const FmVec4* pv,const FmMat4* pm)
{
	Assert( (pOut != NULL) && (pv != NULL) && (pm != NULL) );
	FmVec4 v;

	v.x = (*pm)(0,0) * (*pv).x + (*pm)(0,1) * (*pv).y + (*pm)(0,2) * (*pv).z + (*pm)(0,3) * (*pv).w;
	v.y = (*pm)(1,0) * (*pv).x + (*pm)(1,1) * (*pv).y + (*pm)(1,2) * (*pv).z + (*pm)(1,3) * (*pv).w;
	v.z = (*pm)(2,0) * (*pv).x + (*pm)(2,1) * (*pv).y + (*pm)(2,2) * (*pv).z + (*pm)(2,3) * (*pv).w;
	v.w = (*pm)(3,0) * (*pv).x + (*pm)(3,1) * (*pv).y + (*pm)(3,2) * (*pv).z + (*pm)(3,3) * (*pv).w;
	*pOut = v;
	return pOut;
}

/*!***************************************************************************
@Function			FmMat4Identity
@Input				rhs another FmMat4
@Returns			result of negation
@Description		Creates an identity matrix.
****************************************************************************/
FmMat4* FmMat4Identity(FmMat4* pOut)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=1.0f;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	return pOut;
}


/*!***************************************************************************
@Function			FmMat4Multiply
@Output			mOut	Result of mA x mB
@Input				mA		First operand
@Input				mB		Second operand
@Description		Multiply mA by mB and assign the result to mOut
(mOut = p1 * p2). A copy of the result matrix is done in
the function because mOut can be a parameter mA or mB.
*****************************************************************************/
FmMat4* FmMat4Multiply(FmMat4 *pOut,const FmMat4	*pA,const FmMat4 *pB)
{
	Assert((pOut != NULL) && (pA != NULL ) && ( pB != NULL) );
	FmMat4 mRet;
	FmMat4 mA = *pA;
    FmMat4 mB = *pB;

	/* Perform calculation on a dummy matrix (mRet) */
	mRet.f[ 0] = mA.f[ 0]*mB.f[ 0] + mA.f[ 1]*mB.f[ 4] + mA.f[ 2]*mB.f[ 8] + mA.f[ 3]*mB.f[12];
	mRet.f[ 1] = mA.f[ 0]*mB.f[ 1] + mA.f[ 1]*mB.f[ 5] + mA.f[ 2]*mB.f[ 9] + mA.f[ 3]*mB.f[13];
	mRet.f[ 2] = mA.f[ 0]*mB.f[ 2] + mA.f[ 1]*mB.f[ 6] + mA.f[ 2]*mB.f[10] + mA.f[ 3]*mB.f[14];
	mRet.f[ 3] = mA.f[ 0]*mB.f[ 3] + mA.f[ 1]*mB.f[ 7] + mA.f[ 2]*mB.f[11] + mA.f[ 3]*mB.f[15];

	mRet.f[ 4] = mA.f[ 4]*mB.f[ 0] + mA.f[ 5]*mB.f[ 4] + mA.f[ 6]*mB.f[ 8] + mA.f[ 7]*mB.f[12];
	mRet.f[ 5] = mA.f[ 4]*mB.f[ 1] + mA.f[ 5]*mB.f[ 5] + mA.f[ 6]*mB.f[ 9] + mA.f[ 7]*mB.f[13];
	mRet.f[ 6] = mA.f[ 4]*mB.f[ 2] + mA.f[ 5]*mB.f[ 6] + mA.f[ 6]*mB.f[10] + mA.f[ 7]*mB.f[14];
	mRet.f[ 7] = mA.f[ 4]*mB.f[ 3] + mA.f[ 5]*mB.f[ 7] + mA.f[ 6]*mB.f[11] + mA.f[ 7]*mB.f[15];

	mRet.f[ 8] = mA.f[ 8]*mB.f[ 0] + mA.f[ 9]*mB.f[ 4] + mA.f[10]*mB.f[ 8] + mA.f[11]*mB.f[12];
	mRet.f[ 9] = mA.f[ 8]*mB.f[ 1] + mA.f[ 9]*mB.f[ 5] + mA.f[10]*mB.f[ 9] + mA.f[11]*mB.f[13];
	mRet.f[10] = mA.f[ 8]*mB.f[ 2] + mA.f[ 9]*mB.f[ 6] + mA.f[10]*mB.f[10] + mA.f[11]*mB.f[14];
	mRet.f[11] = mA.f[ 8]*mB.f[ 3] + mA.f[ 9]*mB.f[ 7] + mA.f[10]*mB.f[11] + mA.f[11]*mB.f[15];

	mRet.f[12] = mA.f[12]*mB.f[ 0] + mA.f[13]*mB.f[ 4] + mA.f[14]*mB.f[ 8] + mA.f[15]*mB.f[12];
	mRet.f[13] = mA.f[12]*mB.f[ 1] + mA.f[13]*mB.f[ 5] + mA.f[14]*mB.f[ 9] + mA.f[15]*mB.f[13];
	mRet.f[14] = mA.f[12]*mB.f[ 2] + mA.f[13]*mB.f[ 6] + mA.f[14]*mB.f[10] + mA.f[15]*mB.f[14];
	mRet.f[15] = mA.f[12]*mB.f[ 3] + mA.f[13]*mB.f[ 7] + mA.f[14]*mB.f[11] + mA.f[15]*mB.f[15];

	/* Copy result to mOut */
	*pOut = mRet;
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4Translation
@Output			mOut	Translation matrix
@Input				fX		X component of the translation
@Input				fY		Y component of the translation
@Input				fZ		Z component of the translation
@Description		Build a transaltion matrix mOut using fX, fY and fZ.
*****************************************************************************/
FmMat4* FmMat4Translation(FmMat4	*pOut,const float fX,const float fY,const float	fZ)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=fX;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=fY;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=1.0f;	(*pOut).f[14]=fZ;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4Scaling
@Output			mOut	Scale matrix
@Input				fX		X component of the scaling
@Input				fY		Y component of the scaling
@Input				fZ		Z component of the scaling
@Description		Build a scale matrix mOut using fX, fY and fZ.
*****************************************************************************/
FmMat4* FmMat4Scaling(FmMat4 *pOut,const float fX,const float fY,const float fZ)
{
	Assert( pOut != NULL );
	(*pOut).f[ 0]=fX;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=fY;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=fZ;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMa4RotationX
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an X rotation matrix mOut.
*****************************************************************************/
FmMat4* FmMat4RotationX(FmMat4 *pOut,const float fAngle)
{
	Assert( pOut != NULL );

    FmMat4Identity(pOut);
    pOut->m[1][1] = cos(fAngle);
    pOut->m[2][2] = cos(fAngle);
    pOut->m[1][2] = sin(fAngle);
    pOut->m[2][1] = -sin(fAngle);

	/*
	float		fCosine, fSine;

	fCosine	= (float)FMFCOS(fAngle);
	fSine	= (float)FMFSIN(fAngle);
 
	(*pOut).f[ 0]=1.0f;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;	(*pOut).f[ 5]=fCosine;	(*pOut).f[ 9]=fSine;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;	(*pOut).f[ 6]=-fSine;	(*pOut).f[10]=fCosine;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;	(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	*/
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4RotationY
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Y rotation matrix mOut.
*****************************************************************************/
FmMat4* FmMat4RotationY(FmMat4	*pOut,const float fAngle)
{
	Assert( pOut != NULL );

    FmMat4Identity(pOut);
    pOut->m[0][0] = cos(fAngle);
    pOut->m[2][2] = cos(fAngle);
    pOut->m[0][2] = -sin(fAngle);
    pOut->m[2][0] = sin(fAngle);
	/*
	float		fCosine, fSine;

	fCosine	= (float)FMFCOS(fAngle);
	fSine	= (float)FMFSIN(fAngle);
	(*pOut).f[ 0]=fCosine;	(*pOut).f[ 4]=0.0f;	(*pOut).f[ 8]=-fSine;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=0.0f;		(*pOut).f[ 5]=1.0f;	(*pOut).f[ 9]=0.0f;		(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=fSine;	(*pOut).f[ 6]=0.0f;	(*pOut).f[10]=fCosine;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;		(*pOut).f[ 7]=0.0f;	(*pOut).f[11]=0.0f;		(*pOut).f[15]=1.0f;
	*/
	return pOut;
}

/*!***************************************************************************
@Function Name		FmMat4RotationZ
@Output			mOut	Rotation matrix
@Input				fAngle	Angle of the rotation
@Description		Create an Z rotation matrix mOut.
*****************************************************************************/
FmMat4* FmMat4RotationZ(FmMat4 *pOut,const float fAngle)
{
	Assert( pOut != NULL );

	FmMat4Identity(pOut);
    pOut->m[0][0] = cos(fAngle);
    pOut->m[1][1] = cos(fAngle);
    pOut->m[0][1] = sin(fAngle);
    pOut->m[1][0] = -sin(fAngle);
	/*
	float		fCosine, fSine;

	fCosine =	(float)FMFCOS(fAngle);
	fSine =		(float)FMFSIN(fAngle);
 
	(*pOut).f[ 0]=fCosine;	(*pOut).f[ 4]=fSine;	(*pOut).f[ 8]=0.0f;	(*pOut).f[12]=0.0f;
	(*pOut).f[ 1]=-fSine;	(*pOut).f[ 5]=fCosine;	(*pOut).f[ 9]=0.0f;	(*pOut).f[13]=0.0f;
	(*pOut).f[ 2]=0.0f;		(*pOut).f[ 6]=0.0f;	    (*pOut).f[10]=1.0f;	(*pOut).f[14]=0.0f;
	(*pOut).f[ 3]=0.0f;		(*pOut).f[ 7]=0.0f;	    (*pOut).f[11]=0.0f;	(*pOut).f[15]=1.0f;
	*/

	return pOut;
}

