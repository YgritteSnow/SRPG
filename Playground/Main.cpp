#include "ComPtr.h"
#include "Window.h"
#include "DeviceAndContext.h"
#include "IMGUISystemD3D11.h"
#include "Utility.h"

#include "imgui.h"

#include <iostream>

struct GUI
{
	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_col = ImColor(114, 144, 154);
	float f = 0.0f;

	void RenderGUI()
	{
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_col);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
};

int main()
{
	using namespace X;
	using namespace std;

	auto gui = make_unique<GUI>();

	Ptr<Window> window = Window::Create(L"hello", { 1280,800 });
	Ptr<DeviceAndContext> deviceAndContext = CreatePtr<DeviceAndContext>(window);
	Ptr<IMGUISystemD3D11> imgui = IMGUISystemD3D11::Create();

	imgui->ImGui_ImplDX11_Init(window->GetHWND(), deviceAndContext->GetD3DDevice(), deviceAndContext->GetD3DDeviceContext());


	function<void(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)> imguiInputCallback = [imgui](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
	{
		auto wndProc = static_cast<LRESULT(*)(HWND, UINT msg, WPARAM wParam, LPARAM lParam)>(imgui->GetWndProcHandler());
		wndProc(hWnd, message, wParam, lParam);
	};
	window->SetRawWindowsMessageHook(&imguiInputCallback);

	window->SetResizeHandler([deviceAndContext, imgui](uint32 width, uint32 height)
	{
		deviceAndContext->UpdateWindowSize();
		imgui->ImGui_ImplDX11_InvalidateDeviceObjects();
		imgui->ImGui_ImplDX11_CreateDeviceObjects();
	});

	window->SetMessageIdle([deviceAndContext, imgui, &gui]
	{
		imgui->ImGui_ImplDX11_NewFrame();

		gui->RenderGUI();

		deviceAndContext->GetD3DDeviceContext()->ClearRenderTargetView(deviceAndContext->GetBackBufferRenderTargetView(), (float*)&gui->clear_col);
		ID3D11RenderTargetView* rtvs[] = { deviceAndContext->GetBackBufferRenderTargetView() };
		deviceAndContext->GetD3DDeviceContext()->OMSetRenderTargets(ArraySize(rtvs), rtvs, nullptr);

		{
			auto section = deviceAndContext->StartEventSection(L"IMGUI");
			imgui->ImGui_ImplDX11_Render();
		}
		deviceAndContext->Present();

		cout << "hello" << endl;
	});
	window->StartHandlingMessages();

	imgui->ImGui_ImplDX11_Shutdown();

}

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


struct DML
{
	DML()
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}
	~DML()
	{
		if (_CrtDumpMemoryLeaks())
		{
			OutputDebugString(L"memory leaks.");
		}
		else
		{
			OutputDebugString(L"no memory leaks.");
		}
		//std::cin.get();
	}
} _dml;
