#pragma once
#include <cstdint>

template <class T, uint32_t N>
constexpr uint32_t ArraySize(T(&)[N]) noexcept
{
	return N;
}
