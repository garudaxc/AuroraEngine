#pragma once
#include "stdheader.h"

namespace Aurora
{


	class Transform
	{
	public:
		Transform() {}
		virtual ~Transform() {}

		void Init();

		void				SetLocalTranslation(const Vector3f& vTrans);
		void				SetLocalRotation(const Quaternionf& qRot);
		void				SetLocalScale(const Vector3f& vScale);

		void Rotate(const Quaternionf& qRot);
		void Translate(const Vector3f& vDis);

		const Vector3f& GetLocalScale() const { return m_vScale; }

		const Quaternionf& GetWorldRotation() const { return m_qRotWorld; }
		const Vector3f& GetWorldTranslation() const { return m_vPosWorld; }
		const Matrix4f& GetWorldTransform() const { return m_mTransform; }

		virtual void		Update();

	private:

		Vector3f		m_vPosLocal;
		Quaternionf		m_qRotLocal;
		Vector3f		m_vScale;

		Vector3f		m_vPosWorld;
		Quaternionf		m_qRotWorld;

		Matrix4f		m_mTransform;
	};


}