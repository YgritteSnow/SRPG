#pragma once
#include "BasicType.h"
#include <winerror.h>
#include <exception>
#include <string>
#include <d3d11_3.h>
#include "ComPtr.h"

namespace X
{
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

	template <uint32 StringLength>
	void SetDebugName(ID3D11DeviceChild* obj, char(&name)[StringLength])
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, StringLength, name);
	}
	template <uint32 StringLength>
	void SetDebugName(ComPtr<ID3D11DeviceChild> const& obj, char(&name)[StringLength])
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, StringLength, name);
	}

	inline void SetDebugName(ID3D11DeviceChild* obj, std::string const& name)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.size()), name.c_str());
	}
	inline void SetDebugName(ComPtr<ID3D11DeviceChild> const& obj, std::string const& name)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.size()), name.c_str());
	}

	inline void SetDebugName(ID3D11Device* obj, std::string const& name)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.size()), name.c_str());
	}
	inline void SetDebugName(ComPtr<ID3D11Device> const& obj, std::string const& name)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.size()), name.c_str());
	}

	struct DXEventSection
	{
		~DXEventSection()
		{
			if (_annotation)
			{
				_annotation->EndEvent();
			}
		}
		DXEventSection(DXEventSection&& other) : _annotation(other._annotation)
		{
			other._annotation = nullptr;
		}
	private:
		friend class DeviceAndContext;
		ID3DUserDefinedAnnotation* _annotation = nullptr;

		DXEventSection(ID3DUserDefinedAnnotation* annotation, std::wstring const& section) : _annotation(annotation)
		{
			if (_annotation && _annotation->GetStatus())
			{
				_annotation->BeginEvent(section.c_str());
			}
			else
			{
				_annotation = nullptr;
			}
		}
		DXEventSection(ID3DUserDefinedAnnotation* annotation, wchar_t* section) : _annotation(annotation)
		{
			if (_annotation && _annotation->GetStatus())
			{
				_annotation->BeginEvent(section);
			}
			else
			{
				_annotation = nullptr;
			}
		}

		DXEventSection(DXEventSection const&) = delete;
		DXEventSection& operator=(DXEventSection const&) = delete;
		DXEventSection& operator=(DXEventSection&&) = delete;
	};
}
