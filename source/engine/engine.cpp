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
	const char* ENGINE_NAME = "Aurora Engine";
	const char* ENGINE_VERSION = "0.1";

	static Engine* g_pEngine;
	
	IRenderDevice* GRenderDevice = nullptr;

	Engine* GetEngine()
	{
		return g_pEngine;
	}

	Engine* Engine::Create()
	{
		g_pEngine = new Engine();
		return g_pEngine;
	}


	Engine::Engine():m_fps(60.f)
	{
		m_pTimer = nullptr;
		m_pRenderer = nullptr;
		m_pResourceManager = nullptr;
	}

	Engine::~Engine()
	{

	}

	bool CreateDX11Device(CScreen* InScreen);
    bool CreateOpenGLDevice(CScreen* InScreen);
	
	IRenderDevice* IRenderDevice::CreateDevice(CScreen* InScreen)
	{		
		if (!CreateDX11Device(InScreen)) {
			return nullptr;
		}
			
		// if (!CreateOpenGLDevice(InScreen)) {
		// 	return nullptr;
		// }
		
		return GRenderDevice;
	}

	bool IRenderDevice::Initialized()
	{
		return  GRenderDevice != nullptr;
	}


	void Engine::Init(CScreen* InScreen)
	{
		Config::Initialize();

		GLog->Initialize();
		
		InitPlatform();

		m_pTimer = new Timer();		
		
		m_pRenderer = IRenderDevice::CreateDevice(InScreen);

		m_pResourceManager = new ResourceManager();
		m_pResourceManager->Init();

		GTextureManager.Initialize();

		GMeshManager.Initialize();

		GMaterialTemplateManager.Initialize();

		GMaterialInstanceManager.Initialize();

		GShaderManager.Initialize();

		ModelManager.Initialize();

		GPipeline.Initialize(100, 100);
		GSimpleRendering.Initialize();

		EntityFactory.Initialize();

		m_pTimer->Reset();

		GuiInit(InScreen);
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
		//m_pRenderer = nullptr;

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

		GuiRender();

		//GetScene()->UpdateWorld(timeElapsed);
		//GetScene()->FlushToPipeline(m_pPipeline.get());
		//GetPipeline()->BeginPipe();
		//GetPipeline()->Render();
		//GetPipeline()->EndPipe();

		
		GRenderDevice->Present();
	}

	void Engine::ResizeFrameBuffer(int nWidth, int nHeight)
	{
		LostDevice();
		ResetDevice();
	}


	int Engine::IssuePickupTest(int x, int y, int windowSize)
	{
		GPipeline.BeginOcclusionQuery(x-windowSize, y-windowSize, x+windowSize, y+windowSize);
		//GetScene()->FlushToPipeline(m_pPipeline.get());
		GPipeline.BeginPipe();
		GPipeline.Render();
		GPipeline.EndPipe();
		return GPipeline.EndOcclusionQuery();
	}



}