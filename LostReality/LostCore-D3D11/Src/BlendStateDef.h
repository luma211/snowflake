/*
* file BlendStateDef.h
*
* author luoxw
* date 2017/03/02
*
*
*/

#pragma once

namespace D3D11
{
	struct FBlendState_Solid
	{
		INLINE static std::string GetName()
		{
			static std::string SName = "SOLID";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11BlendState> GetDesc(const TRefCountPtr<ID3D11Device>& device = nullptr, bool bDestroy = false)
		{
			static TRefCountPtr<ID3D11BlendState> SState;
			static bool SCreated = false;

			if (device.IsValid() && !SCreated)
			{
				SCreated = true;
				D3D11_BLEND_DESC desc;
				ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
				desc.AlphaToCoverageEnable = FALSE;
				desc.IndependentBlendEnable = FALSE;
				desc.RenderTarget[0].BlendEnable = FALSE;
				desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				HRESULT hr = device->CreateBlendState(&desc, SState.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FBlendState_Solid::GetDesc", "create blend state failed: 0x%08x(%d)", hr, hr);
				}
			}

			if (bDestroy)
			{
				SState = nullptr;
				SCreated = false;
			}

			return SState;
		}
	};

	struct FBlendState_Add
	{
		INLINE static std::string GetName()
		{
			static std::string SName = "ADD";
			return SName;
		}

		INLINE static TRefCountPtr<ID3D11BlendState> GetDesc(const TRefCountPtr<ID3D11Device>& device = nullptr, bool bDestroy = false)
		{
			static TRefCountPtr<ID3D11BlendState> SState;
			static bool SCreated = false;

			if (device.IsValid() && !SCreated)
			{
				SCreated = true;
				D3D11_BLEND_DESC desc;
				ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
				desc.AlphaToCoverageEnable = TRUE;
				desc.IndependentBlendEnable = FALSE;
				desc.RenderTarget[0].BlendEnable = TRUE;
				desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				HRESULT hr = device->CreateBlendState(&desc, SState.GetInitReference());
				if (FAILED(hr))
				{
					LVERR("FBlendState_Add::GetDesc", "create blend state failed: 0x%08x(%d)", hr, hr);
				}
			}

			if (bDestroy)
			{
				SState = nullptr; 
				SCreated = false;
			}

			return SState;
		}
	};

	struct FBlendStateMap
	{
		static FBlendStateMap* Get()
		{
			static FBlendStateMap Inst;
			return &Inst;
		}

		std::map<std::string, TRefCountPtr<ID3D11BlendState>> StateMap;

		void Initialize(const TRefCountPtr<ID3D11Device>& device)
		{
			StateMap.insert(std::make_pair(FBlendState_Solid::GetName(), FBlendState_Solid::GetDesc(device)));
			StateMap.insert(std::make_pair(FBlendState_Add::GetName(), FBlendState_Add::GetDesc(device)));
		}

		void ReleaseComObjects()
		{
			bool bDestroy = true;
			FBlendState_Solid::GetDesc(nullptr, bDestroy);
			FBlendState_Add::GetDesc(nullptr, bDestroy);

			StateMap.clear();
		}

		INLINE TRefCountPtr<ID3D11BlendState> GetDesc(const std::string& key)
		{
			for (auto element : StateMap)
			{
				if (element.first.compare(key) == 0)
				{
					return element.second;
				}
			}

			return TRefCountPtr<ID3D11BlendState>();
		}
	};
}