/*
* file GizmoLine.h
*
* author luoxw
* date 2017/07/25
*
*
*/

#pragma once


namespace LostCore
{
	class IPrimitiveGroup;
	class IMaterial;

	struct FSegmentVertex
	{
		FFloat3 Coordinate;
		FColor96 Color;
		FFloat2 Unused;

		FSegmentVertex() {}
		FSegmentVertex(const FFloat3& pos, const FColor96& col)
			: Coordinate(pos), Color(col) {}
	};

	struct FSegmentData
	{
		FFloat3 StartPt;
		FFloat3 StopPt;
		FColor96 Color;
		FFloat3 Unused;
	};

	// ��������ƹ���
	class FSegmentTool
	{
	private:
		FFloat4x4 World;
		vector<FSegmentVertex> Data;

		bool bConstructed;
		uint32 CurrentPrimitiveBytes;
		IPrimitiveGroup* Primitive;
		IMaterial* Material;

		void ResetData();
		bool ConstructPrimitive(LostCore::IRenderContext* rc, const void* buf, uint32 bytes);
		void DestroyPrimitive();

	public:
		FSegmentTool();
		~FSegmentTool();

		void Draw(LostCore::IRenderContext* rc, float sec = 0.f);
		void AddSegment(const FSegmentData& seg);

		void SetWorldMatrix(const FFloat4x4& mat);

		static FSegmentTool* Get()
		{
			FSegmentTool SInstance;
			return &SInstance;
		}
	};
}
