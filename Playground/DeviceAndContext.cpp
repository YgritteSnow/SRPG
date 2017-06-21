#include "DeviceAndContext.h"
#include "Utility.h"
#include "D3DHelper.h"
#include "Window.h"
#include <cmath>

using namespace Microsoft::WRL;
using namespace std;
using namespace X;


DeviceAndContext::DeviceAndContext(Ptr<Window> window) :
	_screenViewport(),
	_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	_window(move(window))
{
	_d3dRenderTargetSize = _window->GetClientRegionSize();
	CreateDeviceResources();
	CreateWindowSizeDependentResources();
}

DeviceAndContext::~DeviceAndContext()
{
}


// Configures the Direct3D device, and stores handles to it and the device context.
void DeviceAndContext::CreateDeviceResources()
{
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the Direct3D 11 API device object and a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	ThrowIfFailed(D3D11CreateDevice(
		nullptr,					// Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				// Set debug and Direct2D compatibility flags.
		featureLevels,				// List of feature levels this app can support.
		ArraySize(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,					// Returns the Direct3D device created.
		&_d3dFeatureLevel,			// Returns feature level of device created.
		&context					// Returns the device immediate context.
	));


	// Store pointers to the Direct3D 11.3 API device and immediate context.
	ThrowIfFailed(device.As(&_d3dDevice));

	ThrowIfFailed(context.As(&_d3dContext));
}

// These resources need to be recreated every time the window size is changed.
void DeviceAndContext::CreateWindowSizeDependentResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	constexpr uint32_t s = ArraySize(nullViews);
	_d3dContext->OMSetRenderTargets(ArraySize(nullViews), nullViews, nullptr);
	_d3dRenderTargetView = nullptr;
	_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	UpdateRenderTargetSize();


	if (_swapChain != nullptr)
	{
		// If the swap chain already exists, resize it.
		HRESULT hr = _swapChain->ResizeBuffers(2, // Double-buffered swap chain.
			_d3dRenderTargetSize.X(), _d3dRenderTargetSize.Y(), DXGI_FORMAT_B8G8R8A8_UNORM, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			HandleDeviceLost();

			// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else
		{
			ThrowIfFailed(hr);
		}
	}
	else
	{
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.
		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };

		swapChainDesc.BufferDesc.Width = _d3dRenderTargetSize.X();
		swapChainDesc.BufferDesc.Height = _d3dRenderTargetSize.Y();
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;								// Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;									// Use double-buffering to minimize latency.
		swapChainDesc.OutputWindow = reinterpret_cast<HWND>(_window->GetHWND());
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// All Windows Store apps must use _FLIP_ SwapEffect.
		swapChainDesc.Flags = 0;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		ComPtr<IDXGIDevice3> dxgiDevice;
		ThrowIfFailed(_d3dDevice.As(&dxgiDevice));

		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

		ComPtr<IDXGIFactory4> dxgiFactory;
		ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));


		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = { 0 };

		ComPtr<IDXGISwapChain> swapChain;
		ThrowIfFailed(dxgiFactory->CreateSwapChain(_d3dDevice.Get(), &swapChainDesc, &swapChain));
		ThrowIfFailed(swapChain.As(&_swapChain));

		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each VSync, minimizing power consumption.
		ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
	}

	// Create a render target view of the swap chain back buffer.
	ComPtr<ID3D11Texture2D1> backBuffer;
	ThrowIfFailed(_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

	ThrowIfFailed(_d3dDevice->CreateRenderTargetView1(backBuffer.Get(), nullptr, &_d3dRenderTargetView));

	// Set the 3D rendering viewport to target the entire window.
	_screenViewport = CD3D11_VIEWPORT(0.0f, 0.0f, float32(_d3dRenderTargetSize.X()), float32(_d3dRenderTargetSize.Y()));

	_d3dContext->RSSetViewports(1, &_screenViewport);
}

// Determine the dimensions of the render target and whether it will be scaled down.
void DeviceAndContext::UpdateRenderTargetSize()
{
}


// This method is called in the event handler for the DisplayContentsInvalidated event.
void DeviceAndContext::ValidateDevice()
{
	// The D3D Device is no longer valid if the default adapter changed since the device
	// was created or if the device has been removed.

	// First, get the information for the default adapter from when the device was created.

	ComPtr<IDXGIDevice3> dxgiDevice;
	ThrowIfFailed(_d3dDevice.As(&dxgiDevice));

	ComPtr<IDXGIAdapter> deviceAdapter;
	ThrowIfFailed(dxgiDevice->GetAdapter(&deviceAdapter));

	ComPtr<IDXGIFactory4> deviceFactory;
	ThrowIfFailed(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));

	ComPtr<IDXGIAdapter1> previousDefaultAdapter;
	ThrowIfFailed(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));

	DXGI_ADAPTER_DESC1 previousDesc;
	ThrowIfFailed(previousDefaultAdapter->GetDesc1(&previousDesc));

	// Next, get the information for the current default adapter.

	ComPtr<IDXGIFactory4> currentFactory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

	ComPtr<IDXGIAdapter1> currentDefaultAdapter;
	ThrowIfFailed(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));

	DXGI_ADAPTER_DESC1 currentDesc;
	ThrowIfFailed(currentDefaultAdapter->GetDesc1(&currentDesc));

	// If the adapter LUIDs don't match, or if the device reports that it has been removed,
	// a new D3D device must be created.

	if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
		previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
		FAILED(_d3dDevice->GetDeviceRemovedReason()))
	{
		// Release references to resources related to the old device.
		dxgiDevice = nullptr;
		deviceAdapter = nullptr;
		deviceFactory = nullptr;
		previousDefaultAdapter = nullptr;

		// Create a new device and swap chain.
		HandleDeviceLost();
	}
}

// Recreate all device resources and set them back to the current state.
void DeviceAndContext::HandleDeviceLost()
{
	_swapChain = nullptr;

// 	if (m_deviceNotify != nullptr)
// 	{
// 		m_deviceNotify->OnDeviceLost();
// 	}

	CreateDeviceResources();
	CreateWindowSizeDependentResources();

// 	if (m_deviceNotify != nullptr)
// 	{
// 		m_deviceNotify->OnDeviceRestored();
// 	}
}


// Present the contents of the swap chain to the screen.
void DeviceAndContext::Present()
{
	// The first argument (value : 1) instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	DXGI_PRESENT_PARAMETERS parameters = { 0 };
	HRESULT hr = _swapChain->Present1(0, 0, &parameters);


	// Discard the contents of the render target.
	// This is a valid operation only when the existing contents will be entirely
	// overwritten. If dirty or scroll rects are used, this call should be removed.
	_d3dContext->DiscardView1(_d3dRenderTargetView.Get(), nullptr, 0);

	// If the device was removed either by a disconnection or a driver upgrade, we 
	// must recreate all device resources.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost();
	}
	else
	{
		ThrowIfFailed(hr);
	}
}
