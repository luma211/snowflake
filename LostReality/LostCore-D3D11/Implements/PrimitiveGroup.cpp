/*
* file PrimitiveGroup.cpp
*
* author luoxw
* date 2017/02/10
*
*
*/

#include "stdafx.h"
#include "PrimitiveGroup.h"

using namespace LostCore;

D3D11::FPrimitiveGroup::FPrimitiveGroup()
	: VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
	, VertexBufferSlot(0)
	, VertexBufferNum(0)
	, VertexStride(0)
	, VertexBufferOffset(0)
	, VertexCount(0)
	, IndexFormat(DXGI_FORMAT_UNKNOWN)
	, IndexBufferOffset(0)
	, IndexCount(0)
	, Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, RenderOrder(ERenderOrder::Opacity)
{
}

D3D11::FPrimitiveGroup::~FPrimitiveGroup()
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
}

void D3D11::FPrimitiveGroup::Commit()
{
	FRenderContext::Get()->CommitPrimitiveGroup(this);
}

void D3D11::FPrimitiveGroup::SetVertexElement(uint32 flags)
{
	VertexElement = flags;
}

uint32 D3D11::FPrimitiveGroup::GetVertexElement() const
{
	return VertexElement;
}

void D3D11::FPrimitiveGroup::SetRenderOrder(ERenderOrder ro)
{
	RenderOrder = ro;
}

ERenderOrder D3D11::FPrimitiveGroup::GetRenderOrder() const
{
	return RenderOrder;
}

bool D3D11::FPrimitiveGroup::ConstructVB(const void * buf, uint32 bytes, uint32 stride, bool bDynamic)
{
	const char* head = "D3D11::FPrimitiveGroup::ConstructVB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
	if (!device.IsValid())
	{
		return false;
	}

	VertexStride = stride;
	VertexBufferSlot = 0;
	VertexBufferNum = 1;
	VertexBufferOffset = 0;
	VertexCount = bytes / stride;
	bIsVBDynamic = bDynamic;
	return SSuccess == CreatePrimitiveVertex(device.GetReference(), buf, bytes, bIsVBDynamic, VertexBuffer);
}

bool D3D11::FPrimitiveGroup::ConstructIB(const void * buf, uint32 bytes, uint32 stride, bool bDynamic)
{
	const char* head = "D3D11::FPrimitiveGroup::ConstructIB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
	if (!device.IsValid())
	{
		return false;
	}

	switch (stride)
	{
	case 2:
		IndexFormat = DXGI_FORMAT_R16_UINT;
		break;
	case 4:
		IndexFormat = DXGI_FORMAT_R32_UINT;
		break;
	default:
		break;
	}

	IndexBufferOffset = 0;
	IndexCount = bytes / stride;
	bIsIBDynamic = bDynamic;
	return SSuccess == CreatePrimitiveIndex(device.GetReference(), buf, bytes, bIsIBDynamic, IndexBuffer);
}

void D3D11::FPrimitiveGroup::SetTopology(EPrimitiveTopology topo)
{
	switch (topo)
	{
	case EPrimitiveTopology::PointList:
		Topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case EPrimitiveTopology::LineList:
		Topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case EPrimitiveTopology::TriangleList:
		Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	default:
		break;
	}
}

void D3D11::FPrimitiveGroup::UpdateVB(const void * buf, uint32 bytes)
{
	const char* head = "D3D11::FPrimitiveGroup::UpdateVB";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(head);
	if (!cxt.IsValid())
	{
		return;
	}

	// 强制释放IndexBuffer
	IndexBuffer = nullptr;

	//assert(bIsVBDynamic);

	// 更新的bytes大于vertex buffer，重新创建
	if (bytes > (VertexCount * VertexStride))
	{
		VertexBuffer = nullptr;
		TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
		VertexCount = bytes / VertexStride;
		assert(SSuccess == CreatePrimitiveVertex(device.GetReference(), buf, bytes, bIsVBDynamic, VertexBuffer));
	}
	else
	{
		VertexCount = bytes / VertexStride;
		D3D11_BOX destRegion;
		destRegion.left = 0;
		destRegion.right = bytes;
		destRegion.top = 0;
		destRegion.bottom = 1;
		destRegion.front = 0;
		destRegion.back = 1;
		cxt->UpdateSubresource(VertexBuffer.GetReference(), 0, &destRegion, buf, 0, 0);
	}
}

void D3D11::FPrimitiveGroup::Draw()
{
	const char* head = "D3D11::FPrimitiveGroup::Draw";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(head);
	if (!cxt.IsValid())
	{
		return;
	}

	auto buf = VertexBuffer.GetReference();
	cxt->IASetVertexBuffers(VertexBufferSlot, VertexBufferNum, &buf, &VertexStride, &VertexBufferOffset);
	cxt->IASetIndexBuffer(IndexBuffer.GetReference(), IndexFormat, IndexBufferOffset);
	cxt->IASetPrimitiveTopology(Topology);

	if (IndexBuffer.IsValid())
	{
		cxt->DrawIndexed(IndexCount, 0, 0);
	}
	else
	{
		cxt->Draw(VertexCount, 0);
	}
}
