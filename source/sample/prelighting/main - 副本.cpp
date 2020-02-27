// GaEngine10.cpp : 定义应用程序的入口点。
//
#include <windows.h>
#include <tchar.h>
#include <d3dx9.h>
#include <shellapi.h>



#include "../GaEngine/AppLayer/GaApplication.h"
#include "../GaEngine/math/GaVector.h"
#include "../GaEngine/math/GaColor.h"
#include "../GaEngine/Render/GaCamera.h"
#include "../GaEngine/Render/GaRenderer.h"
#include "../GaEngine/Render/GaPipeline.h"
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



using namespace Ga;
using namespace std;





class TextRenderer
{
public:
	struct UIVertex
	{
		Vector2f		pos;
		Vector2f		tex;
	};

	// name of texture file, number of rows and columns of characters in texture
	TextRenderer(const string& strTexture, uint nRow, uint nCol);
	~TextRenderer();

	bool				Init();
	void				DrawText(const char* pText, uint xPos, uint yPos, uint cw, uint ch, const Color& color);

private:
	static const int				MAX_TEXT = 10;
	string							m_strTexture;
	MaterialInstancePtr				m_pMatInst;

	vector<UIVertex>				m_VertBuff;
	vector<ushort>					m_Index;

	uint							m_nCol;
	uint							m_nRow;

	RenderOperator					m_op;


};


TextRenderer::TextRenderer(const string& strTexture, uint nRow, uint nCol):
m_strTexture(strTexture),m_nRow(nRow),m_nCol(nCol)
{
}

TextRenderer::~TextRenderer()
{
}

bool TextRenderer::Init()
{
	float fWidth = 1.0f / (float)MAX_TEXT;

	m_pMatInst = Global::GetResourceManager()->CreateMaterialInst("MtlUI");
	m_pMatInst->SetTexture("texUI", Global::GetResourceManager()->GetTexture(m_strTexture));

	m_VertBuff.resize((MAX_TEXT + 1) * 4);
	m_Index.resize(MAX_TEXT * 3 * 2);

	for (uint i = 0; i <= MAX_TEXT; i++)
	{
		m_VertBuff[i * 4 + 0].pos.Set(i * fWidth, 0.0f);
		m_VertBuff[i * 4 + 1].pos.Set(i * fWidth, 1.0f);

		m_VertBuff[i * 4 + 2].pos.Set((i + 1) * fWidth, 0.0f);
		m_VertBuff[i * 4 + 3].pos.Set((i + 1) * fWidth, 1.0f);
	}

	for (uint i = 0; i < MAX_TEXT; i++)
	{
		m_Index[i * 6 + 0] = i * 4 + 0;
		m_Index[i * 6 + 1] = i * 4 + 1;
		m_Index[i * 6 + 2] = i * 4 + 2;

		m_Index[i * 6 + 3] = i * 4 + 2;
		m_Index[i * 6 + 4] = i * 4 + 1;
		m_Index[i * 6 + 5] = i * 4 + 3;
	}

	m_op.pSystemVertBuff = (float*)&m_VertBuff[0];
	m_op.pSystemIndexBuff = (ushort*)&m_Index[0];
	m_op.pMaterial = m_pMatInst->GetMaterialTemplate();
	m_op.pMtlInst = m_pMatInst.get();
	m_op.pVertDesc = Global::GetResourceManager()->GetVertexDesc(ResourceManager::UI_VEXDESC).Get();
	m_op.nPrimType = RenderOperator::PT_TRIANGLELIST;
	m_op.nBaseVertexIndex = 0;
	m_op.nMinVertexIndex = 0;
	m_op.nNumVertices = (uint)m_VertBuff.size();
	m_op.nStartIndex = 0;
	m_op.nVertexStride = sizeof(UIVertex);
	return true;
}

void TextRenderer::DrawText(const char* pText, uint xPos, uint yPos, uint cw, uint ch, const Color& color)
{
	float fWidth = 1.0f / (float)MAX_TEXT;
	uint nNumChar = (uint)strlen(pText);
	m_op.nPrimitiveCount = 2 * nNumChar;

	Vector2f uvStride(1.0f / (float)m_nCol, 1.0f / (float)m_nRow);
	for (uint i = 0; i < nNumChar; i++)
	{
		int xidx = pText[i] - '0';
		int yidx = 0;

		Vector2f uvBase(uvStride.x * xidx, uvStride.y * yidx);
		m_VertBuff[i * 4 + 0].tex.Set(uvBase.x, uvBase.y);
		m_VertBuff[i * 4 + 1].tex.Set(uvBase.x, uvBase.y + uvStride.y);
		m_VertBuff[i * 4 + 2].tex.Set(uvBase.x + uvStride.x, uvBase.y);
		m_VertBuff[i * 4 + 3].tex.Set(uvBase.x + uvStride.x, uvBase.y + uvStride.y);
	}

	m_op.pMaterial->BeginPass(0);

	uint nWidth, nHeight;
	Global::GetRenderer()->GetFrameBufferSize(nWidth, nHeight);

	uint dw = nNumChar * cw;
	uint dh = 1 * ch;

	float param[4];
	param[0] = (float)dw / (nWidth * nNumChar * fWidth * 0.5f);
	param[1] = -(float)dh / (nHeight * 0.5f);
	param[2] = 2.0f * xPos / (float)nWidth - 1.0f;
	param[3] = -2.0f * yPos / (float)nHeight + 1.0f;
	m_op.pMtlInst->SetValueF("SpriteParam", param, 4);
	m_op.pMtlInst->SetValueF("Color", color.Ptr(), 4);

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

	int								m_nKeyDown;
	int								m_nFillMode;

	
	OrbitMouseController			m_MouseController;


	CameraPtr						m_pCameraObj;
	vector<Attachable*>				m_Cameras;
	uint							m_nCurentCamera;
	
	string							m_strErrorMsg;


	LightObject*					m_pLight;



	RenderableObjectPtr				m_pTempRenderble;
	SceneNodePtr					m_pNode;

	shared_ptr<TextRenderer>		m_pTextRenderer;

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

	m_nKeyDown = 0;
	m_nFillMode = D3DFILL_SOLID;


	return true;
}





bool MyApp::OnResetDevice()
{

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


	
	if (!m_Pipeline.Init())
	{
		return false;
	}
	return true;
}

void MyApp::OnLostDevice()
{
}

void MyApp::OnDestroyDevice()
{
}


void MyApp::OnUpdateScene(float fElapsedTime)
{

	m_Scene.UpdateWorld(fElapsedTime);
	m_Scene.FlushToPipeline(&m_Pipeline);
}


void MyApp::OnRender()
{
	m_Pipeline.BeginPipe();

	m_Pipeline.Render();

	int fps = (int)m_fps;
	char buff[8];
	itoa(fps, buff, 10);

	m_pTextRenderer->DrawText(buff, 0, 0, 15, 20, Color::WHITE);
	m_Pipeline.EndPipe();
	m_Pipeline.Clear();
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
		m_nFillMode = (m_nFillMode == D3DFILL_SOLID) ? D3DFILL_WIREFRAME : D3DFILL_SOLID;
		//m_pRenderer->m_pd3dDevice9->SetRenderState(D3DRS_FILLMODE, m_nFillMode);
	}

	if (event.Type == KeyEvent::KeyDown && event.Code == KeyEvent::KEYCODE_TAB)
	{
		m_nCurentCamera = ++m_nCurentCamera % m_Cameras.size();
		SwitchCamera();
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


