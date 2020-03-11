#include "stdHeader.h"
#include "Controller.h"
#include "DeviceWindow.h"
#include "Console.h"

namespace Aurora
{

static LinearFloatInterpolater gLinearFloatInterpolater;
static LinearQuaternionInterpolater gLinearQuaternionInterpolater;


void LinearFloatInterpolater::Interpolate(float* des, float* key0, float* key1, float fLerp,
										uint nNumSrcFloats, uint nNumDesFloats)
{
	for (uint i = 0; i < nNumDesFloats; i++) {
		des[i] = key0[i] + fLerp * (key1[i] - key0[i]);
	}
}


void LinearQuaternionInterpolater::Interpolate(float* des, float* key0, float* key1, float fLerp,
										  uint nNumSrcFloats, uint nNumDesFloats)
{
	assert((nNumDesFloats == nNumSrcFloats) && (nNumSrcFloats == 4));

	Quaternionf* qDes = (Quaternionf*)des;
	Quaternionf* qKey0 = (Quaternionf*)key0;
	Quaternionf* qKey1 = (Quaternionf*)key1;

	QuaternionSlerp(*qDes, *qKey0, *qKey1, fLerp);
}

//////////////////////////////////////////////////////////////////////////


float GetTimeRemainder(float time, float period, int playbackType)
{
	// calculate remainder of time in period of this animation
	float fTime = time;
	if (time < 0.0f) {
		// turn negative to positive
		fTime = time - Mathf::Floor(time) * period;
	}
	if (playbackType == Controller::PLAYBACK_ONCE) {
		if (fTime > period) {
			return period;
		} else {
			return fTime;
		}
	} else if (playbackType == Controller::PLAYBACK_LOOP) {
		return Mathf::Mod(fTime, period);
	}
	// PLAYBACK_PINGPONG
	else {
		float rate = fTime / period;
		int integer_part = (int)Mathf::Floor(rate);
		float fractional_part = rate - integer_part;
		if (integer_part % 2 == 0) {
			return fractional_part * period;
		} else {
			return (1.0f - fractional_part) * period;
		}

	}
}


float GetTrackTime(float fTime, float fStartTrackTime, float fEndTrackTime, uint playType)
{
	if (playType == Controller::PLAYBACK_ONCE) {
		if (fTime < fStartTrackTime) {
			return fStartTrackTime;
		}

		if (fTime >= fEndTrackTime) {
			return fEndTrackTime - 0.001f;
		}

		return fTime;
	}
	else if (playType == Controller::PLAYBACK_LOOP) {
		float t = Mathf::Mod(fTime - fStartTrackTime, fEndTrackTime - fStartTrackTime);

		if (t < 0.0f) {
			return t + fEndTrackTime;
		} else {
			return t + fStartTrackTime;
		}
	}

	assert(0);
	return 0.0f;
}



//////////////////////////////////////////////////////////////////////////
float Controller::s_GlobalControllerTime = 0.0f;
uint Controller::s_nNumActiveControllers = 0;



Controller::Controller():m_PlaybackType(PLAYBACK_LOOP),m_bActive(false)
{
}


void Controller::SetActive(bool bActive)
{
	if (bActive != m_bActive) {
		if (bActive) {
			s_nNumActiveControllers++;
		} else {
			s_nNumActiveControllers--;
		}
		m_bActive = bActive;
	}
}



FloatsController::FloatsController():m_pInterpolater(&gLinearFloatInterpolater),
m_fTotalTime(0.0f),m_fTrackTime(0.0f),m_nNumFrames(0),m_fFirstKeyFrameTime(0.0f),
m_fLastKeyFrameTime(0.0f),m_nNumFloats(0),m_pBindedPointer(NULL)
{
	m_FrameInter.nFrame0 = 0;
	m_FrameInter.nFrame1 = 0;
	m_FrameInter.fLerp = 0.0f;
}


float FloatsController::GetAnimLength() const
{
	return m_fLastKeyFrameTime - m_fFirstKeyFrameTime;
}

float FloatsController::GetFirstKeyFrameTime() const
{
	return m_fFirstKeyFrameTime;
}

float FloatsController::GetLastKeyFrameTime() const
{
	return m_fLastKeyFrameTime;
}


void FloatsController::AdvanceTime(float dt)
{
	float delta = dt;
	//forbid animation to 'jump'
	if (dt > 1.0f) delta = 1.0f;

	m_fTotalTime += dt;
	float fPeriod = GetAnimLength();

	m_fTrackTime = GetTrackTime(m_fTotalTime, GetFirstKeyFrameTime(), GetLastKeyFrameTime(), m_PlaybackType);

	Solve();
}

void FloatsController::Solve()
{
	SetupInterpolation(m_fTrackTime);

	m_pInterpolater->Interpolate(m_pBindedPointer, &m_aFrames[m_FrameInter.nFrame0][0],
						&m_aFrames[m_FrameInter.nFrame1][0], m_FrameInter.fLerp, 
						m_nNumFloats, m_nNumFloats);

	/*for (uint i = 0; i < m_nNumFloats; i++)
	{
		m_pBindedPointer[i] = m_aFrames[m_FrameInter.nFrame0][i] + m_FrameInter.fLerp * 
			(m_aFrames[m_FrameInter.nFrame1][i] - m_aFrames[m_FrameInter.nFrame0][i]);
	}*/
}


bool FloatsController::BindFloats(float* pointer, uint nNumFloats)
{
	assert(nNumFloats <= m_nNumFloats);
	m_pBindedPointer = pointer;

	return true;
}

void FloatsController::Alloc(uint nNumFrames, uint nNumFloats)
{
	//m_aFrames.resize(boost::extents[nNumFrames][nNumFloats]);
	m_nNumFrames = nNumFrames;
	m_nNumFloats = nNumFloats;
}

void FloatsController::SetKey(uint nFrame, float fTime, const float* pValue)
{
	m_TimeSamples.insert(make_pair(fTime, nFrame));

	memcpy(&m_aFrames[nFrame][0], pValue, sizeof(float) * m_nNumFloats);

	if (nFrame == 0) {
		m_fFirstKeyFrameTime = fTime;
	}
	else if (nFrame == m_nNumFrames - 1) {
		m_fLastKeyFrameTime = fTime;
	}
}

void FloatsController::SetTime(float fTime)
{
	assert(0);
	m_fTotalTime = fTime;
}


void FloatsController::SetupInterpolation(float fTime)
{
	assert(GetNumFrames() > 0);
	if (GetNumFrames() == 1) {
		m_FrameInter.nFrame0 = 0;
		m_FrameInter.nFrame1 = 0;
		m_FrameInter.fLerp = 0.0f;
		return;
	}

	map<float, uint>::const_iterator it1 = m_TimeSamples.upper_bound(fTime);
	assert(it1 != m_TimeSamples.end());

	map<float, uint>::const_iterator it0 = it1;
	--it0;

	m_FrameInter.nFrame0 = it0->second;
	m_FrameInter.nFrame1 = it1->second;
	m_FrameInter.fLerp = Mathf::GetLerp(it0->first, it1->first, fTime);
}



void FloatsController::SetInterpolaterType(uint type)
{
	if (type == IPT_LINEAR_VALUE)
	{
		m_pInterpolater = &gLinearFloatInterpolater;
	}
	else if (type == IPT_LINEAR_QUATERNION)
	{
		m_pInterpolater = &gLinearQuaternionInterpolater;
	}

}


void CubicBezierCurve(float* pOut, const float* p0, const float* p1, const float* p2, const float* p3, float s, int nNumFloats)
{
	float u = s;
	float u2 = u * u;
	float u3 = u2 * u;
	float mu = 1.0f - u;
	float mu2 = mu * mu;
	float mu3 = mu2 * mu;
	for (int i = 0; i < nNumFloats; i++) {
		pOut[i] = mu3 * p0[i] + 3.0f * (u * mu2 * p1[i] + u2 * mu * p2[i]) + u3 * p3[i];
	}
}


FloatsController* SampleBezierCurve(const BezierKey* keyFrames, int nNumKeyFrames, float samplePerSec)
{
	FloatsController* pController = new FloatsController();

	//pController->SetPlaybackType(playBackType);

	vector<int> samples(nNumKeyFrames - 1);
	int nTotalSample = 0;
	for (int i = 0; i < nNumKeyFrames - 1; i++) {
		float time = keyFrames[i + 1].t[1] - keyFrames[i].t[1];
		int nSample = (int)(time * samplePerSec);
		if (nSample == 0) {
			nSample = 1;
		}
		samples[i] = nSample;
		nTotalSample += nSample;
	}

	pController->Alloc(nTotalSample + 1, 1);
	int nKeyIndex = 0;
	for (int i = 0; i < nNumKeyFrames - 1; i++) {
		int nSample = samples[i];
		float fStride = 1.0f / (float)nSample;
		for (int j = 0; j < nSample; j++) {
			float val[2];

			float p0[2], p1[2], p2[2], p3[2];
			p0[0] = keyFrames[i].t[1];
			p0[1] = keyFrames[i].v[1];

			p1[0] = keyFrames[i].t[2];
			p1[1] = keyFrames[i].v[2];

			p2[0] = keyFrames[i + 1].t[0];
			p2[1] = keyFrames[i + 1].v[0];

			p3[0] = keyFrames[i + 1].t[1];
			p3[1] = keyFrames[i + 1].v[1];

			CubicBezierCurve(val, p0, p1, p2, p3, fStride * j, 2);

			pController->SetKey(nKeyIndex++, val[0], val + 1);
		}
	}

	pController->SetKey(nKeyIndex, keyFrames[nNumKeyFrames - 1].t[1], keyFrames[nNumKeyFrames - 1].v + 1);

	return pController;
}




//////////////////////////////////////////////////////////////////////////

//void QuaternionToSphereCoord(const Quaternionf& qRot, float& theta, float& phi)
//{
//}
//
//Quaternionf SphereCoordToQuaternion(float theta, float phi)
//{
//	
//}




OrbitMouseController::OrbitMouseController()
{

}

OrbitMouseController::~OrbitMouseController()
{

}


void OrbitMouseController::OnMouseEvent(const MouseEvent& event)
{
	static uint32 mouseDownX = 0, mouseDownY = 0;

	static float thetaLast = 0.0f, phiLast = 0.0f;

	static Vector3f vTargetLast = Vector3f::ZERO;

	static bool bControlDown = false;

	if (event.Type == MouseEvent::MButtonDown) {
		event.pWindow->SetCapture();
		mouseDownX = event.xPos;
		mouseDownY = event.yPos;

		thetaLast = m_theta;
		phiLast = m_phi;

		vTargetLast = m_vTarget;
		
		if (event.Param & MouseEvent::PM_Control) {
			bControlDown = true;
		}
	} else if (event.Type == MouseEvent::MButtonUp) {
		event.pWindow->ReleaseCapture();
		bControlDown = false;
	}
	else if (event.Type == MouseEvent::MouseWheel)
	{
		if (event.zDelta > 0) {
			m_radius *= 0.95f;
		} else {
			m_radius *= 1.05f;	
		}
		Solve();		
	}
	else if (event.Type == MouseEvent::MouseMove) {
		if (event.Param & MouseEvent::PM_MButton) {
			if (bControlDown) {
				float dTheta = (float)(mouseDownX - event.xPos) * 0.005f;
				float dPhi = (float)(mouseDownY - event.yPos) * 0.005f;

				m_theta = thetaLast + dTheta;
				m_phi = phiLast + dPhi;
			} else {
				Vector3f right, up;
				Vector3Rotate(right, Vector3f::UNIT_X, m_qRot);
				Vector3Rotate(up, Vector3f::UNIT_Y, m_qRot);

				right *=  (float)(mouseDownX - event.xPos) * m_radius * 0.001f;
				up *= (float)(event.yPos - mouseDownY) * m_radius * 0.001f;

				m_vTarget = vTargetLast + right + up;
			}

			Solve();
		}
	} else if (event.Type == MouseEvent::RButtonDown) {
		m_theta = thetaLast;
		m_phi = phiLast;
		m_vTarget = vTargetLast;

		mouseDownX = event.xPos;
		mouseDownY = event.yPos;

		Solve();
	}
}


void OrbitMouseController::Reset(const Vector3f& vPos, const Quaternionf& qRot)
{
	m_vPos = vPos;
	m_qRot = qRot;

	Vector3f direction;
	Vector3Rotate(direction, Vector3f::UNIT_Z, qRot);

	m_vTarget = vPos - direction * (vPos.z / direction.z);

	Vector3f orbitDir = vPos - m_vTarget;
	m_radius = orbitDir.Length();
	orbitDir *= 1.0f / m_radius;

	m_theta = Mathf::ATan2(orbitDir.x, -orbitDir.y);

	float angle = m_theta * Mathf::RAD_TO_DEG;


	float r = Mathf::Sqrt(orbitDir.x * orbitDir.x + orbitDir.y * orbitDir.y);
	m_phi = Mathf::ATan2(r, orbitDir.z);

	angle = m_phi * Mathf::RAD_TO_DEG;
}



void OrbitMouseController::Solve()
{	
	float z = Mathf::Cos(m_phi);
	float r = Mathf::Sin(m_phi);
	float x = Mathf::Sin(m_theta) * r;
	float y = -Mathf::Cos(m_theta) * r;

	Vector3f orbitDir(x, y, z);

	Quaternionf qRoll;
	QuaternionRotationAxis(qRoll, Vector3f::UNIT_Z, m_theta);

	Quaternionf qVecToVec;
	QuaternionVectorToVector(qVecToVec, Vector3f::UNIT_Z, orbitDir);

	// roll first
	QuaternionMultiply(m_qRot, qVecToVec, qRoll);

	m_vPos = m_vTarget + orbitDir * m_radius;

	MatrixTransform(m_mTransform, m_qRot, m_vPos);
}


//////////////////////////////////////////////////////////////////////////

// 鼠标中间控制变换
// 任何键不按时，在XY平面内平移，滚轮拉近拉远
// 按住ctrl键，旋转视角方向
// 按住shift键，Z轴方向移动

EditorCameraMover::EditorCameraMover():m_vPos(0.f, 0.f, 0.f),m_qRot(1.0f, 0.0f, 0.0f, 0.0f)
{
	m_fSpeed = 1.f;
	m_theta = 0.0f;
	m_phi = 0.0f;
}

EditorCameraMover::~EditorCameraMover()
{
}

void EditorCameraMover::Reset(const Vector3f& vPos)
{
	m_vPos = vPos;
	m_theta = 0.0f;
	m_phi = 0.0f;
	SolveRotation();

	Transform_.SetTranslation(m_vPos);
	Transform_.SetRotation(m_qRot);
}


void EditorCameraMover::OnMouseEvent(const MouseEvent& event)
{
	static uint32 mouseDownX = 0, mouseDownY = 0;

	static float thetaLast = 0.0f, phiLast = 0.0f;

	static Vector3f vTargetLast = Vector3f::ZERO;

	static Vector3f vLastPos = Vector3f::ZERO;

	static bool bControlDown = false;
	static bool bShiftDown = false;

	if (event.Type == MouseEvent::MButtonDown)
	{
		event.pWindow->SetCapture();
		mouseDownX = event.xPos;
		mouseDownY = event.yPos;

		thetaLast = m_theta;
		phiLast = m_phi;

		vLastPos = m_vPos;

		if (event.Param & MouseEvent::PM_Control) {
			bControlDown = true;
		}

		if (event.Param & MouseEvent::PM_Shift) {
			bShiftDown = true;
		}
	} else if (event.Type == MouseEvent::MButtonUp) {
		event.pWindow->ReleaseCapture();
		bControlDown = false;
		bShiftDown = false;
	} else if ((event.Type == MouseEvent::MouseMove) &&
		(event.Param & MouseEvent::PM_MButton))
	{
		Vector3f right, up, front;
		Vector3Rotate(front, Vector3f::UNIT_Y, m_qRot);
		front.z = 0.0f;
		front.Normalize();

		Vector3Rotate(right, Vector3f::UNIT_X, m_qRot);
		right.z = 0.0f;
		right.Normalize();

		up = Vector3f::UNIT_Z;

		//if (event.Param & MouseEvent::PM_Control)
		if (bControlDown) {
			float dTheta = (float)(mouseDownX - event.xPos) * 0.005f;
			float dPhi = (float)(mouseDownY - event.yPos) * 0.005f;

			m_theta = thetaLast + dTheta;
			m_phi = phiLast + dPhi;

			SolveRotation();
		} else if (bShiftDown) {
			float toUp = (float)(event.yPos - mouseDownY) * m_fSpeed;
			m_vPos = vLastPos + up * toUp;
		} else {
			float toRight = (float)(mouseDownX - event.xPos) * m_fSpeed;
			float toFront = (float)(event.yPos - mouseDownY) * m_fSpeed;

			m_vPos = vLastPos + right * toRight + front * toFront;
		}

	}


	Transform_.SetTranslation(m_vPos);
	Transform_.SetRotation(m_qRot);
	//MatrixTransform(m_mTransform, m_qRot, m_vPos);
}

void EditorCameraMover::SolveRotation()
{
	float z = Mathf::Cos(m_phi);
	float r = Mathf::Sin(m_phi);
	float x = Mathf::Sin(m_theta) * r;
	float y = -Mathf::Cos(m_theta) * r;

	Vector3f orbitDir(x, y, z);

	Quaternionf qRoll;
	QuaternionRotationAxis(qRoll, Vector3f::UNIT_Z, m_theta);

	Quaternionf qPhi;
	QuaternionRotationAxis(qPhi, Vector3f::UNIT_X, m_phi);

	//Quaternionf qVecToVec;
	//QuaternionVectorToVector(qVecToVec, Vector3f::UNIT_Z, orbitDir);

	// roll first
	QuaternionMultiply(m_qRot, qPhi, qRoll);
}



MouseEvent lastEvent_;
Transform  lastTransform_;
bool lButtonDown_ = false;
bool rButtonDown_ = false;

void CameraControllerUE::OnMouseEvent(const MouseEvent& event)
{
	if (event.Type == MouseEvent::LButtonDown) {
		lastEvent_ = event;
		lastTransform_ = Transform_;
		lButtonDown_ = true;
	}

	if (event.Type == MouseEvent::LButtonUp) {
		lButtonDown_ = false;
	}

	if (event.Type == MouseEvent::MouseMove) {
		int dx = event.xPos - lastEvent_.xPos;
		int dy = event.yPos - lastEvent_.yPos;

		

		if (lButtonDown_) {


			if (event.Param & MouseEvent::PM_Control) {
			}
			else {

			}

			
			Quaternionf rot(Vector3f::UNIT_Z, dx * -0.01f);
			Transform_ = lastTransform_;
			Transform_.Rotate(rot);
		}

	}
}

}