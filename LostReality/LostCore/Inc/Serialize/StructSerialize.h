/*
* file StructSerialize.h
*
* author luoxw
* date 2017/05/26
*
*
*/

#pragma once

namespace LostCore
{
	inline FVertexTypes::Details GetVertexDetails(int flag)
	{
		return LostCore::FVertexTypes::GetVertexDetail3D(
			(flag & EVertexElement::UV) == EVertexElement::UV,
			(flag & EVertexElement::Normal) == EVertexElement::Normal,
			(flag & EVertexElement::Tangent) == EVertexElement::Tangent,
			(flag & EVertexElement::Binormal) == EVertexElement::Binormal,
			(flag & EVertexElement::VertexColor) == EVertexElement::VertexColor,
			(flag & EVertexElement::Skin) == EVertexElement::Skin);
	}

	inline uint32 GetAlignedSize(uint32 sz, uint32 alignment)
	{
		return (uint32)ceil(sz / (float)alignment)*alignment;
	}

	inline uint32 GetPaddingSize(uint32 sz, uint32 alignment)
	{
		return (uint32)ceil(sz / (float)alignment) * alignment - sz;
	}

	typedef map<string, FMatrix> FPose;

	struct FTreeNode
	{
		string Name;
		vector<FTreeNode> Children;

		FTreeNode() 
		{
		}

		FTreeNode(const FJson& j)
		{
			Load(j);
		}

		void Load(const FJson& j)
		{
			assert(j.find(K_NAME) != j.end());
			Name = j[K_NAME];

			Children.clear();
			if (j.find(K_LAYER) != j.end())
			{
				for (auto it = j[K_LAYER].begin(); it != j[K_LAYER].end(); ++it)
				{
					Children.push_back(FTreeNode(it.value()));
				}
			}
		}
	};

	inline FBinaryIO& operator<<(FBinaryIO& stream, const FTreeNode& data)
	{
		stream << data.Name << data.Children.size();
		for (auto& child : data.Children)
		{
			stream << child;
		}

		return stream;
	}

	inline FBinaryIO& operator >> (FBinaryIO& stream, FTreeNode& data)
	{
		uint32 childNum;
		stream >> data.Name >> childNum;
		for (uint32 i = 0; i < childNum; ++i)
		{
			data.Children.push_back(FTreeNode());
			stream >> *(data.Children.end() - 1);
		}

		return stream;
	}

	struct FMeshData
	{
		uint32 IndexCount;
		uint32 VertexCount;
		uint32 VertexFlags;

		vector<string> Bones;

		vector<uint16> Indices16;
		vector<uint32> Indices32;

		uint32 VertexMagic;

		vector<FVec3> XYZ;
		vector<FVec2> UV;
		vector<FVec3> Normal;
		vector<FVec3> Tangent;
		vector<FVec3> Binormal;
		vector<FVec3> VertexColor;

		vector<FVec4> BlendWeights;
		vector<FVec4> BlendIndices;

		FPose DefaultPose;

		FTreeNode RootNode;

		FMeshData() {}
		~FMeshData() {}

		void FromJson(const FJson& j)
		{
			IndexCount = j[K_INDEX].size();
			VertexCount = j[K_COORDINATE].size();
			VertexFlags = j[K_VERTEX_ELEMENT];
			Bones.reserve(j[K_SKIN].size());
			for (uint32 i = 0; i < j[K_SKIN].size(); ++i)
			{
				Bones.push_back(j[K_SKIN][i]);
			}

			// fill FMeshData(src) first
			if (VertexCount < (1 << 16))
			{
				Indices16.resize(IndexCount);
			}
			else
			{
				Indices32.resize(IndexCount);
			}

			XYZ.resize(VertexCount);
			UV.resize(VertexCount);
			Normal.resize(VertexCount);
			Tangent.resize(VertexCount);
			Binormal.resize(VertexCount);
			VertexColor.resize(VertexCount);

			if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
			{
				BlendWeights.resize(VertexCount);
				BlendIndices.resize(VertexCount);
			}

			for (uint32 i = 0; i < VertexCount; ++i)
			{
				if (VertexCount < (1 << 16))
				{
					Indices16[i] = j[K_INDEX][i];
				}
				else
				{
					Indices32[i] = j[K_INDEX][i];
				}

				XYZ[i] = j[K_COORDINATE][i];
				
				if ((VertexFlags & EVertexElement::UV) == EVertexElement::UV)
				{
					UV[i] = j[K_UV][i];
				}
				
				if ((VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
				{
					Normal[i] = j[K_NORMAL][i];
				}
				
				if ((VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
				{
					Tangent[i] = j[K_TANGENT][i];
				}
				
				if ((VertexFlags & EVertexElement::Binormal) == EVertexElement::Binormal)
				{
					Binormal[i] = j[K_BINORMAL][i];
				}
				
				if ((VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
				{
					VertexColor[i] = j[K_VERTEXCOLOR][i];
				}

				if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
				{
					BlendWeights[i] = j[K_BLEND_WEIGHTS][i];
					BlendIndices[i] = j[K_BLEND_INDICES][i];
				}
			}

			auto it = j.find(K_DEFAULT_POSE);
			if (it != j.end())
			{
				const FJson& matrices = it.value()[K_POSE];
				for (const auto& bone : Bones)
				{
					auto matrixNode = matrices.find(bone);
					if (matrixNode != matrices.end())
					{
						DefaultPose[bone] = matrixNode.value();
					}
				}
			}

			auto itStack = j.find(K_STACK);
			FTreeNode& node = RootNode;
			if (itStack != j.end())
			{
				RootNode.Load(itStack.value());
			}
		}
	};

	// serialize
	template<>
	inline FBinaryIO& operator<<(FBinaryIO& stream, const FMeshData& data)
	{
		stream << data.IndexCount << data.VertexCount << data.VertexFlags;
		if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			stream << data.Bones.size();
			for (size_t i = 0; i < data.Bones.size(); ++i)
			{
				auto it = data.Bones.begin() + i;
				stream << *it;
			}
		}

		if (data.VertexCount < (1 << 16))
		{
			Serialize(stream, (const uint8*)&data.Indices16[0], data.IndexCount * sizeof(uint16));
		}
		else
		{
			Serialize(stream, (const uint8*)&data.Indices32[0], data.IndexCount * sizeof(uint32));
		}

		uint32 stride = GetVertexDetails(data.VertexFlags).Stride;
		uint32 paddingBytes = GetPaddingSize(stride, 16);
		uint8* padding = new uint8[paddingBytes];
		stream << (uint32)MAGIC_VERTEX;
		for (uint32 i = 0; i < data.VertexCount; ++i)
		{
			stream << data.XYZ[i];

			if ((data.VertexFlags & EVertexElement::UV) == EVertexElement::UV)
			{
				stream << data.UV[i];
			}

			if ((data.VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
			{
				stream << data.Normal[i];
			}

			if ((data.VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
			{
				stream << data.Tangent[i];
			}

			if ((data.VertexFlags & EVertexElement::Binormal) == EVertexElement::Binormal)
			{
				stream << data.Binormal[i];
			}

			if ((data.VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
			{
				stream << data.VertexColor[i];
			}

			if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
			{
				stream << data.BlendWeights[i] << data.BlendIndices[i];
			}

			if (paddingBytes != 0)
			{
				Serialize(stream, padding, paddingBytes);
			}
		}

		stream << data.DefaultPose.size();
		for (auto it = data.DefaultPose.begin(); it != data.DefaultPose.end(); ++it)
		{
			stream << it->first << it->second;
		}

		stream << data.RootNode;

		delete[] padding;
		return stream;
	}

	// deserialize
	template<>
	inline FBinaryIO& operator>>(FBinaryIO& stream, FMeshData& data)
	{
		stream >> data.IndexCount >> data.VertexCount >> data.VertexFlags;
		if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			data.Bones.clear();

			size_t boneSize;
			stream >> boneSize;
			for (size_t i = 0; i < boneSize; ++i)
			{
				std::string boneName;
				stream >> boneName;
				data.Bones.push_back(boneName);
			}
		}

		if (data.VertexCount < (1 << 16))
		{
			data.Indices16.resize(data.IndexCount);
			Deserialize(stream, (uint8*)&data.Indices16[0], data.IndexCount * sizeof(uint16));
		}
		else
		{
			data.Indices32.resize(data.IndexCount);
			Deserialize(stream, (uint8*)&data.Indices32[0], data.IndexCount * sizeof(uint32));
		}

		uint32 stride = GetVertexDetails(data.VertexFlags).Stride;
		uint32 paddingBytes = GetPaddingSize(stride, 16);
		uint8* padding = new uint8[paddingBytes];
		bool allocating = true;
		stream >> data.VertexMagic;
		assert(data.VertexMagic == MAGIC_VERTEX && "vertex data is incorrect");
		for (uint32 i = 0; i < data.VertexCount; ++i)
		{
			if (allocating)
			{
				data.XYZ.resize(data.VertexCount);
			}

			stream >> *(data.XYZ.begin() + i);
			if ((data.VertexFlags & EVertexElement::UV) == EVertexElement::UV)
			{
				if (allocating)
				{
					data.UV.resize(data.VertexCount);
				}

				stream >> *(data.UV.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
			{
				if (allocating)
				{
					data.Normal.resize(data.VertexCount);
				}

				stream >> *(data.Normal.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
			{
				if (allocating)
				{
					data.Tangent.resize(data.VertexCount);
				}

				stream >> *(data.Tangent.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::Binormal) == EVertexElement::Binormal)
			{
				if (allocating)
				{
					data.Binormal.resize(data.VertexCount);
				}

				stream >> *(data.Binormal.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
			{
				if (allocating)
				{
					data.VertexColor.resize(data.VertexCount);
				}

				stream >> *(data.VertexColor.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
			{
				if (allocating)
				{
					data.BlendWeights.resize(data.VertexCount);
					data.BlendIndices.resize(data.VertexCount);
				}

				stream >> *(data.BlendWeights.begin() + i)
					>> *(data.BlendIndices.begin() + i);
			}

			allocating = false;

			if (paddingBytes != 0)
			{
				Deserialize(stream, padding, paddingBytes);
			}
		}

		uint32 defaultPoseSz;
		stream >> defaultPoseSz;
		for (uint32 i = 0; i < defaultPoseSz; ++i)
		{
			string boneName;
			FMatrix boneMatrix;
			stream >> boneName >> boneMatrix;
			data.DefaultPose[boneName] = boneMatrix;
		}

		stream >> data.RootNode;

		delete[] padding;
		return stream;
	}

	// FMeshDataGPU可以反序列化FMeshData序列化的数据，并提供面向图形API的数据格式
	struct FMeshDataGPU
	{
		uint32 IndexCount;
		uint32 VertexCount;
		uint32 VertexFlags;
		vector<string> Bones;

		vector<uint8> Indices;
		vector<uint8> Vertices;

		FPose DefaultPose;

		FTreeNode RootNode;

		uint32 VertexMagic;

		FMeshDataGPU() {}
		~FMeshDataGPU() {}

		inline FMeshData ToMeshData()
		{
			FMeshData output;
			output.IndexCount = IndexCount;
			output.VertexCount = VertexCount;
			output.VertexFlags = VertexFlags;
			output.VertexMagic = VertexMagic;
			if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
			{
				output.Bones.clear();
				for (size_t i = 0; i < Bones.size(); ++i)
				{
					output.Bones.push_back(Bones[i]);
				}
			}

			if (VertexCount < (1 << 16))
			{
				output.Indices16.resize(Indices.size() / sizeof(uint16));
				memcpy(&output.Indices16[0], &Indices[0], Indices.size());
			}
			else
			{
				output.Indices32.resize(Indices.size() / sizeof(uint32));
				memcpy(&output.Indices32[0], &Indices[0], Indices.size()); 
			}

			uint32 stride = GetVertexDetails(VertexFlags).Stride;
			uint32 paddingBytes = GetPaddingSize(stride, 16);
			uint32 alignedSz = GetAlignedSize(stride, 16);
			uint8* padding = new uint8[paddingBytes];
			bool allocating = true;
			for (uint32 i = 0; i < VertexCount; ++i)
			{
				FBinaryIO stream(&Vertices[i * alignedSz], alignedSz);

				if (allocating)
				{
					output.XYZ.resize(VertexCount);
				}
				stream >> output.XYZ[i];

				if ((VertexFlags & EVertexElement::UV) == EVertexElement::UV)
				{
					if (allocating)
					{
						output.UV.resize(VertexCount);
					}
					stream >> output.UV[i];
				}

				if ((VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
				{
					if (allocating)
					{
						output.Normal.resize(VertexCount);
					}
					stream >> output.Normal[i];
				}

				if ((VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
				{
					if (allocating)
					{
						output.Tangent.resize(VertexCount);
					}
					stream >> output.Tangent[i];
				}

				if ((VertexFlags & EVertexElement::Binormal) == EVertexElement::Binormal)
				{
					if (allocating)
					{
						output.Binormal.resize(VertexCount);
					}
					stream >> output.Binormal[i];
				}

				if ((VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
				{
					if (allocating)
					{
						output.VertexColor.resize(VertexCount);
					}
					stream >> output.VertexColor[i];
				}

				if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
				{
					if (allocating)
					{
						output.BlendWeights.resize(VertexCount);
						output.BlendIndices.resize(VertexCount);
					}

					stream >> output.BlendWeights[i] >> output.BlendIndices[i];
				}

				allocating = false;
			}

			for (auto it = DefaultPose.begin(); it != DefaultPose.end(); ++it)
			{
				output.DefaultPose[it->first] = it->second;
			}

			output.RootNode = RootNode;

			delete[] padding;
			return (output);
		}
	};

	template<>
	inline FBinaryIO& operator<<(FBinaryIO& stream, const FMeshDataGPU& data)
	{
		stream << data.IndexCount << data.VertexCount << data.VertexFlags;
		if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			stream << data.Bones.size();
			for (size_t i = 0; i < data.Bones.size(); ++i)
			{
				auto it = data.Bones.begin() + i;
				uint32 stringLen = it->length();
				stream << stringLen;
				Serialize(stream, (const uint8*)&((*it)[0]), stringLen);
			}
		}

		uint32 ibSz = data.IndexCount * (data.VertexCount < (1 << 16) ? 2 : 4);
		Serialize(stream, &(data.Indices[0]), ibSz);

		stream << (uint32)MAGIC_VERTEX;
		Serialize(stream, &(data.Vertices[0]), data.Vertices.size());

		stream << data.DefaultPose.size();
		for (auto it = data.DefaultPose.begin(); it != data.DefaultPose.end(); ++it)
		{
			stream << it->first << it->second;
		}

		stream << data.RootNode;

		return stream;
	}

	template<>
	inline FBinaryIO& operator >> (FBinaryIO& stream, FMeshDataGPU& data)
	{
		stream >> data.IndexCount >> data.VertexCount >> data.VertexFlags;
		if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			data.Bones.clear();

			size_t boneSize;
			stream >> boneSize;
			for (size_t i = 0; i < boneSize; ++i)
			{
				uint32 stringLen;
				stream >> stringLen;
				uint8* boneName = new uint8[stringLen + 1];
				Deserialize(stream, boneName, stringLen);
				boneName[stringLen] = '\0';
				data.Bones.push_back(string((char*)boneName));
			}
		}

		uint32 ibSz = data.IndexCount * (data.VertexCount < (1 << 16) ? 2 : 4);
		data.Indices.resize(ibSz);
		Deserialize(stream, &(data.Indices[0]), ibSz);

		stream >> data.VertexMagic;
		assert(data.VertexMagic == MAGIC_VERTEX && "vertex data is incorrect");
		uint32 stride = GetVertexDetails(data.VertexFlags).Stride;
		uint32 sz = (uint32)ceil(stride / 16.f) * 16;
		sz *= data.VertexCount;
		data.Vertices.resize(sz);
		Deserialize(stream, &(data.Vertices[0]), sz);

		uint32 defaultPoseSz;
		stream >> defaultPoseSz;
		for (uint32 i = 0; i < defaultPoseSz; ++i)
		{
			std::string boneName;
			FMatrix boneMatrix;
			stream >> boneName >> boneMatrix;
			data.DefaultPose[boneName] = boneMatrix;
		}

		stream >> data.RootNode;

		return stream;
	}
}