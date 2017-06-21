#include "ComPtr.h"
#include "Window.h"
#include "DeviceAndContext.h"

#include <iostream>

int main()
{
	using namespace X;
	using namespace std;

	struct Counter : ReferenceCountBase<>
	{
		~Counter()
		{

		}
	};

	{
		auto counter = CreatePtr<Counter>();
		int i = 0;
		i = 3;
	}

	Ptr<Window> window = Window::Create(L"hello", { 1280,800 });
	Ptr<DeviceAndContext> deviceAndContext = CreatePtr<DeviceAndContext>(window);
	window->SetMessageIdle([deviceAndContext] { deviceAndContext->Present(); cout << "hello" << endl; });
	window->StartHandlingMessages();

}
