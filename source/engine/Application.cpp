
#include "stdHeader.h"
#include <time.h>
#include "Application.h"
#include "Console.h"




LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Aurora
{

Application* Application::s_App = nullptr;
HWND	GMainHWnd = nullptr;
HINSTANCE ModuleInstance = nullptr;

Application::Application()
{
	s_App	= this;
}


Application::~Application()
{
}

/*
===========================
Application::Create
===========================
*/
bool Application::Create(const wchar_t* appName, int nWidth, int nHeight)
{
	WindowWidth	= nWidth;
	WindowHeight	= nHeight;

	GMainWindow.width	= nWidth;
	GMainWindow.height = nHeight;

	ModuleInstance = ::GetModuleHandle(nullptr);
	static wchar_t s_acWindowClass[] = L"GaEngine Application";
	// Register the window class
	WNDCLASSEXW wc;
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= ModuleInstance;
	wc.hIcon			= nullptr;
	wc.hCursor			= ::LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= s_acWindowClass;
	wc.hIconSm			= nullptr;

	::RegisterClassExW(&wc);

	DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW;
	// require the window to have the specified client area

	RECT rcWindow;
	rcWindow.left	= 0;
	rcWindow.right	= nWidth;
	rcWindow.top	= 0;
	rcWindow.bottom	= nHeight;
	AdjustWindowRect(&rcWindow, dwWindowStyle, false);

	// create the application window
	GMainHWnd = CreateWindowW(s_acWindowClass, appName, dwWindowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rcWindow.right - rcWindow.left, 
		rcWindow.bottom - rcWindow.top, 0, 0, ModuleInstance, 0);

	bool bRes = InitEnvironment();

	// display the window
	ShowWindow(GMainHWnd, SW_SHOW);
	UpdateWindow(GMainHWnd);

	Console::Init(GMainHWnd);
//	Console::SwitchWindow();

	return bRes;
}

/*
===========================
Application::InitEnvironment()
===========================
*/
bool Application::InitEnvironment()
{
	this->OnInitApp();

	Engine = Engine::Create();
	Engine->Init(WindowWidth, WindowHeight, this);

	RectSize windowSize = { WindowWidth, WindowHeight };
	if (!this->OnCreateDevice(windowSize))
		return false;

	Engine->ResetDevice();

	return true;
}


/*
===========================
Application::CleanEnvironment()
===========================
*/
void Application::CleanEnvironment()
{
	Engine->LostDevice();

	this->OnDestroyDevice();

	Engine->Destroy();

	Console::Destroy();
}

/*
===========================
Application::Run()
===========================
*/
void Application::Run()
{
    bool bGotMsg;
    MSG  msg;
    msg.message = WM_NULL;
    PeekMessage( &msg, nullptr, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message  )
    {
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
        bGotMsg = ( PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) != 0 );

		if (Console::IsShowing() && msg.message == WM_KEYDOWN)
		{
			if(msg.wParam == VK_RETURN)
			{
				Console::Test();
			}
		}

        if (bGotMsg)
        {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
        }
        else
        {
			//if (m_pRender->IsRenderingOccluded())
            //{
                // Yield some CPU time to other processes
           //     Sleep( 100 ); // 100 milliseconds
           // }
			if (Engine->GetTimer()->IsStopped())
			{
				Sleep(100);
			}
			else
			{
				Engine->RenderOneFrame();
			}

        }
    }
}

void Application::StartTiming()
{	
	TimeStart = clock();
}

float Application::EndTiming(char* text, bool bTextout)
{
	TimeEnd = clock();

	float elepsed = (TimeEnd - TimeStart) / (float)CLOCKS_PER_SEC;
	if (bTextout)
	{
		std::stringstream ss;
		ss << text << " : " << elepsed << std::endl;
		Console::AppendText(ss.str().c_str());
	}

	return elepsed;	
}

extern void EngineWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT Application::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// call engine's event routin
	EngineWndProc(hWnd, uMsg, wParam, lParam);

	if (s_App != nullptr)
	{
		return s_App->MsgProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}


LRESULT Application::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	static bool bIsResizing = false;
	static int nWidth = 0;
	static int nHeight = 0;

	switch (uMsg)
	{
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
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			/*EventDispatcher* pDispatcher = GetEngine()->GetEventDispatcher();
			if (pDispatcher)
			{
				((WindowsEventDispatcher*)pDispatcher)->OnMessage(hWnd, uMsg, wParam, lParam);
			}*/
		}
		break;


	case WM_CHAR:
		if (wParam == 'c')
		{
			Console::SwitchWindow();
		}
		break;
	case WM_PAINT:
		break;
	case WM_DESTROY:
		CleanEnvironment();
		PostQuitMessage(0);
		break;
	case WM_ENTERSIZEMOVE:
		{
			//Console::AppendText("enter size\n");
			Engine->GetTimer()->Stop();
			bIsResizing = true;
		}
		break;
	case WM_EXITSIZEMOVE:
		{
			//Console::AppendText("exit size\n");
			Engine->GetTimer()->Start();
			bIsResizing = false;
			if (WindowWidth != nWidth || WindowHeight != nHeight)
			{
				WindowWidth = nWidth;
				WindowHeight = nHeight;
				Engine->ResizeFrameBuffer(nWidth, nHeight);
			}
		}
		break;
	case WM_SIZE:
		{
			nWidth	= (int)LOWORD(lParam); 
			nHeight	= (int)HIWORD(lParam); 

			if (wParam == SIZE_MAXHIDE)
			{
				Console::AppendText("sizing MAXHIDE\n");
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				Console::AppendText("sizing SIZE_MAXIMIZED\n");
				if (Engine->GetTimer()->IsStopped())
				{
					Engine->GetTimer()->Start();
				}
				else if (WindowWidth != nWidth || WindowHeight != nHeight)
				{
					WindowWidth = nWidth;
					WindowHeight = nHeight;
					Engine->ResizeFrameBuffer(nWidth, nHeight);
				}
			}
			else if (wParam == SIZE_MAXSHOW)
			{
				Console::AppendText("sizing SIZE_MAXSHOW\n");
			}
			else if (wParam == SIZE_MINIMIZED)
			{
				Console::AppendText("sizing SIZE_MINIMIZED\n");
				Engine->GetTimer()->Stop();
			}
			else if (wParam == SIZE_RESTORED)
			{
				Console::AppendText("sizing SIZE_RESTORED\n");
				if (bIsResizing || !Engine)
				{
					break;
				}

				if (Engine->GetTimer() && Engine->GetTimer()->IsStopped())
				{
					Engine->GetTimer()->Start();
				}
				else if (WindowWidth != nWidth || WindowHeight != nHeight)
				{
					WindowWidth = nWidth;
					WindowHeight = nHeight;
					Engine->ResizeFrameBuffer(nWidth, nHeight);
				}
			}
		}
		break;
	case WM_ACTIVATEAPP:
		/*if (wParam == TRUE)
		{
			Engine->GetTimer()->Start();
			Console::AppendText("actived\n");
		}
		else if (wParam == FALSE)
		{
			Engine->GetTimer()->Stop();
			Console::AppendText("deactived\n");
		}*/

		break;
		
	default:
		
		break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


}