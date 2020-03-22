#include "stdheader.h"
#include <tchar.h>
#include <shellapi.h>
#include <time.h>
#include <sstream>
#include "Application.h"
#include "Camera.h"
#include "Renderer.h"
#include "Util.h"
#include "DeviceWindow.h"
#include "Controller.h"
#include "Model.h"
#include "SceneView.h"
#include "Entity.h"
#include "Platform.h"
#include "CmdSystem.h"
#include "Scene.h"
#include "FileSystem.h"
#include "SimpleRendering.h"
#include "HelperDraw.h"
#include "Gui.h"



using namespace Aurora;


class MyApp : public Application, public KeyEventListener, public MouseEventListener
{
public:
	virtual bool		OnInitApp();
	virtual bool		OnCreateDevice(const RectSize& mainView);
	virtual void		OnDestroyDevice();

	virtual void		OnResizeFrameBuffer(int width, int height);
	virtual void		OnUpdate(float dt);
	virtual void		OnRender();

	virtual void	OnKeyEvent(const KeyEvent& event);
	virtual void	OnMouseEvent(const MouseEvent& event);


	EditorCameraMover m_CamController;
	CameraControllerUE  cameraControl_;
	Camera camera_;
	RectSize mainView_;
	IScene* m_pScene;
};


bool MyApp::OnInitApp()
{
	return true;
}


bool MyApp::OnCreateDevice(const RectSize& mainView)
{
	cameraControl_.Transform_.LootAt(Vector3f(0.f, -2.0f, 1.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f::UNIT_Z);
	camera_.SetFov(Mathf::PI / 4.0f);
	mainView_ = mainView;
	m_pScene = CreateScene();
	GEventDisPatcher->AddMouseEventListener(&m_CamController);
	GEventDisPatcher->AddMouseEventListener(&cameraControl_);

	return true;
}


void MyApp::OnResizeFrameBuffer(int width, int height)
{
	camera_.SetAspect(width / (float)height);
}


void MyApp::OnUpdate(float dt)
{
	camera_.Transform_ = cameraControl_.Transform_;
}

void DrawGrid()
{
	float sideLength = 1.f;
	int32 gridCount = 10;
	Color color = Color::GRAY;

	for (int i = 0; i < gridCount + 1; i++) {
		float step = -sideLength + (2 * sideLength * i) / gridCount;
		Vector3f a0(step, -sideLength, 0.f);
		Vector3f b0(step, sideLength, 0.f);

		HelperDraw.AddLine(a0, b0, color, 0.f);

		Vector3f a1(-sideLength, step, 0.f);
		Vector3f b1(sideLength, step, 0.f);
		HelperDraw.AddLine(a1, b1, color, 0.f);
	}
}

bool show_demo_window = true;
Color  clearColor;

void MyApp::OnRender()
{
	SceneView* pRenderView = m_pScene->MakeSceneView(&camera_, mainView_);
	GSimpleRendering.RenderSceneView(pRenderView);
	DrawGrid();
	GRenderDevice->Present();
}

void MyApp::OnDestroyDevice()
{
}


void MyApp::OnKeyEvent(const KeyEvent& event)
{
}


void MyApp::OnMouseEvent(const MouseEvent& event)
{
}



int WINAPI _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	MyApp app;

	if (!app.Create(L"app", 1200, 800))
		return 1;

	app.Run();

	return 0;
}



