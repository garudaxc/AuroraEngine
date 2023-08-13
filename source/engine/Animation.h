#ifndef GAANIMATION_H
#define GAANIMATION_H

namespace Aurora
{

#define MAX_SYNC_ANIM 3

//=======================
//	AnimationSet
//======================
// a segment of animation of bones
class AnimationSet
{
public:

	struct KeyFrame
	{	
		float			fScale;
		Vector3f		vTranslation;
		Quaternionf		qRotation;
	};


	explicit AnimationSet(const std::string& name);
	AnimationSet(const std::string& name, int nNumKeys, int nNumFrames);
	~AnimationSet();

	void						Allocate(int nNumKeys, int nNumFrames);

	const std::string&			GetName() const { return m_strName; }
	void						SetName(const std::string& name) { m_strName = name; }

	void						SetKeyName(int nKey, const std::string& name) { m_aKeyNames[nKey] = name; }
	const std::string&			GetKeyName(int nKey) const { return m_aKeyNames[nKey]; }
	void						SetTimeSample(int nFrame, float fTime) { m_aTimeSamples[nFrame] = fTime; }
	int							FindKeyName(const std::string& name) const;

	float						GetPeriod() const;
	const std::vector<float>&	GetTimeSamples() const { return m_aTimeSamples; }

	void						SetKeyFrame(int nFrame, int nKey, const KeyFrame& keyFrame);
	const KeyFrame&				GetKeyFrame(int nFrame, int nKey);

	int							GetNumKeys() const { return m_nNumKeys; }
	int							GetNumFrames() const { return m_nNumFrames; }


private:
	std::string							m_strName;
	int									m_nNumFrames;			// number of keys in one frame
	int									m_nNumKeys;
	std::vector<std::string>			m_aKeyNames;

	std::vector<float>					m_aTimeSamples;

	KeyFrame**		m_aKeyFrames;

};


// create an 'all identity' animation like a bind position
AnimationSet* CreateBindPos(AnimationSet* pAnimSet);

//==============================
//		AnimationTrack
//==============================
class AnimationTrack
{
public:
	enum
	{
		PLAYBACK_LOOP	= 0,
		PLAYBACK_ONCE,
		PLAYBACK_PINGPONG,
	};

	struct FrameInterpolation
	{
		int		nFrame0;
		int		nFrame1;
		float	fLerp;
	};


	enum EventType
	{
		EVENT_ENABLE = 0,
		EVENT_SPEED,
		EVENT_WEIGHT,
	};

	struct Event
	{
		int			nType;
		float		fStartTime;
		float		fEndTime;
		float*		pValueToChange;
		union
		{
			bool		bNewEnable;
			float		fNewValue;
			float		fDeltaValue;
		};
	};


	AnimationTrack();
	~AnimationTrack();

	void					SetAnimationSet(AnimationSet* pAnimSet);
	void					ResetAnimation();
	const AnimationSet*		GetAnimationSet() const { return m_pAnimSet; }

	bool					IsActive() const { return m_pAnimSet != nullptr; }

	bool					GetEnable() const { return m_bEnable; }
	void					SetEnable(bool bEnable) { m_bEnable = bEnable; }

	int						GetPlaybackType() const { return m_nPlaybackType; }
	void					SetPlaybackType(int nType) { m_nPlaybackType = nType; }

	float					GetWeight() const { return m_fWeight; }
	void					SetWeight(float fWeight) { m_fWeight = fWeight; }

	void					SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	float					GetSpeed() { return m_fSpeed; }

	float					GetTime() const { return m_fTrackTime; }
	void					SetTime(float fTime);
	void					AdvanceTime(float fDeltaTime);

	void					GetInterpolatedKey(int nKey, AnimationSet::KeyFrame& key);

	void					AddEnableEvent(bool bNewEnable, float fTime);
	void					AddWeightEvent(float fNewWeight, float fStartTime, float fDuration);
	void					AddSpeedEvent(float fNewSpeed, float fStartTime, float fDuration);
	void					RemoveAllEvent();


private:
	void					SetupInterpolation(float fTime);	// find frame interpolation in logic time
	void					ProcessEvents(float fDeltaTime);

	AnimationSet*			m_pAnimSet;

	int						m_nPlaybackType;

	bool					m_bEnable;
	float					m_fWeight;		// blend weight for external use
	float					m_fSpeed;		// animation speed deault is 1.0

	float					m_fTrackTime;	// real time of this animation track
	float					m_fAnimTime;	// logic time of this animation, between 0 and period
	FrameInterpolation		m_FrameInter;	// frame interpolation infomation of current time

	std::list<Event>		m_lEvents;
	std::list<Event>		m_lDealings;

};

float GetTimeRemainder(float time, float period, int playbackType);

//========================
//	AnimationController
//=======================
class Skeleton;
class AnimationController
{
public:
	AnimationController();
	~AnimationController();

	bool				SetAnimationTrack(int nTrack, AnimationSet* pAnimSet);
	AnimationTrack*		GetAnimationTrack(int nTrack) { return &m_aAnimTracks[nTrack]; }
	bool				HasAnimationTrack(int nTrack) const { return m_aAnimTracks[nTrack].GetAnimationSet() != nullptr; }

	void				ResetAnimationTrack(int nTrack);
	void				ResetAllAnimation();

	void				ClearOutputKeys();

	void				RegisterKeyOutput(int nKey, float* pScale, Quaternionf* pRotation, Vector3f* pTranslation);
	void				RegisterKeyOutput(const std::string& keyName, float* pScale, Quaternionf* pRotation, Vector3f* pTranslation);
	void				RegisterSkeleton(Skeleton* pSkeleton);

	void				AdvanceTime(float fDeltaTime);

private:
	int					GetNumTracks() const;
	void				FindAvailableTrack(std::vector<int>& vTracks);
	int					GetNumKeys() const { return (int)m_aOutputKeysScale.size(); }
	void				AllocateOutKeys(int nNumKeys);

	AnimationTrack							m_aAnimTracks[MAX_SYNC_ANIM];
	std::vector<float*>						m_aOutputKeysScale;
	std::vector<Quaternionf*>				m_aOutputKeysRotation;
	std::vector<Vector3f*>					m_aOutputKeysTrans;
	float									m_fAnimTime;

};



class AnimationClip
{
public:



private:

	int		m_numFloatPerFrame;


	vector<float>	m_Data;
};



}// end of namespace


#endif