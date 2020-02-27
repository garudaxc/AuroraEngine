#ifndef GAAPPLICATION_H
#define GAAPPLICATION_H

#include <windows.h>
#include "Timer.h"


class Application
{
public:
					Application();
	virtual			~Application();

	bool			Create(const wchar_t* appName = L"", int nWidth = 800, int nHeight = 600);
	void			Run();


	virtual bool		OnInitApp()						{return true;}
	virtual bool		OnCreateDevice()				{return true;}		// call after device created
	virtual void		OnDestroyDevice()				{}		// call before device destroy
	virtual void		RenderFrame()	{};

	virtual void		OnResizeFrameBuffer(int width, int height)	{}
	virtual void		OnUpdate(float dt)							{}
	virtual void		OnRender()								{}

	HWND				GetHWnd() { return m_hWnd; }


	void			StartTiming();
	float			EndTiming(char* text, bool bTextout = true);


protected:
	static Application*	s_App;


	int				m_nWindowWidth;
	int				m_nWindowHeight;
	HWND			m_hWnd;
	HDC				m_hDC;

	long			m_lTimeStart;
	long			m_lTimeEnd;

private:
	// message call back
	static LRESULT CALLBACK			WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT							MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void		CleanEnvironment();

};







#endif
