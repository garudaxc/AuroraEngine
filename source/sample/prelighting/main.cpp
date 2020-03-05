// GaEngine10.cpp : 定义应用程序的入口点。
//
#include "stdheader.h"
#include <tchar.h>
#include <shellapi.h>
#include <time.h>
#include <sstream>
#include "Application.h"
#include "Primitive.h"
#include "Camera.h"
#include "Renderer.h"
#include "Util.h"
#include "DeviceWindow.h"
#include "Controller.h"
#include "Model.h"
#include "SceneView.h"
#include "Entity.h"
#include "Platform.h"
#include "MaterialInstance.h"
#include "CmdSystem.h"
#include "Scene.h"
#include "FileSystem.h"
#include "Shader.h"
#include "SimpleRendering.h"
//#include "ColladaDocument.h"
//#include "MeshBuilder.h"


using namespace Aurora;
using namespace std;

//#pragma



Entity* CreateEntityFromMeshData(MeshBuildData* pData, const string& name)
{
	Model* pModel = new Model(name);

	Geometry* pGeometry = NULL;
	pGeometry = new Geometry();
	bool bOk = pGeometry->CreateFromBuildData(pData);
	if (!bOk)
	{
		return NULL;
	}
	pModel->SetMesh(pGeometry);
	pData->Release();

	MaterialInstance* pMtlInst = GMaterialInstanceManager.GetMaterialInstance("Model/GeoSphere01.mtlInst", set<string>());
	assert(pMtlInst);
	pModel->SetMaterial(0, pMtlInst);


	Entity* pEntity = EntityFactory::Get()->CreateEntity(pModel);
	return pEntity;
}



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

	Entity* m_pEntity;

	IScene* m_pScene;
	
	Camera camera_ = { Vector3f(0.f, -2.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f::UNIT_Z };
	RectSize mainView_;
};




bool MyApp::OnInitApp()
{
	return true;
}



bool MyApp::OnCreateDevice(const RectSize& mainView)
{
	camera_.SetFov(Mathf::PI / 4.0f);
	mainView_ = mainView;
	
	m_pScene = CreateScene();
		
	GEventDisPatcher->AddKeyEventListener(this);
	GEventDisPatcher->AddMouseEventListener(this);

	
	EditorCameraMover* pCameraController = &m_CamController;
	GEventDisPatcher->AddMouseEventListener(pCameraController);



	TiXmlDocument doc;
	bool bOK = doc.LoadFile("../res/world/TestWorld.world");
	assert(bOK);

	TiXmlElement* pXmlEntity = doc.FirstChildElement("World")->FirstChildElement("Entities")->FirstChildElement("Entity");
	while(pXmlEntity) {
		Entity* pEntity = EntityFactory::Get()->CreateEntity(pXmlEntity);
		m_pEntity = pEntity;
		m_pScene->AddEntity(pEntity);
		pXmlEntity = pXmlEntity->NextSiblingElement("Entity");
	}

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
	pLight->m_Transform.SetLocalTranslation(Vector3f(0.f, -4.f, 5.f));

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
	return;

}


void MyApp::OnRender()
{
	SceneView* pRenderView = m_pScene->MakeSceneView(&camera_, mainView_);

	if (pRenderView)
	{
		GSimpleRendering.RenderSceneView(pRenderView);
		//GPipeline.RenderSceneView(pRenderView);
	}
	
	//GFrameMemoryBuffer->Clear();
}

void MyApp::OnDestroyDevice()
{
}


void MyApp::OnKeyEvent(const KeyEvent& event) 
{

	if (event.Code == KeyEvent::KEYCODE_1)
	{
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



