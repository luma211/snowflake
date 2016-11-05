#pragma once

#include "Direct3D11Renderer.h"

namespace lostvr {
	typedef int EnumEyeID;
	const EnumEyeID SEnumEyeL = 0;
	const EnumEyeID SEnumEyeR = 1;

	class BaseTextureProjector
	{
	public:
		BaseTextureProjector();
		~BaseTextureProjector();

		virtual bool IsInitialized(IDXGISwapChain* swapChain) const = 0;
		virtual bool InitializeRenderer(IDXGISwapChain* swapChain) = 0;
		virtual Direct3D11Helper* GetRenderer() = 0;
		virtual bool InitializeTextureSRV() = 0;
		virtual ID3D11ShaderResourceView* GetTextureSRV() = 0;

		virtual void DestroyRHI();

		virtual bool PrepareSRV() { return true; }

		bool InitializeProjector(
			IDXGISwapChain* swapChain,
			uint32 width, uint32 height,
			const LVMatrix* leftView,
			const LVMatrix* leftProj,
			const LVMatrix* rightView,
			const LVMatrix* rightProj);


		bool UpdateTexture(IDXGISwapChain* swapChain = nullptr);

		ID3D11Texture2D* GetFinalBuffer(EnumEyeID Eye);
		void SetEyePose(EnumEyeID Eye, const LVMatrix& EyeView, const LVMatrix& Proj);

		void* operator new(size_t i)
		{
			return _mm_malloc(i, 16);
		}

		void operator delete(void* p)
		{
			_mm_free(p);
		}

	protected:
		void Update();

		// renderer properties
		ID3D11Texture2D*			BB[2];
		ID3D11RenderTargetView*		RTVs[2];
		ID3D11DepthStencilState*	DSS;
		ID3D11RasterizerState*		RS;

		// scene properties
		ID3D11Buffer*				VB;
		ID3D11Buffer*				IB;
		ID3D11InputLayout*			VL;
		ID3D11VertexShader*			VS;
		ID3D11PixelShader*			PS;
		ID3D11SamplerState*			Sampler;
		ID3D11Buffer*				WVPCB;

		UINT RecommendWidth;
		UINT RecommendHeight;

		FrameBufferWVP	EyePose[2];
	};
}