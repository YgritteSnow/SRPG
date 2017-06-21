#include "D3DHelper.h"
#include "Utility.h"
#include <string>
#include <comdef.h>

using namespace std;

wstring TextFromHResult(HRESULT hr)
{
	_com_error error(hr);
	wchar_t msg[1024];
	swprintf_s(msg, L"Error 0x%08x: %s", hr, error.ErrorMessage());
	return msg;
}