#pragma once
#include <wrl\client.h>

namespace X
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}
