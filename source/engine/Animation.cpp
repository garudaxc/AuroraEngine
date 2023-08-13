#include "stdheader.h"
#include "Animation.h"
#include <algorithm>
#include "Util.h"
#include "MathFunction.h"

//#include "MeshLoader.h"
#include "Skeleton.h"
namespace Aurora
{


/*
================
AnimationSet
================
*/
AnimationSet::AnimationSet(const std::string& name):m_strName(name),m_nNumFrames(0),m_nNumKeys(0)
{
}

AnimationSet::AnimationSet(const std::string& name, int nNumKeys, int nNumFrames):m_strName(name)
{
	this->Allocate(nNumKeys, nNumFrames);
}

AnimationSet::~AnimationSet()
{
}

void AnimationSet::Allocate(int nNumKeys, int nNumFrames)
{
	//m_aKeyFrames.resize(boost::extents[nNumFrames][nNumKeys]);
	m_aTimeSamples.resize(nNumFrames);
	m_aKeyNames.resize(nNumKeys);
	m_nNumFrames	= nNumFrames;
	m_nNumKeys		= nNumKeys;
}

float AnimationSet::GetPeriod() const
{
	if (this->GetNumFrames() == 0)
		return 0.0f;

	return m_aTimeSamples.back();
}

void AnimationSet::SetKeyFrame(int nFrame, int nKey, const AnimationSet::KeyFrame& keyFrame)
{
	m_aKeyFrames[nFrame][nKey] = keyFrame;
}

int AnimationSet::FindKeyName(const std::string& name) const
{
	std::vector<std::string>::const_iterator it = std::find(m_aKeyNames.begin(), m_aKeyNames.end(), name);
	if (it == m_aKeyNames.end())
		return -1;

	return (int)std::distance(m_aKeyNames.begin(), it);
}

const AnimationSet::KeyFrame&	 AnimationSet::GetKeyFrame(int nFrame, int nKey)
{
	return m_aKeyFrames[nFrame][nKey];
}

AnimationSet* CreateBindPos(AnimationSet* pAnimSet)
{
	AnimationSet* pAnim = new AnimationSet("bind pos", pAnimSet->GetNumKeys(), 1);
	pAnim->SetTimeSample(0, 10.0f);
	AnimationSet::KeyFrame key;
	key.fScale = 1.0f;
	key.vTranslation = Vector3f::ZERO;
	key.qRotation = Quaternionf::IDENTITY;
	for (int i = 0; i < pAnimSet->GetNumKeys(); i++)
	{
		pAnim->SetKeyName(i, pAnimSet->GetKeyName(i));
		pAnim->SetKeyFrame(0, i, key);
	}

	return pAnim;

}

/*
================
AnimationTrack
================
*/
AnimationTrack::AnimationTrack():m_pAnimSet(nullptr),m_nPlaybackType(PLAYBACK_LOOP),
m_bEnable(false),m_fWeight(0.0f),m_fTrackTime(0.0f),m_fAnimTime(0.0f),m_fSpeed(1.0f)
{
	m_FrameInter.nFrame0 = 0;
	m_FrameInter.nFrame1 = 0;
	m_FrameInter.fLerp = 0.0f;
}

AnimationTrack::~AnimationTrack()
{
}

void AnimationTrack::SetAnimationSet(AnimationSet* pAnimSet)
{
	assert(pAnimSet != nullptr);
	m_pAnimSet		= pAnimSet;
	m_bEnable		= true;
	m_fWeight		= 1.0f;
	m_fTrackTime	= 0.0f;
	m_fAnimTime		= 0.0f;
	m_fSpeed		= 1.0f;
}

void AnimationTrack::ResetAnimation()
{	
	m_pAnimSet		= nullptr;
	m_bEnable		= false;
	m_fWeight		= 1.0f;
	m_fTrackTime	= 0.0f;
	m_fAnimTime		= 0.0f;
	m_fSpeed		= 1.0f;
}

void AnimationTrack::SetTime(float fTime)
{
	m_fTrackTime = fTime;

	float fPeriod = m_pAnimSet->GetPeriod();
	m_fAnimTime = GetTimeRemainder(m_fTrackTime, fPeriod, m_nPlaybackType);
	this->SetupInterpolation(m_fAnimTime);
	
}

void AnimationTrack::AdvanceTime(float fDeltaTime)
{
	float delta = fDeltaTime;
	//forbid animation to 'jump'
	if (fDeltaTime > 1.0f) delta = 1.0f;

	m_fTrackTime += delta;
	float fPeriod = m_pAnimSet->GetPeriod();
	this->ProcessEvents(delta);

	if (m_bEnable)
	{
		m_fAnimTime = GetTimeRemainder(m_fAnimTime + delta * m_fSpeed, fPeriod, m_nPlaybackType);
		this->SetupInterpolation(m_fAnimTime);
	}
}

void AnimationTrack::GetInterpolatedKey(int nKey, AnimationSet::KeyFrame& key)
{
	AnimationSet::KeyFrame k0, k1;
	k0 = m_pAnimSet->GetKeyFrame(m_FrameInter.nFrame0, nKey);
	k1 = m_pAnimSet->GetKeyFrame(m_FrameInter.nFrame1, nKey);

	key.fScale = k0.fScale + m_FrameInter.fLerp * (k1.fScale - k0.fScale);
	VectorLerp(key.vTranslation, k0.vTranslation, k1.vTranslation, m_FrameInter.fLerp);
	QuaternionSlerp(key.qRotation, k0.qRotation, k1.qRotation, m_FrameInter.fLerp);
}

void AnimationTrack::SetupInterpolation(float fTime)
{
	assert(m_pAnimSet->GetNumFrames() > 0);
	if (m_pAnimSet->GetNumFrames() == 1)
	{
		m_FrameInter.nFrame0 = 0;
		m_FrameInter.nFrame1 = 0;
		m_FrameInter.fLerp = 0.0f;
		return;
	}

	const std::vector<float>& timeSamples = m_pAnimSet->GetTimeSamples();

	float time0 = timeSamples[m_FrameInter.nFrame0];
	float time1 = timeSamples[m_FrameInter.nFrame1];

	// first, match current frame
	if (time0 <= fTime && fTime <= time1)
	{
		m_FrameInter.fLerp = Mathf::GetLerp(time0, time1, fTime);
		return;
	}
	// nFrame1 is not the last frame
	else if (m_FrameInter.nFrame1 < m_pAnimSet->GetNumFrames()-1)
	{
		// match next frame;
		time0 = time1;
		time1 = timeSamples[m_FrameInter.nFrame1+1];
		if (time0 <= fTime && fTime <= time1)
		{
			m_FrameInter.nFrame0++;
			m_FrameInter.nFrame1++;
			m_FrameInter.fLerp = Mathf::GetLerp(time0, time1, fTime);
			return;
		}

	}

	int nFrame = m_pAnimSet->GetNumFrames() - 2;
	for (; nFrame > 0; nFrame--)
	{
		if(timeSamples[nFrame] < fTime) break;
	}
	m_FrameInter.nFrame0 = nFrame;
	m_FrameInter.nFrame1 = nFrame + 1;
	m_FrameInter.fLerp = Mathf::GetLerp(timeSamples[nFrame], timeSamples[nFrame+1], fTime);
}
//
//float GetTimeRemainder(float time, float period, int playbackType)
//{
//	// calculate remainder of time in period of this animation
//	float fTime = time;
//	if (time < 0.0f)
//	{
//		// turn negative to positive
//		fTime = time - Mathf::Floor(time) * period;
//	}
//	if (playbackType == AnimationTrack::PLAYBACK_ONCE)
//	{
//		if (fTime > period)
//		{
//			return period;
//		}
//		else
//		{
//			return fTime;
//		}
//	}
//	else if (playbackType == AnimationTrack::PLAYBACK_LOOP)
//	{
//		return Mathf::Mod(fTime, period);
//	}
//	// PLAYBACK_PINGPONG
//	else
//	{
//		float rate = fTime / period;
//		int integer_part = (int)Mathf::Floor(rate);
//		float fractional_part = rate - integer_part;
//		if (integer_part % 2 == 0)
//		{
//			return fractional_part * period;
//		}
//		else
//		{
//			return (1.0f - fractional_part) * period;
//		}
//
//	}
//
//}


void AnimationTrack::AddEnableEvent(bool bNewEnable, float fTime)
{
	Event e;
	e.nType			= EVENT_ENABLE;
	e.fStartTime	= e.fEndTime = fTime;
	e.bNewEnable	= bNewEnable;
	m_lEvents.push_back(e);
}

void AnimationTrack::AddWeightEvent(float fNewWeight, float fStartTime, float fDuration)
{
	Event e;
	e.nType				= EVENT_WEIGHT;
	e.pValueToChange	= &(this->m_fWeight);
	e.fStartTime		= fStartTime;
	e.fEndTime			= fStartTime + fDuration;
	e.fNewValue			= fNewWeight;
	m_lEvents.push_back(e);
}

void AnimationTrack::AddSpeedEvent(float fNewSpeed, float fStartTime, float fDuration)
{
	Event e;
	e.nType				= EVENT_SPEED;
	e.pValueToChange	= &(this->m_fSpeed);
	e.fStartTime		= fStartTime;
	e.fEndTime			= fStartTime + fDuration;
	e.fNewValue			= fNewSpeed;
	m_lEvents.push_back(e);
}

void AnimationTrack::RemoveAllEvent()
{
	m_lEvents.clear();
	m_lDealings.clear();
}


void AnimationTrack::ProcessEvents(float fDeltaTime)
{
	std::list<Event>::iterator it = m_lDealings.begin();
	while (it != m_lDealings.end())
	{
		if (m_fTrackTime > it->fEndTime)
		{
			float fLastTime = m_fTrackTime - fDeltaTime;
			// change to end value
			*(it->pValueToChange) += (it->fEndTime - fLastTime) * it->fDeltaValue;
			it = m_lDealings.erase(it);
		}
		else
		{
			*(it->pValueToChange) += fDeltaTime * it->fDeltaValue;
			++it;
		}
	}

	it = m_lEvents.begin();
	while (it != m_lEvents.end())
	{
		if (m_fTrackTime > it->fStartTime)
		{
			if (it->nType == EVENT_WEIGHT || it->nType == EVENT_SPEED)
			{
				float fDuration = it->fEndTime - it->fStartTime;
				float exceed = m_fTrackTime - it->fStartTime;
				// "passed" this event, directly set to end value
				if (exceed > fDuration)
				{
					*(it->pValueToChange) = it->fNewValue;
					it = m_lEvents.erase(it);
					continue;
				}
				
				float fCurrent = *(it->pValueToChange);
				it->fDeltaValue = (it->fNewValue - fCurrent) / fDuration;

				*(it->pValueToChange) += it->fDeltaValue * exceed;
				m_lDealings.push_back(*it);
			}
			else if (it->nType == EVENT_ENABLE)
			{
				this->m_bEnable = it->bNewEnable;
			}
			it = m_lEvents.erase(it);
		}
		else
		{
			++it;
		}
	}// end while
}


/*
================
AnimationController
================
*/
AnimationController::AnimationController()
{
}

AnimationController::~AnimationController()
{
}

bool AnimationController::SetAnimationTrack(int nTrack, AnimationSet* pAnimSet)
{
	assert(nTrack >= 0 && nTrack < MAX_SYNC_ANIM);
	if (this->GetNumTracks() == 0)
	{
		this->AllocateOutKeys(pAnimSet->GetNumKeys());

	}
	else
	{
		if (pAnimSet->GetNumKeys() != this->GetNumKeys())
			return false;
	}

	m_aAnimTracks[nTrack].SetAnimationSet(pAnimSet);
	return true;
}


void AnimationController::ResetAnimationTrack(int nTrack)
{
	assert(nTrack >= 0 && nTrack < MAX_SYNC_ANIM);
	m_aAnimTracks[nTrack].ResetAnimation();
}

void AnimationController::ResetAllAnimation()
{
	for (int i = 0; i < MAX_SYNC_ANIM; i++)
	{
		this->ResetAnimationTrack(i);
	}
}

// make a key weighted
void WeightKey(AnimationSet::KeyFrame& key, float fWeight)
{
	key.fScale = 1.0f + (key.fScale - 1.0f) * fWeight;
	key.vTranslation *= fWeight;
	QuaternionSlerp(key.qRotation, Quaternionf::IDENTITY, key.qRotation, fWeight);
}

void AddKeys(AnimationSet::KeyFrame& out, const AnimationSet::KeyFrame& k1,
			 const AnimationSet::KeyFrame& k2)
{
	out.fScale = k1.fScale * k2.fScale;
	out.vTranslation = k1.vTranslation + k2.vTranslation;
	QuaternionMultiply(out.qRotation, k2.qRotation, k1.qRotation);

}

void AnimationController::FindAvailableTrack(std::vector<int>& vTracks)
{
	if (HasAnimationTrack(0) && m_aAnimTracks[0].GetEnable())
		vTracks.push_back(0);

	if (HasAnimationTrack(1) && m_aAnimTracks[1].GetEnable())
		vTracks.push_back(1);

	if (HasAnimationTrack(2) && m_aAnimTracks[2].GetEnable())
		vTracks.push_back(2);
}

// optimize this function
void AnimationController::AdvanceTime(float fDeltaTime)
{
	if (GetNumTracks() == 0)	return;
	for (int i = 0; i < MAX_SYNC_ANIM; i++)
	{
		if (!m_aAnimTracks[i].IsActive())
			continue;
		m_aAnimTracks[i].AdvanceTime(fDeltaTime);
	}

	std::vector<int> vTracks;
	FindAvailableTrack(vTracks);

	// blend animation
	for (int i = 0; i < GetNumKeys(); i++)
	{
		AnimationSet::KeyFrame key;

		if (vTracks.size() == 1)
		{
			m_aAnimTracks[vTracks[0]].GetInterpolatedKey(i, key);
			WeightKey(key, m_aAnimTracks[vTracks[0]].GetWeight());
		}
		else if (vTracks.size() == 2)
		{
			AnimationSet::KeyFrame key0, key1;

			m_aAnimTracks[vTracks[0]].GetInterpolatedKey(i, key0);
			WeightKey(key0, m_aAnimTracks[vTracks[0]].GetWeight());
			m_aAnimTracks[vTracks[1]].GetInterpolatedKey(i, key1);
			WeightKey(key0, m_aAnimTracks[vTracks[1]].GetWeight());

			AddKeys(key, key0, key1);
		}
		else if (vTracks.size() == 3)
		{
			AnimationSet::KeyFrame key0, key1, key2;

			m_aAnimTracks[vTracks[0]].GetInterpolatedKey(i, key0);
			WeightKey(key0, m_aAnimTracks[vTracks[0]].GetWeight());

			m_aAnimTracks[vTracks[1]].GetInterpolatedKey(i, key1);
			WeightKey(key0, m_aAnimTracks[vTracks[1]].GetWeight());

			m_aAnimTracks[vTracks[2]].GetInterpolatedKey(i, key2);
			WeightKey(key0, m_aAnimTracks[vTracks[2]].GetWeight());

			AddKeys(key, key0, key1);
			AddKeys(key, key, key2);
		}

		float*			pScaleKey		= m_aOutputKeysScale[i];
		Quaternionf*	pRotationKey	= m_aOutputKeysRotation[i];
		Vector3f*		pTransKey		= m_aOutputKeysTrans[i];

		if (pScaleKey)
			*pScaleKey = key.fScale;

		if (pRotationKey)
			*pRotationKey = key.qRotation;

		if (pTransKey)
			*pTransKey = key.vTranslation;
	}
}

void AnimationController::RegisterKeyOutput(int nKey, float* pScale, Quaternionf* pRotation, Vector3f* pTranslation)
{
	assert(nKey < (int)m_aOutputKeysScale.size());
	m_aOutputKeysScale[nKey] = pScale;
	m_aOutputKeysRotation[nKey] = pRotation;
	m_aOutputKeysTrans[nKey] = pTranslation;
}

void AnimationController::RegisterKeyOutput(const std::string& keyName, float* pScale, Quaternionf* pRotation, Vector3f* pTranslation)
{
	int nKey = -1;
	if (HasAnimationTrack(0))
		nKey = m_aAnimTracks[0].GetAnimationSet()->FindKeyName(keyName);
	else if (HasAnimationTrack(1))
		nKey = m_aAnimTracks[1].GetAnimationSet()->FindKeyName(keyName);
	else if (HasAnimationTrack(2))
		nKey = m_aAnimTracks[2].GetAnimationSet()->FindKeyName(keyName);

	if (nKey == -1)	return;
	this->RegisterKeyOutput(nKey, pScale, pRotation, pTranslation);
}

void AnimationController::RegisterSkeleton(Skeleton* pSkeleton)
{
	this->ClearOutputKeys();

	int nBones = pSkeleton->GetNumBones();
	for (int i = 0; i < nBones; i++)
	{
		std::string name = pSkeleton->GetBoneName(i);
		Skeleton::Bone* pBone = pSkeleton->GetBone(i);
		this->RegisterKeyOutput(name, &pBone->m_fScaleLocal, &pBone->m_qRotationLocal, &pBone->m_vTransLocal);
	}
}

int AnimationController::GetNumTracks() const
{
	int count = 0;
	if (HasAnimationTrack(0)) count++;
	if (HasAnimationTrack(1)) count++;
	if (HasAnimationTrack(2)) count++;

	return count;
}

void AnimationController::AllocateOutKeys(int nNumKeys)
{
	m_aOutputKeysScale.resize(nNumKeys);
	m_aOutputKeysRotation.resize(nNumKeys);
	m_aOutputKeysTrans.resize(nNumKeys);
}

void AnimationController::ClearOutputKeys()
{
	assert(0);
	//std::for_each(m_aOutputKeysScale.begin(), m_aOutputKeysScale.end(), boost::lambda::_1 = (float*)nullptr);
	//std::for_each(m_aOutputKeysRotation.begin(), m_aOutputKeysRotation.end(), boost::lambda::_1 = (Quaternionf*)nullptr);
	//std::for_each(m_aOutputKeysTrans.begin(), m_aOutputKeysTrans.end(), boost::lambda::_1 = (Vector3f*)nullptr);
}


/*
================
AnimationManager
================
*/
/*
AnimationSet*& AnimationManager::CreateResource(const std::string& name)
{
	static AnimationSet* NullPtr;

	std::string fullName = m_strPath + name;
	if (Util::GetFileNameExt(name) == "sca")
	{
		ScaAnimLoader loader;
		AnimationSet* ptr = loader.LoadAnimation(fullName.c_str());
		if (ptr.get())
		{
			ptr->SetName(name);
			return m_mapResource.insert(std::make_pair(name, ptr)).first->second;
		}
		else
		{
			return NullPtr;
		}

	}
	return NullPtr;
}
*/
}// end of namespace