#include "stdheader.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
namespace Aurora
{
	
	extern HWND	MainHWnd;


	void	GuiInit()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplWin32_Init(MainHWnd);
		//ImGui_ImplDX11_Init(D3D11Device, ImmediateContext);
	}


	void	GuiNewFrame()
	{
		//ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void	GuiRender()
	{
		ImGui::Render();
		//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void	GuiCleanup()
	{
		//ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}


}