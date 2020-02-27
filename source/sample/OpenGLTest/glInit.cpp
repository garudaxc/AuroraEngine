
#include <Windows.h>
#include "gl/glew.h"


BOOL SetupPixelFormat(HDC hDC)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		PFD_SUPPORT_OPENGL |          // support OpenGL
		PFD_DOUBLEBUFFER,             // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		24,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		32,                             // 32-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};
	int pixelformat;

	if ( (pixelformat = ChoosePixelFormat(hDC, &pfd)) == 0 )
	{
		//MessageBox("ChoosePixelFormat failed");
		return FALSE;
	}

	if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE)
	{
		//MessageBox("SetPixelFormat failed");
		return FALSE;
	}

	return TRUE;
}

bool InitOpengl(HWND hwnd, HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int         n;
	HGLRC       hrc;
	GLfloat     fMaxObjSize, fAspect;
	GLfloat     fNearPlane, fFarPlane;


	if (!SetupPixelFormat(hDC))
		return false;

	n = ::GetPixelFormat(hDC);
	::DescribePixelFormat(hDC, n, sizeof(pfd), &pfd);

	//CreateRGBPalette();

	hrc = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hrc);

	RECT rect;
	GetClientRect(hwnd, &rect);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	if (rect.bottom)
		fAspect = (GLfloat)rect.right/rect.bottom;
	else    // don't divide by zero, not that we should ever run into that...
		fAspect = 1.0f;
	fNearPlane = 3.0f;
	fFarPlane = 7.0f;
	fMaxObjSize = 3.0f;
	float fRadius = fNearPlane + fMaxObjSize / 2.0f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, fAspect, fNearPlane, fFarPlane);
	glMatrixMode(GL_MODELVIEW);

	return true;
}



