#pragma once

namespace lostvr {
	typedef int EnumEyeID;

	template<typename ComObjectType>
	inline void ReleaseComObjectRef(ComObjectType* Obj)
	{
		if (Obj != nullptr)
		{
			Obj->Release();
			Obj = nullptr;
		}
	}

	class TextureProjector
	{
	public:
		TextureProjector();
		~TextureProjector();

		bool Init(const ID3D11Resource* Source, uint32 HmdBufWidth, uint32 HmdBufHeight, const LVMatrix* LeftEyeView, const LVMatrix* LeftEyeProj, const LVMatrix* RightEyeView, const LVMatrix* RightEyeProj);
		void Fini();
		bool OnPresent(const ID3D11Resource* Source);

		ID3D11Texture2D* GetFinalBuffer(EnumEyeID Eye);
		void SetEyePose(EnumEyeID Eye, const LVMatrix& EyeView, const LVMatrix& Proj);

		static TextureProjector* Get()
		{
			static TextureProjector Inst;
			return &Inst;
		}

	protected:
		void Update();

		// renderer properties
		ID3D11Device*				DeviceRef;
		ID3D11DeviceContext*		ContextRef;
		ID3D11Texture2D*			BB[2];
		ID3D11RenderTargetView*		RTVs[2];
		//ID3D11DepthStencilView*		DSV;
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
		ID3D11Texture2D*			SourceCopy;
		ID3D11ShaderResourceView*	SRV;

		ID3D11Texture2D*			SourceRef;

		DXGI_FORMAT		BufferFormat;
		UINT			BufferWidth;
		UINT			BufferHeight;

		FrameBufferWVP	EyePose[2];
	};
}