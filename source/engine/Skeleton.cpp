#include "stdheader.h"
#include "Skeleton.h"
#include "MathFunction.h"
namespace Aurora
{

	Skeleton::Bone::Bone():m_pParent(nullptr),m_pSibling(nullptr),m_pFirstChild(nullptr)
	{
	}

	Skeleton::Bone::~Bone()
	{
	}

	void Skeleton::Bone::AddSbiling(Bone* pSibling)
	{
		Bone*	p = this;
		while (p->m_pSibling != nullptr)
			p = p->m_pSibling;

		p->m_pSibling = pSibling;
		pSibling->SetParent(m_pParent);
	}

	void Skeleton::Bone::AddChild(Bone* pChild)
	{
		if (m_pFirstChild == nullptr)
		{
			m_pFirstChild	= pChild;
			pChild->SetParent(this);
		}
		else
		{
			m_pFirstChild->AddSbiling(pChild);
		}
	}

	int Skeleton::Bone::GetCount() const
	{
		return (m_pSibling == nullptr ? 0 : m_pSibling->GetCount()) +
			(m_pFirstChild == nullptr ? 0 : m_pFirstChild->GetCount()) + 1;
	}


	void Skeleton::Bone::UpdateTransformation()
	{
		if (m_pParent)
		{
			QuaternionMultiply(m_qRotationDerived, m_pParent->m_qRotationDerived, m_qRotationLocal);
			Vector3Rotate(m_vTransDerived, m_vTransLocal, m_pParent->m_qRotationDerived);
			m_vTransDerived += m_pParent->m_vTransDerived;
		}
		else
		{
			m_vTransDerived = m_vTransLocal;
			m_qRotationDerived = m_qRotationLocal;
			m_fScaleDerived = m_fScaleLocal;
		}


		if (m_pSibling != nullptr)
		{
			m_pSibling->UpdateTransformation();
		}

		if (m_pFirstChild != nullptr)
		{
			m_pFirstChild->UpdateTransformation();
		}

		// multiply inverse transform matrix to the combined matrix;
		//MatrixMultiply(m_matInvTrans, m_matCombined, m_matCombined);
	}

	//===============
	//
	//===============
	Skeleton::Skeleton(void):m_pRootBone(nullptr)
	{
	}

	Skeleton::~Skeleton(void)
	{
	}

	// setup bone hierarchy by parent index for every bone
	// root bone's parent index is -1
	bool Skeleton::SetupHierarchy(int nBone, int* pBoneParent)
	{
		Allocate(nBone);

		// create bone hierarchy
		Bone* pRootBone = nullptr;
		Bone* pParentBone = nullptr;

		for (int i = 0; i < GetNumBones(); i++)
		{
			int nParentIndex = pBoneParent[i];
			if (nParentIndex == -1)
			{
				// if already had a root bone, error
				if (pRootBone != nullptr)
					return false;
				pRootBone = GetBone(i);
			}
			else
			{
				// add this bone as parent's child
				pParentBone = GetBone(nParentIndex);
				pParentBone->AddChild(GetBone(i));
			}
		}

		// if not find a root bone or not all bones get in hierarchy, error
		if (pRootBone == nullptr || pRootBone->GetCount() != GetNumBones())
			return false;

		m_pRootBone = pRootBone;

		return true;
	}

	void Skeleton::Allocate(int nBones)
	{
		m_aBones.resize(nBones);
		m_aBoneNames.resize(nBones);
	}


	void Skeleton::GetTransformations(Matrix4f* pMatrix)
	{
		Matrix4f	mat;
		Matrix4f*	pMatBone = pMatrix;
		Bone*		pBone = nullptr;

		for (int i = 0; i < GetNumBones(); i++)
		{
			pBone = this->GetBone(i);
			MatrixTransform(mat, pBone->m_qRotationDerived, pBone->m_vTransDerived);

			// combine the transformation with offset matrix
			MatrixMultiply(*pMatBone, pBone->m_matOffset, mat);

			pMatBone++;
		}
	}

}// end of namespace