#ifndef GAQUATERNION_H
#define GAQUATERNION_H

#include <string>
#include <Vector.h>

namespace Aurora
{

template<typename T>
class Quaternion
{
public:
	Quaternion(void)	{};
	Quaternion(T fw, T fx, T fy, T fz);
	Quaternion(const Quaternion<T>& q);
	Quaternion(T fx, T fy, T fz);
	Quaternion(const Vector3<T>& v, T angle);

	inline const Quaternion<T>& operator = (const Quaternion<T>& q);

	inline Quaternion<T> operator -() const;
	inline Quaternion<T> operator +(const Quaternion<T>& q) const;
	inline Quaternion<T> operator -(const Quaternion<T>& q) const;
	inline Quaternion<T> operator *(T scale) const;
	inline Quaternion<T> operator /(T scale) const;

	inline const Quaternion<T>& operator +=(const Quaternion<T>& q);
	inline const Quaternion<T>& operator -=(const Quaternion<T>& q);
	inline const Quaternion<T>& operator *=(T scale);
	inline const Quaternion<T>& operator /=(T scale);

	inline void Set(T fw, T fx, T fy, T fz);
	inline void Set(const T* ptr);
	inline void Set(T fx, T fy, T fz);

	inline T*		Ptr();
	inline const T* Ptr() const;

	inline T LengthSQ()	const;
	inline T Length()	const;

	inline void Normalize();

	const Quaternion<T>& RotationAxis(const Vector3<T>& v, T angle);
	Vector3<T>&& Transform(const Vector3<T>& v) const;

	const Quaternion<T> Inverse() const;
	const Quaternion<T>& InverseSelf();

	float w, x, y, z;
	
	static const Quaternion IDENTITY;
};



//-------------------------------------------------------------------
template<typename T>
Quaternion<T>::Quaternion(T fw, T fx, T fy, T fz)
{
	w = fw;		x = fx;		y = fy;		z = fz;
}
//-------------------------------------------------------------------
template<typename T>
Quaternion<T>::Quaternion(const Quaternion<T>& q)
{
	w = q.w;	x = q.x;	y = q.y;	z = q.z;
}
template<typename T>
Quaternion<T>::Quaternion(const Vector3<T>& v, T angle)
{
	RotationAxis(v, angle);
}
//-------------------------------------------------------------------
template<typename T>
inline const Quaternion<T>&
Quaternion<T>::operator = (const Quaternion<T>& q)
{
	w = q.w;	x = q.x;	y = q.y;	z = q.z;
	return *this;
}
//-------------------------------------------------------------------
template<typename T>
Quaternion<T>::Quaternion(T fx, T fy, T fz)
{
	x = fx;	y = fy;	z = fz;
	w = Math<T>::Sqrt((T)1.0 - x * x - y * y - z * z);
}
//-------------------------------------------------------------------
template<typename T>
inline void
Quaternion<T>::Set(T fw, T fx, T fy, T fz)
{
	w = fw;		x = fx;		y = fy;		z = fz;
}
//-------------------------------------------------------------------
template<typename T>
inline void
Quaternion<T>::Set(const T* ptr)
{
	w = ptr[0];	x = ptr[1];	y = ptr[2];	z = ptr[3];
}
//-------------------------------------------------------------------
template<typename T>
inline void
Quaternion<T>::Set(T fx, T fy, T fz)
{
	x = fx;	y = fy;	z = fz;
	w = Math<T>::Sqrt((T)1.0 - x * x - y * y - z * z);
}
//---------------------------------------------------------------
template<typename T>
inline T 
Quaternion<T>::LengthSQ()	const
{
	return w * w + x * x + y * y + z * z;
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T> 
Quaternion<T>::operator -() const
{
	return Quaternion<T>(-w, -x, -y, -z);
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T> 
Quaternion<T>::operator +(const Quaternion<T>& q) const
{
	return Quaternion<T>(w+q.w, x+q.x, y+q.y, z+q.z);
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T> 
Quaternion<T>::operator -(const Quaternion<T>& q) const
{
	return Quaternion<T>(w-q.w, x-q.x, y-q.y, z-q.z);
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T> 
Quaternion<T>::operator *(T scale) const
{
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T> 
Quaternion<T>::operator /(T scale) const
{
	T inv = (T)1.0 / scale;
	return Quaternion<T>(w * inv, x * inv, y * inv, z * inv);
}
//---------------------------------------------------------------
template<typename T>
inline const Quaternion<T>& 
Quaternion<T>::operator +=(const Quaternion<T>& q)
{
	w += q.w;
	x += q.x;
	y += q.y;
	z += q.z;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Quaternion<T>& 
Quaternion<T>::operator -=(const Quaternion<T>& q)
{
	w -= q.w;
	x -= q.x;
	y -= q.y;
	z -= q.z;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Quaternion<T>& 
Quaternion<T>::operator *=(T scale)
{
	w *= scale;
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Quaternion<T>& 
Quaternion<T>::operator /=(T scale)
{
	T inv = (T)1.0 / scale;
	w *= inv;
	x *= inv;
	y *= inv;
	z *= inv;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline T* 
Quaternion<T>::Ptr()
{
	return &w;
}
//---------------------------------------------------------------
template<typename T>
inline const T* 
Quaternion<T>::Ptr() const
{
	return &w;
}
//---------------------------------------------------------------
template<typename T>
inline T 
Quaternion<T>::Length()	const
{
	return Math<T>::Sqrt(w * w + x * x + y * y + z * z);
}
//---------------------------------------------------------------
template<typename T>
inline void 
Quaternion<T>::Normalize()
{
	T invLen = 1.0f / Length();
	w *= invLen;
	x *= invLen;
	y *= invLen;
	z *= invLen;
}

template<typename T>
Vector3<T>&& Quaternion<T>::Transform(const Vector3<T>& v) const
{
	Vector3<T> b(x, y, z);
	float b2 = b.x * b.x + b.y * b.y + b.z * b.z;
	return move(v * (w * w - b2) + b * (v.Dot(b) * 2.0)
		+ b.Cross(v) * (w * 2.0));
}

template<typename T>
const Quaternion<T>& Quaternion<T>::RotationAxis(const Vector3<T>& v, T angle)
{
	T halfAngle = ((T)0.5) * angle;
	T fSin = Math<T>::Sin(halfAngle);
	w = Math<T>::Cos(halfAngle);
	x = fSin * v.x;
	y = fSin * v.y;
	z = fSin * v.z;
	return *this;
}


template<typename T>
const Quaternion<T> Quaternion<T>::Inverse() const
{
	return Quaternion<T>(w, -x, -y, -z);
}

template<typename T>
const Quaternion<T>& Quaternion<T>::InverseSelf()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

//-------------------------------------------------------------------

template<typename T>
inline T
DotProduct(const Quaternion<T>& ql, const Quaternion<T>& qr)
{
	return ql.w * qr.w + 
		ql.x * qr.x + 
		ql.y * qr.y + 
		ql.z * qr.z;
}

typedef Quaternion<float> Quaternionf;

inline Quaternionf ParseRotation(const std::string& str)
{
	float x, y, z, w;
	sscanf_s(str.c_str(), "%f,%f,%f,%f", &x, &y, &z, &w);
	return Quaternionf(w, x, y, z);	
}

inline Quaternionf ParseRotation(const char* pStr)
{
	float x, y, z, w;
	sscanf_s(pStr, "%f,%f,%f,%f", &x, &y, &z, &w);
	return Quaternionf(w, x, y, z);	
}

}



#endif