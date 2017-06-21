#pragma once
#include "ReferenceCount.h"
#include "GeometryFoundation.h"
#include "ComPtr.h"
#include <d3d11_4.h>
#include <dxgi1_5.h>


namespace X
{
	class Window;

	class DeviceAndContext : public ReferenceCountBase<true>
	{
	public:
		DeviceAndContext(Ptr<Window> window);
		~DeviceAndContext();
		void ValidateDevice();
		void HandleDeviceLost();

		void Present();


		ID3D11Device3* GetD3DDevice() const
		{
			return _d3dDevice.Get();
		}
		ID3D11DeviceContext3* GetD3DDeviceContext() const
		{
			return _d3dContext.Get();
		}
		IDXGISwapChain3* GetSwapChain() const
		{
			return _swapChain.Get();
		}
		D3D_FEATURE_LEVEL GetDeviceFeatureLevel() const
		{
			return _d3dFeatureLevel;
		}
		ID3D11RenderTargetView1* GetBackBufferRenderTargetView() const
		{
			return _d3dRenderTargetView.Get();
		}

		D3D11_VIEWPORT GetScreenViewport() const
		{
			return _screenViewport;
		}

	private:
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void UpdateRenderTargetSize();

		Ptr<Window> _window;

		// Direct3D objects.
		ComPtr<ID3D11Device3>			_d3dDevice;
		ComPtr<ID3D11DeviceContext3>	_d3dContext;
		ComPtr<IDXGISwapChain3>			_swapChain;

		// Direct3D rendering objects. Required for 3D.
		ComPtr<ID3D11RenderTargetView1>	_d3dRenderTargetView;
		D3D11_VIEWPORT					_screenViewport;


		// Cached device properties.
		D3D_FEATURE_LEVEL				_d3dFeatureLevel;
		Size2UI							_d3dRenderTargetSize;

	};
}
