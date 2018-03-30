/**
 * FILE NAME : Point.h
 * COPYRIGHT : Snail Game
 * AUTHOR    : ricky
 * ---------------------------------------------------
 * DISCRIPTION :
 *      point structure
 *
 * Revision:
 *      v0.0 2007.8.15 Create by ricky
 */

//--------------------------------
#pragma once
//--------------------------------


//namespace aw_base
//{

    template <typename T>
    struct SPointTemp
    {
        T x;
        T y;
		T z;

    #ifdef _EDITOR
        SPointTemp (const CPoint& other)
        {
            x = other.x;
            y = other.y;
			z = other.z;
        }

        operator CPoint (void)
        {
            return (CPoint)(*this);
        }
    #endif

        SPointTemp()
        {
            x = 0;
            y = 0;
			z = 0;
        }

        SPointTemp (T x, T y, T z)
        {
            this->x = x;
            this->y = y;
			this->z = z;
        }

        SPointTemp (const SPointTemp<T>& other)
        {
            x = other.x;
            y = other.y;
			z = other.z;
        }

        void SetVector (T x, T y, T z)
        {
            this->x = x;
            this->y = y;
			this->z = z;
        }

        void SetPoint (T x, T y, T z)
        {
            this->x = x;
            this->y = y;
			this->z = z;
        }

        void Offset (T dx, T dy, T dz)
        {
            x += dx;
            y += dy;
			z += dz;
        }

        void Offset (const SPointTemp<T>& offset)
        {
            x += offset.x;
            y += offset.y;
			z += offset.z;
        }

        SPointTemp Normailize (void)
        {
            T len = (T)sqrt ((float)(x * x + y * y + z * z));
			if( !len ) return (*this);
			
            x /= len;
            y /= len;
			z /= len;
            return (*this);
        }

        T Distance (const SPointTemp<T>& other)
        {
            T dx = other.x - x;
            T dy = other.y - y;
			T dz = other.z - z;
            return (T)sqrt((float)(dx * dx + dy * dy + dz * dz));
        }

        T DistanceSqr (const SPointTemp<T>& other)
        {
            T dx = other.x - x;
            T dy = other.y - y;
			T dz = other.z - z;
            return dx * dx + dy * dy + dz * dz;
        }

        T DistanceMht (const SPointTemp<T>& other)
        {
            return abs(other.x-x) + abs(other.y-y) + abs(other.z-z);
        }

        bool operator == (const SPointTemp<T>& other) const 
        {
            return (x == other.x && y == other.y && z == other.z);
        }

        SPointTemp& operator = (const SPointTemp<T>& other)
        {
            x = other.x;
            y = other.y;
			z = other.z;
            return *this;
        }

        SPointTemp operator + (const SPointTemp<T>& other)
        {
            return SPointTemp (x+other.x, y+other.y, z+other.z);
        }

        const SPointTemp operator + (const SPointTemp<T>& other) const
        {
            return SPointTemp (x+other.x, y+other.y+other.z);
        }

        SPointTemp operator - (const SPointTemp<T>& other)
        {
            return SPointTemp (x-other.x, y-other.y, z-other.z);
        }

        const SPointTemp operator - (const SPointTemp<T>& other) const
        {
            return SPointTemp (x-other.x, y-other.y, z-other.z);
        }

        SPointTemp operator * (const SPointTemp<T>& other)
        {
            return SPointTemp (x*other.x, y*other.y, z*other.z);
        }

        SPointTemp operator * (int f)
        {
            return SPointTemp (x*f, y*f, z*f);
        }

        SPointTemp operator += (const SPointTemp<T>& other)
        {
            x += other.x;
            y += other.y;
			z += other.z;
            return (*this);
        }

        SPointTemp operator -= (const SPointTemp<T>& other)
        {
            x -= other.x;
            y -= other.y;
			z -= other.z;
            return (*this);
        }
    };

//}

typedef SPointTemp<float>    SPoint;
typedef SPointTemp<float>  SPointf;