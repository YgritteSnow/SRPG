#include "Window.h"
#include "Input.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>
#include <cassert>
#include <vector>

using namespace std;
using namespace X;

struct WindowImpl : public Window
{
	//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
	//
	//  PURPOSE:  Processes messages for the main window.
	//
	//  WM_COMMAND	- process the application menu
	//  WM_PAINT	- Paint the main window
	//  WM_DESTROY	- post a quit message and return
	//
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		WindowImpl* thiz = reinterpret_cast<WindowImpl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (thiz && thiz->hWnd_)
		{
			assert(hWnd == thiz->hWnd_);
			return thiz->InstanceWndProc(hWnd, message, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	LRESULT InstanceWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (messageHook_ != nullptr)
		{
			messageHook_(hWnd, message, wParam, lParam);
		}

		switch (message)
		{
			// Add all windows message handling here
		case WM_KEYDOWN:
		{
			OnKeyDown(uint32(wParam));
			break;
		}

		case WM_KEYUP:
		{
			OnKeyUp(uint32(wParam));
			break;
		}
		case WM_LBUTTONDOWN:
		{
			wParam = VK_LBUTTON;
			OnMouseDown(VK_LBUTTON, LOWORD(lParam), HIWORD(lParam));
		}
		break;
		case WM_RBUTTONDOWN:
		{
			wParam = VK_RBUTTON;
			OnMouseDown(VK_RBUTTON, LOWORD(lParam), HIWORD(lParam));
		}
		break;
		case WM_MBUTTONDOWN:
		{
			wParam = VK_MBUTTON;
			OnMouseDown(VK_MBUTTON, LOWORD(lParam), HIWORD(lParam));
		}
		break;
		case WM_LBUTTONUP:
		{
			wParam = VK_LBUTTON;
			OnMouseUp(VK_LBUTTON, LOWORD(lParam), HIWORD(lParam));
		}
		break;
		case WM_RBUTTONUP:
		{
			wParam = VK_RBUTTON;
			OnMouseUp(VK_RBUTTON, LOWORD(lParam), HIWORD(lParam));
		}
		break;
		case WM_MBUTTONUP:
		{
			wParam = VK_MBUTTON;
			OnMouseUp(VK_MBUTTON, LOWORD(lParam), HIWORD(lParam));
		}
		break;

		case WM_MOUSEWHEEL:
		{
			// wParam buttons should not be relied on
			OnMouseWheel(GET_KEYSTATE_WPARAM(wParam), LOWORD(lParam), HIWORD(lParam), GET_WHEEL_DELTA_WPARAM(wParam));
		}
		break;

		case WM_MOUSEMOVE:
		{
			// wParam buttons should not be relied on
			OnMouseMove(GET_KEYSTATE_WPARAM(wParam), LOWORD(lParam), HIWORD(lParam));
		}
		break;

		case WM_MOUSELEAVE:
		{

		}
		break;

		case WM_ACTIVATE:
		{
			if (!HIWORD(wParam))
			{
				active_ = true;
			}
			else
			{
				active_ = false;
			}
			break;
		}

		case WM_SIZE:
		{
			OnResize(LOWORD(lParam), HIWORD(lParam)); // LoWord = width, HiWord = height
		}
		break;


		case WM_ERASEBKGND:
		{
			return 1;
		}
		break;
		case WM_CLOSE:
		{
			PostQuitMessage(0);
		}
		break;

		default:
		{
		}
		}
		hWnd = hWnd; // meaningless, just to set breakpoint
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	WindowImpl(wstring title, Size<uint32, 2> size) :
		name_(move(title))
	{
		HINSTANCE hInstance = ::GetModuleHandle(nullptr);

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = &WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = name_.c_str();
		wcex.hIconSm = nullptr;

		::RegisterClassEx(&wcex);

		left_ = 200; // TODO
		top_ = 200;

		RECT windowRect;
		windowRect.left = left_;
		windowRect.top = top_;
		windowRect.right = left_ + size.X();
		windowRect.bottom = top_ + size.Y();

		DWORD style;
		if (fullScreen_)
		{
			style = WS_POPUPWINDOW | WS_OVERLAPPED;
		}
		else
		{
			style = WS_OVERLAPPEDWINDOW;
		}
		::AdjustWindowRect(&windowRect, style, false);
		windowLeft_ = windowRect.left;
		windowTop_ = windowRect.top;

		hWnd_ = ::CreateWindow(name_.c_str(), name_.c_str(), style,
			windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

		RECT clientRect;
		::GetClientRect(hWnd_, &clientRect);
		width_ = clientRect.right - clientRect.left;
		height_ = clientRect.bottom - clientRect.top;

		::SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		::SetWindowLongPtr(hWnd_, GWL_STYLE, style);

		::ShowWindow(hWnd_, SW_SHOWNORMAL);
		::UpdateWindow(hWnd_);

		if (fullScreen_)
		{
			left_ = 0;
			top_ = 0;
			windowLeft_ = 0;
			windowTop_ = 0;

			width_ = ::GetSystemMetrics(SM_CXSCREEN);
			height_ = ::GetSystemMetrics(SM_CYSCREEN);

		}

		windowRect.left = left_;
		windowRect.top = top_;
		windowRect.right = left_ + width_;
		windowRect.bottom = top_ + height_;
		::AdjustWindowRect(&windowRect, style, false);

		::SetWindowPos(hWnd_, nullptr, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			SWP_SHOWWINDOW | SWP_NOZORDER);

		::GetClientRect(hWnd_, &clientRect);
		width_ = clientRect.right - clientRect.left;
		height_ = clientRect.bottom - clientRect.top;

	}

	~WindowImpl()
	{
		if (hWnd_ != nullptr)
		{
			::SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
			if (fullScreen_)
			{
				::ChangeDisplaySettings(nullptr, 0);
				ShowCursor(TRUE);
			}

			::DestroyWindow(hWnd_);
			hWnd_ = nullptr;
		}
	}

	void DoRecreate()
	{
		HINSTANCE hInstance = ::GetModuleHandle(nullptr);

		DWORD style = static_cast<DWORD>(::GetWindowLongPtr(hWnd_, GWL_STYLE));
		RECT windowRect = { left_, top_, sint32(left_ + width_), sint32(top_ + height_) };
		::AdjustWindowRect(&windowRect, style, false);

		::DestroyWindow(hWnd_);

		hWnd_ = ::CreateWindow(name_.c_str(), name_.c_str(), style,
			windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, 0, 0, hInstance, nullptr);

		RECT clientRect;
		::GetClientRect(hWnd_, &clientRect);
		width_ = clientRect.right - clientRect.left;
		height_ = clientRect.bottom - clientRect.top;

		::SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		::SetWindowLongPtr(hWnd_, GWL_STYLE, style);

		::ShowWindow(hWnd_, SW_SHOWNORMAL);
		::UpdateWindow(hWnd_);

		::SetWindowPos(hWnd_, nullptr, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_SHOWWINDOW | SWP_NOZORDER);
	}



	void DoStartHandlingMessages()
	{
		MSG msg;
		BOOL hasMessage;
		// Main message loop:
		running_ = true;
		rendering_ = true;
		while (running_)
		{
			if (active_ && rendering_)
			{
				hasMessage = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
			}
			else
			{
				hasMessage = ::GetMessage(&msg, nullptr, 0, 0);
			}

			if (hasMessage)
			{
				if (msg.message == WM_QUIT)
				{
					running_ = false;
				}
				else
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			else
			{
				OnMessageIdle();
			}
		}

		messageIdle_ = nullptr;
		onResize_ = nullptr;
		inputHandler_ = nullptr;
	}

	void OnResize(uint32 width, uint32 height)
	{
		width_ = width;
		height_ = height;
		if (onResize_)
		{
			onResize_(width, height);
		}
	}

	void OnKeyDown(uint32 winKey)
	{
		winKey = DistinguishLeftRightShiftCtrlAlt(winKey, true);
		InputSemantic input = InputSemanticFromWindowsVK(winKey);
		if (inputHandler_)
		{
			inputHandler_->OnKeyDown(input);
		}
	}

	void OnKeyUp(uint32 winKey)
	{
		winKey = DistinguishLeftRightShiftCtrlAlt(winKey, false);
		InputSemantic input = InputSemanticFromWindowsVK(winKey);
		if (inputHandler_)
		{
			inputHandler_->OnKeyUp(input);
		}
	}

	void OnMouseDown(uint32 winKey, uint32 x, uint32 y)
	{
		InputSemantic input = InputSemanticFromWindowsVK(winKey);
		if (inputHandler_)
		{
			inputHandler_->OnMouseDown(input, x, height_ - y);
		}
	}

	void OnMouseUp(uint32 winKey, uint32 x, uint32 y)
	{
		InputSemantic input = InputSemanticFromWindowsVK(winKey);
		if (inputHandler_)
		{
			inputHandler_->OnMouseUp(input, x, height_ - y);
		}
	}

	void OnMouseWheel(uint32 winKey, uint32 x, uint32 y, sint32 wheelDelta)
	{
		if (inputHandler_)
		{
			inputHandler_->OnMouseWheel(InputSemantic::M_Wheel, x, height_ - y, wheelDelta);
		}
	}

	void OnMouseMove(uint32 winKey, uint32 x, uint32 y)
	{
		if (inputHandler_)
		{
			inputHandler_->OnMouseMove(InputSemantic::M_Move, x, height_ - y);
		}
	}




	uint32 DistinguishLeftRightShiftCtrlAlt(uint32 winKey, bool down)
	{
		if (down)
		{
			switch (winKey)
			{
			case VK_CONTROL:
			{
				if (::GetKeyState(VK_LCONTROL) < 0) // highest bit is 1
				{
					winKey = VK_LCONTROL;
					leftCtrl_ = true;
				}
				else if (::GetKeyState(VK_RCONTROL) < 0) // highest bit is 1
				{
					winKey = VK_RCONTROL;
					rightCtrl_ = true;
				}
			}
			break;
			case VK_SHIFT:
			{
				if (::GetKeyState(VK_LSHIFT) < 0) // highest bit is 1
				{
					winKey = VK_LSHIFT;
					leftShift_ = true;
				}
				else if (::GetKeyState(VK_RSHIFT) < 0) // highest bit is 1
				{
					winKey = VK_RSHIFT;
					rightShift_ = true;
				}
			}
			break;
			case VK_MENU:
			{
				if (::GetKeyState(VK_LMENU) < 0) // highest bit is 1
				{
					winKey = VK_LMENU;
					leftAlt_ = true;
				}
				else if (::GetKeyState(VK_RMENU) < 0) // highest bit is 1
				{
					winKey = VK_RMENU;
					rightAlt_ = true;
				}
			}
			break;
			default:
				break;
			}
		}
		else
		{
			switch (winKey)
			{
			case VK_CONTROL:
			{
				if (leftCtrl_ && !(::GetKeyState(VK_LCONTROL) < 0))
				{
					winKey = VK_LCONTROL;
					leftCtrl_ = false;
				}
				else if (rightCtrl_ && !(::GetKeyState(VK_RCONTROL) < 0))
				{
					winKey = VK_RCONTROL;
					rightCtrl_ = false;
				}
			}
			break;
			case VK_SHIFT:
			{
				if (leftShift_ && !(::GetKeyState(VK_LSHIFT) < 0))
				{
					winKey = VK_LSHIFT;
					leftShift_ = false;
				}
				else if (rightShift_ && !(::GetKeyState(VK_RSHIFT) < 0))
				{
					winKey = VK_RSHIFT;
					rightCtrl_ = false;
				}
			}
			break;
			case VK_MENU:
			{
				if (leftAlt_ && !(::GetKeyState(VK_LMENU) < 0))
				{
					winKey = VK_LMENU;
					leftAlt_ = false;
				}
				else if (rightAlt_ && !(::GetKeyState(VK_RMENU) < 0))
				{
					winKey = VK_RMENU;
					rightAlt_ = false;
				}
			}
			break;
			default:
				break;
			}
		}
		return winKey;
	}


	void OnMessageIdle()
	{
		if (messageIdle_ != nullptr)
		{
			messageIdle_();
		}
	}

	InputSemantic InputSemanticFromWindowsVK(uint32 winKey)
	{
		static vector<InputSemantic> mapping = []
		{
			uint32 const WindowsVKCount = 256;
			vector<InputSemantic> mapping(WindowsVKCount, InputSemantic::InputSemanticInvalid);

			mapping[0] = InputSemantic::NullSemantic;

			mapping[VK_LBUTTON] = InputSemantic::M_Button0;
			mapping[VK_RBUTTON] = InputSemantic::M_Button1;
			mapping[VK_CANCEL] = InputSemantic::InputSemanticInvalid;
			mapping[VK_MBUTTON] = InputSemantic::M_Button2;
			mapping[VK_XBUTTON1] = InputSemantic::M_Button3;
			mapping[VK_XBUTTON2] = InputSemantic::M_Button4;

			mapping[VK_BACK] = InputSemantic::K_BackSpace;
			mapping[VK_TAB] = InputSemantic::K_Tab;
			mapping[VK_CLEAR] = InputSemantic::InputSemanticInvalid;
			mapping[VK_RETURN] = InputSemantic::K_Enter;

			mapping[VK_SHIFT] = InputSemantic::Temp_Shift;
			mapping[VK_CONTROL] = InputSemantic::Temp_Ctrl;
			mapping[VK_MENU] = InputSemantic::Temp_Alt;
			mapping[VK_PAUSE] = InputSemantic::K_Pause;
			mapping[VK_CAPITAL] = InputSemantic::K_CapsLock;

			mapping[VK_KANA] = InputSemantic::InputSemanticInvalid;
			mapping[VK_HANGEUL] = InputSemantic::InputSemanticInvalid;
			mapping[VK_HANGUL] = InputSemantic::InputSemanticInvalid;

			mapping[VK_JUNJA] = InputSemantic::InputSemanticInvalid;
			mapping[VK_FINAL] = InputSemantic::InputSemanticInvalid;
			mapping[VK_HANJA] = InputSemantic::InputSemanticInvalid;
			mapping[VK_KANJI] = InputSemantic::InputSemanticInvalid;

			mapping[VK_ESCAPE] = InputSemantic::K_Escape;
			mapping[VK_CONVERT] = InputSemantic::InputSemanticInvalid;
			mapping[VK_NONCONVERT] = InputSemantic::InputSemanticInvalid;
			mapping[VK_ACCEPT] = InputSemantic::InputSemanticInvalid;
			mapping[VK_MODECHANGE] = InputSemantic::InputSemanticInvalid;

			mapping[VK_SPACE] = InputSemantic::K_Space;
			mapping[VK_PRIOR] = InputSemantic::K_PageUp;
			mapping[VK_NEXT] = InputSemantic::K_PageDown;
			mapping[VK_END] = InputSemantic::K_End;
			mapping[VK_HOME] = InputSemantic::K_Home;
			mapping[VK_LEFT] = InputSemantic::K_LeftArrow;
			mapping[VK_UP] = InputSemantic::K_UpArrow;
			mapping[VK_RIGHT] = InputSemantic::K_RightArrow;
			mapping[VK_DOWN] = InputSemantic::K_DownArrow;
			mapping[VK_SELECT] = InputSemantic::InputSemanticInvalid;
			mapping[VK_PRINT] = InputSemantic::InputSemanticInvalid;
			mapping[VK_EXECUTE] = InputSemantic::InputSemanticInvalid;
			mapping[VK_SNAPSHOT] = InputSemantic::InputSemanticInvalid;
			mapping[VK_INSERT] = InputSemantic::K_Insert;
			mapping[VK_DELETE] = InputSemantic::K_Delete;
			mapping[VK_HELP] = InputSemantic::InputSemanticInvalid;

			mapping['0'] = InputSemantic::K_0;
			mapping['1'] = InputSemantic::K_1;
			mapping['2'] = InputSemantic::K_2;
			mapping['3'] = InputSemantic::K_3;
			mapping['4'] = InputSemantic::K_4;
			mapping['5'] = InputSemantic::K_5;
			mapping['6'] = InputSemantic::K_6;
			mapping['7'] = InputSemantic::K_7;
			mapping['8'] = InputSemantic::K_8;
			mapping['9'] = InputSemantic::K_9;

			mapping['A'] = InputSemantic::K_A;
			mapping['B'] = InputSemantic::K_B;
			mapping['C'] = InputSemantic::K_C;
			mapping['D'] = InputSemantic::K_D;
			mapping['E'] = InputSemantic::K_E;
			mapping['F'] = InputSemantic::K_F;
			mapping['G'] = InputSemantic::K_G;

			mapping['H'] = InputSemantic::K_H;
			mapping['I'] = InputSemantic::K_I;
			mapping['J'] = InputSemantic::K_J;
			mapping['K'] = InputSemantic::K_K;
			mapping['L'] = InputSemantic::K_L;
			mapping['M'] = InputSemantic::K_M;
			mapping['N'] = InputSemantic::K_N;

			mapping['O'] = InputSemantic::K_O;
			mapping['P'] = InputSemantic::K_P;
			mapping['Q'] = InputSemantic::K_Q;
			mapping['R'] = InputSemantic::K_R;
			mapping['S'] = InputSemantic::K_S;
			mapping['T'] = InputSemantic::K_T;

			mapping['U'] = InputSemantic::K_U;
			mapping['V'] = InputSemantic::K_V;
			mapping['W'] = InputSemantic::K_W;
			mapping['X'] = InputSemantic::K_X;
			mapping['Y'] = InputSemantic::K_Y;
			mapping['Z'] = InputSemantic::K_Z;

			mapping[VK_LWIN] = InputSemantic::K_LeftWin;
			mapping[VK_RWIN] = InputSemantic::K_RightWin;
			mapping[VK_APPS] = InputSemantic::K_Apps;

			mapping[VK_SLEEP] = InputSemantic::K_Sleep;

			mapping[VK_NUMPAD0] = InputSemantic::K_NumPad0;
			mapping[VK_NUMPAD1] = InputSemantic::K_NumPad1;
			mapping[VK_NUMPAD2] = InputSemantic::K_NumPad2;
			mapping[VK_NUMPAD3] = InputSemantic::K_NumPad3;
			mapping[VK_NUMPAD4] = InputSemantic::K_NumPad4;
			mapping[VK_NUMPAD5] = InputSemantic::K_NumPad5;
			mapping[VK_NUMPAD6] = InputSemantic::K_NumPad6;
			mapping[VK_NUMPAD7] = InputSemantic::K_NumPad7;
			mapping[VK_NUMPAD8] = InputSemantic::K_NumPad8;
			mapping[VK_NUMPAD9] = InputSemantic::K_NumPad9;

			mapping[VK_MULTIPLY] = InputSemantic::K_NumPadAsterisk;
			mapping[VK_ADD] = InputSemantic::K_NumPadPlus;
			mapping[VK_SEPARATOR] = InputSemantic::InputSemanticInvalid;
			mapping[VK_SUBTRACT] = InputSemantic::K_NumPadMinus;
			mapping[VK_DECIMAL] = InputSemantic::K_NumPadPeriod;
			mapping[VK_DIVIDE] = InputSemantic::K_NumPadSlash;

			mapping[VK_F1] = InputSemantic::K_F1;
			mapping[VK_F2] = InputSemantic::K_F2;
			mapping[VK_F3] = InputSemantic::K_F3;
			mapping[VK_F4] = InputSemantic::K_F4;
			mapping[VK_F5] = InputSemantic::K_F5;
			mapping[VK_F6] = InputSemantic::K_F6;
			mapping[VK_F7] = InputSemantic::K_F7;
			mapping[VK_F8] = InputSemantic::K_F8;
			mapping[VK_F9] = InputSemantic::K_F9;
			mapping[VK_F10] = InputSemantic::K_F10;
			mapping[VK_F11] = InputSemantic::K_F11;
			mapping[VK_F12] = InputSemantic::K_F12;
			mapping[VK_F13] = InputSemantic::K_F13;
			mapping[VK_F14] = InputSemantic::K_F14;
			mapping[VK_F15] = InputSemantic::K_F15;
			mapping[VK_F16] = InputSemantic::InputSemanticInvalid;
			mapping[VK_F17] = InputSemantic::InputSemanticInvalid;
			mapping[VK_F18] = InputSemantic::InputSemanticInvalid;
			mapping[VK_F19] = InputSemantic::InputSemanticInvalid;
			mapping[VK_F20] = InputSemantic::InputSemanticInvalid;
			mapping[VK_F21] = InputSemantic::InputSemanticInvalid;
			mapping[VK_F22] = InputSemantic::InputSemanticInvalid;
			mapping[VK_F23] = InputSemantic::InputSemanticInvalid;
			mapping[VK_F24] = InputSemantic::InputSemanticInvalid;

			mapping[VK_NUMLOCK] = InputSemantic::K_NumLock;
			mapping[VK_SCROLL] = InputSemantic::K_ScrollLock;

			mapping[VK_OEM_NEC_EQUAL] = InputSemantic::K_NumPadEquals;


			mapping[VK_LSHIFT] = InputSemantic::K_LeftShift;
			mapping[VK_RSHIFT] = InputSemantic::K_RightShift;
			mapping[VK_LCONTROL] = InputSemantic::K_LeftCtrl;
			mapping[VK_RCONTROL] = InputSemantic::K_RightCtrl;
			mapping[VK_LMENU] = InputSemantic::K_LeftAlt;
			mapping[VK_RMENU] = InputSemantic::K_RightAlt;

			mapping[VK_BROWSER_BACK] = InputSemantic::K_WebBack;
			mapping[VK_BROWSER_FORWARD] = InputSemantic::K_WebForward;
			mapping[VK_BROWSER_REFRESH] = InputSemantic::K_WebRefresh;
			mapping[VK_BROWSER_STOP] = InputSemantic::K_WebStop;
			mapping[VK_BROWSER_SEARCH] = InputSemantic::K_WebSearch;
			mapping[VK_BROWSER_FAVORITES] = InputSemantic::K_WebFavorites;
			mapping[VK_BROWSER_HOME] = InputSemantic::K_WebHome;

			mapping[VK_VOLUME_MUTE] = InputSemantic::K_Mute;
			mapping[VK_VOLUME_DOWN] = InputSemantic::K_VolumeUp;
			mapping[VK_VOLUME_UP] = InputSemantic::K_VolumeDown;
			mapping[VK_MEDIA_NEXT_TRACK] = InputSemantic::K_NextTrack;
			mapping[VK_MEDIA_PREV_TRACK] = InputSemantic::K_PrevTrack;
			mapping[VK_MEDIA_STOP] = InputSemantic::K_MediaStop;
			mapping[VK_MEDIA_PLAY_PAUSE] = InputSemantic::K_PlayPause;
			mapping[VK_LAUNCH_MAIL] = InputSemantic::K_Mail;
			mapping[VK_LAUNCH_MEDIA_SELECT] = InputSemantic::K_MediaSelect;
			mapping[VK_LAUNCH_APP1] = InputSemantic::InputSemanticInvalid;
			mapping[VK_LAUNCH_APP2] = InputSemantic::InputSemanticInvalid;

			mapping[VK_OEM_1] = InputSemantic::K_Semicolon;
			mapping[VK_OEM_PLUS] = InputSemantic::InputSemanticInvalid;
			mapping[VK_OEM_COMMA] = InputSemantic::K_Comma;
			mapping[VK_OEM_MINUS] = InputSemantic::K_Minus;
			mapping[VK_OEM_PERIOD] = InputSemantic::K_Period;
			mapping[VK_OEM_2] = InputSemantic::K_Slash;
			mapping[VK_OEM_3] = InputSemantic::K_Tilde;

			mapping[VK_OEM_4] = InputSemantic::K_LeftBracket;
			mapping[VK_OEM_5] = InputSemantic::K_BackSlash;
			mapping[VK_OEM_6] = InputSemantic::K_RightBracket;
			mapping[VK_OEM_7] = InputSemantic::K_Quote;
			mapping[VK_OEM_8] = InputSemantic::InputSemanticInvalid;

			return mapping;
		} ();

		return mapping[winKey];
	}




	HWND hWnd_ = nullptr;
	std::function<void(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)> messageHook_;

	bool leftShift_ = false;
	bool rightShift_ = false;
	bool leftCtrl_ = false;
	bool rightCtrl_ = false;
	bool leftAlt_ = false;
	bool rightAlt_ = false;

	bool active_ = false;
	bool running_ = false;
	bool rendering_ = false;

	bool fullScreen_ = false;

	uint32 height_;
	uint32 width_;

	sint32 left_;
	sint32 top_;

	sint32 windowLeft_;
	sint32 windowTop_;

	wstring name_;

	function<void()> messageIdle_;

	function<void(uint32 width, uint32 height)> onResize_;

	Ptr<InputHandler> inputHandler_;
};







Ptr<Window> Window::Create(wstring title, Size2UI size)
{
	return CreatePtr<WindowImpl>(move(title), size);
}

void Window::SetRawWindowsMessageHook(void const* hook)
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->messageHook_ = *static_cast<std::function<void(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)> const*>(hook);
}


Window::~Window()
{
}


void* Window::GetHWND() const
{
	auto thiz = static_cast<WindowImpl const*>(this);
	return thiz->hWnd_;
}

void Window::Recreate()
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->DoRecreate();
}

void Window::SetMessageIdle(function<void()> messageIdle)
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->messageIdle_ = move(messageIdle);
}

void Window::SetResizeHandler(function<void(uint32 width, uint32 height)> onResize)
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->onResize_ = move(onResize);
}

void Window::SetInputHandler(Ptr<InputHandler> inputHanlder)
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->inputHandler_ = move(inputHanlder);
}


void Window::StartHandlingMessages()
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->DoStartHandlingMessages();
}

bool Window::IsRendering() const
{
	auto thiz = static_cast<WindowImpl const*>(this);
	return thiz->rendering_;
}

void Window::SetRendering(bool rendering)
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->rendering_ = rendering;
}

bool Window::IsRunning() const
{
	auto thiz = static_cast<WindowImpl const*>(this);
	return thiz->running_;
}

void Window::SetRunning(bool running)
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->running_ = running;
}

bool Window::IsActive() const
{
	auto thiz = static_cast<WindowImpl const*>(this);
	return thiz->active_;
}

void Window::SetActive(bool active)
{
	auto thiz = static_cast<WindowImpl*>(this);
	thiz->active_ = active;
}






/*
*	Client region size, not the window size.
*/

Size2UI Window::GetClientRegionSize() const
{
	auto thiz = static_cast<WindowImpl const*>(this);
	return Size2UI(thiz->width_, thiz->height_);
}


/*
*	Upper left corner of the window.
*/

Size2SI Window::GetWindowPosition() const
{
	auto thiz = static_cast<WindowImpl const*>(this);
	return Size2SI(thiz->windowLeft_, thiz->windowTop_);
}

std::wstring Window::GetTitleText() const
{
	auto thiz = static_cast<WindowImpl const*>(this);
	sint32 length = ::GetWindowTextLength(thiz->hWnd_);
	std::wstring title;
	title.resize(length + 1);
	::GetWindowText(thiz->hWnd_, &title[0], sint32(title.size()));
	return title;
}

void Window::SetTitleText(std::wstring const& text)
{
	auto thiz = static_cast<WindowImpl const*>(this);
	::SetWindowText(thiz->hWnd_, text.c_str());
}

