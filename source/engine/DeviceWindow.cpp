#include "stdheader.h"
#include <windows.h>
#include "DeviceWindow.h"
namespace Aurora
{

void DeviceWindowWinAPI::SetCapture()
{
	::SetCapture(m_hWnd);
}

void DeviceWindowWinAPI::ReleaseCapture()
{ 
	::ReleaseCapture();
}

void DeviceWindowWinAPI::HideCursor()
{
	::ShowCursor(FALSE);
}

void DeviceWindowWinAPI::ShowCursor()
{ 
	::ShowCursor(TRUE);
}

void DeviceWindowWinAPI::GetWindowPos(int& xPos, int& yPos) const
{
	RECT rect;
	::GetWindowRect(m_hWnd, &rect);
	xPos = (int)rect.left;
	yPos = (int)rect.top;
}

void DeviceWindowWinAPI::GetWindowSize(int& width, int& height) const
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	width = (int)rect.right;
	height = (int)rect.bottom;
}

void DeviceWindowWinAPI::GetCursorPos(int& xPos, int& yPos) const
{
	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(m_hWnd, &pt);
	xPos = (uint)pt.x;
	yPos = (uint)pt.y;
}

void DeviceWindowWinAPI::SetCursorPos(int xPos, int yPos) const
{
	POINT pt = {xPos, yPos};
	::ClientToScreen(m_hWnd, &pt);
	::SetCursorPos(pt.x, pt.y);
}


}// end of namespace
