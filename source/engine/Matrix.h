// Matrix3.h: interface for the Matrix3 class.
//
//////////////////////////////////////////////////////////////////////
#ifndef GAMATRIX3_H
#define GAMATRIX3_H

#include <memory.h>
#pragma warning( disable : 4201 )

#include <Quaternion.h>

namespace Aurora
{
//=================================================
template<typename real>
class Matrix3  
{
public:

	union
	{
        struct
		{
            real        _11, _12, _13;
            real        _21, _22, _23;
            real        _31, _32, _33;
        };
        real m[9];
    };

	Matrix3();
	Matrix3(const Matrix3<real>& matrix);
	Matrix3(const real* pData);
	Matrix3(real m11, real m12, real m13,
		    real m21, real m22, real m23,
			real m31, real m32, real m33);


	inline const Matrix3<real>& operator=(const Matrix3<real>& rhl);

	inline void Set(real m11, real m12, real m13,
					real m21, real m22, real m23,
					real m31, real m32, real m33);

/*	inline const Matrix3<real>& operator +=(const Matrix3<real>& m);
	inline const Matrix3<real>& operator -=(const Matrix3<real>& m);
	inline const Matrix3<real>& operator *=(T scale);
	inline const Matrix3<real>& operator /=(T scale);*/

	inline real*		Ptr();
	inline const real*	Ptr() const;
	inline real* GetRow(size_t row) const;

	inline void TransposeSelf();


	// special matrices
    static const Matrix3 ZERO;
    static const Matrix3 IDENTITY;

};
//==========================================================








//===========================================================
template<typename real>
Matrix3<real>::Matrix3()
{
}
//-----------------------------------------------------------
template<typename real>
Matrix3<real>::Matrix3(const Matrix3<real>& matrix)
{
	memcpy(m, matrix.m, sizeof(m));
}

//-----------------------------------------------------------
template<typename real>
Matrix3<real>::Matrix3(const real* pData)
{
	memcpy(m, pData, sizeof(m));
}
//-----------------------------------------------------------
template<typename real>
Matrix3<real>::Matrix3(real m11, real m12, real m13,
		    real m21, real m22, real m23,
			real m31, real m32, real m33)
{
	_11 = m11;	_12 = m12;	_13 = m13;
	_21 = m21;	_22 = m22;	_23 = m23;
	_31 = m31;	_32 = m32;	_33 = m33;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix3<real>& 
Matrix3<real>::operator=(const Matrix3<real>& rhl)
{
	memcpy(m, rhl.m, sizeof(m));
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline void
Matrix3<real>::Set(real m11, real m12, real m13,
					real m21, real m22, real m23,
					real m31, real m32, real m33)
{
	_11 = m11;	_12 = m12;	_13 = m13;
	_21 = m21;	_22 = m22;	_23 = m23;
	_31 = m31;	_32 = m32;	_33 = m33;
}
//-----------------------------------------------------------
template<typename real>
inline real*		
Matrix3<real>::Ptr()
{
	return m;
}
//-----------------------------------------------------------
template<typename real>
inline const real* 
Matrix3<real>::Ptr() const
{
	return m;
}
//-------------------------------------------------------------
template<typename real>
inline real*
Matrix3<real>::GetRow(size_t row) const
{
	return &m[row*3];
}
//-------------------------------------------------------------
template<typename real>
inline void
Matrix3<real>::TransposeSelf()
{
	real temp;
	temp = _12; _12 = _21; _21 = temp;
	temp = _13; _13 = _31; _31 = temp;

	temp = _23; _23 = _32; _32 = temp;
}

typedef Matrix3<float> Matrix3f;



































template<typename real>
class Matrix4
{
public:
	// data 
	union
	{
		struct
		{
			real	_11, _12, _13, _14;
			real	_21, _22, _23, _24;
			real	_31, _32, _33, _34;
			real	_41, _42, _43, _44;
		};
		real m[16];
	};

	Matrix4() {}
	Matrix4(const Matrix4<real>& matrix);
	Matrix4(const real* data);
	Matrix4(real m11, real m12, real m13, real m14,
		real m21, real m22, real m23, real m24,
		real m31, real m32, real m33, real m34,
		real m41, real m42, real m43, real m44);

	inline const Matrix4<real>& operator=(const Matrix4<real>& rhl);

	inline void Set(real m11, real m12, real m13, real m14,
		real m21, real m22, real m23, real m24,
		real m31, real m32, real m33, real m34,
		real m41, real m42, real m43, real m44);

	inline Matrix4<real> operator +(const Matrix4<real>& m) const;
	inline Matrix4<real> operator -(const Matrix4<real>& m) const;
	inline Matrix4<real> operator *(real scale) const;
	inline Matrix4<real> operator /(real scale) const;

	inline const Matrix4<real>& operator +=(Matrix4<real>& m);
	inline const Matrix4<real>& operator -=(Matrix4<real>& m);
	inline const Matrix4<real>& operator *=(real scale);
	inline const Matrix4<real>& operator /=(real scale);

	Matrix4<real>			operator * (const Matrix4<real>& m);
	Matrix4<real>& operator *= (const Matrix4<real>& m);

	template<typename U>
	friend Vector3<U>	operator * (const Vector3<U>& v, const Matrix4<U>& m);

	template<typename U>
	friend Vector3<U>& operator *= (Vector3<U>& v, const Matrix4<U>& m);

	inline real* Ptr();
	inline const real* Ptr() const;
	inline real* GetRow(size_t row) const;

	Quaternion<real> ToQuaternion() const;

	inline void TransposeSelf();
	Matrix4<real> Inverse() const;
	const Matrix4<real>& InverseSelf();

	static Matrix4<real> RotateX(float angle); // in rad
	static Matrix4<real> RotateY(float angle);
	static Matrix4<real> RotateZ(float angle);
	static Matrix4<real> Scale(const Vector3<real>& s);
	static Matrix4<real> RotationAxis(const Vector3<real>& axis, real angle);

	static Matrix4<real> Translate(const Vector3<real>& t);
	static Matrix4<real> Transform(const Quaternion<real>& q, const Vector3<real>& t);
	static Matrix4<real> LookAtLH(const Vector3<real>& vEye,
		const Vector3<real>& vAt = Vector3<real>::ZERO,
		const Vector3<real>& vUp = Vector3<real>::UNIT_Z);

	static Matrix4<real> LookAtRH(const Vector3<real>& vEye,
		const Vector3<real>& vAt = Vector3<real>::ZERO,
		const Vector3<real>& vUp = Vector3<real>::UNIT_Z);

	static Matrix4<real> PerspectiveFovLH(real fovy = Math<real>::PI / 4.0f,
		real aspect = 1.0f, real zn = 1.0f, real zf = 1000.0f);

	static Matrix4<real> PerspectiveFovRH(real fovy = Math<real>::PI / 4.0f,
		real aspect = 1.0f, real zn = 1.0f, real zf = 1000.0f);

	static Matrix4<real> OrthoLH(real width, real height, real zn = 1.0f, real zf = 1000.0f);
	static Matrix4<real> OrthoRH(real width, real height, real zn = 1.0f, real zf = 1000.0f);
	static Matrix4<real> OrthoOffCenterLH(real l, real r, real b, real t,
		real zn = 1.0f, real zf = 1000.0f);
	static Matrix4<real> OrthoOffCenterRH(real l, real r, real b, real t,
		real zn = 1.0f, real zf = 1000.0f);

	// special matrices
	static const Matrix4 ZERO;
	static const Matrix4 IDENTITY;
};

//-----------------------------------------------------------------
template<typename real>
Matrix4<real>::Matrix4(const Matrix4<real>& matrix)
{
	memcpy(m, matrix.m, sizeof(m));
}

//-----------------------------------------------------------------
template<typename real>
Matrix4<real>::Matrix4(const real* data)
{
	memcpy(m, data, sizeof(m));
}
//-----------------------------------------------------------------
template<typename real>
Matrix4<real>::Matrix4(real m11, real m12, real m13, real m14,
	real m21, real m22, real m23, real m24,
	real m31, real m32, real m33, real m34,
	real m41, real m42, real m43, real m44)
{
	_11 = m11;	_12 = m12;	_13 = m13;	_14 = m14;
	_21 = m21;	_22 = m22;	_23 = m23;	_24 = m24;
	_31 = m31;	_32 = m32;	_33 = m33;	_34 = m34;
	_41 = m41;	_42 = m42;	_43 = m43;	_44 = m44;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator=(const Matrix4<real>& rhl)
{
	memcpy(m, rhl.m, sizeof(m));
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline void
Matrix4<real>::Set(real m11, real m12, real m13, real m14,
	real m21, real m22, real m23, real m24,
	real m31, real m32, real m33, real m34,
	real m41, real m42, real m43, real m44)
{
	_11 = m11;	_12 = m12;	_13 = m13;	_14 = m14;
	_21 = m21;	_22 = m22;	_23 = m23;	_24 = m24;
	_31 = m31;	_32 = m32;	_33 = m33;	_34 = m34;
	_41 = m41;	_42 = m42;	_43 = m43;	_44 = m44;
}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real>
Matrix4<real>::operator +(const Matrix4<real>& m) const
{
	return Matrix4<real>(
		_11 + m._11, _12 + m._12, _13 + m._13, _14 + m._14,
		_21 + m._21, _22 + m._22, _23 + m._23, _24 + m._24,
		_31 + m._31, _32 + m._32, _33 + m._33, _34 + m._34,
		_41 + m._41, _42 + m._42, _43 + m._43, _44 + m._44);

}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real>
Matrix4<real>::operator -(const Matrix4<real>& m) const
{
	return Matrix4<real>(
		_11 - m._11, _12 - m._12, _13 - m._13, _14 - m._14,
		_21 - m._21, _22 - m._22, _23 - m._23, _24 - m._24,
		_31 - m._31, _32 - m._32, _33 - m._33, _34 - m._34,
		_41 - m._41, _42 - m._42, _43 - m._43, _44 - m._44);
}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real>
Matrix4<real>::operator *(real scale) const
{
	return Matrix4<real>(
		_11 * scale, _12 * scale, _13 * scale, _14 * scale,
		_21 * scale, _22 * scale, _23 * scale, _24 * scale,
		_31 * scale, _32 * scale, _33 * scale, _34 * scale,
		_41 * scale, _42 * scale, _43 * scale, _44 * scale);
}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real>
Matrix4<real>::operator /(real scale) const
{
	real inv = (real)1.0 / scale;
	return Matrix4<real>(
		_11 * inv, _12 * inv, _13 * inv, _14 * inv,
		_21 * inv, _22 * inv, _23 * inv, _24 * inv,
		_31 * inv, _32 * inv, _33 * inv, _34 * inv,
		_41 * inv, _42 * inv, _43 * inv, _44 * inv);
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator +=(Matrix4<real>& m)
{
	_11 += m._11;	_12 += m._12;	_13 += m._13;	_14 += m._14;
	_21 += m._21;	_22 += m._22;	_23 += m._23;	_24 += m._24;
	_31 += m._31;	_32 += m._32;	_33 += m._33;	_34 += m._34;
	_41 += m._41;	_42 += m._42;	_43 += m._43;	_44 += m._44;
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator -=(Matrix4<real>& m)
{
	_11 -= m._11;	_12 -= m._12;	_13 -= m._13;	_14 -= m._14;
	_21 -= m._21;	_22 -= m._22;	_23 -= m._23;	_24 -= m._24;
	_31 -= m._31;	_32 -= m._32;	_33 -= m._33;	_34 -= m._34;
	_41 -= m._41;	_42 -= m._42;	_43 -= m._43;	_44 -= m._44;
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator *=(real scale)
{
	for (int i = 0; i < 16; i++)
	{
		m[i] *= scale;
	}
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator /=(real scale)
{
	real inv = (real)1.0 / scale;
	for (int i = 0; i < 16; i++)
	{
		m[i] *= inv;
	}
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline real*
Matrix4<real>::Ptr()
{
	return m;
}
//-----------------------------------------------------------------
template<typename real>
inline const real*
Matrix4<real>::Ptr() const
{
	return m;
}
//-----------------------------------------------------------------
template<typename real>
inline real*
Matrix4<real>::GetRow(size_t row) const
{
	return &m[row * 4];
}
//-----------------------------------------------------------------
template<typename real>
inline void
Matrix4<real>::TransposeSelf()
{
	real temp;
	temp = _12; _12 = _21; _21 = temp;
	temp = _13; _13 = _31; _31 = temp;
	temp = _14; _14 = _41; _41 = temp;

	temp = _23; _23 = _32; _32 = temp;
	temp = _24; _24 = _42; _42 = temp;

	temp = _34; _34 = _43; _43 = temp;
}


template<typename real>
Matrix4<real> Matrix4<real>::operator * (const Matrix4<real>& r)
{
	Matrix4 out(
		/*out._11 = */_11 * r._11 + _12 * r._21 + _13 * r._31 + _14 * r._41,
		/*out._12 = */_11 * r._12 + _12 * r._22 + _13 * r._32 + _14 * r._42,
		/*out._13 = */_11 * r._13 + _12 * r._23 + _13 * r._33 + _14 * r._43,
		/*out._14 = */_11 * r._14 + _12 * r._24 + _13 * r._34 + _14 * r._44,

		/*out._21 = */_21 * r._11 + _22 * r._21 + _23 * r._31 + _24 * r._41,
		/*out._22 = */_21 * r._12 + _22 * r._22 + _23 * r._32 + _24 * r._42,
		/*out._23 = */_21 * r._13 + _22 * r._23 + _23 * r._33 + _24 * r._43,
		/*out._24 = */_21 * r._14 + _22 * r._24 + _23 * r._34 + _24 * r._44,

		/*out._31 = */_31 * r._11 + _32 * r._21 + _33 * r._31 + _34 * r._41,
		/*out._32 = */_31 * r._12 + _32 * r._22 + _33 * r._32 + _34 * r._42,
		/*out._33 = */_31 * r._13 + _32 * r._23 + _33 * r._33 + _34 * r._43,
		/*out._34 = */_31 * r._14 + _32 * r._24 + _33 * r._34 + _34 * r._44,

		/*out._41 = */_41 * r._11 + _42 * r._21 + _43 * r._31 + _44 * r._41,
		/*out._42 = */_41 * r._12 + _42 * r._22 + _43 * r._32 + _44 * r._42,
		/*out._43 = */_41 * r._13 + _42 * r._23 + _43 * r._33 + _44 * r._43,
		/*out._44 = */_41 * r._14 + _42 * r._24 + _43 * r._34 + _44 * r._44);
	return out;
}

template<typename real>
Matrix4<real>& Matrix4<real>::operator *= (const Matrix4<real>& m)
{
	*this = *this * m;
	return *this;
}

template<typename U>
Vector3<U> operator * (const Vector3<U>& v, const Matrix4<U>& m)
{
	Vector3<U> out(
		v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41,
		v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42,
		v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43);
	return out;
}

template<typename U>
Vector3<U>& operator *= (Vector3<U>& v, const Matrix4<U>& m)
{
	v = v * m;
	return v;
}

template<typename real>
Matrix4<real> Matrix4<real>::Inverse() const
{
	real fA0 = m[0] * m[5] - m[1] * m[4];
	real fA1 = m[0] * m[6] - m[2] * m[4];
	real fA2 = m[0] * m[7] - m[3] * m[4];
	real fA3 = m[1] * m[6] - m[2] * m[5];
	real fA4 = m[1] * m[7] - m[3] * m[5];
	real fA5 = m[2] * m[7] - m[3] * m[6];
	real fB0 = m[8] * m[13] - m[9] * m[12];
	real fB1 = m[8] * m[14] - m[10] * m[12];
	real fB2 = m[8] * m[15] - m[11] * m[12];
	real fB3 = m[9] * m[14] - m[10] * m[13];
	real fB4 = m[9] * m[15] - m[11] * m[13];
	real fB5 = m[10] * m[15] - m[11] * m[14];

	real fDet = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
	if (Math<real>::Abs(fDet) <= Math<real>::ZERO_TOLERANCE)
	{
		return Matrix4<real>::ZERO;
	}

	return Matrix4<real>(
		+m[5] * fB5 - m[6] * fB4 + m[7] * fB3,
		-m[1] * fB5 + m[2] * fB4 - m[3] * fB3,
		+m[13] * fA5 - m[14] * fA4 + m[15] * fA3,
		-m[9] * fA5 + m[10] * fA4 - m[11] * fA3,
		-m[4] * fB5 + m[6] * fB2 - m[7] * fB1,
		+m[0] * fB5 - m[2] * fB2 + m[3] * fB1,
		-m[12] * fA5 + m[14] * fA2 - m[15] * fA1,
		+m[8] * fA5 - m[10] * fA2 + m[11] * fA1,
		+m[4] * fB4 - m[5] * fB2 + m[7] * fB0,
		-m[0] * fB4 + m[1] * fB2 - m[3] * fB0,
		+m[12] * fA4 - m[13] * fA2 + m[15] * fA0,
		-m[8] * fA4 + m[9] * fA2 - m[11] * fA0,
		-m[4] * fB3 + m[5] * fB1 - m[6] * fB0,
		+m[0] * fB3 - m[1] * fB1 + m[2] * fB0,
		-m[12] * fA3 + m[13] * fA1 - m[14] * fA0,
		+m[8] * fA3 - m[9] * fA1 + m[10] * fA0);

	//	out /= fDet;
}

template<typename real>
const Matrix4<real>& Matrix4<real>::InverseSelf()
{
	*this = Inverse();
}

template<typename real>
Quaternion<real> Matrix4<real>::ToQuaternion() const
{
	Quaternion<real> out;

	const real* mat = this->m;

	real T = mat[0] + mat[5] + mat[10];

	if (T > (real)0.0)
	{
		real S = Math<real>::Sqrt((real)1.0 + T) * (real)2;
		out.x = (mat[6] - mat[9]) / S;
		out.y = (mat[8] - mat[2]) / S;
		out.z = (mat[1] - mat[4]) / S;
		out.w = (real)0.25 * S;
	}
	else if (mat[0] > mat[5] && mat[0] > mat[10])			// Column 0: 
	{
		real S = Math<real>::Sqrt((real)1.0 + mat[0] - mat[5] - mat[10]) * (real)2;
		out.x = (real)0.25 * S;
		out.y = (mat[4] + mat[1]) / S;
		out.z = (mat[2] + mat[8]) / S;
		out.w = (mat[6] - mat[9]) / S;
	}
	else if (mat[5] > mat[10])						// Column 1: 
	{
		real S = Math<real>::Sqrt((real)1.0 + mat[5] - mat[0] - mat[10]) * (real)2;
		out.x = (mat[4] + mat[1]) / S;
		out.y = (real)0.25 * S;
		out.z = (mat[9] + mat[6]) / S;
		out.w = (mat[8] - mat[2]) / S;
	}
	else												// Column 2:
	{
		real S = Math<real>::Sqrt((real)1.0 + mat[10] - mat[0] - mat[5]) * (real)2;
		out.x = (mat[2] + mat[8]) / S;
		out.y = (mat[9] + mat[6]) / S;
		out.z = (real)0.25 * S;
		out.w = (mat[1] - mat[4]) / S;
	}

	return out;
}

template<typename real>
Matrix4<real> Matrix4<real>::RotateX(float angle)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	real spitch = Math<real>::Sin(angle);
	real cpitch = Math<real>::Cos(angle);

	mat._22 = cpitch;
	mat._23 = spitch;
	mat._32 = -spitch;
	mat._33 = cpitch;
	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::RotateY(float angle)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	real syaw = Math<real>::Sin(angle);
	real cyaw = Math<real>::Cos(angle);

	mat._11 = cyaw;
	mat._13 = -syaw;
	mat._31 = syaw;
	mat._33 = cyaw;
	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::RotateZ(float angle)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	real sroll = Math<real>::Sin(angle);
	real croll = Math<real>::Cos(angle);

	mat._11 = croll;
	mat._12 = sroll;
	mat._21 = -sroll;
	mat._22 = croll;
	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::Scale(const Vector3<real>& s)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	mat._11 = s.x;
	mat._22 = s.y;
	mat._33 = s.z;
	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::RotationAxis(const Vector3<real>& axis, real angle)
{
	Vector3<real> v = axis;
	v.Normalize();

	real c = Math<real>::Cos(angle);
	real s = Math<real>::Sin(angle);
	real minusCos = 1 - c;
	real x2 = v.x * v.x;
	real y2 = v.y * v.y;
	real z2 = v.z * v.z;
	real xy = v.x * v.y;
	real xz = v.x * v.z;
	real yz = v.y * v.z;

	return Matrix4<real>(
		x2 * minusCos + c, xy * minusCos + v.z * s, xz * minusCos - v.y * s, (real)0.0,
		xy * minusCos - v.z * s, y2 * minusCos + c, yz * minusCos + v.x * s, (real)0.0,
		xz * minusCos + v.y * s, yz * minusCos - v.x * s, z2 * minusCos + c, (real)0.0,
		(real)0.0, (real)0.0, (real)0.0, (real)1.0);
}

template<typename real>
Matrix4<real> Matrix4<real>::Translate(const Vector3<real>& t)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	mat._41 = t.x;
	mat._42 = t.y;
	mat._43 = t.z;
	return mat;
}

// matrix from rotation and translation
template<typename real>
Matrix4<real> Matrix4<real>::Transform(const Quaternion<real>& q, const Vector3<real>& t)
{
	Matrix4<real> mat = q.ToMat4();
	mat._41 = t.x;
	mat._42 = t.y;
	mat._43 = t.z;

	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::LookAtLH(const Vector3<real>& vEye, const Vector3<real>& vAt, const Vector3<real>& vUp)
{
	/*	zaxis = normal(At - Eye)
	xaxis = normal(cross(Up, zaxis))
	yaxis = cross(zaxis, xaxis)

	xaxis.x           yaxis.x           zaxis.x          0
	xaxis.y           yaxis.y           zaxis.y          0
	xaxis.z           yaxis.z           zaxis.z          0
	-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l*/

	Vector3<real> z = vAt - vEye;
	z.Normalize();
	Vector3<real> x = Vector3f::Cross(vUp, z);
	x.Normalize();
	Vector3<real> y = Vector3f::Cross(z, x);
	return Matrix4<real>(x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		x.z, y.z, z.z, 0.0f,
		-Vector3f::Dot(x, vEye), -Vector3f::Dot(y, vEye), -Vector3f::Dot(z, vEye), 1.0f);
}

template<typename real>
Matrix4<real> Matrix4<real>::LookAtRH(const Vector3<real>& vEye, const Vector3<real>& vAt, const Vector3<real>& vUp)
{
	/*	zaxis = normal(At - Eye)
	xaxis = normal(cross(Up, zaxis))
	yaxis = cross(zaxis, xaxis)

	xaxis.x           yaxis.x           zaxis.x          0
	xaxis.y           yaxis.y           zaxis.y          0
	xaxis.z           yaxis.z           zaxis.z          0
	-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l*/

	Vector3<real> z = vEye - vAt;
	z.Normalize();
	Vector3<real> x = Vector3f::Cross(vUp, z);
	x.Normalize();
	Vector3<real> y = Vector3f::Cross(z, x);
	return Matrix4<real>(x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		x.z, y.z, z.z, 0.0f,
		-Vector3f::Dot(x, vEye), -Vector3f::Dot(y, vEye), -Vector3f::Dot(z, vEye), 1.0f);
}

template<typename real>
Matrix4<real> Matrix4<real>::PerspectiveFovLH(real fovy, real aspect, real zn, real zf)
{
	/*
	xScale     0          0               0
	0        yScale       0               0
	0          0       zf/(zf-zn)         1
	0          0       -zn*zf/(zf-zn)     0
	where:
	yScale = cot(fovY/2)

	xScale = yScale / aspect ratio
	*/

	real yScale = 1.0f / Math<real>::Tan(fovy / 2.0f);
	real xScale = yScale / aspect;
	real zScale = zf / (zf - zn);

	return Matrix4<real>(xScale, 0.0f, 0.0f, 0.0f,
		0.0f, yScale, 0.0f, 0.0f,
		0.0f, 0.0f, zScale, 1.0f,
		0.0f, 0.0f, -zn * zScale, 0.0f);
}
template<typename real>
Matrix4<real> Matrix4<real>::PerspectiveFovRH(real fovy, real aspect, real zn, real zf)
{

	real yScale = 1.0f / Math<real>::Tan(fovy / 2.0f);
	real xScale = yScale / aspect;
	real zScale = zf / (zn - zf);

	return Matrix4<real>(xScale, 0.0f, 0.0f, 0.0f,
		0.0f, yScale, 0.0f, 0.0f,
		0.0f, 0.0f, zScale, -1.0f,
		0.0f, 0.0f, zn * zScale, 0.0f);
}

template<typename real>
Matrix4<real> Matrix4<real>::OrthoLH(real width, real height, real zn, real zf)
{
	real InvDepth = 1.0f / (zf - zn);

	return Matrix4<real>(2.0f / width, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / height, 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		0.0f, 0.0f, -zn * InvDepth, 1.0f);
}
template<typename real>
Matrix4<real> Matrix4<real>::OrthoRH(real width, real height, real zn, real zf)
{
	real InvDepth = 1.0f / (zn - zf);

	return Matrix4<real>(2.0f / width, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / height, 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		0.0f, 0.0f, zn * InvDepth, 1.0f);
}

template<typename real>
Matrix4<real> Matrix4<real>::OrthoOffCenterLH(real l, real r, real b, real t, real zn, real zf)
{
	real InvDepth = 1.0f / (zf - zn);

	return Matrix4<real>(2.0f / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b), 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		(l + r) / (l - r), (t + b) / (b - t), -zn * InvDepth, 1.0f);
}
template<typename real>
Matrix4<real> Matrix4<real>::OrthoOffCenterRH(real l, real r, real b, real t, real zn, real zf)
{
	real InvDepth = 1.0f / (zn - zf);

	return Matrix4<real>(2.0f / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b), 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		(l + r) / (l - r), (t + b) / (b - t), zn * InvDepth, 1.0f);
}


typedef Matrix4<float> Matrix4f;

}

#endif
