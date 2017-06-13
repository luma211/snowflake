/*
* file BasicStaticMesh.cpp
*
* author luoxw
* date 2017/02/10
*
*
*/

#include "stdafx.h"
#include "BasicModel.h"
#include "PrimitiveGroupInterface.h"
#include "Serialize/StructSerialize.h"
#include "Resource/MeshLoader.h"

using namespace LostCore;

LostCore::FBasicModel::FBasicModel()
	: Primitive(nullptr)
	, Material(nullptr)
	, PrimitiveFlags(0xffffffff)
{
}

LostCore::FBasicModel::~FBasicModel()
{
	assert(Primitive == nullptr);
	assert(Material == nullptr);
}

bool LostCore::FBasicModel::Load(IRenderContext * rc, const char* url)
{
	const char* head = "LostCore::FBasicModel::Load";

	FJson modelJson;
	if (!FDirectoryHelper::Get()->GetModelJson(url, modelJson))
	{
		return false;
	}

	assert(Primitive == nullptr && Material == nullptr && "model should be clear before load anything");
	assert(modelJson.find("type") != modelJson.end() && "model needs [type] section");
	assert((modelJson.find("material") != modelJson.end() ||  modelJson.find("material_prefix") != modelJson.end()) && "model needs [material] section");

	auto itType = modelJson.find("type");
	if (itType.value() == (int)EPrimitiveType::Skeleton)
	{
		assert(modelJson.find("primitive") != modelJson.end() && "model needs [primitive] section");

		string primitivePath;
		if (!FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(modelJson.find("primitive").value(), primitivePath))
		{
			string path = modelJson.find("primitive").value();
			LVERR(head, "failed to find primitive json: %s", path.c_str());
			return false;
		}

		FBinaryIO stream;
		stream.ReadFromFile(primitivePath.c_str());

		FMeshDataGPU data;
		stream >> data;
		PrimitiveFlags = data.VertexFlags;

		if (D3D11::WrappedCreatePrimitiveGroup(&Primitive) == SSuccess)
		{
			uint32 ibStride = data.VertexCount < (1 << 16) ? 2 : 4;
			uint32 vbStride = data.Vertices.size() / data.VertexCount;
			assert(Primitive->ConstructVB(rc, &(data.Vertices[0]), data.Vertices.size(), vbStride, false) &&
				Primitive->ConstructIB(rc, &(data.Indices[0]), data.Indices.size(), ibStride, false));
		}
	}
	else
	{
		assert(0 && "Unknown primitive type");
		return false;
	}

	if (((PrimitiveFlags & EVertexElement::Skin) == EVertexElement::Skin && D3D11::WrappedCreateMaterial_SceneObjectSkinned(&Material) == SSuccess) ||
		((PrimitiveFlags & EVertexElement::Skin) != EVertexElement::Skin && D3D11::WrappedCreateMaterial_SceneObject(&Material) == SSuccess))
	{
		if (modelJson.find("material") != modelJson.end())
		{
			string materialPath = modelJson.find("material").value();
			return Material->Initialize(rc, materialPath.c_str());
		}
		else
		{
			string materialPath = modelJson.find("material_prefix").value();
			string vertexName = GetVertexDetails(PrimitiveFlags).Name;
			materialPath = materialPath + "_" + vertexName + ".json";
			return Material->Initialize(rc, materialPath.c_str());
		}
	}
	else
	{
		assert(0 && "create material failed");
		return false;
	}

	return true;
}

void LostCore::FBasicModel::Fini()
{
	if (Primitive != nullptr)
	{
		D3D11::WrappedDestroyPrimitiveGroup(std::forward<IPrimitiveGroup*>(Primitive));
		Primitive = nullptr;
	}

	if (Material != nullptr)
	{
		if ((PrimitiveFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			D3D11::WrappedDestroyMaterial_SceneObjectSkinned(std::forward<IMaterial*>(Material));
		}
		else
		{
			D3D11::WrappedDestroyMaterial_SceneObject(std::forward<IMaterial*>(Material));
		}

		Material = nullptr;
	}
}

void LostCore::FBasicModel::Tick(float sec)
{
	//for (auto pg : PrimitiveGroups)
	//{
	//	if (pg != nullptr)
	//	{
	//		//pg->Tick(sec);
	//	}
	//}
}

void LostCore::FBasicModel::Draw(IRenderContext * rc, float sec)
{
	//for (auto pg : PrimitiveGroups)
	//{
	//	if (pg != nullptr)
	//	{
	//		pg->Draw(rc, sec);
	//	}
	//}
	if (Material != nullptr)
	{
		Material->UpdateConstantBuffer(rc, (const void*)&WorldMatrix, sizeof(WorldMatrix));
		Material->Draw(rc, sec);
	}

	if (Primitive != nullptr)
	{
		Primitive->Draw(rc, sec);
	}
}

//void LostCore::FBasicModel::AddPrimitiveGroup(IPrimitiveGroup* pg)
//{
//	if (pg != nullptr && std::find(PrimitiveGroups.begin(), PrimitiveGroups.end(), pg) == PrimitiveGroups.end())
//	{
//		PrimitiveGroups.push_back(pg);
//	}
//}
//
//void LostCore::FBasicModel::RemovePrimitiveGroup(IPrimitiveGroup* pg)
//{
//	if (pg == nullptr)
//	{
//		return;
//	}
//
//	auto it = std::find(PrimitiveGroups.begin(), PrimitiveGroups.end(), pg);
//	if (it != PrimitiveGroups.end())
//	{
//		PrimitiveGroups.erase(it);
//	}
//}
//
//void LostCore::FBasicModel::ClearPrimitiveGroup(std::function<void(IPrimitiveGroup*)> func)
//{
//	if (func != nullptr)
//	{
//		for (auto pg : PrimitiveGroups)
//		{
//			if (pg != nullptr)
//			{
//				func(pg);
//			}
//		}
//	}
//
//	PrimitiveGroups.clear();
//}

void LostCore::FBasicModel::SetWorldMatrix(const FMatrix & world)
{
	WorldMatrix = world;
}
