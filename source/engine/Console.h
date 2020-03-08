#ifndef GACONSOLE_H
#define GACONSOLE_H
#include <windows.h>
namespace Aurora
{

class Console
{
public:
	Console(void);
	~Console(void);

	static void			Init(HWND hParentWnd);
	static void			Destroy();
	static void			AppendText(const char *pMsg);
	static void			ShowWindow();
	static void			HideWindow();
	static void			SwitchWindow();
	static bool			IsShowing();

	static void			SetRedirect(string* pRedir);

	static void			Test();
};

} // end of namespace

#endif