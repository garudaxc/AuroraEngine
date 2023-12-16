#include "stdHeader.h"
#include "Input.h"
#include "DeviceWindow.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A // (not always defined)
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120 // (not always defined)
#endif

namespace Aurora
{

	// default dispatcher on windows
	class WindowsEventDispatcher : public EventDispatcher
	{
	public:
		WindowsEventDispatcher();
		void OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

	WindowsEventDispatcher::WindowsEventDispatcher()
	{
	}

	void WindowsEventDispatcher::OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static uint16 eventType[] = {MouseEvent::MouseMove,
			MouseEvent::LButtonDown, MouseEvent::LButtonUp, MouseEvent::LButtonDown,
			MouseEvent::RButtonDown, MouseEvent::RButtonUp, MouseEvent::RButtonDown,
			MouseEvent::MButtonDown, MouseEvent::MButtonUp, MouseEvent::MButtonDown,
			MouseEvent::MouseWheel};

		DeviceWindowWinAPI dw(hWnd);

		switch(message)
		{
			// mouse event
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
			{
				MouseEvent mouseEvent;

				mouseEvent.Type = eventType[message - 0x0200];
				mouseEvent.Param = (uint16)wParam;

				mouseEvent.xPos = (int)LOWORD(lParam); 
				mouseEvent.yPos = (int)HIWORD(lParam); 

				if (message == WM_MOUSEWHEEL)
					mouseEvent.zDelta = (short)HIWORD(wParam);
				else
					mouseEvent.zDelta = 0;

				mouseEvent.pWindow = &dw;

				DispatchMouseEvent(mouseEvent);
				break;
			}

			// key event
		case WM_KEYDOWN:
		case WM_KEYUP:
			{
				KeyEvent keyEvent;
				keyEvent.Code = (unsigned int)wParam;
				if (message == WM_KEYDOWN)
					keyEvent.Type = KeyEvent::KeyDown;
				else
					keyEvent.Type = KeyEvent::KeyUp;

				DispatchKeyEvent(keyEvent);
				break;
			}
		}
	}


//////////////////////////////////////////////////////////////////////////

	static WindowsEventDispatcher eventDispatcher;
	EventDispatcher* GEventDisPatcher = &eventDispatcher;


	void WindowsInputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		eventDispatcher.OnMessage(hWnd, uMsg, wParam, lParam);
	}
}//end of namespace