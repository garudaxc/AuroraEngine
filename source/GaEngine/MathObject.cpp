#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Color.h"
#include "MathFunction.h"
#include "AurMath.h"

namespace Aurora
{
template<> const float Math<float>::ZERO_TOLERANCE = 1e-06f;
template<> const float Math<float>::MAX_VALUE = 3.4e38f;
template<> const float Math<float>::PI = 3.1415926f;
template<> const float Math<float>::HALF_PI = Math<float>::PI / 2.0f;
template<> const float Math<float>::DEG_TO_RAD = Math<float>::PI / 180.0f;
template<> const float Math<float>::RAD_TO_DEG = 180.0f / Math<float>::PI;
//--------------------------------------------------------------------------
const Color Color::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::RED(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::BLUE(0.0f, 0.0f, 1.0f, 1.0f);
const Color Color::GRAY(0.5f, 0.5f, 0.5f, 1.0f);
const Color Color::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
//--------------------------------------------------------------------------
template<> const Vector2<float> Vector2<float>::ZERO(0.0f,0.0f);
template<> const Vector2<float> Vector2<float>::UNIT_X(1.0f,0.0f);
template<> const Vector2<float> Vector2<float>::UNIT_Y(0.0f,1.0f);
//--------------------------------------------------------------------------
template<> const Vector3<float> Vector3<float>::ZERO(0.0f,0.0f,0.0f);
template<> const Vector3<float> Vector3<float>::ONE(1.0f,1.0f,1.0f);
template<> const Vector3<float> Vector3<float>::UNIT_X(1.0f,0.0f,0.0f);
template<> const Vector3<float> Vector3<float>::UNIT_Y(0.0f,1.0f,0.0f);
template<> const Vector3<float> Vector3<float>::UNIT_Z(0.0f,0.0f,1.0f);
//--------------------------------------------------------------------------
template<> const Vector4<float> Vector4<float>::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
//--------------------------------------------------------------------------
template<> const Matrix3<float> Matrix3<float>::ZERO(0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f);
template<> const Matrix3<float> Matrix3<float>::IDENTITY(1.0f, 0.0f, 0.0f,
														 0.0f, 1.0f, 0.0f,
														 0.0f, 0.0f, 1.0f);
//--------------------------------------------------------------------------
template<> const Matrix4<float> Matrix4<float>::ZERO(0.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 0.0f);

template<> const Matrix4<float> Matrix4<float>::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
														 0.0f, 1.0f, 0.0f, 0.0f,
														 0.0f, 0.0f, 1.0f, 0.0f,
														 0.0f, 0.0f, 0.0f, 1.0f);
//--------------------------------------------------------------------------
template<> const Quaternion<float> Quaternion<float>::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f);

}