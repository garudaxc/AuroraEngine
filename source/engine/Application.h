#ifndef GAAPPLICATION_H
#define GAAPPLICATION_H

#include "stdheader.h"
#include "Timer.h"
#include "Pipeline.h"
#include "Engine.h"

namespace Aurora
{

class Application : public FrameListener
{
public:
					Application();
	virtual			~Application();

	bool			Create(const wchar_t* appName = L"", int nWidth = 800, int nHeight = 600);
	void			Run();


	virtual bool		OnInitApp()							= 0;
	virtual bool		OnCreateDevice(const RectSize& mainView)			= 0;		// call after device created
	virtual void		OnDestroyDevice()					= 0;		// call before device destroy

	virtual void		OnResizeFrameBuffer(int width, int height)	{}
	virtual void		OnUpdate(float dt)							{}
	virtual void		OnRender()								{}
	   
	void			StartTiming();
	float			EndTiming(char* text, bool bTextout = true);


protected:
	static Application*	s_App;

	Engine*		m_pEngine;

	int			m_nWindowWidth;
	int			m_nWindowHeight;

	long		m_lTimeStart;
	long		m_lTimeEnd;

private:
	// message call back
	static LRESULT CALLBACK			WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT							MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool		InitEnvironment();
	void		CleanEnvironment();

};


}






#endif
