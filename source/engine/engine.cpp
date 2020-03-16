#include "stdheader.h"
#include "Engine.h"
#include "Pipeline.h"
#include "Renderer.h"
#include "Log.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "Input.h"
#include "Engine.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Model.h"
#include "Texture.h"
#include "Config.h"
#include "Entity.h"
#include "Mesh.h"
#include "SimpleRendering.h"
#include "Gui.h"

namespace Aurora
{

	static Engine* g_pEngine;

	Window	GMainWindow;

	Engine* GetEngine()
	{
		return g_pEngine;
	}

	Engine* Engine::Create()
	{
		g_pEngine = new Engine();
		return g_pEngine;
	}


	Engine::Engine():m_pFrameListener(NULL),m_fps(0.f)
	{
		m_pTimer = NULL;
		m_pRenderer = NULL;
		m_pResourceManager = NULL;
	}

	Engine::~Engine()
	{

	}

	void Engine::Init(int nWidth, int nHeight, FrameListener* pListener)
	{
		Config::Initialize();

		GLog->Initialize();
		
		InitPlatform();

		m_pTimer = new Timer();

		m_pRenderer = IRenderDevice::CreateDevice(nWidth, nHeight);

		m_pResourceManager = new ResourceManager();
		m_pResourceManager->Init();

		GTextureManager.Initialize();

		GMeshManager.Initialize();

		GMaterialTemplateManager.Initialize();

		GMaterialInstanceManager.Initialize();

		GShaderManager.Initialize();

		ModelManager.Initialize();

		GPipeline.Initialize(nWidth, nHeight);
		GSimpleRendering.Initialize();

		EntityFactory.Initialize();

		m_pFrameListener = pListener;

		m_pTimer->Reset();

		GuiInit();
	}


	void Engine::Destroy()
	{
		GuiCleanup();

		EntityFactory.Finalize();

		GPipeline.Finalize();

		m_pResourceManager->CleanUp();
		delete m_pResourceManager;

		ModelManager.Finalize();

		GMaterialInstanceManager.Finalize();

		GMaterialTemplateManager.Finalize();

		GMeshManager.Finalize();

		GTextureManager.Finalize();

		//delete m_pRenderer;
		//m_pRenderer = NULL;

		delete m_pTimer;
		
		GLog->Finalize();

		Config::Finalize();
	}

	// create all device object
	void Engine::ResetDevice()
	{
		GPipeline.OnReset();

		GetResourceManager()->OnResetDevice();

		uint width, height;
		GetRenderer()->GetFrameBufferSize(width, height);
		m_pFrameListener->OnResizeFrameBuffer(width, height);
	}

	void Engine::LostDevice()
	{
		GetResourceManager()->OnLostDevice();

		GPipeline.OnLost();
	}

	void Engine::RenderOneFrame()
	{
		static float fTimes = 0.0f;
		static uint frames = 0;

		float timeElapsed = GetTimer()->GetElapsedTime();
		fTimes += timeElapsed;
		frames++;
		if (fTimes > 1.0f)
		{
			m_fps = (float)frames / fTimes;
			frames = 0;
			fTimes = 0.0f;
		}

		m_pFrameListener->OnUpdate(timeElapsed);

		//GetScene()->UpdateWorld(timeElapsed);
		//GetScene()->FlushToPipeline(m_pPipeline.get());
		//GetPipeline()->BeginPipe();
		//GetPipeline()->Render();
		m_pFrameListener->OnRender();
		//GetPipeline()->EndPipe();
	}

	void Engine::ResizeFrameBuffer(int nWidth, int nHeight)
	{
		LostDevice();

		GetRenderer()->Reset(nWidth, nHeight);

		ResetDevice();
	}


	int Engine::IssuePickupTest(int x, int y, int windowSize)
	{
		GPipeline.BeginOcclusionQuery(x-windowSize, y-windowSize, x+windowSize, y+windowSize);
		//GetScene()->FlushToPipeline(m_pPipeline.get());
		GPipeline.BeginPipe();
		GPipeline.Render();
		m_pFrameListener->OnRender();
		GPipeline.EndPipe();
		return GPipeline.EndOcclusionQuery();
	}



}