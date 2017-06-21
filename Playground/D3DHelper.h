#pragma once
#include <winerror.h>
#include <exception>
#include <string>

std::wstring TextFromHResult(HRESULT hr);

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		std::string hrValue = std::to_string(hr);
		std::wstring hrMessage = TextFromHResult(hr);
		// Set a breakpoint on this line to catch Win32 API errors.
		throw std::exception(hrValue.c_str());
	}
}