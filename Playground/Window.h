#pragma once
#include "GeometryFoundation.h"
#include "ReferenceCount.h"
#include "Input.h"
#include <functional>
#include <string>
#include <memory>

namespace X
{
	class Window : public ReferenceCountBase<true>
	{
	public:

		static Ptr<Window> Create(std::wstring title, Size2UI size);

		~Window();

		void StartHandlingMessages();

		bool IsRendering() const;
		void SetRendering(bool rendering);
		bool IsRunning() const;
		void SetRunning(bool running);

		bool IsActive() const;
		void SetActive(bool active);



		/*
		*	Client region size, not the window size.
		*/
		Size2UI GetClientRegionSize() const;

		/*
		*	Upper left corner of the window.
		*/
		Size2SI GetWindowPosition() const;

		std::wstring GetTitleText() const;
		void SetTitleText(std::wstring const& text);

		/*
		*	@hook: pointer is type: std::function<void(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>*
		*		the pointer will be cast to the type above and make a copy of the function object.
		*/
		void SetRawWindowsMessageHook(void const* hook);

		/*
		*	@return: actual type is HWND, return void* to remove windows.h dependency from hpp.
		*/
		void* GetHWND() const;

		void Recreate();

		void SetMessageIdle(std::function<void()> const& messageIdle);
		void SetInputHandler(Ptr<InputHandler> inputHanlder);

	protected:
		Window() = default;
	};


}
