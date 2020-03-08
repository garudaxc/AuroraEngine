#include "Transform.h"


namespace Aurora
{
	
	void Transform::Init()
	{
		m_vPosLocal = Vector3f::ZERO;
		m_qRotLocal = Quaternionf::IDENTITY;
		m_vScale = Vector3f::ONE;
	}

	void Transform::SetLocalTranslation(const Vector3f& vTrans)
	{
		m_vPosLocal = vTrans;
	}

	void Transform::SetLocalRotation(const Quaternionf& qRot)
	{
		m_qRotLocal = qRot;
	}

	void Transform::SetLocalScale(const Vector3f& vScale)
	{
		m_vScale = vScale;
	}

	void Transform::Rotate(const Quaternionf& qRot)
	{
		QuaternionMultiply(m_qRotLocal, qRot, m_qRotLocal);
	}

	void Transform::Translate(const Vector3f& vDis)
	{
		m_vPosLocal += vDis;
	}

	void Transform::Update()
	{
		m_vPosWorld = m_vPosLocal;
		m_qRotWorld = m_qRotLocal;

		MatrixTransform(m_mTransform, m_qRotWorld, m_vPosWorld);
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


	//Vector3f Camera::GetUpDir() const
	//{
	//	GetViewMatrix();
	//	return Vector3f(m_matView._12, m_matView._22, m_matView._32);
	//}
	////------------------------------------------------------------------------
	//Vector3f Camera::GetLookDir() const
	//{
	//	GetViewMatrix();
	//	return Vector3f(-m_matView._13, -m_matView._23, -m_matView._33);
	//}
	////------------------------------------------------------------------------
	//Vector3f Camera::GetRightDir() const
	//{
	//	GetViewMatrix();
	//	return Vector3f(m_matView._11, m_matView._21, m_matView._31);
	//}



}