#ifndef GAVECTOR2_H
#define GAVECTOR2_H

#include <string>

namespace Aurora
{

	template<typename T>
	class Vector2
	{
	public:
		Vector2();
		Vector2(T fx, T fy);
		Vector2(const Vector2<T>& v);
		Vector2(const T v[2]);


		inline const Vector2<T>& operator = (const Vector2<T>& v);

		inline void Set(T fx, T fy);
		inline void Set(const T* ptr);

		inline T& operator [](size_t index);
		inline const T& operator[](size_t index) const;

		inline Vector2<T> operator -() const;
		inline Vector2<T> operator +(const Vector2<T>& v) const;
		inline Vector2<T> operator -(const Vector2<T>& v) const;
		inline Vector2<T> operator *(T scale) const;
		inline Vector2<T> operator /(T scale) const;

		inline const Vector2<T>& operator +=(const Vector2<T>& v);
		inline const Vector2<T>& operator -=(const Vector2<T>& v);
		inline const Vector2<T>& operator *=(T scale);
		inline const Vector2<T>& operator /=(T scale);

		inline T* Ptr();
		inline const T* Ptr() const;

		inline T LengthSQ()	const;
		inline T Length()	const;

		inline void Normalize();

		T Dot(const Vector2<T>& rhs) const;
		static T Dot(const Vector2<T>& lhs, const Vector2<T>& rhs);

		static Vector2<T> Lerp(const Vector2<T>& a, const Vector2<T>& b, T s);
		static Vector2<T> Modulate(const Vector2<T>& a, const Vector2<T>& b);

		static const Vector2 ZERO;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;

		T	x, y;
	};


	//---------------------------------------------------------------------
	template<typename T>
	class Vector3
	{
	public:
		Vector3();
		Vector3(T fx, T fy, T fz);
		Vector3(const Vector3<T>& v);
		Vector3(const T v[3]);

		inline const Vector3<T>& operator = (const Vector3<T>& v);

		inline void Set(T fx, T fy, T fz);
		inline void Set(const T* ptr);

		inline T& operator [](size_t index);
		inline const T& operator[](size_t index) const;

		inline Vector3<T> operator -() const;
		inline Vector3<T> operator +(const Vector3<T>& v) const;
		inline Vector3<T> operator -(const Vector3<T>& v) const;
		inline Vector3<T> operator *(T scale) const;
		inline Vector3<T> operator /(T scale) const;

		inline const Vector3<T>& operator +=(const Vector3<T>& v);
		inline const Vector3<T>& operator -=(const Vector3<T>& v);
		inline const Vector3<T>& operator *=(T scale);
		inline const Vector3<T>& operator /=(T scale);

		inline T LengthSQ()	const;
		inline T Length()	const;

		inline void Normalize();

		inline T Dot(const Vector3<T>& v) const;
		static T Dot(const Vector3<T>& l, const Vector3<T>& r);

		inline Vector3<T> Cross(const Vector3<T>& v) const;
		static Vector3<T> Cross(const Vector3<T>& l, const Vector3<T>& r);

		inline T* Ptr();
		inline const T* Ptr() const;

		static Vector3<T> Lerp(const Vector3<T>& a, const Vector3<T>& b, T s);

		inline std::string ToString() const;
		static Vector3<T> FromString(const char* str);

		static const Vector3 ZERO;
		static const Vector3 ONE;
		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;

		T	x, y, z;
	};


	//---------------------------------------------------------------------
	template<typename T>
	class Vector4
	{
	public:
		Vector4();
		Vector4(T fx, T fy, T fz, T fw);
		Vector4(const Vector4<T>& v);

		inline const Vector4<T>& operator = (const Vector4<T>& v);

		inline void Set(T fx, T fy, T fz, T fw);
		inline void Set(const T* ptr);

		inline T& operator [](size_t index);
		inline const T& operator[](size_t index) const;

		inline Vector4<T> operator -() const;
		inline Vector4<T> operator +(const Vector4<T>& v) const;
		inline Vector4<T> operator -(const Vector4<T>& v) const;
		inline Vector4<T> operator *(T scale) const;
		inline Vector4<T> operator /(T scale) const;

		inline const Vector4<T>& operator +=(const Vector4<T>& v);
		inline const Vector4<T>& operator -=(const Vector4<T>& v);
		inline const Vector4<T>& operator *=(T scale);
		inline const Vector4<T>& operator /=(T scale);

		inline Vector3<T> ProjectToVector3() const;

		//clamp the color value between 0.0 and 1.0
		inline void EnValidate();

		inline T LengthSQ()	const;
		inline T Length()	const;

		inline void Normalize();

		T Dot(const Vector4<T>& rhs) const;
		static T Dot(const Vector4<T>& lhs, const Vector4<T>& rhs);

		unsigned int	ToDWORD() const;

		inline std::string ToString() const;
		static Vector3<T> FromString(const char* str);

		inline T* Ptr();
		inline const T* Ptr() const;

		static const Vector4 ZERO;

		static const Vector4 WHITE;
		static const Vector4 BLACK;
		static const Vector4 RED;
		static const Vector4 GREEN;
		static const Vector4 BLUE;
		static const Vector4 GRAY;

		union
		{
			struct
			{
				T	x, y, z, w;
			};

			struct
			{
				T	r, g, b, a;
			};
		};
	};



	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------
	template<typename T>
	Vector2<T>::Vector2() {}
	//---------------------------------------------------------------
	template<typename T>
	Vector2<T>::Vector2(T fx, T fy)
	{
		x = fx;
		y = fy;
	}
	//---------------------------------------------------------------
	template<typename T>
	Vector2<T>::Vector2(const Vector2<T>& v)
	{
		x = v.x;
		y = v.y;
	}
	//---------------------------------------------------------------
	template<typename T>
	Vector2<T>::Vector2(const T v[2])
	{
		x = v[0];
		y = v[1];
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector2<T>&
		Vector2<T>::operator = (const Vector2<T>& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector2<T>::Set(T fx, T fy)
	{
		x = fx; y = fy;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector2<T>::Set(const T* ptr)
	{
		x = ptr[0];	y = ptr[1];
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T& Vector2<T>::operator [](size_t index)
	{
		return *(&x + index);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const T& Vector2<T>::operator[](size_t index) const
	{
		return *(&x + index);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector2<T> Vector2<T>::operator -() const
	{
		return Vector2<T>(-x, -y);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector2<T>
		Vector2<T>::operator +(const Vector2<T>& v) const
	{
		return Vector2<T>(x + v.x, y + v.y);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector2<T>
		Vector2<T>::operator -(const Vector2<T>& v) const
	{
		return Vector2<T>(x - v.x, y - v.y);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector2<T>
		Vector2<T>::operator *(T scale) const
	{
		return Vector2<T>(x * scale, y * scale);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector2<T>
		Vector2<T>::operator /(T scale) const
	{
		T reciprocal = (T)1.0 / scale;
		return Vector2<T>(x * reciprocal, y * reciprocal);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector2<T>&
		Vector2<T>::operator +=(const Vector2<T>& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector2<T>&
		Vector2<T>::operator -=(const Vector2<T>& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector2<T>&
		Vector2<T>::operator *=(T scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector2<T>&
		Vector2<T>::operator /=(T scale)
	{
		T reciprocal = (T)1.0 / scale;
		x *= reciprocal;
		y *= reciprocal;
		return *this;
	}

	//---------------------------------------------------------------
	template<typename T>
	inline T*
		Vector2<T>::Ptr()
	{
		return &x;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const T*
		Vector2<T>::Ptr() const
	{
		return &x;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T
		Vector2<T>::LengthSQ()	const
	{
		return x * x + y * y;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T
		Vector2<T>::Length()	const
	{
		return Math<T>::Sqrt(x * x + y * y);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector2<T>::Normalize()
	{
		T len = Length();
		if (len > Math<T>::ZERO_TOLERANCE)
		{
			T invLen = 1.0f / len;
			x *= invLen;
			y *= invLen;
		}
		else
		{
			x = y = (T)0.0;
		}
	}

	//---------------------------------------------------------------
	template<typename T>
	inline Vector2<T> operator / (T a, const Vector2<T> v)
	{
		return Vector2<T>(a / v.x, a / v.y);
	}


	template<typename T>
	inline T Vector2<T>::Dot(const Vector2<T>& rhs) const
	{
		return x * rhs.x +
			y * rhs.y;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T
		Vector2<T>::Dot(const Vector2<T>& lhs, const Vector2<T>& rhs)
	{
		return lhs.x * rhs.x +
			lhs.y * rhs.y;
	}

	template<typename T>
	Vector2<T> Vector2<T>::Lerp(const Vector2<T>& a, const Vector2<T>& b, T s)
	{
		return Vector2<T>(
			a.x + s * (b.x - a.x),
			a.y + s * (b.y - a.y));
	}

	template<typename T>
	Vector2<T> Vector2<T>::Modulate(const Vector2<T>& a, const Vector2<T>& b)
	{
		return Vector2<T>(a.x * b.x, a.y * b.y);
	}

	typedef	Vector2<float>	Vector2f;

	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	Vector3<T>::Vector3() {}
	//---------------------------------------------------------------
	template<typename T>
	Vector3<T>::Vector3(T fx, T fy, T fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}
	//---------------------------------------------------------------
	template<typename T>
	Vector3<T>::Vector3(const Vector3<T>& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	template<typename T>
	Vector3<T>::Vector3(const T v[3])
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector3<T>&
		Vector3<T>::operator = (const Vector3<T>& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector3<T>::Set(T fx, T fy, T fz)
	{
		x = fx; y = fy; z = fz;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector3<T>::Set(const T* ptr)
	{
		x = ptr[0];	y = ptr[1];	z = ptr[2];
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T& Vector3<T>::operator [](size_t index)
	{
		return *(&x + index);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const T& Vector3<T>::operator[](size_t index) const
	{
		return *(&x + index);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector3<T> Vector3<T>::operator -() const
	{
		return Vector3<T>(-x, -y, -z);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector3<T>
		Vector3<T>::operator +(const Vector3<T>& v) const
	{
		return Vector3<T>(x + v.x, y + v.y, z + v.z);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector3<T>
		Vector3<T>::operator -(const Vector3<T>& v) const
	{
		return Vector3<T>(x - v.x, y - v.y, z - v.z);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector3<T>
		Vector3<T>::operator *(T scale) const
	{
		return Vector3<T>(x * scale, y * scale, z * scale);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector3<T>
		Vector3<T>::operator /(T scale) const
	{
		T reciprocal = (T)1.0 / scale;
		return Vector3<T>(x * reciprocal, y * reciprocal, z * reciprocal);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector3<T>&
		Vector3<T>::operator +=(const Vector3<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector3<T>&
		Vector3<T>::operator -=(const Vector3<T>& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector3<T>&
		Vector3<T>::operator *=(T scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector3<T>&
		Vector3<T>::operator /=(T scale)
	{
		T reciprocal = (T)1.0 / scale;
		x *= reciprocal;
		y *= reciprocal;
		z *= reciprocal;
		return *this;
	}

	//---------------------------------------------------------------
	template<typename T>
	inline T*
		Vector3<T>::Ptr()
	{
		return &x;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const T*
		Vector3<T>::Ptr() const
	{
		return &x;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T
		Vector3<T>::LengthSQ()	const
	{
		return x * x + y * y + z * z;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T
		Vector3<T>::Length()	const
	{
		return Math<T>::Sqrt(x * x + y * y + z * z);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector3<T>::Normalize()
	{
		T len = Length();
		if (len > Math<T>::ZERO_TOLERANCE)
		{
			T invLen = (T)1.0 / len;
			x *= invLen;
			y *= invLen;
			z *= invLen;
		}
		else
		{
			x = y = z = (T)0.0;
		}
	}

	//---------------------------------------------------------------
	template<typename T>
	inline Vector3<T> operator / (T a, const Vector3<T> v)
	{
		return Vector3<T>(a / v.x, a / v.y, a / v.z);
	}
	//---------------------------------------------------------------
	template<typename T>
	T Vector3<T>::Dot(const Vector3<T>& l, const Vector3<T>& r)
	{
		return l.x * r.x +
			l.y * r.y +
			l.z * r.z;
	}

	template<typename T>
	inline T Vector3<T>::Dot(const Vector3<T>& v) const
	{
		return Vector3<T>::Dot(*this, v);
	}

	template<typename T>
	Vector3<T> Vector3<T>::Cross(const Vector3<T>& l, const Vector3<T>& r)
	{
		return Vector3<T>(l.y * r.z - l.z * r.y,
			l.z * r.x - l.x * r.z,
			l.x * r.y - l.y * r.x);
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& v) const
	{
		return Vector3<T>::Cross(*this, v);
	}

	template<typename T>
	Vector3<T> Vector3<T>::Lerp(const Vector3<T>& a, const Vector3<T>& b, T s)
	{
		return Vector3<T>(
			a.x + s * (b.x - a.x),
			a.y + s * (b.y - a.y),
			a.z + s * (b.z - a.z));
	}

	template<typename T>
	Vector3<T> Modulate(const Vector3<T>& a, const Vector3<T>& b)
	{
		return Vector3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
	}
	//--------------------------------------------------------------------------

	template<>
	inline std::string Vector3<float>::ToString() const
	{
		char buff[64];
		sprintf(buff, "%f,%f,%f", x, y, z);
		std::string s = buff;
		return s;
	}

	typedef	Vector3<float>	Vector3f;

	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------
	template<typename T>
	Vector4<T>::Vector4() {}
	//---------------------------------------------------------------
	template<typename T>
	Vector4<T>::Vector4(T fx, T fy, T fz, T fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}
	//---------------------------------------------------------------
	template<typename T>
	Vector4<T>::Vector4(const Vector4<T>& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector4<T>&
		Vector4<T>::operator = (const Vector4<T>& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector4<T>::Set(T fx, T fy, T fz, T fw)
	{
		x = fx; y = fy; z = fz; w = fw;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector4<T>::Set(const T* ptr)
	{
		x = ptr[0];	y = ptr[1];	z = ptr[2]; w = ptr[3];
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T& Vector4<T>::operator [](size_t index)
	{
		return *(&x + index);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const T& Vector4<T>::operator[](size_t index) const
	{
		return *(&x + index);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector4<T> Vector4<T>::operator -() const
	{
		return Vector4<T>(-x, -y, -z, -w);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector4<T>
		Vector4<T>::operator +(const Vector4<T>& v) const
	{
		return Vector4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector4<T>
		Vector4<T>::operator -(const Vector4<T>& v) const
	{
		return Vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector4<T>
		Vector4<T>::operator *(T scale) const
	{
		return Vector4<T>(x * scale, y * scale, z * scale, w * scale);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector4<T>
		Vector4<T>::operator /(T scale) const
	{
		T reciprocal = (T)1.0 / scale;
		return Vector4<T>(x * reciprocal, y * reciprocal, z * reciprocal, w * reciprocal);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector4<T>&
		Vector4<T>::operator +=(const Vector4<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector4<T>&
		Vector4<T>::operator -=(const Vector4<T>& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector4<T>&
		Vector4<T>::operator *=(T scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const Vector4<T>&
		Vector4<T>::operator /=(T scale)
	{
		T reciprocal = (T)1.0 / scale;
		x *= reciprocal;
		y *= reciprocal;
		z *= reciprocal;
		w *= reciprocal;
		return *this;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline Vector3<T> Vector4<T>::ProjectToVector3() const
	{
		T reciprocal = (T)1.0 / w;
		return Vector3<T>(x * reciprocal, y * reciprocal, z * reciprocal);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T*
		Vector4<T>::Ptr()
	{
		return &x;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline const T*
		Vector4<T>::Ptr() const
	{
		return &x;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T
		Vector4<T>::LengthSQ()	const
	{
		return x * x + y * y + z * z + w * w;
	}
	//---------------------------------------------------------------
	template<typename T>
	inline T
		Vector4<T>::Length()	const
	{
		return Math<T>::Sqrt(x * x + y * y + z * z + w * w);
	}
	//---------------------------------------------------------------
	template<typename T>
	inline void
		Vector4<T>::Normalize()
	{
		T len = Length();
		if (len > Math<T>::ZERO_TOLERANCE)
		{
			T invLen = (T)1.0 / len;
			x *= invLen;
			y *= invLen;
			z *= invLen;
			w *= invLen;
		}
		else
		{
			x = y = z = w = (T)0.0;
		}
	}

	template<typename T>
	T Vector4<T>::Dot(const Vector4<T>& lhs, const Vector4<T>& rhs)
	{
		return lhs.x * rhs.x +
			lhs.y * rhs.y +
			lhs.z * rhs.z +
			lhs.w * rhs.w;
	}

	template<typename T>
	inline T Vector4<T>::Dot(const Vector4<T>& rhs) const
	{
		return Vector4<T>::Dot(*this, rhs);
	}

	template<typename T>
	inline T
	DotProduct(const Vector4<T>& l, const Vector4<T>& r)
	{
		return l.Dot(r);
	}


	template<typename T>
	inline void Vector4<T>::EnValidate()
	{
		r = Math<T>::Clamp(r, (T)0.0, (T)1.0);
		g = Math<T>::Clamp(g, (T)0.0, (T)1.0);
		b = Math<T>::Clamp(b, (T)0.0, (T)1.0);
		a = Math<T>::Clamp(a, (T)0.0, (T)1.0);
	}

	template<typename T>
	inline std::string Vector4<T>::ToString() const
	{
		return std::string();
	}

	template<typename T>
	Vector3<T> Vector4<T>::FromString(const char* str)
	{
		return  Vector4<T>::ZERO;
	}

	template<typename T>
	unsigned int Vector4<T>::ToDWORD() const
	{
		unsigned char dx = (unsigned char)(r * 255.0f);
		unsigned char dy = (unsigned char)(g * 255.0f);
		unsigned char dz = (unsigned char)(b * 255.0f);
		unsigned char dw = (unsigned char)(a * 255.0f);
		return (dx << 0) | (dy << 8) | (dz << 16) | (dw << 24);
	}

	//---------------------------------------------------------------
	template<typename T>
	inline Vector4<T>
		Modulate(const Vector4<T>& lhs, const Vector4<T>& rhs)
	{
		return Vector4<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
	}

	typedef	Vector4<float>	Vector4f;



}


#endif
