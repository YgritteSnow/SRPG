#include <d3d11_4.h>
#include <dxgi1_5.h>
#include <wrl\client.h>


namespace SRPG
{
	class DeviceResources
	{
	public:
		DeviceResources();
		void SetWindow(HWND window);
		void ValidateDevice();
		void HandleDeviceLost();

		void Present();



		// D3D Accessors.
		ID3D11Device3* GetD3DDevice() const
		{
			return m_d3dDevice.Get();
		}
		ID3D11DeviceContext3* GetD3DDeviceContext() const
		{
			return m_d3dContext.Get();
		}
		IDXGISwapChain3* GetSwapChain() const
		{
			return m_swapChain.Get();
		}
		D3D_FEATURE_LEVEL GetDeviceFeatureLevel() const
		{
			return m_d3dFeatureLevel;
		}
		ID3D11RenderTargetView1* GetBackBufferRenderTargetView() const
		{
			return m_d3dRenderTargetView.Get();
		}
		ID3D11DepthStencilView* GetDepthStencilView() const
		{
			return m_d3dDepthStencilView.Get();
		}
		D3D11_VIEWPORT GetScreenViewport() const
		{
			return m_screenViewport;
		}

	private:
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void UpdateRenderTargetSize();

		HWND m_window;

		// Direct3D objects.
		Microsoft::WRL::ComPtr<ID3D11Device3>			m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext3>	m_d3dContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>			m_swapChain;

		// Direct3D rendering objects. Required for 3D.
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView1>	m_d3dRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_d3dDepthStencilView;
		D3D11_VIEWPORT									m_screenViewport;


		// Cached device properties.
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel;
		Windows::Foundation::Size						m_d3dRenderTargetSize;
		Windows::Foundation::Size						m_outputSize;
		Windows::Foundation::Size						m_logicalSize;

	};
}
