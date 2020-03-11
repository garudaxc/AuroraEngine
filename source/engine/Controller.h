#pragma once
#include "MathFunction.h"
#include "Types.h"
#include "Input.h"
#include "Transform.h"

namespace Aurora
{

class SceneNode;


class KeyInterpolater
{
public:

	virtual void			Interpolate(float* des, float* key0, float* key1,  float fLerp,
										uint nNumSrcFloats, uint nNumDesFloats) = 0;
};


class LinearFloatInterpolater : public KeyInterpolater
{

public:

	virtual void			Interpolate(float* des, float* key0, float* key1,  float fLerp,
										uint nNumSrcFloats, uint nNumDesFloats);
};


class LinearQuaternionInterpolater : public KeyInterpolater
{

public:

	virtual void			Interpolate(float* des, float* key0, float* key1,  float fLerp,
		uint nNumSrcFloats, uint nNumDesFloats);
};




class Controller
{
public:
	enum PlayBackType
	{
		PLAYBACK_LOOP	= 0,
		PLAYBACK_ONCE,
		PLAYBACK_PINGPONG,
	};

	Controller();
	virtual ~Controller()		{}

	virtual void		AdvanceTime(float dt) = 0;
	virtual void		Solve() = 0;

	virtual void		SetInterpolaterType(uint type)						{}
	virtual bool		BindFloats(float* pointer, uint nNumFloats)			{	return false;	}
	virtual bool		BindNode(SceneNode* pNode)							{	return false;	}

	void				SetPlaybackType(PlayBackType type)					{	m_PlaybackType = type;	}

	bool				IsActived() const									{	return m_bActive;	}
	void				SetActive(bool bActive);


protected:
	PlayBackType	m_PlaybackType;

private:
	bool			m_bActive;

public:
	// global controller time driver, advance all controller in system
	static void		AdvanceControllerTime(float dt);
	static float	GetGlobalControllerTime()							{	return s_GlobalControllerTime;	}
	static uint		GetNumActiveControllers()							{	return s_nNumActiveControllers;	}

private:
	static float	s_GlobalControllerTime;
	static uint		s_nNumActiveControllers;


};



//////////////////////////////////////////////////////////////////////////

struct FrameInterpolation
{
	int		nFrame0;
	int		nFrame1;
	float	fLerp;
};


//////////////////////////////////////////////////////////////////////////

class FloatsController : public Controller
{
public:
	enum InterpolateType
	{
		IPT_LINEAR_VALUE				= 0,
		IPT_LINEAR_QUATERNION,
	};


	FloatsController();
	virtual ~FloatsController()			{}

	virtual void				Bind(SceneNode* pNode)			{}

	virtual bool				BindFloats(float* pointer, uint nNumFloats);

	virtual void				AdvanceTime(float dt);
	virtual void				Solve();

	void						Alloc(uint nNumFrames, uint nNumFloats);
	void						SetKey(uint nFrame, float fTime, const float* pValue);
	void						SetTime(float fTime);

	int							GetNumFrames() const	{	return m_nNumFrames;	}

	virtual void				SetInterpolaterType(uint type);

private:
	float						GetAnimLength() const;

	float						GetFirstKeyFrameTime() const;
	float						GetLastKeyFrameTime() const;

	void						SetupInterpolation(float fTime);

	KeyInterpolater*			m_pInterpolater;

	float						m_fTotalTime;
	float						m_fTrackTime;

	int							m_nNumFrames;

	map<float, uint>			m_TimeSamples;
	float						m_fFirstKeyFrameTime;
	float						m_fLastKeyFrameTime;

	FrameInterpolation			m_FrameInter;

	float** m_aFrames;

	uint	m_nNumFloats;

	float*	m_pBindedPointer;


};


//////////////////////////////////////////////////////////////////////////


struct BezierKey
{
	float		t[3];
	float		v[3];
};


FloatsController* SampleBezierCurve(const BezierKey* keyFrames, int nNumKeyFrames, float samplePerSec);



//////////////////////////////////////////////////////////////////////////



class OrbitMouseController : public MouseEventListener
{
public:
	OrbitMouseController();
	~OrbitMouseController();


	virtual void		OnMouseEvent(const MouseEvent& event);

	void				Reset(const Vector3f& vPos, const Quaternionf& qRot);

	const Vector3f&		GetPosition() const		{	return m_vPos;	}
	const Quaternionf&	GetRotation() const		{	return m_qRot;	}
	const Matrix4f&		GetTransfrom() const	{	return m_mTransform;	}

private:
	virtual void		Solve();

	Vector3f		m_vTarget;

	float			m_theta;		// angle in xy plane, -pi - pi
	float			m_phi;			// angle toward z axis, 0 - pi
	float			m_radius;

	Vector3f		m_vPos;		
	Quaternionf		m_qRot;
	Matrix4f	m_mTransform;

};


class EditorCameraMover : public MouseEventListener
{
public:
	EditorCameraMover();
	~EditorCameraMover();

	void				Reset(const Vector3f& vPos);

	virtual void		OnMouseEvent(const MouseEvent& event);

	void				SetSpeed(float speed)		{	m_fSpeed = speed;	}

	Transform		Transform_;

private:
	void		SolveRotation();

	Vector3f		m_vPos;		
	Quaternionf		m_qRot;
	Matrix4f	m_mTransform;

	float		m_theta;		// angle in xy plane, -pi - pi
	float		m_phi;			// angle toward z axis, 0 - pi

	float		m_fSpeed;

};


class CameraControllerUE : public MouseEventListener
{
public:

	virtual void		OnMouseEvent(const MouseEvent& event);
	Transform		Transform_;
	

	float	FocusDistance_ = 1.f;
};

}