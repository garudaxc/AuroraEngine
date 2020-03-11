#include "Transform.h"


namespace Aurora
{	


	void Transform::SetTranslation(const Vector3f& vTrans)
	{
		Position_ = vTrans;
	}

	void Transform::SetRotation(const Quaternionf& qRot)
	{
		Rotation_ = qRot;
	}

	void Transform::SetScale(const Vector3f& vScale)
	{
		Scale_ = vScale;
	}

	void Transform::Rotate(const Quaternionf& qRot)
	{
		QuaternionMultiply(Rotation_, qRot, Rotation_);
	}

	void Transform::Translate(const Vector3f& vDis)
	{
		Position_ += vDis;
	}

	Matrix4f Transform::GetMatrix() const
	{
		Matrix4f transform;
		MatrixTransform(transform, Rotation_, Position_);
		return transform;
	}

	void Transform::LootAt(const Vector3f& eye, const Vector3f& lookAt, const Vector3f& up)
	{
		// transform "camera", not construct view matrix(inverse)
		Position_ = eye;

		Vector3f z = eye - lookAt;
		z.Normalize();
		Vector3f x = up.Cross(z);
		x.Normalize();
		Vector3f y = z.Cross(x);

		Matrix4f t( x.x, x.y, x.z, 0.f,
					y.x, y.y, y.z, 0.f,
					z.x, z.y, z.z, 0.f,
					0.f, 0.f, 0.f, 1.f);

		QuaternionFromRotationMatrix(Rotation_, t);
	}

	//const Matrix4f& Camera::GetViewMatrix() const
	//{
	//	if (m_bUpdateView)
	//	{
	//		// right hand coord
	//		Vector3f z = -m_vLookDir;
	//		Vector3f x = m_vRight;
	//		Vector3f y = m_vUp;
	//		Vector3f vEye = m_vEye;

	//		m_matView.Set(x.x, y.x, z.x, 0.0f,
	//			x.y, y.y, z.y, 0.0f,
	//			x.z, y.z, z.z, 0.0f,
	//			-Vector3f::Dot(x, vEye), -Vector3f::Dot(y, vEye), -Vector3f::Dot(z, vEye), 1.0f);

	//		m_bUpdateView = false;
	//	}
	//	return m_matView;
	//}

	Vector3f Transform::UpDir() const
	{
		return Rotation_.Transform(Vector3f::UNIT_Z);
	}
	//------------------------------------------------------------------------
	Vector3f Transform::ForwardDir() const
	{
		return Rotation_.Transform(Vector3f::UNIT_Y);
	}
	//------------------------------------------------------------------------
	Vector3f Transform::RightDir() const
	{
		return Rotation_.Transform(Vector3f::UNIT_X);
	}
	
	Transform	Transform::Inverse() const
	{
		Transform inv;
		inv.Rotation_ = Rotation_.Inverse();
		inv.Position_ = inv.Rotation_.Transform(-Position_);
		return inv;
	}

	const Transform& Transform::InverseSelf()
	{
		*this = Inverse();
		return *this;
	}

}