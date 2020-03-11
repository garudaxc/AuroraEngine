#include "stdheader.h"
#include "Scene.h"
#include "SceneView.h"
#include "Timer.h"
#include "Camera.h"

namespace Aurora
{


	class UpdateTask : public ITask
	{
	public:
		virtual void Run(void* param);
	};

	void UpdateTask::Run(void* param)
	{
		//MyApp* pApp = (MyApp*)param;
		//while (1)
		//{
		//	pApp->Update();
		//}
	}

	class RenderCmdBuffer
	{
	public:
		void Init();

		SceneView* RenderView;

		ILock* pRenderLock;
		ILock* pUpdateLock;

		Timer timer;
	};


	void RenderCmdBuffer::Init()
	{
		RenderView = NULL;

		pRenderLock = CreateThreadLock();
		pUpdateLock = CreateThreadLock();

		timer.Reset();
		timer.Start();
	}

	IThread* m_pThread;
	ITask* m_pTask;

	class RenderCmdBuffer* m_pCmdBuffer;

	bool m_bMultiThread;

	class SceneLocal : public IScene
	{
	public:
		SceneLocal();
		virtual ~SceneLocal();

		virtual void Release();

		virtual void Load(const string& fileName);
		virtual void Save(const string& fileName);

		virtual void AddEntity(Entity* pEntity);

		virtual void AddLight(Light* pLight);

		virtual int GetNumEntity() const;

		virtual int GetNumLight() const;

		virtual SceneView* MakeSceneView(const Camera* pCamera, const RectSize& viewSize);

	private:
		vector<Entity*> m_Entitys;
		vector<Light*> m_Lights;
	};

	SceneLocal::SceneLocal()
	{
		m_pCmdBuffer = new RenderCmdBuffer();
		m_pCmdBuffer->Init();


		if (m_bMultiThread)
		{
			m_pTask = new UpdateTask();
			m_pThread = CreateThread(NULL, NULL, false);
			m_pThread->AddTask(m_pTask, this);
		}
	}

	SceneLocal::~SceneLocal()
	{

	}

	void SceneLocal::Release()
	{
		delete this;
	}

	void SceneLocal::Load(const string& fileName)
	{

	}

	void SceneLocal::Save(const string& fileName)
	{

	}

	void SceneLocal::AddEntity(Entity* pEntity)
	{
		m_Entitys.push_back(pEntity);
	}

	void SceneLocal::AddLight(Light* pLight)
	{
		m_Lights.push_back(pLight);
	}

	int SceneLocal::GetNumEntity() const
	{
		return m_Entitys.size();
	}

	int SceneLocal::GetNumLight() const
	{
		return m_Lights.size();
	}

	SceneView* SceneLocal::MakeSceneView(const Camera* pCamera, const RectSize& viewSize)
	{

		float dt = m_pCmdBuffer->timer.GetElapsedTime();

		//m_pCmdBuffer->pUpdateLock->Lock();


		SceneView* pUpdateView = GFrameMemoryBuffer->AllocSceneView();
		pUpdateView->pDrawingEntity = NULL;
		pUpdateView->pDrawingLight = NULL;

		for (vector<Entity*>::iterator it = m_Entitys.begin(); it != m_Entitys.end(); ++it)
		{
			Entity* pEntity = (*it);
			pEntity->Update();

			DrawingEntity* pDrawEntity = GFrameMemoryBuffer->AllocDrawingEntity();

			pDrawEntity->pEntity = pEntity;
			pDrawEntity->mWorld = pEntity->GetTransfrom().GetMatrix();
			pDrawEntity->pNext = pUpdateView->pDrawingEntity;
			pUpdateView->pDrawingEntity = pDrawEntity;
		}

		for (vector<Light*>::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it)
		{
			Light* pLight = (*it);
			
			DrawingLight* pDrawingLight = GFrameMemoryBuffer->AllocDrawingLight();
			memset(pDrawingLight, 0, sizeof(DrawingLight));

			pDrawingLight->mWorld = pLight->m_Transform.GetMatrix();

			pDrawingLight->Type		= pLight->Type;
			pDrawingLight->cColor	= pLight->cBaseColor * pLight->fMultiplier;
			pDrawingLight->cShadowColor	= pLight->cShadowColor;
			pDrawingLight->bCastShadow	= pLight->bCastShadow;

			if (pLight->Type == LT_POINT || pLight->Type == LT_SPOT)
			{
				pDrawingLight->fRange	= pLight->fRange;
			}

			if (pLight->Type == LT_SPOT)
			{
				pDrawingLight->fInnerCone	= pLight->fInnerCone;
				pDrawingLight->fOutterCone	= pLight->fOutterCone;
			}

			pDrawingLight->pNext = pUpdateView->pDrawingLight;
			pUpdateView->pDrawingLight = pDrawingLight;
		}
			   
		pUpdateView->matView = pCamera->Transform_.Inverse().GetMatrix();

		pUpdateView->matProj = Matrix4f::PerspectiveFovRH(
			pCamera->m_fFov, viewSize.Width / (float)viewSize.Height,
			pCamera->GetNearPlane(), pCamera->GetFarPlane());

		pUpdateView->matViewProj = pUpdateView->matView * pUpdateView->matProj;

	
		pUpdateView->fNearPlane = pCamera->GetNearPlane();
		pUpdateView->fFarPlane = pCamera->GetFarPlane();
		//m_pCmdBuffer->pUpdateLock->Unlock();

		//m_pCmdBuffer->pRenderLock->Lock();

		//m_pCmdBuffer->RenderView = pUpdateView;
		GFrameMemoryBuffer->ClearAndSwap();

		//m_pCmdBuffer->pRenderLock->Unlock();

		//m_pCmdBuffer->pRenderLock->Lock();

		////Pipeline::Get()->RenderSceneView(m_pSceneView);

		//if (m_pCmdBuffer->RenderView)
		//{
		//	Pipeline::Get()->RenderSceneView(m_pCmdBuffer->RenderView);
		//}

		//m_pCmdBuffer->pRenderLock->Unlock();

		//m_pCmdBuffer->pUpdateLock->Lock();
		//m_pCmdBuffer->pUpdateLock->Unlock();

		//GLog->Append("render thread");

		return pUpdateView;
	}

	IScene* CreateScene()
	{
		return new SceneLocal();
	}

}