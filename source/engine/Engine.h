#pragma once
#include "stdheader.h"

namespace Aurora
{

	class Engine;
	class Pipeline;
	class IRenderDevice;
	class Timer;
	class ResourceManager;
	class CScreen;

	Engine*		GetEngine();


	class Engine
	{
	public:
		static Engine*		Create();

		~Engine();
		void			Init(CScreen* InScreen);
		void			Destroy();

		IRenderDevice*		GetRenderer();
		Timer*				GetTimer();
		ResourceManager*		GetResourceManager();

		void		ResetDevice();
		void		LostDevice();

		void		RenderOneFrame();
		void		ResizeFrameBuffer(int width, int height);
		int			IssuePickupTest(int x, int y, int windowSize);

		float		GetFPS() const;

	private:
		Engine();

		IRenderDevice*			m_pRenderer;
		Timer*			m_pTimer;
		ResourceManager*	m_pResourceManager;

		float				m_fps;

	};

	inline IRenderDevice* Engine::GetRenderer()
	{
		return m_pRenderer;
	}

	inline Timer* Engine::GetTimer()
	{
		return m_pTimer;
	}

	inline ResourceManager* Engine::GetResourceManager()
	{
		return m_pResourceManager;
	}

	inline float Engine::GetFPS() const
	{
		return m_fps;
	}



}