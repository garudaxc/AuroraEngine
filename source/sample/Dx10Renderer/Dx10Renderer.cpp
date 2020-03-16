// GaEngine10.cpp : 定义应用程序的入口点。
//
//#include "stdafx.h"
#include "stdheader.h"
#include <tchar.h>
#include <shellapi.h>
#include <time.h>
#include <sstream>
#include "GaApplication.h"
#include "GaPrimitive.h"
#include "GaCamera.h"
#include "GaRenderer.h"
#include "GaUtil.h"
#include "GaDeviceWindow.h"
#include "GaController.h"
#include "GaModel.h"
#include "GaSceneView.h"
#include "GaEntity.h"
#include "GaMaterialInstance.h"
#include "ColladaDocument.h"
#include "MeshBuilder.h"
#include "AABB.h"
#include "RenderPipe.h"
#include "GaHelperRenderer.h"
#include "CubicNTree.h"
#include "GaRendererDX10.h"
#include "GaPlatform.h"


using namespace Ga;
using namespace std;


class OrbitMouseTransformStrategy
{
public:
	OrbitMouseTransformStrategy()	{}
	virtual ~OrbitMouseTransformStrategy()	{}

	OrbitMouseController*	GetController()		{	return &m_Controller;	}

	virtual void			Update();

	Transform&	GetTransfrom()	{	return m_Transform;	}

private:
	OrbitMouseController	m_Controller;

	Transform m_Transform;

};

void OrbitMouseTransformStrategy::Update()
{
	m_Transform.SetLocalTranslation( m_Controller.GetPosition());
	m_Transform.SetLocalRotation(m_Controller.GetRotation());

	m_Transform.Update();
}




class EditorMouseTransformStrategy
{
public:
	EditorMouseTransformStrategy()	{}
	virtual ~EditorMouseTransformStrategy()	{}

	EditorCameraMover*	GetController()		{	return &m_Controller;	}

	virtual void			Update();

	Transform&	GetTransfrom()	{	return m_Transform;	}

private:
	EditorCameraMover	m_Controller;

	Transform m_Transform;
};

void EditorMouseTransformStrategy::Update()
{
	m_Transform.SetLocalTranslation( m_Controller.GetPosition());
	m_Transform.SetLocalRotation(m_Controller.GetRotation());

	m_Transform.Update();
}




Entity* CreateEntityFromMeshData(const string& name, MeshBuildData* pData, MaterialInstance* pMtlInst)
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

	pModel->SetMaterial(0, pMtlInst);

	Entity* pEntity = EntityFactory.CreateEntity(pModel);
	return pEntity;
}



class MyApp : public Application, public KeyEventListener, public MouseEventListener
{
public:
	virtual bool		OnInitApp();
	virtual bool		OnCreateDevice();
	virtual void		OnDestroyDevice();

	virtual void		OnResizeFrameBuffer(int width, int height);
	virtual void		OnUpdate(float dt);
	virtual void		OnRender();

	virtual void	OnKeyEvent(const KeyEvent& event);
	virtual void	OnMouseEvent(const MouseEvent& event);

	void			LoadResource();

	LightObject* m_pLight;

	SceneView* m_pSceneView;

	EditorMouseTransformStrategy* m_pCameraTransfrom;

	RenderPipe* m_pRenderPipe;

	HelperRenderer* m_pHelperRenderer;

	Vector3f triangle[3];

	CubicNTree m_tree;

	Texture3D* m_pTexture;

	Texture* m_pScreenTex;
	MaterialInstance* m_pScreenQuad;
};


bool MyApp::OnInitApp()
{
	return true;
}


void DrawCubicNTree(HelperRenderer* pRenderer, CubicNTree* pTree)
{
	Vector3f scale = pTree->GetScale();
	Vector3f translate = pTree->GetTranslate();

	int numNode = pTree->GetNumNode();
	for (int i = 0; i < numNode; i++)
	{
		const CubicNTree::Node* pNode = pTree->GetNode(i);

		if (i == 200)
		{
			int a = 0;
			a++;
		}

		for (int j = 0; j < 8; j++)
		{
			if (pNode->nChild[j] == CubicNTree::NODE_LEAF)
			{
				AABB aabb = pNode->GetSubNodeAABB(j);
				Vector3f vMin = Modulate(aabb.vMin, scale) + translate;
				Vector3f vMax = Modulate(aabb.vMax, scale) + translate;
				pRenderer->DrawAABB(vMin, vMax, Color::RED, 0, NULL);

			}
		}
	}
}

//static uint FormatTexelValue(ubyte r, ubyte g, ubyte b, ubyte a)
//{
//	return a << 24 | b << 16 | g << 8 | r;
//}

ubyte PercentValue(uint x, int range)
{
	float p = (float)x / (float)range;
	return (ubyte)(p * 255);
}

void FillTexture(Texture* pTexture)
{
	Texture::Desc desc = pTexture->GetDesc();

	vector<uint> buffer(desc.nWidth * desc.nHeight * desc.nDepth);

	int i = 0;
	for (uint w = 0; w < desc.nDepth; w++)
	{
		for (uint v = 0; v < desc.nHeight; v++)
		{
			for (uint u = 0; u < desc.nWidth; u++)
			{
				ubyte r = PercentValue(u, desc.nWidth);
				ubyte g = PercentValue(v, desc.nHeight);
				ubyte b = PercentValue(w, desc.nDepth);
				buffer[i] = FormatTexelValue(r, g, b, 255);
				i++;
			}
		}
	}

	pTexture->Update(0, &buffer[0]);
}



Texture3D* CreateTestTexture()
{
	Texture::Desc desc;
	desc.nWidth = 64;
	desc.nHeight = 64;
	desc.nDepth = 64;
	desc.nFormat = Surface::FMT_R8G8B8A8;
	desc.nMipLevels = 1;
	desc.nUsage = Surface::USAGE_DEFAULT;
	desc.bDynamic = 0;
	desc.bMultiSample = 0;

	Texture3D* pTexture = (Texture3D*)GetRendererDx10()->CreateTexture(Texture::TP_3D, desc);
	assert(pTexture);
	FillTexture(pTexture);

	return pTexture;
}

Texture2D* CreateTexture2D(int nWidth, int nHeight)
{
	Texture::Desc desc;
	desc.nWidth = nWidth;
	desc.nHeight = nHeight;
	desc.nDepth = 1;
	desc.nFormat = Surface::FMT_R8G8B8A8;
	desc.nMipLevels = 1;
	desc.nUsage = Surface::USAGE_DEFAULT;
	desc.bDynamic = 0;
	desc.bMultiSample = 0;

	Texture2D* pTexture = (Texture2D*)GetRendererDx10()->CreateTexture(Texture::TP_2D, desc);
	assert(pTexture);

	FillTexture(pTexture);
	return pTexture;
}

Texture3D* CreateCubicNTexture(CubicNTree* pTree, Vector3f& invS)
{
	int numNode = pTree->GetNumNode();
	float s = Mathf::Pow((float)numNode, 0.3333f);
	int size = (int)(s + 1.0f);

	Texture::Desc desc;
	desc.nWidth = size * 2;
	desc.nHeight = size * 2;
	desc.nDepth = size * 2;
	desc.nFormat = Surface::FMT_R8G8B8A8;
	desc.nMipLevels = 1;
	desc.nUsage = Surface::USAGE_DEFAULT;
	desc.bDynamic = 0;
	desc.bMultiSample = 0;

	Texture3D* pTexture = (Texture3D*)GetRendererDx10()->CreateTexture(Texture::TP_3D, desc);

	vector<uint> buffer(desc.nWidth * desc.nHeight * desc.nDepth);
	VolumeTextureInfo vtInfo;
	vtInfo.nWidth = size;
	vtInfo.nHeight = size;
	vtInfo.nDepth = size;
	vtInfo.nPitch = vtInfo.nWidth * 2;
	vtInfo.nDepthPitch = vtInfo.nPitch * vtInfo.nHeight * 2;

	vtInfo.pData = &buffer[0];
	pTree->BuildVolumeTexture(&vtInfo);
	pTexture->Update(0, &buffer[0]);

	invS.Set(1.f / (float)size, 1.f / (float)size, 1.f / (float)size);


	return pTexture;
}

Texture3D* CreateBrickTexture(CubicNTree* pTree, Vector3f& invSize)
{

	BrickPool* pBrickPool = new BrickPool();
	pTree->BuildBrickPool(pBrickPool);

	uint nBrickSize = pBrickPool->GetBrickSize();

	Texture::Desc desc;
	desc.nWidth = pBrickPool->GetWidth() * nBrickSize;
	desc.nHeight = pBrickPool->GetHeight() * nBrickSize;
	desc.nDepth = pBrickPool->GetDepth() * nBrickSize;
	desc.nFormat = Surface::FMT_A32B32G32R32F;
	desc.nMipLevels = 1;
	desc.nUsage = Surface::USAGE_DEFAULT;
	desc.bDynamic = 0;
	desc.bMultiSample = 0;

	Texture3D* pTexture = (Texture3D*)GetRendererDx10()->CreateTexture(Texture::TP_3D, desc);


	// ... 

	return pTexture;
}


bool MyApp::OnCreateDevice()
{

	triangle[0] = Vector3f(-7.f, -1.f, 0.f);
	triangle[1] = Vector3f(-2.f, 0.f, 0.f);
	triangle[2] = Vector3f(-4.f, 2.f, 4.f);


	m_pRenderPipe = new RenderPipe();
	m_pRenderPipe->Init();

	m_pHelperRenderer = new HelperRenderer();
	m_pHelperRenderer->Init();


	EventDispatcher::Get()->AddKeyEventListener(this);
	EventDispatcher::Get()->AddMouseEventListener(this);

	m_pSceneView = new SceneView();
	m_pSceneView->Initialize();

#if 0
	OrbitMouseTransformStrategy* m_pCameraTransfrom = new OrbitMouseTransformStrategy();
	OrbitMouseController* pCameraController = m_pCameraTransfrom->GetController();
	pCameraController->Reset(Vector3f(0.0f, 0.0f, 80.0f), Quaternionf::IDENTITY);
#else
	m_pCameraTransfrom = new EditorMouseTransformStrategy();
	EditorCameraMover* pCameraController = m_pCameraTransfrom->GetController();

#endif
	EventDispatcher::Get()->AddMouseEventListener(pCameraController);

	m_pScreenTex = CreateTexture2D(this->m_nWindowWidth, this->m_nWindowHeight);
	m_pScreenQuad = MaterialInstanceManager::Get()->GetMaterialInstance("asset:ScreenQuad.mtlInst");
	m_pScreenQuad->SetVariableValue("tTexture", m_pScreenTex);

	//TiXmlDocument doc;
	//bool bOK = doc.LoadFile("D:/develop/GaEngine/res/world/TestWorld.world");
	//assert(bOK);

	//TiXmlElement* pXmlEntity = doc.FirstChildElement("World")->FirstChildElement("Entities")->FirstChildElement("Entity");

	//while(pXmlEntity)
	//{
	//	Entity* pEntity = EntityFactory::Get()->CreateEntity(pXmlEntity);
	//	m_pSceneView->AddEntity(pEntity);

	//	pXmlEntity = pXmlEntity->NextSiblingElement("Entity");
	//}

	//TestTexture3D(m_pTexture);


	ColladaDocument colladaDoc;
	bool bOk = colladaDoc.Open("D:/develop/GaEngine/res/aaa.DAE");
	assert(bOk);
	int numGeometry = colladaDoc.GetNumGeometry();
	MeshBuildData* pData = colladaDoc.Test();

	if (IsFileExist("TreeData.dat"))
	{
		m_tree.ReadFromFile("TreeData.dat");
	}
	else
	{
		m_tree.Build(pData);
		m_tree.SaveToFile("TreeData.dat");
	}

	
	//m_pTexture = CreateTestTexture();

	Vector3f invS;
	m_pTexture = CreateCubicNTexture(&m_tree, invS);
	
	Vector3f offset = -m_tree.GetTranslate();
	Vector3f scale = 1.f / m_tree.GetScale();


	MaterialInstance* pMtlInst = MaterialInstanceManager::Get()->GetMaterialInstance("resource:Model/GeoSphere02.mtlInst");
	assert(pMtlInst);

	pMtlInst->SetVariableValue("tVoxelMap", m_pTexture);

	pMtlInst->SetVariableValue("vScale", scale.Ptr());
	pMtlInst->SetVariableValue("vOffset", offset.Ptr());
	pMtlInst->SetVariableValue("vInvS", invS.Ptr());

	Entity* pEntity = CreateEntityFromMeshData("Collada Test", pData, pMtlInst);
	m_pSceneView->AddEntity(pEntity);
	pData->Release();

	return true;
}



void MyApp::OnResizeFrameBuffer(int width, int height)
{
	Entity* pCameraEntity = m_pSceneView->GetCamera();
	Camera* pCamera = EntityFactory.GetCameraObject(pCameraEntity);
	assert(pCamera);

	pCamera->SetAspect(width / (float)height);
}

void TraceVoxel(Texture* pTexture, CubicNTree* pTree, Camera* pCamera)
{
	Texture::Desc desc = pTexture->GetDesc();
	vector<uint> buffer(desc.nWidth * desc.nHeight);

	int i = 0;
	for (uint v = 0; v < desc.nHeight; v++)
	{
		for (uint u = 0; u < desc.nWidth; u++)
		{
			Ray ray = pCamera->GetRay(u, v, desc.nWidth, desc.nHeight);
			bool bIntersect = pTree->TraceVoxel(ray);

			if (bIntersect)
			{
				buffer[i] = FormatTexelValue(255, 0, 0, 255);
			}
			else
			{
				buffer[i] = FormatTexelValue(255, 255, 255, 255);
			}

			i++;
		}
	}

	pTexture->Update(0, &buffer[0]);
}

void MyApp::OnUpdate(float dt)
{
	Entity* pCameraEntity = m_pSceneView->GetCamera();
	m_pCameraTransfrom->Update();
	pCameraEntity->SetTransform(m_pCameraTransfrom->GetTransfrom());

	Camera* pCamera = EntityFactory.GetCameraObject(pCameraEntity);
	//static bool b = false;
	//if (!b)
	//{
	//	TraceVoxel(m_pScreenTex, &m_tree, pCamera);
	//	b = true;
	//}
}


void MyApp::OnRender()
{
	m_pRenderPipe->BeginPipe();

	GetEngine()->GetRenderer()->Clear(Renderer::CLEAR_FRAME_BUFFER | Renderer::CLEAR_DEPTH_BUFFER, Color(0.2f, 0.2f, 0.2f, 0.2f));

	m_pRenderPipe->RenderSceneView(m_pSceneView);

	DrawCubicNTree(m_pHelperRenderer, &m_tree);

	//m_pHelperRenderer->DrawScreenQuad(m_pScreenQuad);

	//AABB aabb(Vector3f(-3.f, -4.f, -2.f), Vector3f(3.f, 6.f, 5.f));
	//m_pHelperRenderer->DrawAABB(aabb.vMin, aabb.vMax, Color::BLUE, 0, NULL);

	//Color c = Color::BLUE;
	//if (aabb.TestTriangle(triangle[0], triangle[1], triangle[2]))
	//{
	//	c = Color::RED;
	//}
	//m_pHelperRenderer->DrawLine(triangle[0], triangle[1], c, 0, NULL);
	//m_pHelperRenderer->DrawLine(triangle[1], triangle[2], c, 0, NULL);
	//m_pHelperRenderer->DrawLine(triangle[2], triangle[0], c, 0, NULL);

	m_pRenderPipe->EndPipe();
}

void MyApp::OnDestroyDevice()
{
}


void MyApp::OnKeyEvent(const KeyEvent& event) 
{

	if (event.Code == KeyEvent::KEYCODE_UP)
	{
		for (int i = 0; i < 3; i++)
		{
			triangle[i].z += 1.0f;
		}
	}
	else if (event.Code == KeyEvent::KEYCODE_DOWN)
	{
		for (int i = 0; i < 3; i++)
		{
			triangle[i].z -= 1.0f;
		}
	}
	else if (event.Code == KeyEvent::KEYCODE_LEFT)
	{
		for (int i = 0; i < 3; i++)
		{
			triangle[i].x -= 1.0f;
		}
	}
	else if (event.Code == KeyEvent::KEYCODE_RIGHT)
	{
		for (int i = 0; i < 3; i++)
		{
			triangle[i].x += 1.0f;
		}
	}
}


void MyApp::OnMouseEvent(const MouseEvent& event)
{
}



int WINAPI _tWinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	//ColladaDocument doc;
	//bool bOk = doc.Open("D:/develop/GaEngine/res/aaa.DAE");

	//assert(bOk);

	//int numGeometry = doc.GetNumGeometry();

	//doc.Test();


	MyApp app;

	if (!app.Create(L"app", 800, 600))
		return 1;

	app.Run();

	return 0;

}






