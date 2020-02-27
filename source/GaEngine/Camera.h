#ifndef GACAMERA_H
#define GACAMERA_H

#include "MathFunction.h"
#include "Types.h"
#include "Input.h"
#include "Primitive.h"

namespace Aurora
{


class Camera;
class ArcBall;


//void LoadXmlCamera(TiXmlNode* pXmlCamera, SceneNode* pSceneNode);


class ArcBall : public MouseEventListener
{
public:
	ArcBall(int nCenterX, int nCenterY, uint nRadius, float fTransScale = 1.0f, bool bLmtInArea = false);
	virtual void OnMouseEvent(const MouseEvent& event);

	void SetOperatingArea(int nCenterX, int nCenterY, uint nRadius);

	const Quaternionf& GetRotation() const { return m_qCurr; }
	const Vector3f& GetTranslation() const { return m_vCurrTrans; }
	void GetTransformation(Matrix4f& t) const;

	void SetActive(bool bActive = true) { m_bIsActive = bActive; }
	bool IsActive() const { return m_bIsActive; }

private:
	Vector2f ScreenPointToPos(int xPos, int yPos);

	bool m_bIsActive;

	int m_nCenterX;
	int m_nCenterY;
	uint m_nRadius;

	// for rotation
	bool m_bIsLButtonDown;
	Quaternionf m_qCurr;
	Quaternionf m_qLast;
	Vector3f m_vBtnDown;

	// for translation
	bool m_bIsRButtonDown;
	bool m_bIsMButtonDown;
	Vector2f m_vLastPoint;		//point on botton down
	Vector3f m_vCurrTrans;
	float m_fTransScale;

	// wether only mouse event in a limited area can affect the arc ball
	bool m_bLmtInArea;


};


struct ViewPort
{
	unsigned int	Left;
	unsigned int	Top;
	unsigned int	Width;
	unsigned int	Height;
	float			MinZ;
	float			MaxZ;
	ViewPort()
	{
		Left	= 0;
		Top		= 0;
		MinZ	= 0.0f;
		MaxZ	= 1.0f;
	}
};


class Frustum
{
public:
	enum PlaneID
	{
		PID_TOP = 0,
		PID_BOTTOM,
		PID_LEFT,
		PID_RIGHT,
		PID_FRONT,
		PID_BACK,
		PID_NUM_PLANE,
	};

	Plane			planes[PID_NUM_PLANE];
};


// find intersect point of three planes
Vector3f IntersectPlane(const Plane& p0, const Plane& p1, const Plane& p2);
void ExtractFrustum(Camera* pCamera, Frustum& frustum);



class Camera
{
public:
	Camera();
	virtual ~Camera() {}

	Camera(const Vector3f& pos, const Vector3f& lookAt, const Vector3f& up = Vector3f::UNIT_Y);


	void SetViewPort(unsigned int Width,	unsigned int Height,
		unsigned int Left, unsigned int Top,
		float MinZ, float MaxZ);

	void SetFov(float fFov)			{ m_fFov = fFov; }
	void SetAspect(float fAspect)	{ m_fAspect = fAspect; }
	void SetNearFarPlane(float zn, float zf) { m_fZn = zn; m_fZf = zf;  };
	void SetViewVolume(float width, float height)	{	m_fWidth = width; m_fHeight = height;}
	void SetViewVolume(float l, float r, float b, float t);


	const Vector3f& GetEyePos() const		{ return m_vEye; }
	void	SetPosition(const Vector3f& pos) { m_vEye = pos; }
	Vector3f GetUpDir() const;
	Vector3f GetLookDir() const;
	Vector3f GetRightDir() const;

	float GetFov() const			{ return m_fFov; }
	float GetAspect() const		{ return m_fAspect; }
	float GetNearPlane() const	{ return m_fZn;	}
	float GetFarPlane() const		{ return m_fZf;	}


	const ViewPort& GetViewPort()	const	{ return m_ViewPort; }

	void SetTransform(const Matrix4f& matTransform);

	virtual void GetPickRay(Vector3f& origin, Vector3f& dir, const DeviceWindow* pWindow);

	virtual Ray GetRay(int x, int y, int nWindowWidth, int nWindowHeight)	{ assert(0); return Ray();	}
	

	Vector3f m_vEye = Vector3f(0.f, -45.0f, 0.0f);
	Vector3f m_vRight;
	Vector3f m_vUp = Vector3f::UNIT_Z;
	Vector3f m_vLookDir;

	Vector3f LookAt_ = Vector3f(0.0f, 0.0f, 0.0f);

	mutable Matrix4f m_matView;
	mutable bool m_bUpdateView;

	float m_fFov = Mathf::PI / 3.0f;
	float m_fAspect;
	float m_fZn = 1.0f;
	float m_fZf = 1000.0f;

	// Ortho
	float m_fWidth;
	float m_fHeight;
	float m_fL, m_fR, m_fB, m_fT;


	ViewPort m_ViewPort;

private:

};











/*



template<typename ValType>
class KeyMapper
{
public:
	typedef ValType ValueType;

	KeyMapper(ValType defaultVal = 0)
	{
		SetAllToDefault(defaultVal);
	}

	void SetAllToDefault(ValType defaultVal)
	{
		for (int i = 0; i < KeyEvent::KEYCODE_MAX; i++)
			m_aMap[i] = defaultVal;
	}


	void SetKey(uint key, ValType val)
	{
		m_aMap[key] = val;
	}

	ValType operator[] (uint key)
	{
		return m_aMap[key];
	}

protected:
	ValType	m_aMap[KeyEvent::KEYCODE_MAX];
};

typedef KeyMapper<ubyte> ByteKeyMapper;



class FPSCamera : public Camera, public KeyEventListener
{
public:
	enum CameraKeys
	{
		CAM_STRAFE_LEFT = 0,
		CAM_STRAFE_RIGHT,
		CAM_MOVE_FORWARD,
		CAM_MOVE_BACKWARD,
		CAM_MOVE_UP,
		CAM_MOVE_DOWN,
		CAM_RESET,
		CAM_MAXKEY,
		CAM_UNKNOWN = 0xFF
	};

	enum KeyStateMask
	{
		KEY_MASK_DOWN = 0x01,
	};


	FPSCamera(void);
	void ComputeRotAngle();

	virtual void OnKeyEvent(const KeyEvent& event);
	void UpdateFrame(float fElapsedTime);


	void SetMovVelocity(float v)		{	m_fMovVelocity = v;	}
	float GetMovVelocity() const		{	return m_fMovVelocity;	}
	void SetRotVelocity(float v)		{	m_fRotVelocity = v;	}
	float GetRotVelocity() const		{	return m_fRotVelocity;	}

	void SetAllowMoveTowardY(bool enable)
	{	m_bAllowMoveTowardY = enable;	}
	bool GetAllowMoveTowardY()	const
	{	return m_bAllowMoveTowardY;	}


protected:
	void GetMovement(Vector3f& mov, Vector2f& rot) const;
	void SetDefaultKey();

	float		m_fPitchAngle;
	float		m_fYawAngle;
	float		m_fMovVelocity;
	float		m_fRotVelocity;

	bool		m_bAllowMoveTowardY;
	

	ByteKeyMapper	m_KeyMapper;
	ubyte		m_aKeyState[CAM_MAXKEY];

	int m_nMouseLockX;
	int m_nMouseLockY;
	
		

};

*/

}// namespace





#endif