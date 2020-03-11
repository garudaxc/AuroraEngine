#pragma once
#include "stdheader.h"

namespace Aurora
{


	class Transform
	{
	public:
		void	LootAt(const Vector3f& eye, const Vector3f& lookAt, const Vector3f& up);

		void	SetTranslation(const Vector3f& vTrans);
		void	SetRotation(const Quaternionf& qRot);
		void	SetScale(const Vector3f& vScale);

		void	Rotate(const Quaternionf& qRot);
		void	Translate(const Vector3f& vDis);

		const Vector3f& GetScale() const { return Scale_; }

		const Quaternionf& GetRotation() const { return Rotation_; }
		const Vector3f& GetTranslation() const { return Position_; }

		Matrix4f GetMatrix() const;

		Vector3f	UpDir() const;
		Vector3f	ForwardDir() const;
		Vector3f	RightDir() const;

		Transform	Inverse() const;
		const Transform& InverseSelf();
		
	private:

		Vector3f		Position_ = Vector3f::ZERO;
		Quaternionf		Rotation_ = Quaternionf::IDENTITY;
		Vector3f		Scale_ = Vector3f::ONE;
		
	};


}