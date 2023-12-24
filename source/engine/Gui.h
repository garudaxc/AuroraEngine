#pragma once

#include "imgui.h"

namespace Aurora
{
	void	GuiInit(class CScreen* InScreen);
	void	GuiNewFrame();
	void	GuiRender();
	void	GuiCleanup();
}
