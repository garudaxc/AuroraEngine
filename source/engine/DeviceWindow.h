#ifndef GADEVICEWINDOW_H
#define GADEVICEWINDOW_H
#include <windows.h>
namespace Aurora
{

class DeviceWindow
{
public:
	DeviceWindow()				{}
	virtual ~DeviceWindow()		{}

	virtual void					SetCapture()			{}
	virtual void					ReleaseCapture()		{}
	virtual void					HideCursor()			{}
	virtual void					ShowCursor()			{}

	virtual void					GetWindowPos(int& xPos, int& yPos) const		{}
	virtual void					GetWindowSize(int& width, int& height) const	{}
	virtual void					GetCursorPos(int& xPos, int& yPos) const		{}
	virtual void					SetCursorPos(int xPos, int yPos) const			{}
};





class DeviceWindowWinAPI : public DeviceWindow
{
public:
	DeviceWindowWinAPI(HWND hWnd):m_hWnd(hWnd)	{}
	virtual ~DeviceWindowWinAPI()	{}

	virtual void					SetCapture();
	virtual void					ReleaseCapture();
	virtual void					HideCursor();
	virtual void					ShowCursor();

	virtual void					GetWindowPos(int& xPos, int& yPos) const;
	virtual void					GetWindowSize(int& width, int& height) const;
	virtual void					GetCursorPos(int& xPos, int& yPos) const;
	virtual void					SetCursorPos(int xPos, int yPos) const;

private:
	HWND		m_hWnd;

};










}// end of namespace

#endif