#include "stdHeader.h"
#include "SkinInstance.h"


namespace Aurora
{


SkinInstance::SkinInstance(void)
{
}

SkinInstance::~SkinInstance(void)
{
}


bool SkinInstance::SetAnimationTrack(int nTrack, AnimationSet* pAnimSet)
{
	return m_AnimController.SetAnimationTrack(nTrack, pAnimSet);
}

AnimationTrack* SkinInstance::GetAnimationTrack(int nTrack)
{
	return m_AnimController.GetAnimationTrack(nTrack);
}

void SkinInstance::RegisterSkeleton(Skeleton* pSkeleton)
{
	m_pSkeleton = pSkeleton;
	m_matTrans.resize(pSkeleton->GetNumBones());
	m_AnimController.RegisterSkeleton(pSkeleton);

	for (int i = 0; i < pSkeleton->GetNumBones(); i++)
		m_matTrans[i] = Matrix4f::IDENTITY;
}

void SkinInstance::AdvanceTime(float fDeltaTime)
{
	m_AnimController.AdvanceTime(fDeltaTime);
	m_pSkeleton->UpdateTransformation();
	m_pSkeleton->GetTransformations(&m_matTrans[0]);
}



}