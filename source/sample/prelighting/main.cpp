// GaEngine10.cpp : 定义应用程序的入口点。
//
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


void TestRapidjson()
{
	GFileSys->SetRootPath("E:/develop/GaEngine/res", false);
	Aurora::StringBuffer buffer("test.json");

	Document document;
	document.Parse(buffer.Ptr());

	string s = document["hello"].GetString();
	bool b = document["t"].GetBool();


	//Document document;

	//Value s;
	//s = "rapidjson";
	//
	//Value contact(kObjectType);
	//contact.AddMember("name", "Milo", document.GetAllocator());
	//contact.AddMember("married", true, document.GetAllocator());
	//contact.AddMember("value", s, document.GetAllocator());



	//FILE* fp = fopen("e:\\output.json", "wb"); // non-Windows use "w"

	//char writeBuffer[65536];
	//FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	//Writer<FileWriteStream> writer(os);
	//contact.Accept(writer);
	//fclose(fp);

}


Entity* CreateSimpleEntity()
{
	auto model = ModelManager.GetModel("resource:Model/White_GrenadeLauncher.model");	
	auto entity =  EntityFactory.CreateEntity(model);
	
	entity->GetTransfrom().Rotate(Quaternionf(Vector3f::UNIT_X, Mathf::PI));
	
	return entity;

}


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

	void			LoadResource();


	LightObject* m_pLight;
	EditorCameraMover m_CamController;
	CameraControllerUE  cameraControl_;

	Entity* m_pEntity;
	IScene* m_pScene;
	
	Camera camera_;
	RectSize mainView_;
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
		
	GEventDisPatcher->AddKeyEventListener(this);
	GEventDisPatcher->AddMouseEventListener(this);
		
	GEventDisPatcher->AddMouseEventListener(&m_CamController);
	GEventDisPatcher->AddMouseEventListener(&cameraControl_);


	auto entity = CreateSimpleEntity();
	m_pScene->AddEntity(entity);

	return true;
	

	Light* pLight = new Light(LT_DIRECTIONAL);

	//Light* pLight = new Light(LT_SPOT);
	//pLight->fRange = 50.f;
	//pLight->fInnerCone = 15.f;
	//pLight->fOutterCone = 30.f;
	//pLight->cBaseColor = Color::BLUE;

	Quaternionf qRot;
	//QuaternionRotationAxis(qRot, Vector3f::UNIT_X, Mathf::PI / 4.f);
	//pLight->m_Transform.SetLocalRotation(qRot);
	pLight->m_Transform.SetTranslation(Vector3f(0.f, -4.f, 5.f));

	m_pScene->AddLight(pLight);
	 
	return true;
}


void MyApp::OnResizeFrameBuffer(int width, int height)
{
	camera_.SetAspect(width / (float)height);
}


void MyApp::OnUpdate(float dt)
{
	//m_pCamera->SetTransform(m_CamController.GetTransfrom());

	//if (!m_bMultiThread)
	//{
	//	Update();
	//}

	//camera_.Transform_ = m_CamController.Transform_;
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
		Vector3f b1( sideLength, step, 0.f);
		HelperDraw.AddLine(a1, b1, color, 0.f);
	}
}

bool show_demo_window = true;
Color  clearColor;

void MyApp::OnRender()
{
	SceneView* pRenderView = m_pScene->MakeSceneView(&camera_, mainView_);

	if (pRenderView)
	{
		GSimpleRendering.RenderSceneView(pRenderView);
		//GPipeline.RenderSceneView(pRenderView);
	}

	
	//GFrameMemoryBuffer->Clear();

	DrawGrid();


	GuiNewFrame();
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", clearColor.Ptr()); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	GuiRender();

	GRenderDevice->Present();
}

void MyApp::OnDestroyDevice()
{
}


void MyApp::OnKeyEvent(const KeyEvent& event) 
{
	if (event.Code == KeyEvent::KEYCODE_1) {
		int a = 0;
		a++;
	}
}


void MyApp::OnMouseEvent(const MouseEvent& event)
{
}



void MyCommand(const Arguments& args)
{
	MessageBox(NULL, args.Args(1), args.Args(2), MB_OK);
}


int WINAPI _tWinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	//GCommandSystem->AddCommand("MyCommand", MyCommand, "abck");
	//GCommandSystem->ExecuteCommand("MyCommand");
	
	MyApp app;

	if (!app.Create(L"app", 1200, 800))
		return 1;

	app.Run();

	return 0;
}



