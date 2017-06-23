#include "DeviceAndContext.h"
#include "Utility.h"
#include "D3DHelper.h"
#include "Window.h"
#include <cmath>

using namespace Microsoft::WRL;
using namespace std;
using namespace X;


// Constructor for DeviceResources.
DeviceResources::DeviceResources() :
	m_screenViewport(),
	m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_d3dRenderTargetSize(0, 0),
	m_outputSize(0, 0),
	m_logicalSize(0, 0)
{
	CreateDeviceResources();
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DeviceResources::CreateDeviceResources()
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
		&m_d3dFeatureLevel,			// Returns feature level of device created.
		&context					// Returns the device immediate context.
	));


	// Store pointers to the Direct3D 11.3 API device and immediate context.
	ThrowIfFailed(device.As(&m_d3dDevice));

	ThrowIfFailed(context.As(&m_d3dContext));
}

// These resources need to be recreated every time the window size is changed.
void DeviceResources::CreateWindowSizeDependentResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	constexpr uint32_t s = ArraySize(nullViews);
	m_d3dContext->OMSetRenderTargets(ArraySize(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView = nullptr;
	m_d3dDepthStencilView = nullptr;
	m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	UpdateRenderTargetSize();


	if (m_swapChain != nullptr)
	{
		// If the swap chain already exists, resize it.
		HRESULT hr = m_swapChain->ResizeBuffers(2, // Double-buffered swap chain.
			m_d3dRenderTargetSize.X(), m_d3dRenderTargetSize.Y(), DXGI_FORMAT_B8G8R8A8_UNORM, 0);

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
// 		UINT Width;
// 		UINT Height;
// 		DXGI_RATIONAL RefreshRate;
// 		DXGI_FORMAT Format;
// 		DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
// 		DXGI_MODE_SCALING Scaling;

		swapChainDesc.BufferDesc.Width = m_d3dRenderTargetSize.X();
		swapChainDesc.BufferDesc.Height = m_d3dRenderTargetSize.Y();
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;								// Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;									// Use double-buffering to minimize latency.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Windows Store apps must use this SwapEffect.
		swapChainDesc.Flags = 0;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		ComPtr<IDXGIDevice3> dxgiDevice;
		ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

		ComPtr<IDXGIFactory4> dxgiFactory;
		ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));


		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = { 0 };

		ComPtr<IDXGISwapChain> swapChain;
		ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, &swapChain));
		ThrowIfFailed(swapChain.As(&m_swapChain));

		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each VSync, minimizing power consumption.
		ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
	}

	// Create a render target view of the swap chain back buffer.
	ComPtr<ID3D11Texture2D1> backBuffer;
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

	ThrowIfFailed(m_d3dDevice->CreateRenderTargetView1(backBuffer.Get(), nullptr, &m_d3dRenderTargetView));

	// Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		m_d3dRenderTargetSize.X(),
		m_d3dRenderTargetSize.Y(),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	ComPtr<ID3D11Texture2D1> depthStencil;
	ThrowIfFailed(
		m_d3dDevice->CreateTexture2D1(
			&depthStencilDesc,
			nullptr,
			&depthStencil
		)
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	ThrowIfFailed(
		m_d3dDevice->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			&m_d3dDepthStencilView
		)
	);

	// Set the 3D rendering viewport to target the entire window.
	m_screenViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		m_d3dRenderTargetSize.X(),
		m_d3dRenderTargetSize.Y()
	);

	m_d3dContext->RSSetViewports(1, &m_screenViewport);

	// Create a Direct2D target bitmap associated with the
	// swap chain back buffer and set it as the current target.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			m_dpi,
			m_dpi
		);

	ComPtr<IDXGISurface2> dxgiBackBuffer;
	ThrowIfFailed(
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
	);

	ThrowIfFailed(
		m_d2dContext->CreateBitmapFromDxgiSurface(
			dxgiBackBuffer.Get(),
			&bitmapProperties,
			&m_d2dTargetBitmap
		)
	);

	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
	m_d2dContext->SetDpi(m_effectiveDpi, m_effectiveDpi);

	// Grayscale text anti-aliasing is recommended for all Windows Store apps.
	m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

// Determine the dimensions of the render target and whether it will be scaled down.
void DeviceResources::UpdateRenderTargetSize()
{
	m_effectiveDpi = m_dpi;

	// To improve battery life on high resolution devices, render to a smaller render target
	// and allow the GPU to scale the output when it is presented.
	if (!DisplayMetrics::SupportHighResolutions && m_dpi > DisplayMetrics::DpiThreshold)
	{
		float width = ConvertDipsToPixels(m_logicalSize.Width, m_dpi);
		float height = ConvertDipsToPixels(m_logicalSize.Height, m_dpi);

		// When the device is in portrait orientation, height > width. Compare the
		// larger dimension against the width threshold and the smaller dimension
		// against the height threshold.
		if (max(width, height) > DisplayMetrics::WidthThreshold && min(width, height) > DisplayMetrics::HeightThreshold)
		{
			// To scale the app we change the effective DPI. Logical size does not change.
			m_effectiveDpi /= 2.0f;
		}
	}

	// Calculate the necessary render target size in pixels.
	m_outputSize.Width = ConvertDipsToPixels(m_logicalSize.Width, m_effectiveDpi);
	m_outputSize.Height = ConvertDipsToPixels(m_logicalSize.Height, m_effectiveDpi);

	// Prevent zero size DirectX content from being created.
	m_outputSize.Width = max(m_outputSize.Width, 1);
	m_outputSize.Height = max(m_outputSize.Height, 1);
}

// This method is called when the CoreWindow is created (or re-created).
void DeviceResources::SetWindow(HWND window)
{
	m_window = window;
	m_logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);

	CreateWindowSizeDependentResources();
}

// This method is called in the event handler for the SizeChanged event.
void DeviceResources::SetLogicalSize(Windows::Foundation::Size logicalSize)
{
	if (m_logicalSize != logicalSize)
	{
		m_logicalSize = logicalSize;
		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the DpiChanged event.
void DeviceResources::SetDpi(float dpi)
{
	if (dpi != m_dpi)
	{
		m_dpi = dpi;

		// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
		m_logicalSize = Windows::Foundation::Size(m_window->Bounds.Width, m_window->Bounds.Height);

		m_d2dContext->SetDpi(m_dpi, m_dpi);
		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the OrientationChanged event.
void DeviceResources::SetCurrentOrientation(DisplayOrientations currentOrientation)
{
	if (m_currentOrientation != currentOrientation)
	{
		m_currentOrientation = currentOrientation;
		CreateWindowSizeDependentResources();
	}
}

// This method is called in the event handler for the DisplayContentsInvalidated event.
void DeviceResources::ValidateDevice()
{
	// The D3D Device is no longer valid if the default adapter changed since the device
	// was created or if the device has been removed.

	// First, get the information for the default adapter from when the device was created.

	ComPtr<IDXGIDevice3> dxgiDevice;
	ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

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
		FAILED(m_d3dDevice->GetDeviceRemovedReason()))
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
void DeviceResources::HandleDeviceLost()
{
	m_swapChain = nullptr;

	if (m_deviceNotify != nullptr)
	{
		m_deviceNotify->OnDeviceLost();
	}

	CreateDeviceResources();
	m_d2dContext->SetDpi(m_dpi, m_dpi);
	CreateWindowSizeDependentResources();

	if (m_deviceNotify != nullptr)
	{
		m_deviceNotify->OnDeviceRestored();
	}
}

// Register our DeviceNotify to be informed on device lost and creation.
void DeviceResources::RegisterDeviceNotify(IDeviceNotify* deviceNotify)
{
	m_deviceNotify = deviceNotify;
}

// Call this method when the app suspends. It provides a hint to the driver that the app 
// is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
void DeviceResources::Trim()
{
	ComPtr<IDXGIDevice3> dxgiDevice;
	m_d3dDevice.As(&dxgiDevice);

	dxgiDevice->Trim();
}

// Present the contents of the swap chain to the screen.
void DeviceResources::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	DXGI_PRESENT_PARAMETERS parameters = { 0 };
	HRESULT hr = m_swapChain->Present1(1, 0, &parameters);

	// Discard the contents of the render target.
	// This is a valid operation only when the existing contents will be entirely
	// overwritten. If dirty or scroll rects are used, this call should be removed.
	m_d3dContext->DiscardView1(m_d3dRenderTargetView.Get(), nullptr, 0);

	// Discard the contents of the depth stencil.
	m_d3dContext->DiscardView1(m_d3dDepthStencilView.Get(), nullptr, 0);

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

// This method determines the rotation between the display device's native orientation and the
// current display orientation.
DXGI_MODE_ROTATION DeviceResources::ComputeDisplayRotation()
{
	DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

	// Note: NativeOrientation can only be Landscape or Portrait even though
	// the DisplayOrientations enum has other values.
	switch (m_nativeOrientation)
	{
	case DisplayOrientations::Landscape:
		switch (m_currentOrientation)
		{
		case DisplayOrientations::Landscape:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case DisplayOrientations::Portrait:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case DisplayOrientations::LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;

		case DisplayOrientations::PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;
		}
		break;

	case DisplayOrientations::Portrait:
		switch (m_currentOrientation)
		{
		case DisplayOrientations::Landscape:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;

		case DisplayOrientations::Portrait:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case DisplayOrientations::LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case DisplayOrientations::PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;
		}
		break;
	}
	return rotation;
}