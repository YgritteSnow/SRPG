#pragma once

enum class InputSemantic
{
	NullSemantic = 0x00,

	K_Escape = 0x01,
	K_Tilde = 0x02,		// ` on main keyboard
	K_1 = 0x03,
	K_2 = 0x04,
	K_3 = 0x05,
	K_4 = 0x06,
	K_5 = 0x07,
	K_6 = 0x08,
	K_7 = 0x09,
	K_8 = 0x0A,
	K_9 = 0x0B,
	K_0 = 0x0C,
	K_Minus = 0x0D,		// - on main keyboard
	K_Equals = 0x0E,
	K_BackSpace = 0x0F,		// backspace
	K_Tab = 0x10,
	K_Q = 0x11,
	K_W = 0x12,
	K_E = 0x13,
	K_R = 0x14,
	K_T = 0x15,
	K_Y = 0x16,
	K_U = 0x17,
	K_I = 0x18,
	K_O = 0x19,
	K_P = 0x1A,
	K_LeftBracket = 0x1B,		// [ on main keyboard
	K_RightBracket = 0x1C,		// ] on main keybaord
	K_Enter = 0x1D,		// Enter on main keyboard
	K_LeftCtrl = 0x1E,
	K_A = 0x1F,
	K_S = 0x20,
	K_D = 0x21,
	K_F = 0x22,
	K_G = 0x23,
	K_H = 0x24,
	K_J = 0x25,
	K_K = 0x26,
	K_L = 0x27,
	K_Semicolon = 0x28,		// ; on main keyboard
	K_Quote = 0x29,		// ' on main keyboard
	K_Caret = 0x2A,		// ^
	K_LeftShift = 0x2B,
	K_BackSlash = 0x2C,		// \ on main keyboard
	K_Z = 0x2D,
	K_X = 0x2E,
	K_C = 0x2F,
	K_V = 0x30,
	K_B = 0x31,
	K_N = 0x32,
	K_M = 0x33,
	K_Comma = 0x34,		// , on main keyboard
	K_Period = 0x35,		// . on main keyboard
	K_Slash = 0x36,		// / on main keyboard
	K_RightShift = 0x37,
	K_NumPadAsterisk = 0x38,		// * on numeric keypad
	K_LeftAlt = 0x39,
	K_Space = 0x3A,
	K_CapsLock = 0x3B,
	K_F1 = 0x3C,
	K_F2 = 0x3D,
	K_F3 = 0x3E,
	K_F4 = 0x3F,
	K_F5 = 0x40,
	K_F6 = 0x41,
	K_F7 = 0x42,
	K_F8 = 0x43,
	K_F9 = 0x44,
	K_F10 = 0x45,
	K_NumLock = 0x46,
	K_ScrollLock = 0x47,
	K_NumPad7 = 0x48,
	K_NumPad8 = 0x49,
	K_NumPad9 = 0x4A,
	K_NumPadMinus = 0x4B,		// - on numeric keypad
	K_NumPad4 = 0x4C,
	K_NumPad5 = 0x4D,
	K_NumPad6 = 0x4E,
	K_NumPadPlus = 0x4F,		// + on numeric keypad
	K_NumPad1 = 0x50,
	K_NumPad2 = 0x51,
	K_NumPad3 = 0x52,
	K_NumPad0 = 0x53,
	K_NumPadPeriod = 0x54,		// . on numeric keypad

	K_OEM_102 = 0x56,		// <> or \| on RT 102-key keyboard (Non-U.S.)
	K_F11 = 0x57,
	K_F12 = 0x58,

	K_F13 = 0x64,		//                     (NEC PC98)
	K_F14 = 0x65,		//                     (NEC PC98)
	K_F15 = 0x66,		//                     (NEC PC98)

	K_Kana = 0x70,		// (Japanese keyboard)
	K_ABNT_C1 = 0x73,		// /? on Brazilian keyboard
	K_Convert = 0x79,		// (Japanese keyboard)
	K_NoConvert = 0x7B,		// (Japanese keyboard)
	K_Yen = 0x7D,		// (Japanese keyboard)
	K_ABNT_C2 = 0x7E,		// Numpad . on Brazilian keyboard
	K_NumPadEquals = 0x8D,		// = on numeric keypad (NEC PC98)
	K_PrevTrack = 0x90,		// Previous Track (DK_CIRCUMFLEX on Japanese keyboard)
	K_AT = 0x91,		//                     (NEC PC98)
	K_Colon = 0x92,		//                     (NEC PC98)
	K_Underline = 0x93,		//                     (NEC PC98)
	K_Kanji = 0x94,		// (Japanese keyboard)
	K_Stop = 0x95,		//                     (NEC PC98)
	K_AX = 0x96,		//                     (Japan AX)
	K_Unlabeled = 0x97,		//                        (J3100)
	K_NextTrack = 0x99,		// Next Track
	K_NumPadEnter = 0x9C,		// Enter on numeric keypad
	K_RightCtrl = 0x9D,
	K_Mute = 0xA0,		// Mute
	K_Calculator = 0xA1,		// Calculator
	K_PlayPause = 0xA2,		// Play / Pause
	K_MediaStop = 0xA4,		// Media Stop
	K_VolumeDown = 0xAE,		// Volume -
	K_VolumeUp = 0xB0,		// Volume +
	K_WebHome = 0xB2,		// Web home
	K_NumPadComma = 0xB3,		// , on numeric keypad (NEC PC98)
	K_NumPadSlash = 0xB5,		// / on numeric keypad
	K_SysRQ = 0xB7,
	K_RightAlt = 0xB8,		// right Alt
	K_Pause = 0xC5,		// Pause
	K_Home = 0xC7,		// Home on arrow keypad
	K_UpArrow = 0xC8,		// UpArrow on arrow keypad
	K_PageUp = 0xC9,		// PgUp on arrow keypad
	K_LeftArrow = 0xCB,		// LeftArrow on arrow keypad
	K_RightArrow = 0xCD,		// RightArrow on arrow keypad
	K_End = 0xCF,		// End on arrow keypad
	K_DownArrow = 0xD0,		// DownArrow on arrow keypad
	K_PageDown = 0xD1,		// PgDn on arrow keypad
	K_Insert = 0xD2,		// Insert on arrow keypad
	K_Delete = 0xD3,		// Delete on arrow keypad
	K_LeftWin = 0xDB,		// Left Windows key
	K_RightWin = 0xDC,		// Right Windows key
	K_Apps = 0xDD,		// AppMenu key
	K_Power = 0xDE,		// System Power
	K_Sleep = 0xDF,		// System Sleep
	K_Wake = 0xE3,		// System Wake
	K_WebSearch = 0xE5,		// Web Search
	K_WebFavorites = 0xE6,		// Web Favorites
	K_WebRefresh = 0xE7,		// Web Refresh
	K_WebStop = 0xE8,		// Web Stop
	K_WebForward = 0xE9,		// Web Forward
	K_WebBack = 0xEA,		// Web Back
	K_MyComputer = 0xEB,		// My Computer
	K_Mail = 0xEC,		// Mail
	K_MediaSelect = 0xED,		// Media Select



	M_Button0 = 0xF0,
	M_Button1 = 0xF1,
	M_Button2 = 0xF2,
	M_Button3 = 0xF3,
	M_Button4 = 0xF4,
	M_Button5 = 0xF5,
	M_Button6 = 0xF6,
	M_Button7 = 0xF7,
	M_Move = 0xFA,
	M_Wheel = 0xFB,

	Temp_Shift = 0x100,
	Temp_Ctrl = 0x101,
	Temp_Alt = 0x102,

	InputSemanticInvalid
};

class InputHandler
{
	virtual ~InputHandler() = 0
	{}

	virtual void OnKeyDown(InputSemantic key) = 0;

	virtual void OnKeyUp(InputSemantic key) = 0;

	virtual void OnMouseDown(InputSemantic key, uint32 x, uint32 y) = 0;

	virtual void OnMouseUp(InputSemantic key, uint32 x, uint32 y) = 0;

	virtual void OnMouseWheel(InputSemantic key, uint32 x, uint32 y, sint32 wheelDelta) = 0;

	virtual void OnMouseMove(InputSemantic key, uint32 x, uint32 y) = 0;
};