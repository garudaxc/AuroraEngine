#include "Application.h"
#include "stdHeader.h"
#include "Engine.h"
#include "Renderer.h"
#include "imgui.h"
#include "Platform.h"
#include "Log.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Aurora
{

    
    bool CreateDX11Device(CScreen* InScreen);
    bool CreateOpenGLDevice(CScreen* InScreen);
	
    IRenderDevice* IRenderDevice::CreateDevice(RendererType InType, CScreen* InScreen)
    {
        if(InType == RendererType::DirectX11)
        {
            CreateDX11Device(InScreen);
        }
        else if(InType == RendererType::OpenGL)
        {
            CreateOpenGLDevice(InScreen);
        }
		
        return GRenderDevice;
    }


    
    class CScreenWindows : public CScreen
    {
    public:
        CScreenWindows()
        {
            CPlatform::MainScreen = this;
        }
        
        HWND MainHWnd = nullptr;
        HINSTANCE ModuleInstance = nullptr;        
    };
    
    static CScreenWindows GMainScreen;

    
    HWND GetHWNDFromScreen(CScreen* InScreen)
    {
        auto WindowsScreen = static_cast<CScreenWindows*>(InScreen);
        return WindowsScreen->MainHWnd;        
    }


    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool CreateCanvas(int nWidth, int nHeight)
    {
        GMainScreen.ModuleInstance = ::GetModuleHandle(nullptr);
        static wchar_t s_acWindowClass[] = L"Aurora Engine";
        // Register the window class
        WNDCLASSEXW wc;
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GMainScreen.ModuleInstance;
        wc.hIcon = nullptr;
        wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = s_acWindowClass;
        wc.hIconSm = nullptr;

        ::RegisterClassExW(&wc);

        DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW;
        // DWORD dwWindowStyle = WS_POPUP;
        // require the window to have the specified client area

        RECT rcWindow;
        rcWindow.left = 0;
        rcWindow.right = nWidth;
        rcWindow.top = 0;
        rcWindow.bottom = nHeight;
        AdjustWindowRect(&rcWindow, dwWindowStyle, false);

        // create the application window

        const wchar_t* appName = L"Aurora";

        GMainScreen.MainHWnd = CreateWindowExW(WS_EX_APPWINDOW, s_acWindowClass, appName, dwWindowStyle,
                                    CW_USEDEFAULT, CW_USEDEFAULT, rcWindow.right - rcWindow.left,
                                    rcWindow.bottom - rcWindow.top, NULL, NULL, GMainScreen.ModuleInstance, NULL);
        // GMainHWnd = CreateWindowW(s_acWindowClass, appName, dwWindowStyle,
        // 	CW_USEDEFAULT, CW_USEDEFAULT, rcWindow.right - rcWindow.left, 
        // 	rcWindow.bottom - rcWindow.top, 0, 0, ModuleInstance, 0);

        if (!GMainScreen.MainHWnd)
        {
            return false;
        }

        ShowWindow(GMainScreen.MainHWnd, SW_SHOW);
        UpdateWindow(GMainScreen.MainHWnd);
        
        GMainScreen.mScreenSize.Width = nWidth;
        GMainScreen.mScreenSize.Height = nHeight;
        
        return true;
    }

    
    void WindowsInputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if(!IRenderDevice::Initialized())
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        
        
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        {
            return true;            
        }

        WindowsInputWndProc(hWnd, uMsg, wParam, lParam);
        

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
                // Console::SwitchWindow();
            }
            break;
        case WM_PAINT:
            break;
        case WM_DESTROY:
            // CleanEnvironment();
            PostQuitMessage(0);
            break;
        case WM_ENTERSIZEMOVE:
            {
                //Console::AppendText("enter size\n");
                // Engine->GetTimer()->Stop();
                bIsResizing = true;
            }
            break;
        case WM_EXITSIZEMOVE:
            {
                // //Console::AppendText("exit size\n");
                // Engine->GetTimer()->Start();
                // bIsResizing = false;
                // if (WindowWidth != nWidth || WindowHeight != nHeight)
                // {
                // 	WindowWidth = nWidth;
                // 	WindowHeight = nHeight;
                // 	Engine->ResizeFrameBuffer(nWidth, nHeight);
                // }
            }
            break;
        case WM_SIZE:
            {
                nWidth = (int)LOWORD(lParam);
                nHeight = (int)HIWORD(lParam);

                if (wParam == SIZE_MAXHIDE)
                {
                    // Console::AppendText("sizing MAXHIDE\n");
                }
                else if (wParam == SIZE_MAXIMIZED)
                {
                    // Console::AppendText("sizing SIZE_MAXIMIZED\n");
                    // if (Engine->GetTimer()->IsStopped())
                    // {
                    // 	Engine->GetTimer()->Start();
                    // }
                    // else if (WindowWidth != nWidth || WindowHeight != nHeight)
                    // {
                    // 	WindowWidth = nWidth;
                    // 	WindowHeight = nHeight;
                    // 	Engine->ResizeFrameBuffer(nWidth, nHeight);
                    // }
                }
                else if (wParam == SIZE_MAXSHOW)
                {
                    // Console::AppendText("sizing SIZE_MAXSHOW\n");
                }
                else if (wParam == SIZE_MINIMIZED)
                {
                    // Console::AppendText("sizing SIZE_MINIMIZED\n");
                    // Engine->GetTimer()->Stop();
                }
                else if (wParam == SIZE_RESTORED)
                {
                    // Console::AppendText("sizing SIZE_RESTORED\n");
                    // if (bIsResizing || !Engine)
                    // {
                    // 	break;
                    // }
                    //
                    // if (Engine->GetTimer() && Engine->GetTimer()->IsStopped())
                    // {
                    // 	Engine->GetTimer()->Start();
                    // }
                    // else if (WindowWidth != nWidth || WindowHeight != nHeight)
                    // {
                    // 	WindowWidth = nWidth;
                    // 	WindowHeight = nHeight;
                    // 	Engine->ResizeFrameBuffer(nWidth, nHeight);
                    // }
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

        return DefWindowProc(hWnd, uMsg, wParam, lParam);

    }

    Engine* GEngineInstance;


    void EngineLoop()
    {
        Application* CurrentApplication = Application::GetCurrentApplication();
        
        if(!CurrentApplication)
        {
            GLog->Error("Engine Loop : No aviliable application");
            return;
        }

        CurrentApplication->OnInitApp();

        
        bool bGotMsg;
        MSG msg;
        msg.message = WM_NULL;
        PeekMessage(&msg, nullptr, 0U, 0U, PM_NOREMOVE);

        while (WM_QUIT != msg.message)
        {
            // Use PeekMessage() if the app is active, so we can use idle time to
            // render the scene. Else, use GetMessage() to avoid eating CPU time.
            bGotMsg = (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE) != 0);

            // if (Console::IsShowing() && msg.message == WM_KEYDOWN)
            // {
            // 	if(msg.wParam == VK_RETURN)
            // 	{
            // 		Console::Test();
            // 	}
            // }

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


                if (GEngineInstance->GetTimer()->IsStopped())
                {
                	Sleep(100);
                }
                else
                {
                    CurrentApplication->OnUpdate(GEngineInstance->GetTimer()->GetElapsedTime());
                    CurrentApplication->OnRender();
                	GEngineInstance->RenderOneFrame();
                }
            }
        }
    }


    int EngineMain(LPTSTR lpCmdLine)
    {
        int Width = 1200;
        int Height = 800;

        if (!Aurora::CreateCanvas(1200, 800))
        {
            return -1;
        };

        GEngineInstance = Engine::Create();
        GEngineInstance->mCommandLine = lpCmdLine;
        
        GEngineInstance->Init(&GMainScreen);

        EngineLoop();

        return 0;
    }
}


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
    //GCommandSystem->AddCommand("MyCommand", MyCommand, "abck");
    //GCommandSystem->ExecuteCommand("MyCommand");
    

    int Result = Aurora::EngineMain(lpCmdLine);

    return Result;
}
