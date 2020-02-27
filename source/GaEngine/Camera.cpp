#include "stdHeader.h"
#include "Camera.h"
#include "DeviceWindow.h"
#include "Console.h"
#include <sstream>
namespace Aurora
{

Vector3f ScreenPointToVector(int xPos, int yPos, int nCenterX, int nCenterY, int nRadius)
{
	float x = (xPos - nCenterX) / (float)nRadius;
	float y = -(yPos - nCenterY) / (float)nRadius;
	
	float z = 0.0f;
	float mag = x * x + y * y;
	if (mag > 1.0f)
	{
		float inv = 1.0f / Mathf::Sqrt(mag);
		x *= inv;
		y *= inv;
	}
	else
	{
		z = Mathf::Sqrt(1.0f - mag);
	}
	
	return Vector3f(x, y, z);
}
//
//
//void LoadXmlCamera(TiXmlNode* pXmlCamera, SceneNode* pSceneNode)
//{
//	CameraPtr pCamera(new CameraPerspectiveFov());
//
//	float nearPlane = 1.0f, farPlane = 1000.0f, fov = Mathf::HALF_PI / 2.0f;
//
//	TiXmlNode* pElem = pXmlCamera->FirstChild();
//	while (pElem)
//	{
//		if (strcmp(pElem->Value(), "FOV") == 0)
//		{
//			string text = pElem->FirstChild()->ToText()->Value();
//			fov = (float)atof(text.c_str());
//		}
//		else if (strcmp(pElem->Value(), "NearPlane") == 0)
//		{
//			string text = pElem->FirstChild()->ToText()->Value();
//			nearPlane = (float)atof(text.c_str());
//		}
//		else if (strcmp(pElem->Value(), "FarPlane") == 0)
//		{
//			string text = pElem->FirstChild()->ToText()->Value();
//			farPlane = (float)atof(text.c_str());
//		}
//
//		pElem = pElem->NextSibling();
//	}
//
//	pCamera->SetNearFarPlane(nearPlane, farPlane);
//
//	uint nWidth = 0, nHeight = 0;
//	Global::GetRenderer()->GetFrameBufferSize(nWidth, nHeight);
//	float aspect = nWidth / (float)nHeight;
//	pCamera->SetAspect(aspect);
//	pCamera->SetFov(fov / aspect);
//
//	//pSceneNode->Attach(pCamera);
//
//	//pCamera->UpdateFromParentNode();
//}



//--------------------------------------------------
ArcBall::ArcBall(int nCenterX, int nCenterY, uint nRadius, float fTransScale, bool bLmtInArea)
:m_bIsActive(true),m_nCenterX(nCenterX),m_nCenterY(nCenterY),m_nRadius(nRadius),
m_bIsLButtonDown(false),m_bIsRButtonDown(false),m_bIsMButtonDown(false),m_fTransScale(fTransScale),
m_bLmtInArea(bLmtInArea)
{
	m_vCurrTrans = Vector3f::ZERO;
	m_qCurr = Quaternionf::IDENTITY;
	m_qLast = Quaternionf::IDENTITY;


}

void ArcBall::SetOperatingArea(int nCenterX, int nCenterY, uint nRadius)
{
	m_nCenterX = nCenterX;
	m_nCenterY = nCenterY;
	m_nRadius  = nRadius;
}

//---------------------------------------------------
Vector2f
ArcBall::ScreenPointToPos(int xPos, int yPos)
{
	Vector2f v;
	v.x = (xPos - m_nCenterX) / (float)m_nRadius;
	v.y = -(yPos - m_nCenterY) / (float)m_nRadius;
	return v;
}
//---------------------------------------------------
void 
ArcBall::OnMouseEvent(const MouseEvent& event)
{
	if (!m_bIsActive)
	{
		return;
	}
	
	if (event.Type == MouseEvent::LButtonDown)
	{
		// judge if the point is in limting area
		// the limiting area is a square
		if (m_bLmtInArea)
		{
			// limting area is a little bigger than the arc ball
			int areaRadius = (int)(m_nRadius * 1.2f);
			if (event.xPos < m_nCenterX - areaRadius ||
				event.xPos > m_nCenterX + areaRadius ||
				event.yPos < m_nCenterY - areaRadius ||
				event.yPos > m_nCenterY + areaRadius)
			{
				return;
			}

		}
		m_bIsLButtonDown = true;
		m_vBtnDown = ScreenPointToVector(event.xPos, event.yPos, m_nCenterX, m_nCenterY, m_nRadius);
		event.pWindow->SetCapture();
	}
	else if (event.Type == MouseEvent::LButtonUp)
	{
		if (m_bIsLButtonDown)
		{
			m_bIsLButtonDown = false;
			m_qLast = m_qCurr;
			event.pWindow->ReleaseCapture();
		}
	}
	else if (event.Type == MouseEvent::RButtonDown)
	{
		m_bIsRButtonDown = true;
		m_vLastPoint = ScreenPointToPos(event.xPos, event.yPos);
		event.pWindow->SetCapture();
	}
	else if (event.Type == MouseEvent::RButtonUp)
	{
		if (m_bIsRButtonDown)
		{
			m_bIsRButtonDown = false;
			event.pWindow->ReleaseCapture();
		}
	}
	else if (event.Type == MouseEvent::MButtonDown)
	{
		m_bIsMButtonDown = true;
		m_vLastPoint = ScreenPointToPos(event.xPos, event.yPos);
		event.pWindow->SetCapture();
	}
	else if (event.Type == MouseEvent::MButtonUp)
	{
		if (m_bIsMButtonDown)
		{
			m_bIsMButtonDown = false;
			event.pWindow->ReleaseCapture();
		}
	}
	else if (event.Type == MouseEvent::MouseMove)
	{
		if (m_bIsLButtonDown)
		{
			Vector3f vCurr = ScreenPointToVector(event.xPos, event.yPos, m_nCenterX, m_nCenterY, m_nRadius);

			Quaternionf qDrag;
			QuaternionVectorToVector(qDrag, m_vBtnDown, vCurr);
			QuaternionMultiply(m_qCurr, qDrag, m_qLast);
			m_qCurr.Normalize();
		}
		if (m_bIsRButtonDown || m_bIsMButtonDown)
		{
			Vector2f currPos = ScreenPointToPos(event.xPos, event.yPos);
			if (m_bIsRButtonDown)
				m_vCurrTrans += Vector3f((currPos.x - m_vLastPoint.x) * m_fTransScale,
								(currPos.y - m_vLastPoint.y) * m_fTransScale, 0.0f);
			if (m_bIsMButtonDown)
				m_vCurrTrans += Vector3f(0.0f, 0.0f, (m_vLastPoint.y - currPos.y) * 2.0f * m_fTransScale);
			m_vLastPoint = currPos;
		}
	}


}
//--------------------------------------------------
void 
ArcBall::GetTransformation(Matrix4f& t) const
{
	Matrix3f mRot;
	MatrixFromQuaternion(mRot, m_qCurr);
	MatrixTransform(t, mRot, m_vCurrTrans);
}
//================================================================================================




Vector3f IntersectPlane(const Plane& p0, const Plane& p1, const Plane& p2)
{
	Matrix3f mat(p0.n.x, p1.n.x, p2.n.x,
		p0.n.y, p1.n.y, p2.n.y,
		p0.n.z, p1.n.z, p2.n.z);

	Matrix3f matInv;
	MatrixInverse(matInv, mat);

	Vector3f point;
	Vector3Transform(point, Vector3f(-p0.d, -p1.d, -p2.d), matInv);

	return point;
}




void ExtractFrustum(const Matrix4f& viewProj, Frustum& frustum)
{
	Matrix4f mat = viewProj;


	Vector4f m1(mat._11, mat._21, mat._31, mat._41);
	Vector4f m2(mat._12, mat._22, mat._32, mat._42);
	Vector4f m3(mat._13, mat._23, mat._33, mat._43);
	Vector4f m4(mat._14, mat._24, mat._34, mat._44);


	frustum.planes[Frustum::PID_TOP] = m4 - m2;
	frustum.planes[Frustum::PID_TOP].Normalize();

	frustum.planes[Frustum::PID_BOTTOM] = m4 + m2;
	frustum.planes[Frustum::PID_BOTTOM].Normalize();

	frustum.planes[Frustum::PID_LEFT] = m4 + m1;
	frustum.planes[Frustum::PID_LEFT].Normalize();

	frustum.planes[Frustum::PID_RIGHT] = m4 - m1;
	frustum.planes[Frustum::PID_RIGHT].Normalize();

	frustum.planes[Frustum::PID_FRONT] = m4 + m3;
	frustum.planes[Frustum::PID_FRONT].Normalize();

	frustum.planes[Frustum::PID_BACK] = m4 - m3;
	frustum.planes[Frustum::PID_BACK].Normalize();

}



//=============================================================================================
Camera::Camera():
m_vEye(0.0f, 0.0f, 0.0f), m_vRight(1.0f, 0.0f, 0.0f), m_vUp(0.0f, 1.0f, 0.0f),m_vLookDir(0.0f, 0.0f, -1.0f),
m_bUpdateView(true), m_fAspect(1.0f),m_fWidth(100.0f),m_fHeight(100.0f)
{
}

Camera::Camera(const Vector3f& pos, const Vector3f& lookAt, const Vector3f& up)
{
	m_vEye = pos;
	LookAt_ = lookAt;
	m_vUp = up;
}

//---------------------------------------------------------------------------
void Camera::SetViewPort(unsigned int Width,	unsigned int Height,
					unsigned int Left, unsigned int Top,
					float MinZ, float MaxZ)
{
	m_ViewPort.Left		= Left;
	m_ViewPort.Top		= Top;
	m_ViewPort.Width	= Width;
	m_ViewPort.Height	= Height;
	m_ViewPort.MinZ		= MinZ;
	m_ViewPort.MaxZ		= MaxZ;
}

//---------------------------------------------------------------------------
void Camera::GetPickRay(Vector3f& origin, Vector3f& dir, const DeviceWindow* pWindow)
{
	assert(0 && "not implentmented");

	//int width, height, xMouse, yMouse;
	//pWindow->GetWindowSize(width, height);
	//pWindow->GetCursorPos(xMouse, yMouse);

	//float xPos, yPos;
	//// get mouse position in screen cord in range [-1, 1]
	//xPos = ((float)xMouse / (float)(width - 1)) * 2.0f - 1.0f;
	//yPos = ((float)(height - 1 - yMouse) / (float)(height - 1)) * 2.0f - 1.0f;

	//Matrix4f matProj = GetProjMatrix();
	//// get view cord
	//xPos /= matProj._11;
	//yPos /= matProj._22;

	//// update view Matrix
	//GetViewMatrix();

	//Vector3f xDir = GetRightDir();
	//xDir *= xPos;

	//Vector3f yDir(m_matView._12*yPos, m_matView._22*yPos, m_matView._32*yPos);

	//Vector3f zDir = GetLookDir();

	//dir = xDir + yDir + zDir;
	//dir.Normalize();
	//origin = m_vEye;
}


//------------------------------------------------------------------------


void Camera::SetViewVolume(float l, float r, float b, float t)
{
	m_fL = l;
	m_fR = r;
	m_fB = b;
	m_fT = t;
}

void Camera::SetTransform(const Matrix4f& matTransform)
{
	Vector3Transform(m_vEye, Vector3f::ZERO, matTransform);
	Vector3Rotate(m_vRight, Vector3f::UNIT_X, matTransform);
	Vector3Rotate(m_vUp, Vector3f::UNIT_Z, matTransform);
	Vector3Rotate(m_vLookDir, Vector3f::UNIT_Y, matTransform);

}


//
//void Camera::UpdateFromParentNode()
//{
//	m_vEye = GetParentNode()->GetWorldTranslation();
//
//	Quaternionf qRot = GetParentNode()->GetWorldRotation();
//
//	Vector3Rotate(m_vRight, Vector3f::UNIT_X, qRot);
//	Vector3Rotate(m_vUp, Vector3f::UNIT_Y, qRot);
//	Vector3Rotate(m_vLookDir, -Vector3f::UNIT_Z, qRot);
//
//	m_bUpdateView = true;
//	m_bUpdateViewProj = true;
//}

//////////////////////////////////////////////////////////////////////////

//
//const Matrix4f& CameraOrthoGetProjMatrix()	const
//{
//	if (m_bUpdateProj)
//	{
//		MatrixOrthoRH(m_matProj, m_fWidth, m_fHeight, m_fZn, m_fZf);
//		m_bUpdateProj = false;
//	}
//	return m_matProj;
//}
//
//
//const Matrix4f& CameraOrthoOffCenter::GetProjMatrix()	const
//{
//	if (m_bUpdateProj)
//	{
//		MatrixOrthoOffCenterRH(m_matProj, m_fL, m_fR, m_fB, m_fT, m_fZn, m_fZf);
//		m_bUpdateProj = false;
//	}
//	return m_matProj;
//}
//
//const Matrix4f& CameraPerspectiveFov::GetProjMatrix()	const
//{
//	if (m_bUpdateProj)
//	{
//		MatrixPerspectiveFovRH(m_matProj, m_fFov, m_fAspect, m_fZn, m_fZf);
//		m_bUpdateProj = false;
//	}
//	return m_matProj;
//}
//
//Ray CameraPerspectiveFov::GetRay(int x, int y, int nWindowWidth, int nWindowHeight)
//{
//	// get mouse position in screen cord in range [-1, 1]
//	float xPos = ((float)x / (float)(nWindowWidth - 1)) * 2.0f - 1.0f;
//	float yPos = ((float)(nWindowHeight - 1 - y) / (float)(nWindowHeight - 1)) * 2.0f - 1.0f;
//
//	Vector3f lookDir = GetLookDir();
//	Vector3f upDir = GetUpDir();
//	Vector3f rightDir = GetRightDir();
//
//	const Matrix4f& matProj = GetProjMatrix();
//
//	float u = xPos / matProj._11;
//	float v = yPos / matProj._22;
//
//	Vector3f dir = lookDir + rightDir * u + upDir * v;
//	dir.Normalize();
//
//	return Ray(m_vEye, dir);
//}
//






/*


//=========================================================================================

FPSCamera::FPSCamera(void):
m_KeyMapper(CAM_UNKNOWN)
{
	m_fMovVelocity = 10.0f;
	m_fRotVelocity = 0.01f;
	m_bAllowMoveTowardY = false;
	memset(m_aKeyState, 0, sizeof(m_aKeyState));

//	System::Instance()->GetWindowPos(m_nMouseLockX, m_nMouseLockY);
	m_nMouseLockX = 100;
	m_nMouseLockY = 100;
	Device::Instance()->SetCursorPos(m_nMouseLockX, m_nMouseLockY);
	SetDefaultKey();
	
	ComputeRotAngle();
}

void FPSCamera::SetDefaultKey()
{
	m_KeyMapper.SetKey(KeyEvent::KEYCODE_W, CAM_MOVE_FORWARD);
	m_KeyMapper.SetKey(KeyEvent::KEYCODE_S, CAM_MOVE_BACKWARD);
	m_KeyMapper.SetKey(KeyEvent::KEYCODE_A, CAM_STRAFE_LEFT);
	m_KeyMapper.SetKey(KeyEvent::KEYCODE_D, CAM_STRAFE_RIGHT);
	m_KeyMapper.SetKey(KeyEvent::KEYCODE_Q, CAM_MOVE_UP);
	m_KeyMapper.SetKey(KeyEvent::KEYCODE_E, CAM_MOVE_DOWN);

	m_KeyMapper.SetKey(KeyEvent::KEYCODE_HOME, CAM_RESET);
}
//-------------------------------------------------------------
// computer pitch and yaw angle from looking direction
void FPSCamera::ComputeRotAngle()
{
	Vector3f lookDir = m_vLookAt - m_vEye;

	m_fYawAngle = Mathf::ATan2(lookDir.x, -lookDir.z);
	float len = Mathf::Sqrt(lookDir.x * lookDir.x + lookDir.z * lookDir.z);
	m_fPitchAngle = Mathf::ATan2(lookDir.y, len);
}

//-------------------------------------------------------------

void FPSCamera::OnKeyEvent(const KeyEvent& event)
{
	ubyte camKey = m_KeyMapper[event.Code];
	if (event.Type == KeyEvent::KeyDown)
	{
		if (camKey != CAM_UNKNOWN)
		{
			m_aKeyState[camKey] |= KEY_MASK_DOWN;
		}
	}
	else
	{
		if (camKey != CAM_UNKNOWN)
		{
			m_aKeyState[camKey] &= ~KEY_MASK_DOWN;
		}
	}
}
//-------------------------------------------------------------
// get move direction and rotate offset from input
void FPSCamera::GetMovement(Vector3f& mov, Vector2f& rot) const
{
	Vector3f m = Vector3f::ZERO;
	if (m_aKeyState[CAM_MOVE_FORWARD] & KEY_MASK_DOWN)
	{
		m.z -= 1.0f;
	}
	if (m_aKeyState[CAM_MOVE_BACKWARD] & KEY_MASK_DOWN)
	{
		m.z += 1.0f;
	}
	if (m_bAllowMoveTowardY)
	{
		if (m_aKeyState[CAM_MOVE_UP] & KEY_MASK_DOWN)
		{
			m.y += 1.0f;
		}
		if (m_aKeyState[CAM_MOVE_DOWN] & KEY_MASK_DOWN)
		{
			m.y -= 1.0f;
		}
	}
	if (m_aKeyState[CAM_STRAFE_LEFT] & KEY_MASK_DOWN)
	{
		m.x -= 1.0f;
	}
	if (m_aKeyState[CAM_STRAFE_RIGHT] & KEY_MASK_DOWN)
	{
		m.x += 1.0f;
	}
	// has key down
	if (m.LengthSQ() > 1.0f)
		m.Normalize();
	mov = m;

	int mouseX, mouseY;
	Device::Instance()->GetCursorPos(mouseX, mouseY);
	// rotation along y axis with right hand cord
	rot.y = (float)(m_nMouseLockX - mouseX);
	// rotation along x axis with right hand cord
	rot.x = (float)(m_nMouseLockY - mouseY);

	Device::Instance()->SetCursorPos(m_nMouseLockX, m_nMouseLockY);

}

//--------------------------------------------------------------------
void FPSCamera::UpdateFrame(float fElapsedTime)
{
	if (m_aKeyState[CAM_RESET] & KEY_MASK_DOWN)
	{
		Reset();
		ComputeRotAngle();
		return;
	}

	Vector3f move;
	Vector2f rot;

	GetMovement(move, rot);
	move *= (m_fMovVelocity * fElapsedTime);
	rot	 *= m_fRotVelocity;

	m_fPitchAngle += rot.x;
	m_fYawAngle   += rot.y;
	m_fPitchAngle = Mathf::Clamp(m_fPitchAngle, -Mathf::HALF_PI, Mathf::HALF_PI);

	Matrix3f matRot;
	MatrixRotationYawPitchRoll(matRot, m_fYawAngle, m_fPitchAngle, 0.0f);

	Vector3f vAhead(0.0f, 0.0f, -1.0f);
	// ×ó³ËÓÒ³Ë£¿
	//Vector3Transform(vAhead, matRot, vAhead);

	// if not allow move toward y recompute rotate matrix
	if (!m_bAllowMoveTowardY)
	{
		MatrixRotationYawPitchRoll(matRot, m_fYawAngle, 0.0f, 0.0f);
	}
	//Vector3Transform(move, matRot, move);
	
	Vector3f newEye = m_vEye + move;
	SetEyePos(newEye);
	SetLookAt(newEye + vAhead);

}

*/
}// end of namespace