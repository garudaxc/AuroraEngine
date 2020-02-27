#include "H3DSTLHeader.h"
#include "DebugConsole.h"
#include "Globals.h"
#include "CommandSystem.h"

namespace H3D
{

#define ENTERKEY_ID		1
#define OPENLOG_ID		201
#define QUIT_ID			202
#define CLEAR_ID		203

#define ERRORBOX_ID		10

#define EDIT_ID			100
#define INPUT_ID		101

#define	COMMAND_HISTORY	64

	const int MAX_EDIT_LINE = 256;

typedef struct {
	bool		bInit;
	bool		bShow;
	HWND		hWnd;
	HWND		hwndBuffer;

	HWND		hwndButtonClear;
	HWND		hwndButtonCopy;
	HWND		hwndButtonQuit;

	HWND		hwndErrorBox;
	HWND		hwndErrorText;

	HBITMAP		hbmLogo;
	HBITMAP		hbmClearBitmap;
	
	HFONT		hfBufferFont;
	HFONT		hfButtonFont;

	HWND		hwndInputLine;

	char		errorString[80];

	char		consoleText[512], returnedText[512];
	int			windowWidth, windowHeight;
	 
	WNDPROC		SysInputLineWndProc;

	HINSTANCE   hInstance;

	H3DString	historyEditLines[COMMAND_HISTORY];
	H3DString	consoleField;
	
	int			nextHistoryLine;// the last line in the history buffer, not masked
	int			historyLine;	// the line being displayed from history buffer
								// will be <= nextHistoryLine

} WinConData;

static WinConData s_wcd;

void Sys_ShowConsole(int visLevel);
void OnDropFile(HDROP hDrop);
void OnExcute();

static LONG WINAPI ConWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//char *cmdString;
	static bool s_timePolarity;

	switch (uMsg) {
		case WM_ACTIVATE:
			if ( LOWORD( wParam ) != WA_INACTIVE ) {
				SetFocus( s_wcd.hwndInputLine );
			}
		break;
		case WM_CLOSE:
			Sys_ShowConsole(0);
			return 0;
		case WM_SYSCOMMAND:
			if ( wParam == SC_CLOSE ) {
				Sys_ShowConsole(0);
				//PostQuitMessage( 0 );
			}
			break;
		case WM_COMMAND:
			if ( wParam == OPENLOG_ID ) {
				/*SendMessage( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
				SendMessage( s_wcd.hwndBuffer, WM_COPY, 0, 0 );*/
				const H3DString& logFileName = getGlobalObjMnger()->GetLogMng()->GetFileName();
				ShellExecute(NULL, "open", logFileName.c_str(), NULL, NULL, SW_SHOW);
			} 
			else if ( wParam == QUIT_ID ) 
			{
				// 隐藏console窗口
				Sys_ShowConsole(0);
			} 
			else if ( wParam == CLEAR_ID ) 
			{
				SendMessage( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
				SendMessage( s_wcd.hwndBuffer, EM_REPLACESEL, FALSE, ( LPARAM ) "" );
				UpdateWindow( s_wcd.hwndBuffer );
			}
			// .net环境中，应在edit控件中收到的回车键消息会被转换成id为1的command消息
			// 因此需要在这里响应一下，处理命令输入
			else if (wParam == ENTERKEY_ID)
			{
				OnExcute();
			}
			break;
		case WM_DROPFILES:
			OnDropFile((HDROP)wParam);
			break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

LONG WINAPI InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int key;
	switch ( uMsg ) {
	case WM_KILLFOCUS:
		if ( ( HWND ) wParam == s_wcd.hWnd || ( HWND ) wParam == s_wcd.hwndErrorBox ) {
			SetFocus( hWnd );
			return 0;
		}
		break;

	case WM_KEYDOWN:
		if (wParam == VK_UP)
		{
			if ( s_wcd.nextHistoryLine - s_wcd.historyLine < COMMAND_HISTORY && s_wcd.historyLine > 0 ) {
				s_wcd.historyLine--;
			}
			s_wcd.consoleField = s_wcd.historyEditLines[ s_wcd.historyLine % COMMAND_HISTORY ];

			SetWindowText( s_wcd.hwndInputLine, s_wcd.consoleField.c_str() );
			SendMessage( s_wcd.hwndInputLine, EM_SETSEL, 0, -1 );
			return 0;
		}

		if (wParam == VK_DOWN)
		{
			if ( s_wcd.historyLine == s_wcd.nextHistoryLine ) {
				return 0;
			}
			s_wcd.historyLine++;
			s_wcd.consoleField = s_wcd.historyEditLines[ s_wcd.historyLine % COMMAND_HISTORY ];

			SetWindowText( s_wcd.hwndInputLine, s_wcd.consoleField.c_str() );
			SendMessage( s_wcd.hwndInputLine, EM_SETSEL, 0, -1 );
			return 0;
		}

		break;

	case WM_CHAR:
		key = (int)wParam;

		GetWindowText( s_wcd.hwndInputLine, s_wcd.consoleText, MAX_EDIT_LINE );

	//	// enter the line
		if ( key == VK_RETURN ) {
			OnExcute();
			return 0;
		}

		// command completion
		if ( key == VK_TAB ) {
	//		s_wcd.consoleField.AutoComplete();

	//		SetWindowText( s_wcd.hwndInputLine, s_wcd.consoleField.GetBuffer() );
	//		//s_wcd.consoleField.SetWidthInChars( strlen( s_wcd.consoleField.GetBuffer() ) );
	//		SendMessage( s_wcd.hwndInputLine, EM_SETSEL, s_wcd.consoleField.GetCursor(), s_wcd.consoleField.GetCursor() );

	//		return 0;
	//	}

	//	// clear autocompletion buffer on normal key input
	//	if ( ( key >= K_SPACE && key <= K_BACKSPACE ) || 
	//		( key >= K_KP_SLASH && key <= K_KP_PLUS ) || ( key >= K_KP_STAR && key <= K_KP_EQUALS ) ) {
	//		s_wcd.consoleField.ClearAutoComplete();
		}
		break;
	}

	return CallWindowProc( s_wcd.SysInputLineWndProc, hWnd, uMsg, wParam, lParam );
}

void OnExcute()
{
	GetWindowText( s_wcd.hwndInputLine, s_wcd.consoleText, MAX_EDIT_LINE );
	if (strlen(s_wcd.consoleText) == 0)
	{
		return;
	}

	// copy line to history buffer
	s_wcd.historyEditLines[s_wcd.nextHistoryLine % COMMAND_HISTORY] = s_wcd.consoleText;
	s_wcd.nextHistoryLine++;
	s_wcd.historyLine = s_wcd.nextHistoryLine;

	strcat( s_wcd.consoleText, "\n" );
	SetWindowText( s_wcd.hwndInputLine, "" );
	GDebugConsole.AppendText(s_wcd.consoleText);
	GCommandSystem->ExecuteCommandText(s_wcd.consoleText);
}


/*
** Sys_ShowConsole
*/
void Sys_ShowConsole( int visLevel) {
	
	if ( !s_wcd.hWnd ) {
		return;
	}

	switch ( visLevel ) {
		case 0:
			ShowWindow( s_wcd.hWnd, SW_HIDE );
			s_wcd.bShow = false;
		break;
		case 1:
			ShowWindow( s_wcd.hWnd, SW_SHOWNORMAL );
			SendMessage( s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff );
			UpdateWindow( s_wcd.hWnd );
			s_wcd.bShow = true;
		break;
		case 2:
			ShowWindow( s_wcd.hWnd, SW_MINIMIZE );
		break;
		default:
			//Sys_Error( "Invalid visLevel %d sent to Sys_ShowConsole\n", visLevel );
		break;
	}
}


void OnDropFile(HDROP hDrop)
{
	if (hDrop == NULL)
	{
		return;
	}

	UINT numFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	for (UINT i = 0; i < numFiles; i++)
	{
		TCHAR buffer[256];
		DragQueryFile(hDrop, i, buffer, sizeof(buffer) / sizeof(buffer[0]));
		GCommandSystem->ExecuteFile(H3DString(buffer));
	}

	DragFinish(hDrop);
}

/*
** Win_SetErrorText
*/
void Win_SetErrorText( const char *buf ) {
	//idStr::Copynz( s_wcd.errorString, buf, sizeof( s_wcd.errorString ) );
	if ( !s_wcd.hwndErrorBox ) {
		s_wcd.hwndErrorBox = CreateWindow( "static", NULL, WS_CHILD | WS_VISIBLE | SS_SUNKEN,
													6, 5, 526, 30,
													s_wcd.hWnd, 
													( HMENU ) ERRORBOX_ID,	// child window ID
													s_wcd.hInstance, NULL );
		SendMessage( s_wcd.hwndErrorBox, WM_SETFONT, ( WPARAM ) s_wcd.hfBufferFont, 0 );
		SetWindowText( s_wcd.hwndErrorBox, s_wcd.errorString );

		DestroyWindow( s_wcd.hwndInputLine );
		s_wcd.hwndInputLine = NULL;
	}
}


	DebugConsole GDebugConsole;
	

	DebugConsole::DebugConsole()
	{
		s_wcd.bInit = false;
	}

	DebugConsole::~DebugConsole()
	{

	}

	void DebugConsole::Init()
	{
		if (s_wcd.bInit)
		{
			return;
		}
		s_wcd.bInit = true;

		HDC hDC;
		WNDCLASS wc;
		RECT rect;
		const char *DEDCLASS = "H3DDebugConsole";
		int nHeight;
		int swidth, sheight;
		int DEDSTYLE = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;

		memset( &wc, 0, sizeof( wc ) );

		s_wcd.hInstance = GetModuleHandle(NULL);

		wc.style         = 0;
		wc.lpfnWndProc   = (WNDPROC) ConWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = s_wcd.hInstance;
		wc.hIcon         = NULL; //LoadIcon( win32.hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor       = LoadCursor (NULL,IDC_ARROW);
		wc.hbrBackground = (struct HBRUSH__ *)COLOR_WINDOW;
		wc.lpszMenuName  = 0;
		wc.lpszClassName = DEDCLASS;

		if ( !RegisterClass (&wc) ) {
			return;
		}

		rect.left = 0;
		rect.right = 800;
		rect.top = 0;
		rect.bottom = 600;
		AdjustWindowRect( &rect, DEDSTYLE, FALSE );

		hDC = GetDC( GetDesktopWindow() );
		swidth = GetDeviceCaps( hDC, HORZRES );
		sheight = GetDeviceCaps( hDC, VERTRES );
		ReleaseDC( GetDesktopWindow(), hDC );

		s_wcd.windowWidth = rect.right - rect.left + 1;
		s_wcd.windowHeight = rect.bottom - rect.top + 1;

		//s_wcd.hbmLogo = LoadBitmap( win32.hInstance, MAKEINTRESOURCE( IDB_BITMAP_LOGO) );

		s_wcd.hWnd = CreateWindowEx( 0,
			DEDCLASS,
			"H3DEngine",
			DEDSTYLE,
			( swidth - 600 ) / 2, ( sheight - 450 ) / 2 , rect.right - rect.left + 1, rect.bottom - rect.top + 1,
			NULL,
			NULL,
			s_wcd.hInstance,
			NULL );

		if ( s_wcd.hWnd == NULL ) {
			return;
		}

		DragAcceptFiles(s_wcd.hWnd, TRUE);

		//
		// create fonts
		//
		hDC = GetDC( s_wcd.hWnd );
		nHeight = -MulDiv( 8, GetDeviceCaps( hDC, LOGPIXELSY ), 72 );

		s_wcd.hfBufferFont = CreateFont( nHeight, 0, 0, 0, FW_LIGHT, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN | FIXED_PITCH, "Courier New" );

		ReleaseDC( s_wcd.hWnd, hDC );

		//
		// create the input line
		//
		s_wcd.hwndInputLine = CreateWindow( "edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | 
			ES_LEFT | ES_AUTOHSCROLL,
			6, 540, 780, 20,
			s_wcd.hWnd, 
			( HMENU ) INPUT_ID,	// child window ID
			s_wcd.hInstance, NULL );

		//
		// create the buttons
		//
		s_wcd.hwndButtonClear = CreateWindow( "button", NULL, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			5, 565, 72, 24,
			s_wcd.hWnd, 
			( HMENU ) CLEAR_ID,	// child window ID
			s_wcd.hInstance, NULL );
		SendMessage( s_wcd.hwndButtonClear, WM_SETTEXT, 0, ( LPARAM ) "clear" );

		s_wcd.hwndButtonCopy = CreateWindow( "button", NULL, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			82, 565, 100, 24,
			s_wcd.hWnd, 
			( HMENU ) OPENLOG_ID,	// child window ID
			s_wcd.hInstance, NULL );
		SendMessage( s_wcd.hwndButtonCopy, WM_SETTEXT, 0, ( LPARAM ) "打开日志文件" );


		s_wcd.hwndButtonQuit = CreateWindow( "button", NULL, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			562, 565, 72, 24,
			s_wcd.hWnd, 
			( HMENU ) QUIT_ID,	// child window ID
			s_wcd.hInstance, NULL );
		SendMessage( s_wcd.hwndButtonQuit, WM_SETTEXT, 0, ( LPARAM ) "关闭" );


		//
		// create the scrollbuffer
		//
		s_wcd.hwndBuffer = CreateWindow( "edit", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | 
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
			6, 40, 780, 484,
			s_wcd.hWnd, 
			( HMENU ) EDIT_ID,	// child window ID
			s_wcd.hInstance, NULL );
		SendMessage( s_wcd.hwndBuffer, WM_SETFONT, ( WPARAM ) s_wcd.hfBufferFont, 0 );

		s_wcd.SysInputLineWndProc = ( WNDPROC ) SetWindowLong( s_wcd.hwndInputLine, GWL_WNDPROC, ( long ) InputLineWndProc );
		SendMessage( s_wcd.hwndInputLine, WM_SETFONT, ( WPARAM ) s_wcd.hfBufferFont, 0 );
		
		// don't show it now that we have a splash screen up
		//if ( win32.win_viewlog.GetBool() ) {
		//ShowWindow( s_wcd.hWnd, SW_SHOWDEFAULT);
		//UpdateWindow( s_wcd.hWnd );
		//SetForegroundWindow( s_wcd.hWnd );
		//SetFocus( s_wcd.hwndInputLine );
		//}

		//s_wcd.consoleField.Clear();

		//for ( i = 0 ; i < COMMAND_HISTORY ; i++ ) {
		//	s_wcd.historyEditLines[i].Clear();
		//}

		s_wcd.historyLine = 0;
		s_wcd.nextHistoryLine = 0;

		Sys_ShowConsole(0);
	}

	void DebugConsole::Destroy()
	{
		if (!s_wcd.bInit)
		{
			return;
		}
		if ( s_wcd.hWnd ) {
			ShowWindow( s_wcd.hWnd, SW_HIDE );
			CloseWindow( s_wcd.hWnd );
			DestroyWindow( s_wcd.hWnd );
			s_wcd.hWnd = 0;
		}
	}

	void DebugConsole::Show()
	{
		if (!s_wcd.bInit)
		{
			return;
		}

		Sys_ShowConsole(1);
	}

	void DebugConsole::AppendText(const char* pMsg)
	{
		if (!s_wcd.bInit || !s_wcd.bShow)
		{
			return;
		}

#define CONSOLE_BUFFER_SIZE		16384

		char buffer[CONSOLE_BUFFER_SIZE*2];
		char *b = buffer;
		const char *msg;
		int bufLen;
		int i = 0;
		static unsigned long s_totalChars;

		//
		// if the message is REALLY long, use just the last portion of it
		//
		if ( strlen( pMsg ) > CONSOLE_BUFFER_SIZE - 1 )	{
			msg = pMsg + strlen( pMsg ) - CONSOLE_BUFFER_SIZE + 1;
		} else {
			msg = pMsg;
		}

		//
		// copy into an intermediate buffer
		//
		while ( msg[i] && ( ( b - buffer ) < sizeof( buffer ) - 1 ) ) {
			if ( msg[i] == '\n' && msg[i+1] == '\r' ) {
				b[0] = '\r';
				b[1] = '\n';
				b += 2;
				i++;
			} else if ( msg[i] == '\r' ) {
				b[0] = '\r';
				b[1] = '\n';
				b += 2;
			} else if ( msg[i] == '\n' ) {
				b[0] = '\r';
				b[1] = '\n';
				b += 2;
				//} else if ( idStr::IsColor( &msg[i] ) ) {
				//	i++;
			} else {
				*b= msg[i];
				b++;
			}
			i++;
		}
		*b = 0;
		bufLen = b - buffer;

		s_totalChars += bufLen;

		//
		// replace selection instead of appending if we're overflowing
		//
		if ( s_totalChars > 0x7000 ) {
			SendMessage( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
			s_totalChars = bufLen;
		}
		else {
			SendMessage( s_wcd.hwndBuffer, EM_SETSEL, 0xFFFF, 0xFFFF );
		}

		SendMessage( s_wcd.hwndBuffer, EM_REPLACESEL, 0, (LPARAM) buffer );
		SendMessage( s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff );


		//
		// put this text into the windows console
		//
		//SendMessage( s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff );
		//SendMessage( s_wcd.hwndBuffer, EM_SCROLLCARET, 0, 0 );
		//SendMessage( s_wcd.hwndBuffer, EM_REPLACESEL, 0, (LPARAM) buffer );
	}
}