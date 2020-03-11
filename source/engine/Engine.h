#pragma once
#include "stdheader.h"

namespace Aurora
{

	class Engine;
	class Pipeline;
	class IRenderDevice;
	class Timer;
	class ResourceManager;

	Engine*		GetEngine();


	class FrameListener
	{
	public:
		FrameListener()				{}
		virtual ~FrameListener()	{}

		virtual void		OnResizeFrameBuffer(int width, int height)	{}

		virtual void		OnUpdate(float dt)		{}
		virtual void		OnRender()				{}
	};


	struct Window
	{
		int32		width;
		int32		height;
	};

	extern Window GMainWindow;



	class Engine
	{
	public:
		static Engine*		Create();

		~Engine();
		void			Init(int nWidth, int nHeight, FrameListener* pListener);
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

		FrameListener*		m_pFrameListener;
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