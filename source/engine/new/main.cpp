// GaEngine10.cpp : 定义应用程序的入口点。
//
#include <windows.h>
#include <tchar.h>
//#include <d3dx9.h>
#include <shellapi.h>



#include "../GaEngine/AppLayer/GaApplication.h"
#include "../GaEngine/math/GaVector.h"
#include "../GaEngine/math/GaColor.h"
#include "../GaEngine/math/GaPrimitive.h"
#include "../GaEngine/Render/GaCamera.h"
#include "../GaEngine/Render/GaRenderer.h"
#include <boost/shared_ptr.hpp>
#include "../GaEngine/kernel/GaUtil.h"

#include "../GaEngine/Kernel/GaConsole.h"
#include <time.h>

#include <string>
#include <sstream>
#include "../GaEngine/Kernel/GaDeviceWindow.h"

#include "../GaEngine/Material/GaMaterial.h"
#include "../GaEngine/Material/GaMaterialInstance.h"
#include "../GaEngine/Material/GaMaterial.h"
#include "../GaEngine/Render/GaResourceManager.h"
#include "../GaEngine/AppLayer/GaResourceFactory.h"
#include "../GaEngine/scene/GaController.h"
#include "../GaEngine/Render/GaHelper.h"



using namespace Ga;
using namespace std;



// 测试两平面的交界相对于某一个方向是否为边界
bool TestEdge(const Plane& p0, const Plane& p1, const Vector3f& dir)
{
	float t0 = DotProduct(p0.n, dir);
	float t1 = DotProduct(p1.n, dir);
	if (t0 * t1 < 0.0f)
	{
		return true;
	}

	return false;
}




class LineRenderer
{
public:
	LineRenderer();
	~LineRenderer();

	bool						Init();
	void						DrawFrustum(Camera* pCamera, const Color& color, const Vector3f& dir);



private:
	MaterialInstancePtr			m_pMatInst;
	RenderOperator				m_op;


};

LineRenderer::LineRenderer()
{

}

LineRenderer::~LineRenderer()
{

}

bool LineRenderer::Init()
{
	m_pMatInst = Global::GetResourceManager()->CreateMaterialInst("MtlHelper");

	//m_op.pMaterial = m_pMatInst->GetMaterialTemplate();
	//m_op.pMtlInst = m_pMatInst.get();
	//m_op.pVertDesc = Global::GetResourceManager()->GetVertexDesc(ResourceManager::PLAIN_VEXDESC).Get();
	//m_op.nPrimType = RenderOperator::PT_LINELIST;
	//m_op.nBaseVertexIndex = 0;
	//m_op.nMinVertexIndex = 0;
	//m_op.nStartIndex = 0;
	//m_op.nVertexStride = sizeof(Vector3f);
	return true;

}

void LineRenderer::DrawFrustum(Camera* pCamera, const Color& color, const Vector3f& dir)
{
	float halfFov = pCamera->GetFov() / 2.0f;
	float nearHalfHeight = pCamera->GetNearPlane() * Mathf::Tan(halfFov);
	float farHalfHeight = pCamera->GetFarPlane() * Mathf::Tan(halfFov);
	float nearHalfWidth = pCamera->GetAspect() * nearHalfHeight;
	float farHalfWidth = pCamera->GetAspect() * farHalfHeight;

	Vector3f lookDir = pCamera->GetLookDir();
	Vector3f upDir = pCamera->GetUpDir();
	Vector3f rightDir = pCamera->GetRightDir();

	Vector3f nearPos = pCamera->GetEyePos() + lookDir * pCamera->GetNearPlane();
	Vector3f farPos = pCamera->GetEyePos() + lookDir * pCamera->GetFarPlane();

	static Vector3f corner[8];
	corner[0] = nearPos - rightDir * nearHalfWidth + upDir * nearHalfHeight;
	corner[1] = nearPos + rightDir * nearHalfWidth + upDir * nearHalfHeight;
	corner[2] = nearPos + rightDir * nearHalfWidth - upDir * nearHalfHeight;
	corner[3] = nearPos - rightDir * nearHalfWidth - upDir * nearHalfHeight;

	corner[4] = farPos - rightDir * farHalfWidth + upDir * farHalfHeight;
	corner[5] = farPos + rightDir * farHalfWidth + upDir * farHalfHeight;
	corner[6] = farPos + rightDir * farHalfWidth - upDir * farHalfHeight;
	corner[7] = farPos - rightDir * farHalfWidth - upDir * farHalfHeight;

	static ushort index[] = 
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4, 
		0, 4, 1, 5, 2, 6, 3, 7,
	};

	m_op.pMtlInst->SetValueF("Color", color.Ptr(), 4);


	m_op.pSystemVertBuff = (float*)corner[0].Ptr();
	m_op.pSystemIndexBuff = index;
	m_op.nNumVertices = 8;
	m_op.nStartIndex = 0;
	m_op.nPrimitiveCount = 12;
	m_op.pMaterial->BeginPass(0);
	Global::GetRenderer()->ExecuteOperator(m_op);




	Frustum frustum;
	ExtractFrustum(pCamera, frustum);

	/*corner[0] = IntersectPlane(frustum.planes[Frustum::PID_FRONT], frustum.planes[Frustum::PID_LEFT], frustum.planes[Frustum::PID_TOP]);
	corner[1] = IntersectPlane(frustum.planes[Frustum::PID_FRONT], frustum.planes[Frustum::PID_TOP], frustum.planes[Frustum::PID_RIGHT]);
	corner[2] = IntersectPlane(frustum.planes[Frustum::PID_FRONT], frustum.planes[Frustum::PID_RIGHT], frustum.planes[Frustum::PID_BOTTOM]);
	corner[3] = IntersectPlane(frustum.planes[Frustum::PID_FRONT], frustum.planes[Frustum::PID_BOTTOM], frustum.planes[Frustum::PID_LEFT]);

	corner[4] = IntersectPlane(frustum.planes[Frustum::PID_BACK], frustum.planes[Frustum::PID_LEFT], frustum.planes[Frustum::PID_TOP]);
	corner[5] = IntersectPlane(frustum.planes[Frustum::PID_BACK], frustum.planes[Frustum::PID_TOP], frustum.planes[Frustum::PID_RIGHT]);
	corner[6] = IntersectPlane(frustum.planes[Frustum::PID_BACK], frustum.planes[Frustum::PID_RIGHT], frustum.planes[Frustum::PID_BOTTOM]);
	corner[7] = IntersectPlane(frustum.planes[Frustum::PID_BACK], frustum.planes[Frustum::PID_BOTTOM], frustum.planes[Frustum::PID_LEFT]);

	m_op.pMtlInst->SetValueF("Color", Color::RED.Ptr(), 4);

*/

	Vector3f insidePoint = (nearPos + farPos) / 2.0f;

	// 前一个面顺着边，后一个面逆着边
	vector<pair<Frustum::PlaneID, Frustum::PlaneID>> planePair;
	planePair.push_back(make_pair(Frustum::PID_FRONT, Frustum::PID_TOP));
	planePair.push_back(make_pair(Frustum::PID_FRONT, Frustum::PID_RIGHT));
	planePair.push_back(make_pair(Frustum::PID_FRONT, Frustum::PID_BOTTOM));
	planePair.push_back(make_pair(Frustum::PID_FRONT, Frustum::PID_LEFT));

	planePair.push_back(make_pair(Frustum::PID_TOP, Frustum::PID_BACK));
	planePair.push_back(make_pair(Frustum::PID_RIGHT, Frustum::PID_BACK));
	planePair.push_back(make_pair(Frustum::PID_BOTTOM, Frustum::PID_BACK));
	planePair.push_back(make_pair(Frustum::PID_LEFT, Frustum::PID_BACK));

	planePair.push_back(make_pair(Frustum::PID_TOP, Frustum::PID_LEFT));
	planePair.push_back(make_pair(Frustum::PID_RIGHT, Frustum::PID_TOP));
	planePair.push_back(make_pair(Frustum::PID_BOTTOM, Frustum::PID_RIGHT));
	planePair.push_back(make_pair(Frustum::PID_LEFT, Frustum::PID_BOTTOM));


	//PID_TOP = 0,
	//	PID_BOTTOM,
	//	PID_LEFT,
	//	PID_RIGHT,
	//	PID_FRONT,
	//	PID_BACK,

	

	vector<Vector3f> points;
	vector<Vector3f> normals;
	ushort edgeIndex[24];
	int numEdge = 0;
	for (int i = 0; i < planePair.size(); i++)
	{	
		pair<Frustum::PlaneID, Frustum::PlaneID> twoPlane = planePair[i];
		if (TestEdge(frustum.planes[twoPlane.first],
			frustum.planes[twoPlane.second], dir))
		{
			edgeIndex[numEdge * 2 + 0] = index[i * 2 + 0];
			edgeIndex[numEdge * 2 + 1] = index[i * 2 + 1];
			numEdge++;

			// 如果是第一个平面，边是顺着的，如果是第二个平面，边是逆着的
			Vector3f edgeDir;
			if (DotProduct(frustum.planes[twoPlane.first].n, dir) > 0.0f)
			{
				edgeDir = corner[index[i * 2 + 1]] - corner[index[i * 2 + 0]];
			}
			else
			{
				edgeDir = corner[index[i * 2 + 0]] - corner[index[i * 2 + 1]];
			}

			edgeDir.Normalize();

			Vector3f normal = CrossProduct(dir, edgeDir);
			
			Vector3f pos = corner[index[i * 2 + 0]];
			points.push_back(pos);
			points.push_back(pos + dir * 200.0f);

			pos = corner[index[i * 2 + 1]];
			points.push_back(pos);
			points.push_back(pos + dir * 200.0f);

			//  normal
			pos = corner[index[i * 2 + 0]];
			normals.push_back(pos);
			normals.push_back(pos + normal * 200.0f);

			pos = corner[index[i * 2 + 1]];
			normals.push_back(pos);
			normals.push_back(pos + normal * 200.0f);


		}
	}

	m_op.pMtlInst->SetValueF("Color", Color::RED.Ptr(), 4);
	m_op.nPrimitiveCount = numEdge;
	m_op.pSystemIndexBuff = edgeIndex;
	Global::GetRenderer()->ExecuteOperator(m_op);



	points.push_back(pCamera->GetEyePos());
	points.push_back( pCamera->GetEyePos() + dir * 200.0f);


	m_op.nPrimitiveCount = points.size() / 2;
	m_op.pSystemVertBuff = points[0].Ptr();
	m_op.pSystemIndexBuff = NULL;
	Global::GetRenderer()->ExecuteOperator(m_op);

	//show normal
	m_op.pMtlInst->SetValueF("Color", Color::GREEN.Ptr(), 4);
	m_op.nPrimitiveCount = normals.size() / 2;
	m_op.pSystemVertBuff = normals[0].Ptr();
	m_op.pSystemIndexBuff = NULL;
	Global::GetRenderer()->ExecuteOperator(m_op);
}



class MyApp : public Application, public KeyEventListener, public MouseEventListener
{
public:


	virtual bool		OnInitApp();
	virtual bool		OnCreateDevice();
	virtual bool		OnResetDevice();
	virtual void		OnUpdateScene(float fElapsedTime);
	virtual void		OnRender();
	virtual void		OnLostDevice();
	virtual void		OnDestroyDevice();





	virtual void	OnKeyEvent(const KeyEvent& event);
	virtual void	OnMouseEvent(const MouseEvent& event);


	void			SwitchCamera();


	void			LoadResource();

	bool			LoadConfig();

	const string&	GetErrorMsg() const { return m_strErrorMsg;	}
	
	Color							m_Color;
	
	OrbitMouseController			m_MouseController;


	CameraPtr						m_pCameraObj;
	vector<Attachable*>				m_Cameras;
	uint							m_nCurentCamera;
	
	string							m_strErrorMsg;


	LightObject*					m_pLight;



	RenderableObjectPtr				m_pTempRenderble;
	SceneNodePtr					m_pNode;

	shared_ptr<TextRenderer>		m_pTextRenderer;
	LineRenderer					m_LineRenderer;

};




bool MyApp::LoadConfig()
{
	
	return true;

}



void ComputeWorldPos(uint x, uint y)
{
	float xPos = (x / 800.0f) * 2.0f - 1.0f;
	float yPos = ((600 - y) / 600.0f) * 2.0f - 1.0f;

	Camera* pCamera = Global::GetPipeline()->GetRenderingContext()->GetCamera();
	Matrix4f matProj = pCamera->GetProjMatrix();

	float u = xPos / matProj._11;
	float v = yPos / matProj._22;

	Vector3f lookDir = pCamera->GetLookDir();
	Vector3f upDir = pCamera->GetUpDir();
	Vector3f rightDir = pCamera->GetRightDir();

	Vector3f dir = lookDir + rightDir * u + upDir * v;

	float nearplane = pCamera->GetNearPlane();
	float farplane = pCamera->GetFarPlane();

	Vector3f nearPos = dir * nearplane;
	Vector3f farPos = dir * farplane;

	stringstream str;
	str << x << "  " << y << endl;
	str << nearPos.x << ", " << nearPos.y << ", " << nearPos.z << endl;
	str << farPos.x << ", " << farPos.y << ", " << farPos.z << endl << endl;

	Console::AppendText(str.str().c_str());
}





bool MyApp::OnInitApp()
{
	return true;
}

bool MyApp::OnCreateDevice()
{
	m_Color = Color::WHITE;	

	WindowsEventDispatcher::Instance()->SetMouseEventListener(this);
	WindowsEventDispatcher::Instance()->SetKeyEventListener(this);


	ResourceFactory factory;

	string scriptFileName = "../GaEngine/material.script";
	string strCmd = GetCommandLineA();
	size_t pos = strCmd.find_last_of('\"');

	if (pos < strCmd.length() - 2)
	{
		scriptFileName = strCmd.substr(pos + 2);
	}

	factory.LoadResource(scriptFileName);

	m_pTextRenderer.reset(new TextRenderer("../res/text.bmp", 1, 10));
	m_pTextRenderer->Init();
	m_LineRenderer.Init();


	m_nCurentCamera = 0;
	m_Cameras.clear();
	m_Scene.GetRootNode()->GetAttachableRecursive(m_Cameras, Attachable::TYPE_CAMERA);

	if (m_Cameras.empty())
	{
		m_pCameraObj.reset(new CameraPerspectiveFov());

		Vector3f vLookAt(0.0f, 0.0f, 0.0f);
		Vector3f vEyePos(0.0f, -20.0f, 10.0f);

		/*m_pCameraObj->SetEyePos(vEyePos);
		m_pCameraObj->SetLookAt(vLookAt);
		m_pCameraObj->SetUpDir(Vector3f::UNIT_Z);*/
		m_pCameraObj->SetAspect(m_nWindowWidth / (float)m_nWindowHeight);

		//m_CameraController.SetCamera(pCamera, vLookAt, vEyePos);
		m_Cameras.push_back(m_pCameraObj.get());
	}

	Camera* pCamObj = (Camera*)m_Cameras[0];
	Global::GetPipeline()->GetRenderingContext()->SetCamera(pCamObj);
	m_Scene.UpdateWorld(0.0f, true);
	SwitchCamera();




	if (!m_pPipeline->Init())
	{
		return false;
	}

	return true;
}





bool MyApp::OnResetDevice()
{
	float aspect = (float)m_nWindowWidth / (float)m_nWindowHeight;
	for (vector<Attachable*>::iterator it = m_Cameras.begin();
		it != m_Cameras.end(); ++it)
	{
		Camera* pCamera = (Camera*)(*it);
		pCamera->SetAspect(aspect);
	}

	return true;
}

void MyApp::OnLostDevice()
{	
}

void MyApp::OnDestroyDevice()
{
	m_pTextRenderer.reset();
}


void MyApp::OnUpdateScene(float fElapsedTime)
{

	m_Scene.UpdateWorld(fElapsedTime);
	m_Scene.FlushToPipeline(m_pPipeline.get());
}

static bool gUsePlane = false;
void MyApp::OnRender()
{
	m_pPipeline->BeginPipe();

	m_pPipeline->Render();

	SceneNode* pNode = m_Scene.GetRootNode()->FindNodeByName("FDirect01");
	Vector3f dir = Vector3f::UNIT_Z;
	if (pNode)
	{
		Quaternionf qRot = pNode->GetWorldRotation();
		Vector3Rotate(dir, Vector3f::UNIT_Z, qRot);
	}
	m_LineRenderer.DrawFrustum((Camera*)m_Cameras[0], Color::WHITE, dir);

	int fps = (int)m_fps;
	char buff[8];
	itoa(fps, buff, 10);

	m_pTextRenderer->DrawText(buff, 0, 0, 15, 20, Color::WHITE);
	m_pPipeline->EndPipe();
}


void MyApp::SwitchCamera()
{
	Camera* pCamObj = (Camera*)m_Cameras[m_nCurentCamera];
	Global::GetPipeline()->GetRenderingContext()->SetCamera(pCamObj);

	m_MouseController.BindNode(pCamObj->GetParentNode());
}


void MyApp::OnKeyEvent(const KeyEvent& event) 
{
	if (event.Code == KeyEvent::KEYCODE_W && event.Type == KeyEvent::KeyDown)
	{
		//m_nFillMode = (m_nFillMode == D3DFILL_SOLID) ? D3DFILL_WIREFRAME : D3DFILL_SOLID;
		//m_pRenderer->m_pd3dDevice9->SetRenderState(D3DRS_FILLMODE, m_nFillMode);
	}

	if (event.Type == KeyEvent::KeyDown && event.Code == KeyEvent::KEYCODE_TAB)
	{
		m_nCurentCamera = ++m_nCurentCamera % m_Cameras.size();
		SwitchCamera();
	}

	if (event.Type == KeyEvent::KeyDown && event.Code == KeyEvent::KEYCODE_1)
	{
		gUsePlane = !gUsePlane;
	}
	
	if (event.Type == KeyEvent::KeyDown && event.Code == KeyEvent::KEYCODE_SPACE)
	{
		static bool b = true;
		if (b = !b)
		{
			Global::GetTimer()->Start();
		}
		else
		{
			Global::GetTimer()->Stop();
		}
	}
}


void MyApp::OnMouseEvent(const MouseEvent& event)
{
	m_MouseController.OnMouseEvent(event);


	/*if (event.Type == MouseEvent::LButtonDown)
	{
		int width, height, xMouse, yMouse;
		event.pWindow->GetWindowSize(width, height);
		event.pWindow->GetCursorPos(xMouse, yMouse);

		ComputeWorldPos(xMouse, yMouse);
	}*/

}




int WINAPI _tWinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{

	MyApp app;

	if (!app.Create(L"app", 1024, 768))
		return 1;

	app.Run();

	return 0;

}


