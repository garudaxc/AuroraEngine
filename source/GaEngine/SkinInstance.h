#ifndef GASKININSTANCE
#define GASKININSTANCE
#include "Skeleton.h"
#include "Animation.h"

namespace Aurora
{

class SkinInstance
{
public:
	SkinInstance(void);
	~SkinInstance(void);

	bool				SetAnimationTrack(int nTrack, AnimationSet* pAnimSet);
	AnimationTrack*		GetAnimationTrack(int nTrack);

	const Matrix4f*		GetTransformMatrix() const { return &m_matTrans[0]; }
	Matrix4f*			GetTransformMatrix() { return &m_matTrans[0]; }

	void				AdvanceTime(float fDeltaTime);
	void				RegisterSkeleton(Skeleton* pSkeleton);


private:
	Skeleton*						m_pSkeleton;
	std::vector<Matrix4f>			m_matTrans;
	AnimationController				m_AnimController;
};


}//end of namespace

#endif