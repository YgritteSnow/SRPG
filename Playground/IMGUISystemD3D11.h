// ImGui Win32 + DirectX11 binding
// In this binding, ImTextureID is used to store a 'ID3D11ShaderResourceView*' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "ReferenceCount.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ImDrawData;

namespace X
{
	class IMGUISystemD3D11 : public ReferenceCountBase<true>
	{
	public:
		virtual ~IMGUISystemD3D11() = default;

		virtual bool ImGui_ImplDX11_Init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context) = 0;
		virtual void ImGui_ImplDX11_Shutdown() = 0;
		virtual void ImGui_ImplDX11_NewFrame() = 0;
		virtual void ImGui_ImplDX11_RenderDrawLists(ImDrawData* draw_data) = 0;

		// Use if you want to reset your rendering device without losing ImGui state.
		virtual void ImGui_ImplDX11_InvalidateDeviceObjects() = 0;
		virtual bool ImGui_ImplDX11_CreateDeviceObjects() = 0;

		// Handler for Win32 messages, update mouse/keyboard data.
		// You may or not need this for your implementation, but it can serve as reference for handling inputs.
		// Commented out to avoid dragging dependencies on <windows.h> types. You can copy the extern declaration in your code.

		// LRESULT   ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		virtual void* GetWndProcHandler() = 0;

		static Ptr<IMGUISystemD3D11> Create();
	};

}

