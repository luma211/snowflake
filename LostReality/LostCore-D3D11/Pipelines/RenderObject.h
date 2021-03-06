/*
* file RenderObject.h
*
* author luoxw
* date 2017/10/09
*
*
*/

namespace D3D11
{
	struct FRenderObject
	{
		FPrimitiveGroup* PrimitiveGroup;
		vector<FConstantBuffer*> ConstantBuffers;
		vector<FTexture2D*> ShaderResources;
		vector<FInstancingData*> InstancingDatas;

		FRenderObject();

		void Reset();
		uint32 GetVertexFlags() const;
	};
}