#include "stdHeader.h"
#include "Console.h"

namespace Aurora
{

#define	CLEAR_ID		3

struct ConsoleData
{
	HWND		hParentWnd;
	HWND		hWnd;
	HWND		hwndBuffer;
	HWND		hInput;

	HWND		hwndButtonClear;

	HFONT		hfBufferFont;
	HFONT		hfButtonFont;

	bool		bWindowShowing;


	int			windowWidth, windowHeight;
	string*		s_pRedirect;


	ConsoleData()
	{
		memset(this, 0, sizeof(ConsoleData));
	}

};

static ConsoleData g_ConData;





static LRESULT WINAPI ConWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//	char *cmdString;
	switch (uMsg)
	{
	case WM_ACTIVATE:
		if (wParam == WA_ACTIVE)
		{
			SetActiveWindow(g_ConData.hParentWnd);
		}
		break;
	case WM_CLOSE:
		Console::HideWindow();
		return 0;
	case WM_COMMAND:
		if ( wParam == CLEAR_ID )
		{
			SendMessage(g_ConData.hwndBuffer, EM_SETSEL, 0, -1 );
			SendMessage(g_ConData.hwndBuffer, EM_REPLACESEL, FALSE, (LPARAM)L"");
			UpdateWindow(g_ConData.hwndBuffer );
		}
		break;
	default:
		break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

Console::Console(void)
{
}

Console::~Console(void)
{
}

void Console::Init(HWND hParentWnd)
{
	RECT rect;
	static wchar_t *DEDCLASS = L"ConsoleWindow";
	int swidth, sheight;

	g_ConData.hParentWnd = hParentWnd;
	HINSTANCE hInst = ::GetModuleHandle(nullptr);
	WNDCLASSEXW wc;
	memset(&wc, 0, sizeof( wc ));

	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC) ConWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= nullptr;
	wc.hCursor			= ::LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= DEDCLASS;
	wc.hIconSm			= nullptr;

	if (!RegisterClassExW(&wc))
		return;
// 
	int style = WS_POPUPWINDOW | WS_CAPTION;
	int styleEx = WS_EX_PALETTEWINDOW;
	rect.left = 0;
	rect.right = 540;
	rect.top = 0;
	rect.bottom = 440;
	AdjustWindowRectEx(&rect, style, FALSE, styleEx);

	HDC hDC = GetDC( GetDesktopWindow() );
	swidth = GetDeviceCaps( hDC, HORZRES );
	sheight = GetDeviceCaps( hDC, VERTRES );
	ReleaseDC( GetDesktopWindow(), hDC );

	g_ConData.windowWidth = rect.right - rect.left + 1;
	g_ConData.windowHeight = rect.bottom - rect.top + 1;

	g_ConData.hWnd = CreateWindowExW(styleEx, DEDCLASS, L"Console", style,( swidth - 600 ) / 2, ( sheight - 450 ) / 2 ,
		rect.right - rect.left + 1, rect.bottom - rect.top + 1, nullptr, nullptr, hInst, nullptr );
	::SetWindowLong(g_ConData.hWnd, GWL_EXSTYLE, WS_EX_TOPMOST | ::GetWindowLong(g_ConData.hWnd, GWL_EXSTYLE));

	if (g_ConData.hWnd == nullptr)
	{


		return;
	}


	hDC = GetDC(g_ConData.hWnd);
	int nHeight = -MulDiv(8, GetDeviceCaps( hDC, LOGPIXELSY), 72);

	g_ConData.hfBufferFont = CreateFontW(nHeight, 0, 0, 0, FW_LIGHT, 0, 0, 0, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS,  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN | FIXED_PITCH, L"Courier New");

	ReleaseDC(g_ConData.hWnd, hDC);

	//
	// create the input line
	//
	g_ConData.hInput  = CreateWindowW(L"edit", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | 
		ES_LEFT | ES_AUTOVSCROLL, 6, 10, 526, 20, g_ConData.hWnd, nullptr, hInst, nullptr);
	SendMessage(g_ConData.hInput, WM_SETFONT, (WPARAM)g_ConData.hfBufferFont, 0);


	g_ConData.hwndBuffer = CreateWindowW(L"edit", nullptr, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | 
												ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
												6, 40, 526, 354, g_ConData.hWnd, nullptr, hInst, nullptr);
	SendMessage(g_ConData.hwndBuffer, WM_SETFONT, (WPARAM)g_ConData.hfBufferFont, 0);

	g_ConData.hwndButtonClear = CreateWindowW(L"button", nullptr, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		400, 400, 72, 24, g_ConData.hWnd, (HMENU)CLEAR_ID, hInst, nullptr);
	SendMessage(g_ConData.hwndButtonClear, WM_SETFONT, (WPARAM)g_ConData.hfBufferFont, 0 );
	SendMessage(g_ConData.hwndButtonClear, WM_SETTEXT, 0, (LPARAM)L"Clear");

	g_ConData.bWindowShowing = false;

}

void Console::Destroy()
{
	if (g_ConData.hWnd)
	{
		::ShowWindow(g_ConData.hWnd, SW_HIDE);
		::CloseWindow(g_ConData.hWnd);
		::DestroyWindow(g_ConData.hWnd);
		g_ConData.hWnd = 0;
	}
}

void Console::SetRedirect(string* pRedir)
{
	g_ConData.s_pRedirect = pRedir;
}


void Console::AppendText(const char *pMsg)
{
	const int CONSOLE_BUFFER_SIZE = 16384;

	char buffer[CONSOLE_BUFFER_SIZE];
	wchar_t wbuffer[CONSOLE_BUFFER_SIZE];
	const char *msg = pMsg;
	static unsigned long s_totalChars = 0;
	
	// copy into an intermediate buffer
	char *b = buffer;
	int i = 0;
	while (msg[i] && ( ( b - buffer ) < sizeof( buffer ) - 1 ))
	{
		if ( msg[i] == '\n' && msg[i+1] == '\r' )
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
			i++;
		}
		else if ( msg[i] == '\r' )
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
		}
		else if ( msg[i] == '\n' )
		{
			b[0] = '\r';
			b[1] = '\n';
			b += 2;
		}
		else
		{
			*b= msg[i];
			b++;
		}
		i++;
	}
	*b = 0;
	int bufLen = (int)(b - buffer);

	s_totalChars += bufLen;

	// replace selection instead of appending if we're overflowing
	if ( s_totalChars > 0x3fff )
	{
		SendMessage(g_ConData.hwndBuffer, EM_SETSEL, 0, -1);
		s_totalChars = bufLen;
	}
	MultiByteToWideChar(CP_ACP, 0, buffer, CONSOLE_BUFFER_SIZE, wbuffer, CONSOLE_BUFFER_SIZE);

	// put this text into the windows console
	SendMessage(g_ConData.hwndBuffer, EM_LINESCROLL, 0, 0xffff);
	SendMessage(g_ConData.hwndBuffer, EM_SCROLLCARET, 0, 0);
	SendMessage(g_ConData.hwndBuffer, EM_REPLACESEL, 0, (LPARAM)wbuffer);

	if (g_ConData.s_pRedirect != nullptr)
	{
		(*g_ConData.s_pRedirect) += pMsg;
	}
}


void Console::ShowWindow()
{
	::ShowWindow(g_ConData.hWnd, SW_SHOWNORMAL);
	::UpdateWindow(g_ConData.hWnd);
	g_ConData.bWindowShowing = true;
}

void Console::HideWindow()
{
	::ShowWindow(g_ConData.hWnd, SW_HIDE);
	g_ConData.bWindowShowing = false;
}


void Console::SwitchWindow()
{
	if (g_ConData.bWindowShowing)
	{
		Console::HideWindow();
	}
	else
	{
		Console::ShowWindow();
	}
}


bool Console::IsShowing()
{
	return g_ConData.bWindowShowing;
}

void Console::Test()
{	

	//SendMessage(g_ConData.hwndBuffer, EM_SETSEL, 0, -1 );
	//SendMessage(g_ConData.hwndBuffer, EM_REPLACESEL, FALSE, (LPARAM)L"");
	//UpdateWindow(g_ConData.hwndBuffer );
	SendMessage(g_ConData.hInput, WM_SETTEXT, 0, (LPARAM)L"");

	
}





}//end of namespace
